#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct vyjazd{
    int x;
    int y;
    int typVyjazdu;
} VYJAZD;

typedef struct buffer{
    VYJAZD* data;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    int n;
    int k;
    int pocetVyjazdov[4];
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int k;
    int pocetNestasti;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONZUMENTDATA;

void* producent(void* data){
    PRODUCENTDATA* d = (PRODUCENTDATA*)data;

    printf("Zaciatok vlakna producent!\n");

    for (int i = 0; i < d->n*d->k; ++i) {
        usleep((rand() % 4 + 1) * 1000000);
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("Cakanie na uvolnenie bufferu!\n");
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        printf("Vkladanie do bufferu!\n");

        int r = rand() % 100;
        if(r < 45){
            d->buffer->data[d->buffer->index].typVyjazdu = 1;
            d->pocetVyjazdov[0]++;
        } else if(r < 80){
            d->buffer->data[d->buffer->index].typVyjazdu = 2;
            d->pocetVyjazdov[1]++;
        } else {
            d->buffer->data[d->buffer->index].typVyjazdu = 3;
            d->pocetVyjazdov[2]++;
        }
        d->pocetVyjazdov[3]++;
        d->buffer->data[d->buffer->index].x = (rand() % 81) - 40;
        d->buffer->data[d->buffer->index].y = (rand() % 121) - 60;
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Pocet zdravotnych vyjazdov: %d a podiel: %f!\n", d->pocetVyjazdov[0], (double)d->pocetVyjazdov[0] / d->pocetVyjazdov[3]);
    printf("Pocet hasicskych vyjazdov: %d a podiel: %f!\n", d->pocetVyjazdov[1], (double)d->pocetVyjazdov[0] / d->pocetVyjazdov[3]);
    printf("Pocet policjanych vyjazdov: %d a podiel: %f!\n", d->pocetVyjazdov[2], (double)d->pocetVyjazdov[0] / d->pocetVyjazdov[3]);
    printf("Pocet vsetkych vyjazdov %d!\n", d->pocetVyjazdov[3]);
    printf("Koniec vlakna producent!\n");
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    printf("Zaciatok vlakna konzument!\n");

    for (int i = 0; i < d->k; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Cakanie na zaplnenie bufferu!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        printf("Vyberanie z bufferu!\n");

        int x = d->buffer->data[d->buffer->index - 1].x;
        int y = d->buffer->data[d->buffer->index - 1].y;
        int typVyjazdu = d->buffer->data[d->buffer->index - 1].typVyjazdu;
        d->buffer->index--;
        d->pocetNestasti++;

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);

        usleep(abs(x) + abs(y) / 6.0 * 1000000);
        printf("Vyriesenie pripadu a cesta spat!\n");
        usleep(abs(x) + abs(y) / 6.0 * 1000000);
        if(rand() % 100 < 15){
            printf("Nahodou som narazil na dalsi pripad!\n");
            usleep(((rand() % 4) + 1) * 1000000);
            d->pocetNestasti++;
        }
    }
    printf("Pocet vyriesenych pripadov: %d!\n", d->pocetNestasti);
    printf("Koniec vlakna konzument!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int k, n;
    k = atoi(argv[1]);
    if(argc == 3){
        n = atoi(argv[2]);
    } else {
        n = 8;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    VYJAZD * data = malloc(sizeof (VYJAZD) * 10);

    BUFFER buffer = {
            data,
            0,
            10,
            &mutex
    };

    //Producent
    PRODUCENTDATA pd = {
            &buffer,
            n,
            k,
            0,
            0,
            0,
            0,
            &plno,
            &prazdno
    };

    pthread_t producent_vlakno;

    pthread_create(&producent_vlakno, NULL, &producent, &pd);

    //Konzument
    KONZUMENTDATA kd[n];

    pthread_t konzument_vlakno[n];

    for (int i = 0; i < n; ++i) {
        kd[i].buffer = &buffer;
        kd[i].k = k;
        kd[i].pocetNestasti = 0;
        kd[i].plno = &plno;
        kd[i].prazdno = &prazdno;

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