#include "main.h"

void load_config(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
    }
    
    fscanf(file, "simulation_duration=%d\n", &config.simulation_duration);
    fscanf(file, "num_intersections=%d\n", &config.num_intersections);
    fscanf(file, "num_roads=%d\n", &config.num_roads);
    fscanf(file, "max_vehicles=%d\n", &config.max_vehicles);
    fscanf(file, "max_emergency=%d\n", &config.max_emergency);
    fscanf(file, "vehicle_gen_interval=%d\n", &config.vehicle_gen_interval);
    fscanf(file, "emergency_gen_interval=%d\n", &config.emergency_gen_interval);
    fscanf(file, "scheduling_policy=%d\n", &config.scheduling_policy);
    
    fclose(file);
}

void* config_manager(void* arg) {
    int fd = inotify_init();
    int wd = inotify_add_watch(fd, "config.txt", IN_MODIFY);
    char buffer[1024];
    
    while (1) {
        read(fd, buffer, sizeof(buffer));
        load_config("config.txt");
        log_event("Configuration reloaded");
    }
    close(fd);
    return NULL;
}