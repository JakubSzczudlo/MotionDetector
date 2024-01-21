// #include <pthread.h>
// #include "analyzer.h"
// #include <unistd.h>
// #include <stdio.h>
// #include <semaphore.h>
// #define MICROSECONDS_MULTIPLIER 1000000

// extern sem_t produced_reader;
// extern sem_t produced_analyzer;

// void* analyzer_thread(void* pArg)
// {
//     (void) pArg;
//     init_cores_table();
//     while (1)
//     {
//         sem_wait(&produced_reader);
//         read_stats_and_put_result_in_queue();
//         sem_post(&produced_analyzer);
//         usleep(1*MICROSECONDS_MULTIPLIER);
//     }
//     free_cores_table();
//     return 0;
// }

// void run_analyzer_thread(void)
// {
//     pthread_t thread = 0U;
//     pthread_attr_t threadAttr;

//     pthread_attr_init(&threadAttr);
//     pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

//     pthread_create(&thread, &threadAttr, analyzer_thread, 0);

//     pthread_attr_destroy(&threadAttr);
// }