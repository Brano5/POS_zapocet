#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct pasazier{
    int vek;
} PASAZIER;

typedef struct lod{
    PASAZIER* pasazieri;
    int index;
    int kapacita;
    int listok;
    int potopena;
    double zisk;
    double ziskZaJedlo;
    int pocetPorcii;
    pthread_mutex_t* mutex;
} LOD;

typedef struct plavbaData{
    LOD* lod;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PLAVBADATA;

typedef struct prichodData{
    LOD* lod;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRICHODDATA;

void *plavba(void *data){
    printf("Zaciatok vlakna plavba!\n");
    int bez = 0;
    PLAVBADATA* d = (PLAVBADATA*)data;

    while (bez == 0){
        pthread_mutex_lock(d->lod->mutex);

        printf("index: %d a kapacita: %d!\n", d->lod->index, d->lod->kapacita);

        while(d->lod->index < d->lod->kapacita){
            pthread_cond_wait(d->plno, d->lod->mutex);
            printf("index: %d a kapacita: %d!\n", d->lod->index, d->lod->kapacita);
        }

        printf("Zaciatok plavby!\n");

        usleep(((rand() % 31) + 20) * 1000);

        if(rand() % 100 < 15){
            printf("Lod potopena!\n");
            printf("Zisk: %f\n", d->lod->zisk + d->lod->ziskZaJedlo);
            printf("Zisk za jedlo: %f\n", d->lod->ziskZaJedlo);
            printf("Priemer: %f\n", d->lod->ziskZaJedlo / d->lod->pocetPorcii);
            d->lod->potopena = 1;
            bez = 1;
            d->lod->index = 0;
        } else {
            printf("Koniec plavby!\n");
            while (d->lod->index > 0){
                usleep(((rand() % 2) + 1) * 1000);
                d->lod->index--;
            }

        }
        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->lod->mutex);
    }
    printf("Koniec vlakna plavba!\n");
}

void *prichod(void *data){
    printf("Zaciatok vlakna prichod!\n");
    int bez = 0;
    PRICHODDATA* d = (PRICHODDATA*)data;

    while (bez == 0){
        usleep(((rand() % 3) + 4) * 1000);

        int r = (rand() % 3) + 2;
        for (int i = 0; i < r; ++i) {
            pthread_mutex_lock(d->lod->mutex);

            if(d->lod->potopena == 1){
                pthread_mutex_unlock(d->lod->mutex);
                bez = 1;
                break;
            }

            while(d->lod->index >= d->lod->kapacita){
                pthread_cond_wait(d->prazdno, d->lod->mutex);
            }
            printf("Prichod pasaziera!\n");

            if((rand() % 100) < 35){
                // pasazieri co maju 12 a menej
                d->lod->pasazieri[d->lod->index].vek = 0;
                d->lod->zisk += d->lod->listok / 2.0;
            } else {
                // pasazieri 12 a viac
                d->lod->pasazieri[d->lod->index].vek = 1;
                d->lod->zisk += d->lod->listok;
            }
            int r = (rand() % 8) + 3;
            d->lod->ziskZaJedlo += r;
            d->lod->pocetPorcii++;

            d->lod->index++;

            pthread_cond_broadcast(d->plno);

            pthread_mutex_unlock(d->lod->mutex);
        }

    }
    printf("Koniec vlakna prichod!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Hello, World!\n");

    int kapacita = atoi(argv[1]);
    int listok = atoi(argv[2]);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    PASAZIER* pasazieri = malloc(sizeof (PASAZIER) * kapacita);

    LOD lod = {
            pasazieri,
            0,
            kapacita,
            listok,
            0,
            0,
            0,
            0,
            &mutex
    };

    PRICHODDATA prd = {
            &lod,
            &plno,
            &prazdno
    };

    PLAVBADATA pd = {
            &lod,
            &plno,
            &prazdno
    };

    pthread_t prichod_vlakno;
    pthread_t plavba_vlakno;

    pthread_create(&prichod_vlakno, NULL, &prichod, &prd);
    pthread_create(&plavba_vlakno, NULL, &plavba, &pd);



    pthread_join(prichod_vlakno, NULL);
    pthread_join(plavba_vlakno, NULL);

    free(pasazieri);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);


    return 0;
}
