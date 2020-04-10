//gcc CSMC.c -o CSMC -lpthread
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <semaphore.h>




// STRUCTS
struct StudentNode {
    pthread_t threadId;
    pthread_t tutorThreadId;
    int priority;
    sem_t * studentWaiting;
    struct StudentNode * next;
};

struct StudentWaiting {
    struct StudentNode * student;
    struct StudentWaiting * next;
};


// GLOBAL VARIABLES
struct StudentNode * allStudentsHead;
struct StudentWaiting * studentWaitingQueueHead;

sem_t mutexChairs;
sem_t mutexStudentToQueue;
sem_t mutexStudentWaitingQueue;
sem_t mutexAmountOfStudentBeingTutored;
sem_t mutexTotalStudentsTutored;

sem_t coordinatorWaiting;
sem_t studentArrived;
sem_t receivedStudentToQueue;
sem_t tutorWaiting;

int numberOfChairs;
int totalNumberOfChairs;
int numberOfHelp;
int totalStudentsTutored;
int amountOfStudentsBeingTutored;
const float tutorSleepTime = 0.2;
const float programmingSleepTime = 2.0;
pthread_t studentToQueue;
struct StudentNode *currentStudent;


/****************************
 *   DATA STRUCTURE FUNCTIONS
 ***************************/


//  STUDENT PRIORITY DATA STRUCTURE

//PROTOTYPES
void tutor();
//  ADD TO ALL STUDENTS
void addToAllStudents(struct StudentNode * studentToAdd) {
    //  if list is not empty, set new node next to head for insertion at front
    if(allStudentsHead != NULL) {
        studentToAdd->next = allStudentsHead;
    }       
    allStudentsHead = studentToAdd;
}


//  FIND IN ALL STUDENTS WITH ID
struct StudentNode * findInAllStudents(pthread_t threadId) {
    struct StudentNode * traversalStudentNode = allStudentsHead;

    while(traversalStudentNode != NULL) {
        if(traversalStudentNode->threadId == threadId) {
            break;
        }
        traversalStudentNode = traversalStudentNode->next;
    }

    return traversalStudentNode;
}


//  STUDENT WAITING QUEUEU DATA STRUCTURE


//  ENQUEUE STUDENT WAITING QUEUE
void enqueueToStudentWaitingQueue(struct StudentWaiting * studentWaitingToQueue) {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;
    struct StudentWaiting * previousTraversalStudentWaiting = NULL;
    
    //  if the queue is empty and checks if input is not null
    if(studentWaitingQueueHead == NULL) {
        studentWaitingQueueHead = studentWaitingToQueue;
        studentWaitingQueueHead->next = NULL;
	// traversalStudentWaiting = studentWaitingQueueHead;
    //  if the queue is not empty
    } else {
        //  find the node to insert before
        while(traversalStudentWaiting != NULL) {
            //  if the node had less than or equal to priority
            if(traversalStudentWaiting->student->priority > studentWaitingToQueue->student->priority) {
                previousTraversalStudentWaiting = traversalStudentWaiting;
                traversalStudentWaiting = traversalStudentWaiting->next;
            } else {
                break;
            }
        }

        //  if the previous student is NULL, then insert before the head
        if(previousTraversalStudentWaiting == NULL) {
            studentWaitingToQueue->next = studentWaitingQueueHead;
            studentWaitingQueueHead = studentWaitingToQueue;        
        //  insert between the traversal and previous node
        } else {
            studentWaitingToQueue->next = traversalStudentWaiting;
            previousTraversalStudentWaiting->next = studentWaitingToQueue;
        }
    }
}



//  DEQUEUE FROM STUDENT WAITING QUEUE
struct StudentWaiting * dequeueFromStudentWaitingQueue() {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;
    struct StudentWaiting * dequeuedStudent = NULL;

