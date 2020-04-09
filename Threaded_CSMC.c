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
    int priority;
    sem_t studentWaiting;
    struct StudentNode * next;
};

struct StudentWaitingQueue {
    sem_t studentWaiting;
    struct StudentWaitingQueue * next;
};



// GLOBAL VARIABLES
struct StudentNode * allStudentsHead;
struct StudentWaitingQueue * studentWaitingQueueHead;
struct StudentWaitingQueue * studentWaitingQueueTail;

sem_t mutexChairs;
sem_t mutexStudentToQueue;

sem_t coordinatorWaiting;
sem_t receivedStudentToQueue;
sem_t tutorWaiting;

int numberOfChairs;
int studentToQueue;




/****************************
 *   DATA STRUCTURE FUNCTIONS
 ***************************/


//  ADD TO ALL STUDENTS
void addToAllStudents(struct StudentNode * studentToAdd) {
    //  if list is not empty, set new node next to head for insertion at front
    if(allStudentsHead != NULL) {
        studentToAdd->next = allStudentsHead;
    }
        
    allStudentsHead = studentToAdd;
}


//  FIND PRIORITY OF STUDENT WITH ID
int findPriorityOfSTudent(int studentId) {
    return 1;
}




//  ENQUEUE
//  DEQUEUE




/********************
 *   THREAD FUNCTIONS
 *******************/


//  STUDENT THREAD
void * student()
{
    int studentId = pthread_self();

    printf("Student Id %d", studentId);

    // //  Tries to enter waiting room
    // sem_wait(&mutexChairs);
    // if (numberOfChairs < 0)
    // {
    //     sem_post(&mutexChairs);
    //     return; 
    // }
    // numberOfChairs = numberOfChairs - 1;

    // //  Set student to queue
    // sem_wait(&mutexStudentToQueue);
    // studentToQueue = studentId;
    // sem_post(&mutexStudentToQueue);

    // sem_post(&coordinatorWaiting);
    // sem_wait(&receivedStudentToQueue);
    // sem_post(&mutexChairs);

    // //  Wait for tutor
    // //  sem_wait();

    // //  Leave waiting room
    // sem_wait(&mutexChairs);
    // numberOfChairs = numberOfChairs + 1;
    // sem_post(&mutexChairs);

    // //  Get tutored

    // //  Change priority
}


//  COORDINATOR THREAD
void *coordinator()
{
  // Waiting for student
  sem_wait(coordinatorWaiting);

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

    // //ALLOCATE STUDENTS QUEUES
    // arrivedStudentQueue = malloc(numberOfStudents * sizeof *arrivedStudentQueue);   
    // waitingStudentQueue = malloc(numberOfStudents * sizeof *waitingStudentQueue);      
    //  INITIALIZE SEMAPHORES
    
    
    sem_init(&mutexChairs, 0, 1);
    sem_init(&mutexStudentToQueue, 0, 1);
    sem_init(&coordinatorWaiting, 0, 0);
    sem_init(&receivedStudentToQueue, 0, 0);

    //Creates student threads
    for(i = 0; i < numberOfStudents; i++)
    {
        printf("Thread Id %d", (pthread_create(&students[i], NULL, student, (void *) i)));
    }

    /*
    //Creates tutor threads
    for(i = 0; i < numOfTutors; i++) 
    {

        assert(pthread_create(&tutors[i], NULL, thread, (void *) i) == 0);
    }
    */
}
