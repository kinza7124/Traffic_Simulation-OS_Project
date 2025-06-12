#include "main.h"

void* vehicle_generator(void* arg) {
    while (1) {
        int intersection_id = rand() % NUM_INTERSECTIONS;
        int road_id = rand() % NUM_ROADS_PER_INTERSECTION;
        
        pthread_mutex_lock(&intersections[intersection_id].roads[road_id].lock);
        if (intersections[intersection_id].roads[road_id].vehicle_count < MAX_VEHICLES) {
            intersections[intersection_id].roads[road_id].vehicle_count++;
            log_event("Regular vehicle added to Intersection %d, Road %d",
                     intersection_id, road_id);
            
            if (intersections[intersection_id].roads[road_id].vehicle_count == 1) {
                pthread_cond_signal(&intersections[intersection_id].roads[road_id].condition);
            }
        }
        pthread_mutex_unlock(&intersections[intersection_id].roads[road_id].lock);
        
        usleep(config.vehicle_gen_interval);
    }
    return NULL;
}

void* emergency_vehicle_generator(void* arg) {
    while (1) {
        sleep(5 + rand() % 15);
        
        int intersection_id = rand() % NUM_INTERSECTIONS;
        int road_id = rand() % NUM_ROADS_PER_INTERSECTION;
        
        pthread_mutex_lock(&intersections[intersection_id].roads[road_id].lock);
        if (intersections[intersection_id].roads[road_id].emergency_vehicle_count < MAX_EMERGENCY_VEHICLES) {
            intersections[intersection_id].roads[road_id].emergency_vehicle_count++;
            log_event("EMERGENCY vehicle added to Intersection %d, Road %d",
                     intersection_id, road_id);
            
            pthread_cond_signal(&intersections[intersection_id].roads[road_id].condition);
        }
        pthread_mutex_unlock(&intersections[intersection_id].roads[road_id].lock);
    }
    return NULL;
}

void* vehicle_mover(void* arg) {
    while (1) {
        usleep(500000 + rand() % 1000000);
        //sleeps for 0.5 to 1.5 seconds
        //Move vehicles between connected roads
        int from_intersection = rand() % NUM_INTERSECTIONS;
        int from_road = rand() % NUM_ROADS_PER_INTERSECTION;
        Road* road = &intersections[from_intersection].roads[from_road];
        
        if (road->connected_to_intersection != -1) {
            pthread_mutex_lock(&road->lock);
            if (road->vehicle_count > 0 || road->emergency_vehicle_count > 0) {
                int to_intersection = road->connected_to_intersection;
                int to_road = road->connected_to_road;
                
                if (pthread_mutex_trylock(&intersections[to_intersection].roads[to_road].lock) == 0) {
                    int vehicles_to_move = road->vehicle_count / 3;
                    if (vehicles_to_move < 1 && road->vehicle_count > 0) vehicles_to_move = 1;
                    
                    int emergencies_to_move = road->emergency_vehicle_count;
                    
                    if (vehicles_to_move > 0 || emergencies_to_move > 0) {
                        log_event("Moving %d vehicles and %d emergencies from Int %d Road %d to Int %d Road %d",
                                 vehicles_to_move, emergencies_to_move,
                                 from_intersection, from_road,
                                 to_intersection, to_road);
                        
                        road->vehicle_count -= vehicles_to_move;
                        road->emergency_vehicle_count -= emergencies_to_move;
                        
                        intersections[to_intersection].roads[to_road].vehicle_count += vehicles_to_move;
                        intersections[to_intersection].roads[to_road].emergency_vehicle_count += emergencies_to_move;
                        
                        if (intersections[to_intersection].roads[to_road].vehicle_count == vehicles_to_move &&
                            intersections[to_intersection].roads[to_road].emergency_vehicle_count == emergencies_to_move) {
                            pthread_cond_signal(&intersections[to_intersection].roads[to_road].condition);
                        }
                    }
                    
                    pthread_mutex_unlock(&intersections[to_intersection].roads[to_road].lock);
                }
            }
            pthread_mutex_unlock(&road->lock);
        }
    }
    return NULL;
}