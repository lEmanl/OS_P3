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
    struct StudentWaiting * next;
    struct StudentWaiting * previous;
};



// GLOBAL VARIABLES
struct StudentNode * allStudentsHead;
struct StudentWaiting * studentWaitingQueueHead;



//  ENQUEUE STUDENT WAITING QUEUE
void enqueueToStudentWaitingQueue(struct StudentWaiting * studentWaitingToQueue) {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;
    struct StudentWaiting * previousTraversalStudentWaiting = NULL;
    
    //  if the queue is empty
    if(studentWaitingQueueHead == NULL) {
        printf("Q is empty\n");
        studentWaitingToQueue->next = NULL;
        studentWaitingQueueHead = studentWaitingToQueue;

    //  if the queue is not empty
    } else {
        //  find the node to insert before
        while(traversalStudentWaiting != NULL) {
            //  if the node had less than or equal to priority
            if(traversalStudentWaiting->priority > studentWaitingToQueue->priority) {
                previousTraversalStudentWaiting = traversalStudentWaiting;
                traversalStudentWaiting = traversalStudentWaiting->next;
            } else {
                break;
            }
        }

        //  if the previous student is NULL, then insert before the head
        if(previousTraversalStudentWaiting == NULL) {
            printf("Q insert before head\n");
            studentWaitingToQueue->next = studentWaitingQueueHead;
            studentWaitingQueueHead = studentWaitingToQueue;        
        //  insert between the traversal and previous node
        } else {
            printf("Q insert between traversal and previous\n");
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
        printf("queue empty\n");
    //  if the head is the only item in the list
    } else if(studentWaitingQueueHead->next == NULL) {
        printf("head is the only item\n");
        dequeuedStudent = studentWaitingQueueHead;
        studentWaitingQueueHead = NULL;
    //  if there are only 2 items in the list
    } else if(studentWaitingQueueHead->next->next == NULL) {
        printf("there are only 2 items\n");
        dequeuedStudent = studentWaitingQueueHead->next;
        studentWaitingQueueHead->next = NULL;
    //  if the queue is not empty
    } else {
        while(traversalStudentWaiting->next->next != NULL) {
            traversalStudentWaiting = traversalStudentWaiting->next;
        }

        printf("queue is not empty\n");
        dequeuedStudent = traversalStudentWaiting->next;
        traversalStudentWaiting->next = NULL;
    }

    return dequeuedStudent;
}

//  PRINTS THE STUDENT WAITING QUEUE
void printStudentWaitingQueue() {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;

    while(traversalStudentWaiting != NULL) {
        printf("Priority: %d\n", traversalStudentWaiting->priority);
        traversalStudentWaiting = traversalStudentWaiting->next;
    }
}

//  MAIN PROGRAM
int main(int argc, char *argv[])
{
    struct StudentWaiting s1;
    s1.priority = 3;
    struct StudentWaiting s2;
    s2.priority = 4;
    struct StudentWaiting s3;
    s3.priority = 8;
    struct StudentWaiting s4;
    s3.priority = 1;

    enqueueToStudentWaitingQueue(&s3);
    enqueueToStudentWaitingQueue(&s1);
    enqueueToStudentWaitingQueue(&s2);

    printStudentWaitingQueue();

    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);

    printStudentWaitingQueue();

    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);

    // enqueueToStudentWaitingQueue(&s4);

    // printStudentWaitingQueue();


    // printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);

    // printStudentWaitingQueue();

    // printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);

    // printStudentWaitingQueue();

    // dequeueFromStudentWaitingQueue();
}