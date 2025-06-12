#ifndef TRAFFIC_SIMULATION_H
#define TRAFFIC_SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <sys/msg.h>
#include <sys/inotify.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUM_INTERSECTIONS 4
#define NUM_ROADS_PER_INTERSECTION 4
#define MAX_VEHICLES 100
#define MAX_EMERGENCY_VEHICLES 5
#define SIMULATION_DURATION 60

typedef enum { RED, GREEN, YELLOW } TrafficLightState;
typedef enum { REGULAR, EMERGENCY } VehicleType;

typedef struct {
    int id;
    TrafficLightState state;
    int vehicle_count;
    int emergency_vehicle_count;
    pthread_mutex_t lock;
    pthread_cond_t condition;
    int connected_to_intersection;
    int connected_to_road;
} Road;

typedef struct {
    int id;
    Road roads[NUM_ROADS_PER_INTERSECTION];
    pthread_mutex_t intersection_lock;
    int total_vehicles_passed;
    int total_emergency_passed;
    time_t last_emergency_time;
} Intersection;

typedef struct {
    time_t start_time;
    int total_vehicles;
    int total_emergency_vehicles;
    int max_wait_time;
    int min_wait_time;
    int total_wait_time;
} Statistics;

typedef struct {
    int simulation_duration;
    int num_intersections;
    int num_roads;
    int max_vehicles;
    int max_emergency;
    int vehicle_gen_interval;
    int emergency_gen_interval;
    int scheduling_policy;
} Config;

typedef struct {
    long mtype;
    int intersection_id;
    int road_id;
} EmergencyMsg;

extern Intersection intersections[NUM_INTERSECTIONS];
extern Statistics stats;
extern Config config;
extern pthread_mutex_t stats_lock;
extern WINDOW *intersection_windows[NUM_INTERSECTIONS];
extern WINDOW *stats_window;

void initialize_system();
void load_config(const char* filename);
void* traffic_light_controller(void* arg);
void* vehicle_generator(void* arg);
void* emergency_vehicle_generator(void* arg);
void* vehicle_mover(void* arg);
void* update_gui(void* arg);
void* monitor_performance(void* arg);
void* config_manager(void* arg);
void log_event(const char* format, ...);
void change_light_state(Intersection* intersection, int road_id, TrafficLightState new_state);
bool check_conflicting_roads(Intersection* intersection, int road_id);
void handle_emergency_vehicle(Intersection* intersection, int road_id);
void move_vehicles_between_roads(Road* from_road, Road* to_road, int intersection_from, int intersection_to);
void print_statistics();
const char* get_light_state_name(TrafficLightState state);
void init_gui();
void cleanup_gui();

#endif