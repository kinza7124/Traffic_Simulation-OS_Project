#include "main.h"

void* traffic_light_controller(void* arg) {
    int intersection_id = *(int*)arg;
    Intersection* intersection = &intersections[intersection_id];
    time_t last_change = time(NULL);
    
    while (1) {
        for (int i = 0; i < NUM_ROADS_PER_INTERSECTION; i++) {
            pthread_mutex_lock(&intersection->roads[i].lock);
            if (intersection->roads[i].emergency_vehicle_count > 0) {
                pthread_mutex_unlock(&intersection->roads[i].lock);
                handle_emergency_vehicle(intersection, i);
                last_change = time(NULL);
                continue;
            }
            pthread_mutex_unlock(&intersection->roads[i].lock);
        }
        
        for (int i = 0; i < NUM_ROADS_PER_INTERSECTION; i++) {
            pthread_mutex_lock(&intersection->roads[i].lock);
            if (intersection->roads[i].vehicle_count > 0) {
                pthread_mutex_unlock(&intersection->roads[i].lock);
                
                change_light_state(intersection, i, GREEN);
                last_change = time(NULL);
                
                int sleep_time = 2 + (intersection->roads[i].vehicle_count / 5);
                if (sleep_time > 5) sleep_time = 5;
                
                if (time(NULL) - intersection->last_emergency_time < 10) {
                    sleep_time = sleep_time / 2;
                    if (sleep_time < 1) sleep_time = 1;
                }
                
                sleep(sleep_time);
                change_light_state(intersection, i, YELLOW);
                sleep(1);
                change_light_state(intersection, i, RED);
            } else {
                pthread_mutex_unlock(&intersection->roads[i].lock);
            }
            usleep(100000);
        }
        
        if (time(NULL) - last_change > 20) {
            for (int i = 0; i < NUM_ROADS_PER_INTERSECTION; i++) {
                pthread_mutex_lock(&intersection->roads[i].lock);
                if (intersection->roads[i].state == GREEN) {
                    pthread_mutex_unlock(&intersection->roads[i].lock);
                    change_light_state(intersection, i, YELLOW);
                    sleep(1);
                    change_light_state(intersection, i, RED);
                    last_change = time(NULL);
                    break;
                }
                pthread_mutex_unlock(&intersection->roads[i].lock);
            }
        }
    }
    return NULL;
}

void change_light_state(Intersection* intersection, int road_id, TrafficLightState new_state) {
    pthread_mutex_lock(&intersection->intersection_lock);
    
    if (new_state == GREEN) {
        while (check_conflicting_roads(intersection, road_id)) {
            pthread_cond_wait(&intersection->roads[road_id].condition, 
                            &intersection->intersection_lock);
        }
    }
    
    pthread_mutex_lock(&intersection->roads[road_id].lock);
    TrafficLightState old_state = intersection->roads[road_id].state;
    intersection->roads[road_id].state = new_state;
    
    log_event("Intersection %d - Road %d changed from %s to %s",
             intersection->id, road_id, 
             get_light_state_name(old_state),
             get_light_state_name(new_state));
    
    if (new_state == RED) {
        pthread_cond_broadcast(&intersection->roads[road_id].condition);
        
        pthread_mutex_lock(&stats_lock);
        stats.total_vehicles += intersection->roads[road_id].vehicle_count;
        stats.total_emergency_vehicles += intersection->roads[road_id].emergency_vehicle_count;
        pthread_mutex_unlock(&stats_lock);
        
        intersection->total_vehicles_passed += intersection->roads[road_id].vehicle_count;
        intersection->total_emergency_passed += intersection->roads[road_id].emergency_vehicle_count;
        
        intersection->roads[road_id].vehicle_count = 0;
        intersection->roads[road_id].emergency_vehicle_count = 0;
    }
    
    pthread_mutex_unlock(&intersection->roads[road_id].lock);
    pthread_mutex_unlock(&intersection->intersection_lock);
}

bool check_conflicting_roads(Intersection* intersection, int road_id) {
    for (int i = 0; i < NUM_ROADS_PER_INTERSECTION; i++) {
        if (i == road_id) continue;
        
        pthread_mutex_lock(&intersection->roads[i].lock);
        if (intersection->roads[i].state == GREEN) {
            if ((road_id + 2) % NUM_ROADS_PER_INTERSECTION == i) {
                pthread_mutex_unlock(&intersection->roads[i].lock);
                return true;
            }
            
            if (intersection->roads[i].emergency_vehicle_count > 0) {
                pthread_mutex_unlock(&intersection->roads[i].lock);
                return true;
            }
        }
        pthread_mutex_unlock(&intersection->roads[i].lock);
    }
    return false;
}

void handle_emergency_vehicle(Intersection* intersection, int road_id) {
    log_event("EMERGENCY at Intersection %d, Road %d", intersection->id, road_id);
    
    change_light_state(intersection, road_id, GREEN);
    sleep(3);
    change_light_state(intersection, road_id, YELLOW);
    sleep(1);
    change_light_state(intersection, road_id, RED);
    
    intersection->last_emergency_time = time(NULL);
}