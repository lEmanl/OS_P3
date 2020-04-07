#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>




// STRUCTS
struct StudentNode {
    int threadId;
    sem_t studentWaiting;
};

struct StudentQueue {
    struct StudentNode * student;
    struct StudentNode * nextStudent;
};



// GLOBAL VARIABLES
sem_t mutexChairs = 1;
sem_t mutexArrivedStudentQ = 1;
sem_t mutexWaitingStudentQ = 1;

sem_t coordinatorWaiting = 0;
sem_t tutorWaiting = 0;

int **studentIDQueue;
int **priorities;
int newStudentInQueue;




//  STUDENT THREAD
void *student()
{

}




//  COORDINATOR THREAD
void *coordinator()
{

}




//  TUTOR THREAD
void *tutor()
{

}




//  MAIN PROGRAM
int main(int argc, char *argv[])
{
    //  THREADS
    pthread_t * students;
    pthread_t * tutors;
    pthread_t coordinator;

    //  LOCALS
    int numberOfStudents;
    int numberOfTutors;
    int numberOfChairs;
    int numberOfHelp;
    long i;

    //Casts arguments into integer and returns error if no digits are passed
    sscanf(argv[1], "%d", &numberOfStudents);
    sscanf(argv[2], "%d", &numberOfTutors);
    sscanf(argv[3], "%d", &numberOfChairs);
    sscanf(argv[4], "%d", &numberOfHelp);

    //  ALLOCATE THREADS
    students = malloc(sizeof(pthread_t) * numberOfStudents);
    tutors = malloc(sizeof(pthread_t) * numberOfTutors);

    //  ALLOCATE DATA
    studentIDQueue = malloc(1000 * sizeof(int*));
    priorities = malloc(1000 * sizeof(int*));
    newStudentInQueue = 0;

    //  INITIALIZE SEMAPHORES
    sem_init(&mutexChairs, 0, 1);
    sem_init(&mutexArrivedStudentQ, 0, 1);
    sem_init(&mutexWaitingStudentQ, 0, 1);
    sem_init(&coordinatorWaiting, 0, 0);
    sem_init(&tutorWaiting, 0, 0);


    //Creates student threads
    for(i = 0; i < numberOfStudents; i++)
    {
        //  returns thread id
        assert(pthread_create(&students[i], NULL, student, (void *) i) == 0);
    }

    /*
    //Creates tutor threads
    for(i = 0; i < numOfTutors; i++) 
    {
        
        assert(pthread_create(&tutors[i], NULL, thread, (void *) i) == 0);
    }
    */
}
