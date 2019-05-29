/*
 To compile:
 gcc -lpthread thread_test.c -o thread_test
*/
 #include "string.h"
 #include "stdio.h"
 #include "stdlib.h"
 #include "pthread.h"
 #include "unistd.h"

 void *some_thread_function ( void *thread_information )
{{
 *((unsigned char *) thread_information) = 1;
 
 for ( int i = 0; i < 1000; i ++ ) {
       printf ( "Thread test...\n" );
 }

 //It's actually possible that this thread starts and finishes before the main thread realizes that it ever started.
 //*((unsigned char *) thread_information) = 0;

 return 0;
}}

 int main ( int argc, char **argv )
{{
 pthread_t thread_identifier = 0;

 //This would be a structure with information to use to communicate with the thread, normally.
 //In this case, we're just using it to see whether or not the thread has started and whether or it it's still running, once it has.
 unsigned char thread_information = 0;

 printf ( "Starting a new thread.\n" );
 int result = pthread_create ( &thread_identifier, NULL, some_thread_function, (void *) &thread_information );

 printf ( "The new thread identifier is this: %d\n", thread_identifier );

 while ( ! thread_information ) {
         usleep ( 10000 );
 }

 printf ( "The thread is actually started, now.\n" );

 for ( int i = 0; i< 100; i ++ )
      {
       printf ( "xx" );
      }

 printf ( "\n" );

 pthread_detach ( thread_identifier );
 printf ( "The thread should be killed now.\n" );

 return 0;
}}
