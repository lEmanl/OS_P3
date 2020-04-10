//gcc Threaded_CSMC.c -o Threaded_CSMC -lpthread
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

sem_t coordinatorWaiting;
sem_t studentArrived;
sem_t receivedStudentToQueue;
sem_t tutorWaiting;

int numberOfChairs;
int totalStudentsTutored;
int amountOfStudentsBeingTutored;
pthread_t studentToQueue;
struct StudentNode *currentStudent;
sem_t mutexTotalStudentsTutored;


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
    if(studentWaitingToQueue != NULL && studentWaitingQueueHead == NULL) {
        studentWaitingQueueHead = studentWaitingToQueue;
        studentWaitingQueueHead->next = NULL;
	traversalStudentWaiting = studentWaitingQueueHead;
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
    sem_t * studentWaiting = (sem_t *) arg;
    pthread_t studentThreadId = pthread_self();

    //  LOCK to try and enter waiting room
    sem_wait(&mutexChairs);
    if (numberOfChairs < 0)
    {
        printf("St: Student %ul found no empty chairs. Will try again later.\n", studentThreadId);
        sem_post(&mutexChairs);
        return; 
    }
    numberOfChairs = numberOfChairs - 1;
    printf("St: Student %ul takes a seat. Empty chairs = %d\n", studentThreadId, numberOfChairs);
    sem_post(&mutexChairs);


    //  LOCK on sharing student arrival
    sem_wait(&studentArrived);
    //  LOCK on student to queue
    sem_wait(&mutexStudentToQueue);
    printf("Student: setting student to queue\n");
    studentToQueue = studentThreadId;
    sem_post(&mutexStudentToQueue);
    //  NOTIFIES coordinator that student arrived
    sem_post(&coordinatorWaiting);
    //  WAITING for coordinator to queue student
    sem_wait(&receivedStudentToQueue);
    sem_post(&studentArrived);

    //  Wait for tutor
    printf("Student: waiting for tutor\n");
    sem_wait(studentWaiting);
    printf("Student: getting tutored\n");
    
    //  LOCK on waiting room chairs
    sem_wait(&mutexChairs);
    numberOfChairs = numberOfChairs + 1;
    sem_post(&mutexChairs);

    // Student is getting tutored
    sleep(.2); 
}


//  COORDINATOR THREAD
void *coordinatorThread()
{
    pthread_t nextStudentToQueue;
    struct StudentNode * nextStudentNode;

    int count = 10;
    int counter = 0;
    
    while(counter < count) {
        //  WAITING for student to arrive
        printf("Coordinator: waiting for student to arrive\n");
        sem_wait(&coordinatorWaiting);
        //  LOCK on the student to queue
        sem_wait(&mutexStudentToQueue);
        printf("Coordinator: received student to queue\n");
        nextStudentToQueue = studentToQueue;
	
        sem_post(&mutexStudentToQueue);

        //  NOTIFIES student that they were received
        sem_post(&receivedStudentToQueue);

        nextStudentNode = findInAllStudents(nextStudentToQueue);

        //  LOCK on the queue of students
        sem_wait(&mutexStudentWaitingQueue);
        enqueueToStudentWaitingQueue(nextStudentNode);
        printf("Co: Student %ul with priority %d in the queue. Waiting students now = %d. Total requests = %d\n", nextStudentToQueue, nextStudentNode->priority, 0, 0);
        sem_post(&mutexStudentWaitingQueue);
        
        //  NOTIFIES tutors that there is another student to tutor
        printf("Coordinator: notifying tutor\n");
        sem_post(&tutorWaiting);

        counter = counter + 1;
        

    }
}


//  TUTOR THREAD
void *tutorThread()
{
    pthread_t tutorThreadId = pthread_self();
    struct StudentNode * studentNode;

    //  WAITING for student to tutor
    printf("Tutor: waiting for coordinator\n");
    sem_wait(&tutorWaiting);

    //  LOCK on the queue of students
    sem_wait(&mutexStudentWaitingQueue);
    printf("Tutor: dequeueing student\n");
    studentNode = dequeueFromStudentWaitingQueue();
    sem_post(&mutexStudentWaitingQueue);

    //  set the tutor for the student
    studentNode->tutorThreadId = tutorThreadId;

    //  Tutor student
    sem_post(studentNode->studentWaiting);
    tutor(studentNode);
    sem_post(studentNode->studentWaiting);
}


//Student getting tutored
void tutor(struct StudentNode * studentNode)
{
    //  LOCK amount of students getting tutored
    sem_wait(&mutexAmountOfStudentBeingTutored);
    amountOfStudentsBeingTutored = amountOfStudentsBeingTutored + 1;
    sem_post(&mutexAmountOfStudentBeingTutored);

    printf("Tu: Student %ul tutored by Tutor %ul. Students tutored now = %d. Total session tutored%d.\n", studentNode->threadId, studentNode->tutorThreadId, amountOfStudentsBeingTutored, totalStudentsTutored);

    sleep(.2);  

    //  LOCK amount of students getting tutored
    sem_wait(&mutexAmountOfStudentBeingTutored);
    amountOfStudentsBeingTutored = amountOfStudentsBeingTutored - 1;
    sem_post(&mutexAmountOfStudentBeingTutored);

    //  LOCK chaning total students tutored
    sem_wait(&mutexTotalStudentsTutored);
    totalStudentsTutored = totalStudentsTutored + 1;
    sem_post(&mutexTotalStudentsTutored);

    studentNode->priority = studentNode->priority + 1;
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
    int numberOfHelp;
    void * value;
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

        assert(pthread_create(&students[i], NULL, studentThread, (void *) studentWaiting) == 0);

        //  add student to list of students
        studentToAdd = malloc(sizeof(struct StudentNode));
        studentToAdd->priority = 0;
        studentToAdd->studentWaiting = studentWaiting;
        //studentToAdd->threadId = students[i];
	//Gets id from current thread executing
        studentToAdd->threadId = pthread_self();

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
        assert(pthread_join(tutors[i], &value) == 0);
    }

    //Join coordinator
    assert(pthread_join(coordinator, &value) == 0);
}
