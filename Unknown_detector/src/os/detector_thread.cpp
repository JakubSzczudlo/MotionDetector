// #include <pthread.h>
// #include "printer.h"
// #include <unistd.h>
// #include <stdio.h>
// #include <semaphore.h>
// #define MICROSECONDS_MULTIPLIER 1000000

// extern sem_t produced_analyzer;

// void* printer_thread(void* pArg)
// {
//     (void) pArg;
//     while (1)
//     {
//         sem_wait(&produced_analyzer);
//         print_usage();
//         usleep(1*MICROSECONDS_MULTIPLIER);
//     }
//     return 0;
// }


// void run_printer_thread(void)
// {
//     pthread_t thread = 0U;
//     pthread_attr_t threadAttr;

//     pthread_attr_init(&threadAttr);
//     pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

//     pthread_create(&thread, &threadAttr, printer_thread, 0);

//     pthread_attr_destroy(&threadAttr);
// }