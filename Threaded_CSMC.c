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
sem_t mutexChairs;
sem_t mutexArrivedStudentQ;
sem_t mutexWaitingStudentQ;

sem_t coordinatorWaiting;
sem_t tutorWaiting;
int numberOfChairs;



//  STUDENT THREAD
int *student()
{
  //Student tries to enter the waiting room (SXS170005)
  sem_wait(&mutexChairs);
  if (numberOfChairs < 0)
  {
     sem_post(&mutexChairs);
     return 0; 
  }
  //Decrement number of chairs in the waiting room by 1 (SXS170005)
  numberOfChairs = numberOfChairs - 1;
  sem_post(&mutexChairs);
  sem_post(&coordinatorWaiting);
  //Add student id to the queue
  sem_wait(&mutexArrivedStudentQ);
}




//  COORDINATOR THREAD
void *coordinator()
{
  //sem_wait(coordinatorWaiting);

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
