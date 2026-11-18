#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5    // Tamaño del buffer acotado (n en tus apuntes)
#define NUM_ITEMS 10     // Número de elementos que producirá/consumirá cada hilo

// Macros basadas en tus apuntes para la notación P(s) y V(s)
#define P(s) sem_wait(s)
#define V(s) sem_post(s)

// Variables compartidas del buffer circular
int buff[BUFFER_SIZE];
int front = 0;           // Indicador de lectura (en algunos apuntes llamado 'from')
int rear = 0;            // Indicador de escritura

// Semáforos
sem_t empty;             // Cuenta los espacios vacíos en el buffer
sem_t full;              // Cuenta los elementos disponibles en el buffer
sem_t mutex;             // Exclusión mutua para proteger la manipulación del buffer

// Función del Proceso Productor
void* productor(void* arg) {
    int id = *((int*) arg);

    for (int i = 0; i < NUM_ITEMS; i++) {
        int data = (id * 100) + i; // Simula "produce data"

        // Espera un espacio vacío en el almacén (P(empty))
        P(&empty);

        // Sección crítica para modificar el buffer compartido
        P(&mutex);
        buff[rear] = data;
        printf("[PRODUCTOR %d] Produjo: %d en la posición %d\n", id, data, rear);
        rear = (rear + 1) % BUFFER_SIZE;
        V(&mutex);

        // Avisa que hay un nuevo elemento disponible (V(full))
        V(&full);

        usleep(rand() % 500000); // Pausa aleatoria para simular tiempo de producción
    }

    pthread_exit(NULL);
}

// Función del Proceso Consumidor
void* consumidor(void* arg) {
    int id = *((int*) arg);

    for (int i = 0; i < NUM_ITEMS; i++) {
        int result;

        // Espera a que haya elementos disponibles en el almacén (P(full))
        P(&full);

        // Sección crítica para leer del buffer compartido
        P(&mutex);
        result = buff[front];
        printf("[CONSUMIDOR %d] Consumió: %d de la posición %d\n", id, result, front);
        front = (front + 1) % BUFFER_SIZE;
        V(&mutex);

        // Avisa que hay un espacio libre nuevo (V(empty))
        V(&empty);

        usleep(rand() % 800000); // Pausa aleatoria para simular tiempo de consumo
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t prod1, prod2, cons1, cons2;
    int id1 = 1, id2 = 2;

    // Inicialización de semáforos
    // empty se inicializa con el tamaño total del buffer (espacios libres iniciales)
    sem_init(&empty, 0, BUFFER_SIZE);
    // full se inicializa en 0 porque al inicio no hay elementos
    sem_init(&full, 0, 0);
    // mutex se inicializa en 1 para control de exclusión mutua
    sem_init(&mutex, 0, 1);

    // Creación de múltiples productores y consumidores (dos de cada uno)
    pthread_create(&prod1, NULL, productor, &id1);
    pthread_create(&prod2, NULL, productor, &id2);
    pthread_create(&cons1, NULL, consumidor, &id1);
    pthread_create(&cons2, NULL, consumidor, &id2);

    // Esperar a que todos los hilos terminen su ejecución
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    // Destrucción de semáforos
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("\n¡Simulación de Productor-Consumidor finalizada con éxito!\n");
    return EXIT_SUCCESS;
}
