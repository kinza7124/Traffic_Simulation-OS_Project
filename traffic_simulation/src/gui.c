#include "main.h"

void init_gui() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        intersection_windows[i] = newwin(10, 30, 2 + (i/2)*12, (i%2)*35);
        box(intersection_windows[i], 0, 0);
        mvwprintw(intersection_windows[i], 0, 2, "Intersection %d", i);
        wrefresh(intersection_windows[i]);
    }
    
    stats_window = newwin(15, 70, 2 + ((NUM_INTERSECTIONS+1)/2)*12, 0);
    box(stats_window, 0, 0);
    mvwprintw(stats_window, 0, 2, "Simulation Statistics");
    wrefresh(stats_window);
}

void cleanup_gui() {
    delwin(stats_window);
    for (int i = 0; i < NUM_INTERSECTIONS; i++) {
        delwin(intersection_windows[i]);
    }
    endwin();
}

void* update_gui(void* arg) {
    while (1) {
        for (int i = 0; i < NUM_INTERSECTIONS; i++) {
            wclear(intersection_windows[i]);
            box(intersection_windows[i], 0, 0);
            mvwprintw(intersection_windows[i], 0, 2, "Intersection %d", i);
            
            for (int j = 0; j < NUM_ROADS_PER_INTERSECTION; j++) {
                pthread_mutex_lock(&intersections[i].roads[j].lock);
                int color_pair;
                switch(intersections[i].roads[j].state) {
                    case RED: color_pair = 1; break;
                    case GREEN: color_pair = 2; break;
                    case YELLOW: color_pair = 3; break;
                }
                
                wattron(intersection_windows[i], COLOR_PAIR(color_pair));
                mvwprintw(intersection_windows[i], 2 + j*2, 2, "Road %d: %s", j, 
                         get_light_state_name(intersections[i].roads[j].state));
                wattroff(intersection_windows[i], COLOR_PAIR(color_pair));
                
                mvwprintw(intersection_windows[i], 2 + j*2 + 1, 2, 
                         "Vehicles: %d (E:%d)", 
                         intersections[i].roads[j].vehicle_count,
                         intersections[i].roads[j].emergency_vehicle_count);
                pthread_mutex_unlock(&intersections[i].roads[j].lock);
            }
            wrefresh(intersection_windows[i]);
        }
        
        wclear(stats_window);
        box(stats_window, 0, 0);
        mvwprintw(stats_window, 0, 2, "Simulation Statistics");
        
        pthread_mutex_lock(&stats_lock);
        mvwprintw(stats_window, 2, 2, "Runtime: %ld seconds", time(NULL) - stats.start_time);
        mvwprintw(stats_window, 3, 2, "Total vehicles: %d", stats.total_vehicles);
        mvwprintw(stats_window, 4, 2, "Emergency vehicles: %d", stats.total_emergency_vehicles);
        pthread_mutex_unlock(&stats_lock);
        
        for (int i = 0; i < NUM_INTERSECTIONS; i++) {
            mvwprintw(stats_window, 6 + i, 2, "Int %d: %d vehicles (%d emergency)", 
                     i, intersections[i].total_vehicles_passed,
                     intersections[i].total_emergency_passed);
        }
        wrefresh(stats_window);
        
        usleep(200000);
    }
    return NULL;
}