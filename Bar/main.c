#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct buffer{
    int* data;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    int n;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int id;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONZUMENTDATA;

void* producent(void* data){
    PRODUCENTDATA* d = (PRODUCENTDATA*)data;

    printf("Zaciatok vlakna producent!\n");

    for (int i = 0; i < d->n; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("Cakanie na uvolnenie bufferu!\n");
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        printf("Vkladanie do bufferu!\n");

        if(rand() % 2 == 0){
            usleep(2000);
            d->buffer->data[d->buffer->index] = 1;
        } else {
            usleep(1000);
            d->buffer->data[d->buffer->index] = 2;
        }
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Koniec vlakna producent!\n");
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    printf("Zaciatok vlakna konzument!\n");

    usleep((rand() % 8) + 1);

    pthread_mutex_lock(d->buffer->mutex);

    while(d->buffer->index <= 0){
        printf("Cakanie na zaplnenie bufferu!\n");
        pthread_cond_wait(d->plno, d->buffer->mutex);
    }

    printf("Vyberanie z bufferu!\n");

    int cislo = d->buffer->data[d->buffer->index - 1];
    d->buffer->index--;

    pthread_cond_broadcast(d->prazdno);

    pthread_mutex_unlock(d->buffer->mutex);

    printf("Koniec vlakna konzument!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int n;
    if(argc == 2){
        n = atoi(argv[1]);
    } else {
        n = 10;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 5);

    BUFFER buffer = {
            data,
            0,
            1,
            &mutex
    };

    //Producent
    PRODUCENTDATA pd = {
            &buffer,
            n,
            &plno,
            &prazdno
    };

    pthread_t producent_vlakno;

    pthread_create(&producent_vlakno, NULL, &producent, &pd);

    //Konzument


    pthread_t konzument_vlakno[n];

    for (int i = 0; i < n; ++i) {
        KONZUMENTDATA kd = {
                &buffer,
                i,
                &plno,
                &prazdno
        };
        pthread_create(&konzument_vlakno[i], NULL, &konzument, &kd);
    }


    //Spojenie vlakien
    pthread_join(producent_vlakno, NULL);
    for (int i = 0; i < n; ++i) {
        pthread_join(konzument_vlakno[i], NULL);
    }

    //Cistenie pamate
    free(data);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}