    //  if the queue is empty
    if(studentWaitingQueueHead == NULL) {
        //printf("queue empty\n");
    //  if the head is the only item in the list
    } else if(studentWaitingQueueHead->next == NULL) {
        dequeuedStudent = studentWaitingQueueHead;
        studentWaitingQueueHead = NULL;
    //  if there are only 2 items in the list
    } else if(studentWaitingQueueHead->next->next == NULL) {
        dequeuedStudent = studentWaitingQueueHead->next;
        studentWaitingQueueHead->next = NULL;
    //  if the queue is not empty
    } else {
        while(traversalStudentWaiting->next->next != NULL) {
            traversalStudentWaiting = traversalStudentWaiting->next;
        }

        dequeuedStudent = traversalStudentWaiting->next;
        traversalStudentWaiting->next = NULL;
    }

    return dequeuedStudent;
}




/********************
 *   THREAD FUNCTIONS
 *******************/




//  STUDENT THREAD
void * studentThread(void * arg)
{
    struct StudentNode * studentNode= (struct StudentNode *) arg;
    studentNode->threadId = pthread_self();
    
    while(studentNode->priority < numberOfHelp) {
        //  LOCK to try and enter waiting room
        sem_wait(&mutexChairs);
        if (numberOfChairs < 0)
        {
            printf("St: Student %ul found no empty chairs. Will try again later.\n", pthread_self());
            sem_post(&mutexChairs);
            return; 
        }

        numberOfChairs = numberOfChairs - 1;
        printf("St: Student %ul takes a seat. Empty chairs = %d\n", pthread_self(), numberOfChairs);
        sem_post(&mutexChairs);


        //  LOCK on sharing student arrival
        sem_wait(&studentArrived);
        //  LOCK on student to queue
        sem_wait(&mutexStudentToQueue);
        studentToQueue = studentNode->threadId;
        sem_post(&mutexStudentToQueue);

        //  NOTIFIES coordinator that student arrived
        sem_post(&coordinatorWaiting);

        //  WAITING for coordinator to queue student
        sem_wait(&receivedStudentToQueue);
        sem_post(&studentArrived);

        //  Wait for tutor
        sem_wait(studentNode->studentWaiting);
        
        //  LOCK on waiting room chairs
        sem_wait(&mutexChairs);
        numberOfChairs = numberOfChairs + 1;
        sem_post(&mutexChairs);

        // Student is getting tutored
        sleep(tutorSleepTime); 

        // Increment student priotiy
        studentNode->priority = studentNode->priority + 1;
    }
}


//  COORDINATOR THREAD
void *coordinatorThread()
{
    pthread_t nextStudentToQueue;
    struct StudentNode * nextStudentNode;
    struct StudentWaiting * nextStudentWaiting;
    int numberOfStudentRequestsReceived = 0;

    while(1) {
        //  WAITING for student to arrive
        sem_wait(&coordinatorWaiting);

        //  LOCK on the student to queue
        sem_wait(&mutexStudentToQueue);
        nextStudentToQueue = studentToQueue;
        sem_post(&mutexStudentToQueue);

        //  NOTIFIES student that they were received
        sem_post(&receivedStudentToQueue);
        numberOfStudentRequestsReceived = numberOfStudentRequestsReceived + 1;
        nextStudentNode = findInAllStudents(nextStudentToQueue);

        //  LOCK on the queue of students
        sem_wait(&mutexStudentWaitingQueue);
        nextStudentWaiting = malloc(sizeof(struct StudentWaiting *));
        nextStudentWaiting->student = nextStudentNode;
        enqueueToStudentWaitingQueue(nextStudentWaiting);
        printf("Co: Student %ul with priority %d in the queue. Waiting students now = %d. Total requests = %d\n", nextStudentToQueue, nextStudentNode->priority, totalNumberOfChairs-numberOfChairs, numberOfStudentRequestsReceived);
        sem_post(&mutexStudentWaitingQueue);
        
        //  NOTIFIES tutors that there is another student to tutor
        sem_post(&tutorWaiting);
    }
}


//  TUTOR THREAD
void *tutorThread()
{
    struct StudentNode * studentNode;
    
    while(1) {
        //  WAITING for student to tutor
        sem_wait(&tutorWaiting);

        //  LOCK on the queue of students
        sem_wait(&mutexStudentWaitingQueue);
        studentNode = dequeueFromStudentWaitingQueue()->student;
        sem_post(&mutexStudentWaitingQueue);

        //  set the tutor for the student
        studentNode->tutorThreadId = pthread_self();

        //  Tutor student
        sem_post(studentNode->studentWaiting);
        tutor(studentNode);
        sem_post(studentNode->studentWaiting);
    }
}


