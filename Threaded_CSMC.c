//gcc MyProgram.c -o MyProgram -lpthread
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

struct StudentWaiting {
    sem_t studentWaiting;
    int priority;
    struct StudentWaitingQueue * next;
};


// GLOBAL VARIABLES
struct StudentNode * allStudentsHead;
struct StudentWaiting * studentWaitingQueueHead;
struct StudentWaiting * studentWaitingQueueTail;

sem_t mutexChairs;
sem_t mutexStudentToQueue;
sem_t mutexStudentWaitingQueue;

sem_t coordinatorWaiting;
sem_t studentArrived;
sem_t receivedStudentToQueue;
sem_t tutorWaiting;

int numberOfChairs;
int studentToQueue;




/****************************
 *   DATA STRUCTURE FUNCTIONS
 ***************************/


//  STUDENT PRIORITY DATA STRUCTURE


//  ADD TO ALL STUDENTS
void addToAllStudents(struct StudentNode * studentToAdd) {
    //  if list is not empty, set new node next to head for insertion at front
    if(allStudentsHead != NULL) {
        studentToAdd->next = allStudentsHead;
    }
        
    allStudentsHead = studentToAdd;
}


//  STUDENT WAITING QUEUEU DATA STRUCTURE


//  ENQUEUE STUDENT WAITING QUEUE
void enqueueToStudentWaitingQueue(struct StudentWaiting * studentWaitingToQueue) {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;
    
    //  if the queue is empty
    if(studentWaitingQueueHead == NULL) {
        studentWaitingToQueue->next = studentWaitingQueueTail;
        studentWaitingQueueHead = studentWaitingToQueue;
    //  if the queue is not empty
    } else {
        //  if the student has the highest priority
        if(studentWaitingQueueTail->priority > studentWaitingToQueue->priority) {
            studentWaitingToQueue->next = traversalStudentWaiting;
                traversalStudentWaiting = studentWaitingToQueue;
        }

        while(traversalStudentWaiting != NULL) {
            if(traversalStudentWaiting->priority <= studentWaitingToQueue->priority) {
                studentWaitingToQueue->next = traversalStudentWaiting;
                traversalStudentWaiting = studentWaitingToQueue;
                traversalStudentWaiting = NULL;
            } else {
                traversalStudentWaiting = traversalStudentWaiting->next;
            }
        }
    }
}


//  DEQUEUE STUDENT WAITING QUEUE
struct StudentWaiting * dequeueFromStudentWaitingQueue() {

}




/********************
 *   THREAD FUNCTIONS
 *******************/


//  STUDENT THREAD
void * student()
{
    int studentId = pthread_self();
    printf("Student Id %d", studentId);

    //  LOCK to try and enter waiting room
    sem_wait(&mutexChairs);
    if (numberOfChairs < 0)
    {
        sem_post(&mutexChairs);
        return; 
    }
    numberOfChairs = numberOfChairs - 1;
    sem_post(&mutexChairs);


    //  LOCK on sharing student arrival
    sem_wait(&studentArrived);
    //  LOCK on student to queue
    sem_wait(&mutexStudentToQueue);
    studentToQueue = studentId;
    sem_post(&mutexStudentToQueue);
    //  NOTIFIES coordinator that student arrived
    sem_post(&coordinatorWaiting);
    //  WAITING for coordinator to queue student
    sem_wait(&receivedStudentToQueue);
    sem_post(&studentArrived);

    //  Wait for tutor
    //  sem_wait();

    //  LOCK on waiting room chairs
    sem_wait(&mutexChairs);
    numberOfChairs = numberOfChairs + 1;
    sem_post(&mutexChairs);

    //  Get tutored

    //  Change priority
}


//  COORDINATOR THREAD
void *coordinator()
{
    //  WAITING for student to arrive
    sem_wait(&coordinatorWaiting);

    //  LOCK on the student to queue
    sem_wait(&mutexStudentToQueue);
    //      receives student to queue semaphore
    sem_post(&mutexStudentToQueue);

    //  NOTIFIES student that they were queued
    sem_post(&receivedStudentToQueue);

    //  LOCK on the queue of students
    sem_wait(&mutexStudentWaitingQueue);
    //      enqueues the arrived student
    sem_post(&mutexStudentWaitingQueue);

    //  NOTIFIES tutors that there is another student to tutor
    sem_post(&tutorWaiting);
}


//  TUTOR THREAD
void *tutor()
{
    //  WAITING for student to tutor
    sem_wait(&tutorWaiting);

    //  LOCK on the queue of students
    sem_wait(&mutexStudentWaitingQueue);
    //      dequeue and receive student
    sem_wait(&mutexStudentWaitingQueue);

    //  Tutor student
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
