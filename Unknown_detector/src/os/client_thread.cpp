// #include <pthread.h>
// #include "reader.h"
// #include <unistd.h>
// #include <stdio.h>
// #include <semaphore.h>
// #define MICROSECONDS_MULTIPLIER 1000000

// extern sem_t produced_reader;

// void* reader_thread(void* pArg)
// {
//     (void) pArg;
//     while (1)
//     {
//         put_stats_in_queue();
//         sem_post(&produced_reader);
//         usleep(1*MICROSECONDS_MULTIPLIER);
//     }
//     return 0;
// }


// void run_reader_thread(void)
// {
//     pthread_t thread = 0U;
//     pthread_attr_t threadAttr;

//     pthread_attr_init(&threadAttr);
//     pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

//     pthread_create(&thread, &threadAttr, reader_thread, 0);

//     pthread_attr_destroy(&threadAttr);
// }