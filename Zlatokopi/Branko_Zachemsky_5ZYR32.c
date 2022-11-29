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

typedef struct zlatokopData{
    BUFFER* buffer;
    double peniaze;
    int id;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} ZLATOKOPDATA;

typedef struct zlatnikData{
    BUFFER* buffer;
    int pocetZlatokopov;
    int zlato;
    int zarobok;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} ZLATNIKDATA;

void* zlatokop(void* data){
    ZLATOKOPDATA * d = (ZLATOKOPDATA*)data;

    printf("Zlatokop %d: Zaciatok vlakna zlatokop!\n", d->id);

    for (int i = 0; i < 7; ++i) {
        int cinnost = (rand() % 3) + 6;
        int zlato = 0;

        for (int j = 0; j < cinnost; ++j) {
            sleep(1);
            zlato += (rand() % 6) + 20;
        }

        printf("Zlatokop %d: Tazil: %d hodin a vytazil: %d g zlata!\n", d->id, cinnost, zlato);

        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("Zlatokop %d: Cakanie na uvolnenie zlatnika!\n", d->id);
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        d->buffer->data[d->buffer->index] = zlato;
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);

        int zarobok = zlato * 5;

        d->peniaze += zarobok;

        double utrata = zarobok * 0.25;
        if(utrata < 200){
            utrata = 200;
        }
        d->peniaze -= utrata;

        printf("Zlatokop %d: Predal zlato a isiel do lokalu, zarobil: %d a utratil: %.2f, celkovy zarobok: %.2f!\n", d->id, zarobok, utrata, d->peniaze);

        sleep(24 - cinnost);

        if(rand() % 100 < 10){
            printf("Zlatokop %d: Zlatokop bol okradnuty!\n", d->id);
            d->peniaze = 0;
        }
    }
    printf("Zlatokop %d: Koniec vlakna zlatokop, zarobok: %.2f!\n", d->id, d->peniaze);
}

void* zlatnik(void* data){
    ZLATNIKDATA* d = (ZLATNIKDATA*)data;

    printf("Zlatnik: Zaciatok vlakna zlatnik!\n");

    for (int i = 0; i < d->pocetZlatokopov * 7; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Zlatnik: Cakanie na prichod zlatokopa!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        int z = d->buffer->data[d->buffer->index - 1];
        d->zlato += z;
        d->buffer->index--;

        printf("Zlatnik: Prisiel zlatokop s %d g zlata!\n", z);

        while (d->zlato >= 50){
            if(d->zlato >= 150){
                d->zlato -= 150;
                d->zarobok += 1000;
                printf("Zlatnik: Vyrabam nahrdelnik!\n");
                sleep(5);
                printf("Zlatnik: Vyrabil som nahrdelnik, zostalo mi %d g zlata a celkovo som zarobil: %d!\n", d->zlato, d->zarobok);
            }
            if(d->zlato >= 100){
                d->zlato -= 100;
                d->zarobok += 550;
                printf("Zlatnik: Vyrabam prsten!\n");
                sleep(3);
                printf("Zlatnik: Vyrabil som prsten, zostalo mi %d g zlata a celkovo som zarobil: %d!\n", d->zlato, d->zarobok);
            }
            if(d->zlato >= 50){
                d->zlato -= 50;
                d->zarobok += 280;
                printf("Zlatnik: Vyrabam nausnice!\n");
                sleep(2);
                printf("Zlatnik: Vyrabil som nausnice, zostalo mi %d g zlata a celkovo som zarobil: %d!\n", d->zlato, d->zarobok);
            }
        }

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Zlatnik: Koniec vlakna zlatnik, zarobok: %d!\n", d->zarobok);
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int pocetZlatokopov = atoi(argv[1]);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 1);

    BUFFER buffer = {
            data,
            0,
            1,
            &mutex
    };

    //Zlatnik
    ZLATNIKDATA zd = {
            &buffer,
            pocetZlatokopov,
            0,
            0,
            &plno,
            &prazdno
    };

    pthread_t zlatnik_vlakno;

    pthread_create(&zlatnik_vlakno, NULL, &zlatnik, &zd);

    //Zlatokop
    pthread_t zlatokop_vlakno[pocetZlatokopov];
    ZLATOKOPDATA zld[pocetZlatokopov];

    for (int i = 0; i < pocetZlatokopov; ++i) {
        zld[i].buffer = &buffer;
        zld[i].peniaze = 0;
        zld[i].id = i;
        zld[i].plno = &plno;
        zld[i].prazdno = &prazdno;

        pthread_create(&zlatokop_vlakno[i], NULL, &zlatokop, &zld[i]);
    }


    //Spojenie vlakien
    pthread_join(zlatnik_vlakno, NULL);
    for (int i = 0; i < pocetZlatokopov; ++i) {
        pthread_join(zlatokop_vlakno[i], NULL);
    }

    //Cistenie pamate
    free(data);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}