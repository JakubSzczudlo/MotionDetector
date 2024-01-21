// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>

// extern circ_bbuf_info_t circular_buffer_analyzer_printer;

// extern pthread_mutex_t mutex_analyzer_printer;

// void print_usage(void)
// {
//     cpu_info_t cpu_statistics = {0};
//     pthread_mutex_lock(&mutex_analyzer_printer);
//     if (circ_bbuf_pop_info(&circular_buffer_analyzer_printer, &cpu_statistics)) {
//         return;
//     }
//     pthread_mutex_unlock(&mutex_analyzer_printer);
//     double cpu_usage = cpu_statistics.cpu_usage;
//     printf("CPU core: %d, usage: %.2f%%\n", (int)cpu_statistics.cpu_id, cpu_usage);
//     fflush(stdout);
// }