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
};



// GLOBAL VARIABLES
struct StudentNode * allStudentsHead;
struct StudentWaiting * studentWaitingQueueHead;
struct StudentNode * allStudentsHead;


//  ADD TO ALL STUDENTS
void addToAllStudents(struct StudentNode * studentToAdd) {
    //  if list is not empty, set new node next to head for insertion at front
    if(allStudentsHead != NULL) {
        studentToAdd->next = allStudentsHead;
    }
        
    allStudentsHead = studentToAdd;
}


//  FIND IN ALL STUDENTS WITH ID
struct StudentNode * findInAllStudents(int threadId) {
    struct StudentNode * traversalStudentNode = allStudentsHead;

    while(traversalStudentNode != NULL) {
        if(traversalStudentNode->threadId == threadId) {
            break;
        }
        traversalStudentNode = traversalStudentNode->next;
    }

    return traversalStudentNode;
}

//  PRINTS ALL STUDENTS
void printAllStudents() {
    struct StudentNode * traversalStudentNode = allStudentsHead;

    while(traversalStudentNode != NULL) {
        printf("Student thread ID: %d\n", traversalStudentNode->threadId);
        traversalStudentNode = traversalStudentNode->next;
    }
}




//  ENQUEUE STUDENT WAITING QUEUE
void enqueueToStudentWaitingQueue(struct StudentWaiting * studentWaitingToQueue) {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;
    struct StudentWaiting * previousTraversalStudentWaiting = NULL;
    
    //  if the queue is empty
    if(studentWaitingQueueHead == NULL) {
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
        printf("queue empty\n");
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

//  PRINTS THE STUDENT WAITING QUEUE
void printStudentWaitingQueue() {
    struct StudentWaiting * traversalStudentWaiting = studentWaitingQueueHead;

    while(traversalStudentWaiting != NULL) {
        printf("priority: %d\n", traversalStudentWaiting->priority);
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
    s4.priority = 1;

    struct StudentNode sn1;
    sn1.threadId = 1234;
    sn1.priority = 10;
    struct StudentNode sn2;
    sn2.threadId = 15432;
    sn2.priority = 1;
    struct StudentNode sn3;
    sn3.threadId = 1763;
    sn3.priority = 30;

    //  TESTS FOR QUEUE

    printf("\n\nSTUDENT QUEUE\n\n");
    enqueueToStudentWaitingQueue(&s3);
    enqueueToStudentWaitingQueue(&s1);
    enqueueToStudentWaitingQueue(&s2);
    printStudentWaitingQueue();

    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);
    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);

    enqueueToStudentWaitingQueue(&s4);
    printStudentWaitingQueue();

    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);
    printStudentWaitingQueue();
    printf("Dequeued student: %d\n", dequeueFromStudentWaitingQueue()->priority);
    printStudentWaitingQueue();
    dequeueFromStudentWaitingQueue();

    //  TESTS FOR ALL STUDENTS

    printf("\n\nALL STUDENTS\n\n");
    addToAllStudents(&sn1);
    addToAllStudents(&sn2);
    addToAllStudents(&sn3);
    printAllStudents();
    printf("Looking for thread id: %d\n", sn1.threadId);
    printf("Found: %d\n", findInAllStudents(sn1.threadId)->threadId);
    printf("Looking for thread id: %d\n", sn2.threadId);
    printf("Found: %d\n", findInAllStudents(sn2.threadId)->threadId);
    printf("Looking for thread id: %d\n", sn3.threadId);
    printf("Found: %d\n", findInAllStudents(sn3.threadId)->threadId);
}