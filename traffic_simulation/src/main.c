#include "main.h"

Intersection intersections[NUM_INTERSECTIONS];
Statistics stats;
Config config;
pthread_mutex_t stats_lock = PTHREAD_MUTEX_INITIALIZER;
WINDOW *intersection_windows[NUM_INTERSECTIONS];
WINDOW *stats_window;

int main() {
    pthread_t controllers[NUM_INTERSECTIONS];
    pthread_t vehicle_thread, emergency_thread, mover_thread;
    pthread_t gui_thread, monitor_thread, config_thread;
    int ids[NUM_INTERSECTIONS];
    
    load_config("config.txt");
    srand(time(NULL));
    initialize_system();
    
    stats.start_time = time(NULL);
    stats.total_vehicles = 0;
    stats.total_emergency_vehicles = 0;
    stats.max_wait_time = 0;
    stats.min_wait_time = INT_MAX;
    stats.total_wait_time = 0;
    
    init_gui();
    
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        ids[i] = i;
        pthread_create(&controllers[i], NULL, traffic_light_controller, &ids[i]);
    }
    
    pthread_create(&vehicle_thread, NULL, vehicle_generator, NULL);
    pthread_create(&emergency_thread, NULL, emergency_vehicle_generator, NULL);
    pthread_create(&mover_thread, NULL, vehicle_mover, NULL);
    pthread_create(&gui_thread, NULL, update_gui, NULL);
    pthread_create(&monitor_thread, NULL, monitor_performance, NULL);
    pthread_create(&config_thread, NULL, config_manager, NULL);
    
    sleep(config.simulation_duration);
    
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        pthread_cancel(controllers[i]);
    }
    pthread_cancel(vehicle_thread);
    pthread_cancel(emergency_thread);
    pthread_cancel(mover_thread);
    pthread_cancel(gui_thread);
    pthread_cancel(monitor_thread);
    pthread_cancel(config_thread);
    
    print_statistics();
    cleanup_gui();
    return 0;
}

void initialize_system() {
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        intersections[i].id = i;
        intersections[i].total_vehicles_passed = 0;
        intersections[i].total_emergency_passed = 0;
        intersections[i].last_emergency_time = 0;
        pthread_mutex_init(&intersections[i].intersection_lock, NULL);
        
        for (int j = 0; j < NUM_ROADS_PER_INTERSECTION; j++) {
            intersections[i].roads[j].id = j;
            intersections[i].roads[j].state = RED;
            intersections[i].roads[j].vehicle_count = 0;
            intersections[i].roads[j].emergency_vehicle_count = 0;
            pthread_mutex_init(&intersections[i].roads[j].lock, NULL);
            pthread_cond_init(&intersections[i].roads[j].condition, NULL);
            
            if (i < NUM_INTERSECTIONS - 1 && j == 1) {
                intersections[i].roads[j].connected_to_intersection = i + 1;
                intersections[i].roads[j].connected_to_road = 3;
            } else if (i > 0 && j == 3) {
                intersections[i].roads[j].connected_to_intersection = i - 1;
                intersections[i].roads[j].connected_to_road = 1;
            } else {
                intersections[i].roads[j].connected_to_intersection = -1;
            }
        }
    }
}

const char* get_light_state_name(TrafficLightState state) {
    switch (state) {
        case RED: return "RED";
        case GREEN: return "GREEN";
        case YELLOW: return "YELLOW";
        default: return "UNKNOWN";
    }
}

void print_statistics() {
    printf("\n=== Simulation Statistics ===\n");
    printf("Total runtime: %ld seconds\n", time(NULL) - stats.start_time);
    printf("Total vehicles processed: %d\n", stats.total_vehicles);
    printf("Total emergency vehicles processed: %d\n", stats.total_emergency_vehicles);
    
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        printf("Intersection %d: %d vehicles (%d emergency)\n", 
               i, intersections[i].total_vehicles_passed,
               intersections[i].total_emergency_passed);
    }
}