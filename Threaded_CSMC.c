#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#define MAX_STUDENTS 25
int numOfStudents;
int numOfTutors;
int numOfChairs;
int help;
int MAX;
sem_t waitingRoom;
int **studentIDQueue;
int **priorities;
int newStudentInQueue;
/// comment1
// comment 2
// comment 3
// comment 4
void *notifyCoordinator();
void *student()
{
  printf("bye");
  sem_wait(&waitingRoom);
  printf("bye(1)");
  //Queues student based on priority
  notifyCoordinator();
  sem_post(&waitingRoom);
  //sem_post(&helpStudent);
  //Notifies an idle tutor

}
/*
void *tutor()
{
  while (finishedTutoring == 0)
  {
     sem_wait(helpStudent)
     
  }
  sem_wait(&studentQueue);
  sem_wait(&requiredNumberOfHelpTimes);
}
*/
void *notifyCoordinator()
{
  newStudentInQueue = 1;
}
void *coordinator()
{
  if (newStudentInQueue == 1)
  {
   //printf("Hello");
    studentIDQueue[getLengthOfIDQueue()] = studentIDQueue[getLengthOfIDQueue() - 1] + 1;
    //priorities[getLengthOfIDQueue()] = atoi(1);
    printf("Student %d takes a seat", studentIDQueue[getLengthOfIDQueue()]);
  }
}
int getLengthOfIDQueue()
{
   //Gets length of path variable
    int index;
    index = 0;
    while (studentIDQueue[index] != NULL)
    {
       index++;
    }
    return index;
}

int main(int argc, char *argv[])
{
 long i;
 //Initializes threads
 pthread_t *students;
 pthread_t *tutors;
 pthread_t coordinator;
 //Casts arguments into integer and returns error if no digits are passed
 sscanf(argv[1],"%d",&numOfStudents);
 sscanf(argv[2], "%d", &numOfTutors);
 sscanf(argv[3], "%d", &numOfChairs);
 sscanf(argv[4], "%d", &help);
 //Allocates dynamically threads for students and tutors
 students = malloc(sizeof(pthread_t) * numOfStudents);
 tutors = malloc(sizeof(pthread_t) * numOfStudents);
 //Allocates dynamically student queue
 studentIDQueue = malloc(1000 * sizeof(int*));
 priorities = malloc(1000 * sizeof(int*));
 newStudentInQueue = 0;
 //Initialize semaphores
 sem_init(&waitingRoom, 0, numOfChairs);
 printf("hello");
 //sem_init(&studentQueue, 0, 0);
 //sem_init(&tutorQueue, 0, 0);
 //sem_init(&requiredNumberOfHelpTimes, 0, help);
 //Creates student threads
 for(i = 0; i < numOfStudents; i++)
 {
    assert(pthread_create(&students[i], NULL, student, (void *) i) == 0);
 }
 /*
 //Creates tutor threads
 for(i = 0; i < numOfTutors; i++) 
 {
     assert(pthread_create(&tutors[i], NULL, thread, (void *) i) == 0);
 }
 */
 pthread_create(&coordinator, NULL, thread, NULL);
 };
