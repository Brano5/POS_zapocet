#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct kava{
    int horkost;
    int kyslost;
    int mnamKava;
    pthread_mutex_t* mutex;
} KAVA;

typedef struct doktorandData{
    KAVA* kava;
    double sucetHorkosti;
    int pocetKav;
    pthread_cond_t* urob;
    pthread_cond_t* urobena;
} DOKTORANDDATA;

typedef struct kaviarenData{
    KAVA* kava;
    pthread_cond_t* urob;
    pthread_cond_t* urobena;
} KAVIARENDATA;

void *doktorand(void *data){
    printf("Zaciatok vlakna doktorand!\n");
    DOKTORANDDATA * d = (DOKTORANDDATA *)data;

    int run = 1;
    while (run == 1){
        pthread_mutex_lock(d->kava->mutex);

        while (d->kava->horkost == -1) {
            pthread_cond_wait(d->urobena, d->kava->mutex);
        }

        printf("Prisla mi kava s horkostou: %d a kyslostou: %d!\n", d->kava->horkost, d->kava->kyslost);

        if(d->kava->horkost < 20){
            printf("Fuj, takuto kavu nevypijem!\n");
        }

        d->sucetHorkosti += d->kava->horkost;
        d->pocetKav++;
        d->kava->horkost = -1;

        printf("Sucet horkosti: %f, pocetKav: %d, spokojnost: %f\n", d->sucetHorkosti, d->pocetKav, d->sucetHorkosti / d->pocetKav);

        if(d->sucetHorkosti / d->pocetKav < 40){
            printf("Prestavam chodit do tejto hnusnej kaviarne!\n");
            run = 0;
            d->kava->mnamKava = 0;
        }

        if(d->kava->horkost >= 20) {
            printf("zzzzzzzzzzzzzzzzzz!\n");
            usleep(((rand() % 4) + 2) * 1000);
        }

        pthread_cond_broadcast(d->urob);

        pthread_mutex_unlock(d->kava->mutex);
    }
    printf("Koniec vlakna doktorand!\n");
}

void *kaviaren(void *data){
    printf("Zaciatok vlakna kaviaren!\n");
    KAVIARENDATA* d = (KAVIARENDATA *)data;

    int run = 1;
    while (run == 1){
        pthread_mutex_lock(d->kava->mutex);

        while (d->kava->horkost != -1){
            pthread_cond_wait(d->urob, d->kava->mutex);
        }

        run = d->kava->mnamKava;
        if(run != 1)
            break;

        printf("Robim kavu!\n");
        usleep(1000);

        int r = (rand() % 100 + 1);
        d->kava->horkost = r;
        d->kava->kyslost = 100 - r;

        printf("Urobil som kavu s horkostou: %d a kyslostou: %d!\n", d->kava->horkost, d->kava->kyslost);

        pthread_cond_broadcast(d->urobena);

        pthread_mutex_unlock(d->kava->mutex);

        printf("Prestavka!\n");
        usleep(3000);
    }
}

int main() {
    srand(time(NULL));

    printf("Hello, World!\n");

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t urob;
    pthread_cond_t urobena;

    pthread_cond_init(&urob, NULL);
    pthread_cond_init(&urobena, NULL);

    KAVA kava = {
            -1,
            -1,
            1,
            &mutex
    };

    struct doktorandData dd = {
            &kava,
            0,
            0,
            &urob,
            &urobena
    };

    struct kaviarenData kd = {
            &kava,
            &urob,
            &urobena
    };

    pthread_t doktorand_vlakno;
    pthread_t kaviaren_vlakno;

    pthread_create(&doktorand_vlakno, NULL, &doktorand, &dd);
    pthread_create(&kaviaren_vlakno, NULL, &kaviaren, &kd);



    pthread_join(doktorand_vlakno, NULL);
    pthread_join(kaviaren_vlakno, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&urob);
    pthread_cond_destroy(&urobena);
    return 0;
}
