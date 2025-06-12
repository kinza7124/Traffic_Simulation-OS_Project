#include "main.h"

void* monitor_performance(void* arg) {
    FILE* stat_file;
    unsigned long long last_total = 0, last_idle = 0;
    
    while (1) {
        // CPU usage
        stat_file = fopen("/proc/stat", "r");
        char buf[256];
        if (stat_file) {
            fgets(buf, sizeof(buf), stat_file);
            unsigned long long user, nice, system, idle;
            sscanf(buf, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
            fclose(stat_file);
            
            unsigned long long total = user + nice + system;
            unsigned long long diff_total = total - last_total;
            unsigned long long diff_idle = idle - last_idle;
            
            double cpu_usage = 100.0 * (1.0 - (double)diff_idle / diff_total);
            
            // Memory usage
            stat_file = fopen("/proc/self/status", "r");
            long vm_size = 0;
            if (stat_file) {
                while (fgets(buf, sizeof(buf), stat_file)) {
                    if (strstr(buf, "VmSize:")) {
                        sscanf(buf, "VmSize: %ld kB", &vm_size);
                        break;
                    }
                }
                fclose(stat_file);
            }
            
            // Update metrics display
            pthread_mutex_lock(&stats_lock);
            mvwprintw(stats_window, 10, 2, "System Metrics:");
            mvwprintw(stats_window, 11, 2, "CPU Usage: %.2f%%", cpu_usage);
            mvwprintw(stats_window, 12, 2, "Memory Usage: %ld kB", vm_size);
            pthread_mutex_unlock(&stats_lock);
            
            last_total = total;
            last_idle = idle;
        }
        
        sleep(1);
    }
    return NULL;
}