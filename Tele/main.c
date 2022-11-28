#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct maziar{
    int* data;
    int index;
    pthread_mutex_t* mutex;
} MAZIAR;

typedef struct TinkyWinkyData{
    MAZIAR* maziar;
    pthread_cond_t* prazdno;
    pthread_cond_t* plno;
} TINKYWINKYDATA;

typedef struct LaaLaaData{
    MAZIAR* maziar;
    int pocetParnych;
    int pocetNeparnych;
    pthread_cond_t* prazdno;
    pthread_cond_t* plno;
} LAALAADATA;

void* TinkyWinky(void *data) {
    TINKYWINKYDATA* d = (TINKYWINKYDATA*)data;

    for (int i = 0; i < 1000; ++i) {
        int r = rand();

        pthread_mutex_lock(d->maziar->mutex);

        while(d->maziar->index >= 8){
            pthread_cond_wait(d->prazdno, d->maziar->mutex);
        }

        d->maziar->data[d->maziar->index] = r;
        d->maziar->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->maziar->mutex);
    }
}

void* LaaLaa(void *data) {
    LAALAADATA* d = (LAALAADATA*)data;

    for (int i = 0; i < 1000; ++i) {
        pthread_mutex_lock(d->maziar->mutex);

        while(d->maziar->index <= 0){
            pthread_cond_wait(d->plno, d->maziar->mutex);
        }

        int r = d->maziar->data[d->maziar->index-1];
        d->maziar->index--;

        pthread_cond_broadcast(d->prazdno);

        if(r % 2 == 0){
            d->pocetParnych++;
        } else {
            d->pocetNeparnych++;
        }

        pthread_mutex_unlock(d->maziar->mutex);
    }

    printf("Skore: %d : %d\n", d->pocetParnych, d->pocetNeparnych);
    if(d->pocetNeparnych > 500){
        printf("Vyhral Tinky-Winky!");
    } else {
        printf("Vyhral laalalalalalalalala!");
    }

}

int main() {
    srand(time(NULL));

    printf("Hello, World!\n");

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t prazdno;
    pthread_cond_init(&prazdno, NULL);

    pthread_cond_t plno;
    pthread_cond_init(&plno, NULL);

    int* data = malloc(sizeof (int) * 8);

    MAZIAR m = {
            data,
            0,
            &mutex
    };

    TINKYWINKYDATA TWd = {
            &m,
            &prazdno,
            &plno
    };

    LAALAADATA LLd = {
            &m,
            0,
            0,
            &prazdno,
            &plno
    };

    pthread_t TinkyWinky_vlakno;
    pthread_create(&TinkyWinky_vlakno, NULL, &TinkyWinky, &TWd);

    pthread_t LaaLaa_vlakno;
    pthread_create(&LaaLaa_vlakno, NULL, &LaaLaa, &LLd);




    pthread_join(TinkyWinky_vlakno, NULL);
    pthread_join(LaaLaa_vlakno, NULL);

    free(data);
    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&prazdno);
    pthread_cond_destroy(&plno);

    return 0;
}
