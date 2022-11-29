#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct buffer{
    double* data;
    int index;
    int kapacita;
    int behProgramu;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    double prijem;
    double zlatky;
    int pocetTovarov;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int pocetMagSlov;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONZUMENTDATA;

void* producent(void* data){
    PRODUCENTDATA* d = (PRODUCENTDATA*)data;

    printf("Zaciatok vlakna producent %f!\n", d->prijem);

    while (d->buffer->behProgramu == 1) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("Cakanie na uvolnenie bufferu!\n");
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        //usleep(3000);

        double hodnotaTovaru = (rand() % 3781 + 420) / 100.0;

        if(hodnotaTovaru > d->prijem){
            d->buffer->behProgramu = 0;
        } else {
            d->prijem -= hodnotaTovaru;

            d->buffer->data[d->buffer->index] = hodnotaTovaru;
            d->buffer->index++;
            d->pocetTovarov++;

            if(hodnotaTovaru > 12){
                d->zlatky += hodnotaTovaru * 0.05;
            }

            printf("Objednal som si tovar v hodnote: %f!\n", hodnotaTovaru);
        }

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Pocet obj. tovarov: %d!\n", d->pocetTovarov);
    printf("Pocet zlatiek: %f!\n", d->zlatky);
    printf("Koniec vlakna producent!\n");
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    printf("Zaciatok vlakna konzument!\n");

    while (d->buffer->behProgramu == 1 || d->buffer->index != 0) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Cakanie na zaplnenie bufferu!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        printf("Vyberanie z bufferu!\n");


        if(rand() % 100 < 75){
            if(rand() % 100 < 45){
                printf("Spomenul som si na zazracne slovisko!\n");
                d->pocetMagSlov++;
            } else {
                printf("Cakam kym prestane svietit slnko!\n");
                usleep(1000);
            }
        }

        double cislo = d->buffer->data[d->buffer->index - 1];
        d->buffer->index--;

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Pocet mag. slov: %d!\n", d->pocetMagSlov);
    printf("Koniec vlakna konzument!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int kapacitaR = rand() % 11 + 10;
    int* data = malloc(sizeof (int) * kapacitaR);

    BUFFER buffer = {
            data,
            0,
            kapacitaR,
            1,
            &mutex
    };

    //Producent
    PRODUCENTDATA pd = {
            &buffer,
            rand() % 5700 + 1800,
            0,
            0,
            &plno,
            &prazdno
    };

    pthread_t producent_vlakno;

    pthread_create(&producent_vlakno, NULL, &producent, &pd);

    //Konzument
    KONZUMENTDATA kd = {
            &buffer,
            0,
            &plno,
            &prazdno
    };

    pthread_t konzument_vlakno;

    pthread_create(&konzument_vlakno, NULL, &konzument, &kd);

    //Spojenie vlakien
    pthread_join(producent_vlakno, NULL);
    pthread_join(konzument_vlakno, NULL);

    //Cistenie pamate
    free(data);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}