//Student getting tutored
void tutor(struct StudentNode * studentNode)
{
    //  LOCK amount of students getting tutored
    sem_wait(&mutexAmountOfStudentBeingTutored);
    amountOfStudentsBeingTutored = amountOfStudentsBeingTutored + 1;
    sem_post(&mutexAmountOfStudentBeingTutored);

    sleep(tutorSleepTime);  

    //  LOCK amount of students getting tutored
    sem_wait(&mutexAmountOfStudentBeingTutored);
    amountOfStudentsBeingTutored = amountOfStudentsBeingTutored - 1;
    sem_post(&mutexAmountOfStudentBeingTutored);

    //  LOCK changing total students tutored
    sem_wait(&mutexTotalStudentsTutored);
    totalStudentsTutored = totalStudentsTutored + 1;
    sem_post(&mutexTotalStudentsTutored);

    printf("Tu: Student %ul tutored by Tutor %ul. Students tutored now = %d. Total session tutored = %d.\n", studentNode->threadId, studentNode->tutorThreadId, amountOfStudentsBeingTutored, totalStudentsTutored);
}



//  MAIN PROGRAM
int main(int argc, char *argv[])
{
    //  THREADS
    pthread_t * students;
    pthread_t * tutors;
    pthread_t coordinator;

    //  LOCALS
    struct StudentNode * studentToAdd;
    sem_t * studentWaiting;
    int numberOfStudents;
    int numberOfTutors;
    void * value;
    long i;

    //Casts arguments into integer and returns error if no digits are passed
    sscanf(argv[1], "%d", &numberOfStudents);
    sscanf(argv[2], "%d", &numberOfTutors);
    sscanf(argv[3], "%d", &numberOfChairs);
    sscanf(argv[4], "%d", &numberOfHelp);

    //  INITIALIZE GLOBAL VALUES
    totalNumberOfChairs = numberOfChairs;
    totalStudentsTutored = 0;

    //  ALLOCATE THREADS
    students = malloc(sizeof(pthread_t) * numberOfStudents);
    tutors = malloc(sizeof(pthread_t) * numberOfTutors);
    
    //  INITIALIZE SEMAPHORES
    sem_init(&mutexChairs, 0, 1);
    sem_init(&mutexStudentToQueue, 0, 1);
    sem_init(&mutexStudentWaitingQueue, 0, 1);
    sem_init(&mutexAmountOfStudentBeingTutored, 0, 1);
    sem_init(&mutexTotalStudentsTutored, 0, 1);

    sem_init(&coordinatorWaiting, 0, 0);
    sem_init(&studentArrived, 0, 1);
    sem_init(&receivedStudentToQueue, 0, 0);
    sem_init(&tutorWaiting, 0, 0);


    //  CREATE THREADS


    //Creates student threads
    for(i = 0; i < numberOfStudents; i++)
    {
        //  allocate student semaphore
        studentWaiting = malloc(sizeof(sem_t));
        sem_init(studentWaiting, 0, 0);

        //  add student to list of students
        studentToAdd = malloc(sizeof(struct StudentNode));
        studentToAdd->priority = 0;
        studentToAdd->studentWaiting = studentWaiting;

        assert(pthread_create(&students[i], NULL, studentThread, (void *) studentToAdd) == 0);

        addToAllStudents(studentToAdd);
    }

    //Creates tutor threads
    for(i = 0; i < numberOfTutors; i++) 
    {
        assert(pthread_create(&tutors[i], NULL, tutorThread, (void *) i) == 0);
    }

    //Create coordinator
    assert(pthread_create(&coordinator, NULL, coordinatorThread, (void *) i) == 0);


    //  JOINS THREADS


    //Join student threads
    for(i = 0; i < numberOfStudents; i++)
    {
        assert(pthread_join(students[i], &value) == 0);
    }

    //Join tutor threads
    for(i = 0; i < numberOfTutors; i++) 
    {
        assert(pthread_cancel(tutors[i]) == 0);
    }

    //Join coordinator
    assert(pthread_cancel(coordinator) == 0);
}
