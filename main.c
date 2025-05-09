#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUS_X 5
#define BUS_Y 4
#define NB_TRAJETS 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_XY = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_YX = PTHREAD_COND_INITIALIZER;

int attente_XY = 0;
int attente_YX = 0;
int dans_tunnel = 0;
char sens_tunnel = 0; // 'X' ou 'Y' ou 0 (vide)

void sleep_random() {
    int ms = (rand() % 500) + 1000; // entre 1000ms et 1500ms
    usleep(ms * 1000);
}

void entrer_tunnel(char direction, int id) {
    pthread_mutex_lock(&mutex);

    if (direction == 'X') {
        attente_XY++;
        while (sens_tunnel == 'Y' || (sens_tunnel != 0 && sens_tunnel != 'X')) {
            printf("Bus %d attend pour X -> Y\n", id);
            pthread_cond_wait(&cond_XY, &mutex);
        }
        attente_XY--;
    } else {
        attente_YX++;
        while (sens_tunnel == 'X' || (sens_tunnel != 0 && sens_tunnel != 'Y')) {
            printf("Bus %d attend pour Y -> X\n", id);
            pthread_cond_wait(&cond_YX, &mutex);
        }
        attente_YX--;
    }

    if (dans_tunnel == 0)
        sens_tunnel = direction;

    dans_tunnel++;
    printf(">>> Tunnel : %d bus dedans, sens : %c -> %c\n", dans_tunnel, direction, (direction == 'X') ? 'Y' : 'X');

    pthread_mutex_unlock(&mutex);
}

void sortir_tunnel() {
    pthread_mutex_lock(&mutex);

    dans_tunnel--;

    if (dans_tunnel == 0) {
        sens_tunnel = 0;
        printf("<<< Tunnel : 0 bus encore dedans\n");

        if (attente_YX > 0)
            pthread_cond_broadcast(&cond_YX);
        else if (attente_XY > 0)
            pthread_cond_broadcast(&cond_XY);
    }

    pthread_mutex_unlock(&mutex);
}

void *bus_thread(void *arg) {
    int id = ((int *)arg)[0];
    char origine = ((int *)arg)[1];
    char direction, retour;

    if (origine == 'X') {
        direction = 'X'; // X -> Y
        retour = 'Y';    // Y -> X
    } else {
        direction = 'Y'; // Y -> X
        retour = 'X';    // X -> Y
    }
    int i;

    for (i = 1; i <= NB_TRAJETS; i++) {
        entrer_tunnel(direction, id);
        printf("Bus %d de %c : %c -> %c (Trajet %d)\n", id, origine, direction, retour, i);
        sleep_random();
        sortir_tunnel();

        entrer_tunnel(retour, id);
        printf("Bus %d de %c : %c -> %c (Trajet %d)\n", id, origine, retour, direction, i);
        sleep_random();
        sortir_tunnel();
    }

    free(arg);
    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t threads[BUS_X + BUS_Y];

    // Créer les bus de X
    int i;
    for (i = 0; i < BUS_X; i++) {
        int *args = malloc(2 * sizeof(int));
        args[0] = i + 1;
        args[1] = 'X';
        pthread_create(&threads[i], NULL, bus_thread, args);
    }

    // Créer les bus de Y
    for (i = 0; i < BUS_Y; i++) {  // Utiliser la même variable 'i'
        int *args = malloc(2 * sizeof(int));
        args[0] = i + 1 + BUS_X;
        args[1] = 'Y';
        pthread_create(&threads[i + BUS_X], NULL, bus_thread, args);
    }

    // Attendre la fin
    for (i = 0; i < BUS_X + BUS_Y; i++) {  // Utiliser la même variable 'i'
        pthread_join(threads[i], NULL);
    }

    printf("Simulation terminée.\n");
    return 0;
}

