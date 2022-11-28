#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <tgmath.h>

typedef struct buffer{
    int* data;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    int id;
    int nazovOvocia;
    int x;
    int k;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int k;
    int n;
    double* pole;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONZUMENTDATA;

void* producent(void* data){
    PRODUCENTDATA* d = (PRODUCENTDATA*)data;

    printf("%d: Zaciatok vlakna producent!\n", d->id);

    for (int i = 0; i < d->k; ++i) {
        usleep(((rand() % 2*d->x) + 1) * 1000);

        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("%d: Cakanie na uvolnenie bufferu!\n", d->id);
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        printf("%d: Vkladanie do bufferu!\n", d->id);

        d->buffer->data[d->buffer->index] = d->nazovOvocia;
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("%d: Koniec vlakna producent!\n", d->id);
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    printf("Zaciatok vlakna konzument!\n");

    for (int i = 0; i < d->k * d->n; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Cakanie na zaplnenie bufferu!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        printf("Vyberanie z bufferu!\n");

        int cislo = d->buffer->data[d->buffer->index - 1];
        d->buffer->index--;

        int r,x;
        if(cislo == 1){
            r = (rand() % 401) + 800;
            x = 3;
            d->pole[0] += r;
        } else if(cislo == 2){
            r = (rand() % 201) + 300;
            x = 2;
            d->pole[1] += r;
        } else if(cislo == 3){
            r = (rand() % 601) + 600;
            x = 1;
            d->pole[2] += r;
        }else{
            r = (rand() % 501) + 100;
            x = 3;
            d->pole[3] += r;
        }
        d->pole[4] += r;

        usleep(x * 1000);

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Koniec vlakna konzument!\n");
    printf("Jablka: %.2f a percent %.2f!\n",  d->pole[0] * 1000, d->pole[0] / d->pole[4] * 100);
    printf("Hrusky: %.2f a percent %.2f!\n", d->pole[1] * 1000, d->pole[1] / d->pole[4] * 100);
    printf("Slivky: %.2f a percent %.2f!\n", d->pole[2] * 1000, d->pole[2] / d->pole[4] * 100);
    printf("Broskyne: %.2f a percent %.2f!\n", d->pole[3] * 1000, d->pole[3] / d->pole[4] * 100);
    printf("Celkova hmotnost: %.2f!\n", d->pole[4] * 1000);
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int n, k;
    k = atoi(argv[1]);
    if(argc == 3){
        n = atoi(argv[2]);
    } else {
        n = 6;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 15);

    BUFFER buffer = {
            data,
            0,
            15,
            &mutex
    };

    //Producent
    pthread_t producent_vlakno[n];
    PRODUCENTDATA pd[n];

    for (int i = 0; i < n; ++i) {
        int r = rand() % 100;
        int typOvocia, x;
        if(r < 40){
            typOvocia = 1;
            x = 3;
        } else if(r < 65){
            typOvocia = 2;
            x = 2;
        } else if(r < 75){
            typOvocia = 3;
            x = 1;
        } else {
            typOvocia = 4;
            x = 3;
        };

        pd[i].buffer = &buffer;
        pd[i].id = i;
        pd[i].nazovOvocia = typOvocia;
        pd[i].x = x;
        pd[i].k = k;
        pd[i].plno = &plno;
        pd[i].prazdno = &prazdno;

        pthread_create(&producent_vlakno[i], NULL, &producent, &pd[i]);
    }

    //Konzument
    double* pole = malloc(sizeof (double) * 5);
    KONZUMENTDATA kd = {
            &buffer,
            k,
            n,
            pole,
            &plno,
            &prazdno
    };

    pthread_t konzument_vlakno;

    pthread_create(&konzument_vlakno, NULL, &konzument, &kd);

    //Spojenie vlakien
    for (int i = 0; i < n; ++i) {
        pthread_join(producent_vlakno[i], NULL);
    }

    pthread_join(konzument_vlakno, NULL);

    //Cistenie pamate
    free(data);
    free(pole);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}