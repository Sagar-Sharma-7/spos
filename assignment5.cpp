#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

pthread_mutex_t mutexLock;
sem_t emptySlots;  // counts empty slots
sem_t fullSlots;   // counts filled slots

void* producer(void* arg) {
    int item;
    for (int i = 1; i <= 10; i++) {
        item = i;

        sem_wait(&emptySlots);           // wait if buffer is full
        pthread_mutex_lock(&mutexLock);  // enter critical section

        buffer[in] = item;
        cout << "Producer produced item " << item << endl;
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutexLock); // leave critical section
        sem_post(&fullSlots);             // increment full slots

        sleep(1);
    }
    pthread_exit(NULL);
return NULL;
}

void* consumer(void* arg) {
    int item;
    for (int i = 1; i <= 10; i++) {
        sem_wait(&fullSlots);            // wait if buffer empty
        pthread_mutex_lock(&mutexLock);  // enter critical section

        item = buffer[out];
        cout << "Consumer consumed item " << item << endl;
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutexLock); // leave critical section
        sem_post(&emptySlots);            // increment empty slots

        sleep(1);
    }
    pthread_exit(NULL);
return NULL;
}

int main() {
    pthread_t prod, cons;

    pthread_mutex_init(&mutexLock, NULL);
    sem_init(&emptySlots, 0, BUFFER_SIZE); // all slots empty
    sem_init(&fullSlots, 0, 0);            // no item initially

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    pthread_mutex_destroy(&mutexLock);
    sem_destroy(&emptySlots);
    sem_destroy(&fullSlots);

    cout << "\nAll items produced and consumed successfully.\n";
    return 0;
}
