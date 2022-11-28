#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct plantaz{
    int index;
    int kapacita;
    int nachadzaSa;
    pthread_mutex_t* mutex;
} PLANTAZ;

typedef struct sklad{
    int* data;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} SKLAD;

typedef struct plantaznikData{
    PLANTAZ* plantaz;
    SKLAD * sklad;
    int id;
    int trieda;
    int vhodna;
    pthread_cond_t* nenachadzaSa;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PLANTAZNIKDATA;

typedef struct kontrolorData{
    PLANTAZ* plantaz;
    SKLAD * sklad;
    int pocetOdovzdanychDodavok;
    int pocetVhodnych;
    int pocetNevhodnych;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONTROLORDATA;

void *plantaznik(void *data){
    PLANTAZNIKDATA * d = (PLANTAZNIKDATA *)data;
    printf("Plantaznik %d : Zaciatok vlakna plantaznik!\n", d->id);

    int vytazena = 0;
    while (vytazena == 0) {
        pthread_mutex_lock(d->plantaz->mutex);

        if(d->plantaz->index >= d->plantaz->kapacita)
            break;

        while(d->plantaz->nachadzaSa == 1){
            printf("Cakanie na uvolnenie plantaze!\n");
            pthread_cond_wait(d->nenachadzaSa, d->plantaz->mutex);
        }

        d->plantaz->nachadzaSa = 1;

        printf("Plantaznik %d : Dostavenie na plantazi!\n", d->id);
        switch (d->trieda) {
            case 1:
                usleep(4000);
                break;
            case 2:
                usleep(6000);
                break;
            case 3:
                usleep(3000);
                break;
        }

        printf("Plantaznik %d : Zbieranie %d caju!\n", d->id, d->plantaz->index + 1);
        switch (d->trieda) {
            case 1:
                usleep(1000);
                break;
            case 2:
                usleep(2000);
                break;
            case 3:
                usleep(1500);
                break;
        }
        d->plantaz->index++;

        d->plantaz->nachadzaSa = 0;

        if(d->plantaz->index >= d->plantaz->kapacita)
            vytazena = 1;

        pthread_cond_broadcast(d->nenachadzaSa);

        pthread_mutex_unlock(d->plantaz->mutex);

        printf("Plantaznik %d : Cesta naspat!\n", d->id);
        switch (d->trieda) {
            case 1:
                usleep(2000);
                break;
            case 2:
                usleep(3000);
                break;
            case 3:
                usleep(3000);
                break;
        }

        if(d->trieda == 3) {
            d->vhodna = 0;
            printf("Plantaznik %d : Vysypal som tovar!\n", d->id);
        } else {
            d->vhodna = 1;
        }

        pthread_mutex_lock(d->sklad->mutex);

        while(d->sklad->index == d->sklad->kapacita){
            pthread_cond_wait(d->prazdno, d->sklad->mutex);
        }

        printf("Plantaznik %d : Vkladam do skladu!\n", d->id);
        d->sklad->data[d->sklad->index] = d->vhodna;
        d->sklad->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->sklad->mutex);
    }
    printf("Plantaznik %d : Koniec vlakna plantaznik!\n", d->id);
}

void *kontrolor(void *data){
    KONTROLORDATA * d = (KONTROLORDATA *)data;
    printf("Kontrolor: Zaciatok vlakna kontrolor!\n");

    int koniec = 0;
    while (koniec == 0){
        pthread_mutex_lock(d->sklad->mutex);

        while(d->sklad->index == 0){
            pthread_cond_wait(d->plno, d->sklad->mutex);
        }

        printf("Kontrolor: Beriem zo skladu!\n");
        int vhodna = d->sklad->data[d->sklad->index - 1];
        d->sklad->index--;

        if(vhodna){
            printf("Kontrolor: Vhodna!\n");
            d->pocetVhodnych++;
            usleep(2000);
        } else {
            printf("Kontrolor: Nevhodna ehmeeahamehe!\n");
            d->pocetNevhodnych++;
            usleep(4000);
        }
        d->pocetOdovzdanychDodavok++;

        if(d->pocetOdovzdanychDodavok == 60)
            koniec = 1;

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->sklad->mutex);
    }

    printf("Kontrolor: Pocet vhodnych: %d, pocet nevhodnych: %d, celkovo: %d, percento: %f!\n", d->pocetVhodnych, d->pocetNevhodnych, d->pocetOdovzdanychDodavok, ((double)d->pocetVhodnych)/d->pocetOdovzdanychDodavok);

    printf("Kontrolor: Koniec vlakna kontrolor!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Hello, World!\n");

    int n;
    if(argc == 2){
        n = atoi(argv[1]);
    } else {
        n = 10;
    }

    pthread_mutex_t mutexPlantaz;
    pthread_mutex_init(&mutexPlantaz, NULL);

    pthread_mutex_t mutexSklad;
    pthread_mutex_init(&mutexSklad, NULL);

    pthread_cond_t nenachadzaSa;
    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&nenachadzaSa, NULL);
    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 5);

    PLANTAZ plantaz = {
            0,
            60,
            0,
            &mutexPlantaz
    };

    SKLAD sklad = {
            data,
            0,
            5,
            &mutexSklad
    };

    KONTROLORDATA kd = {
            &plantaz,
            &sklad,
            0,
            0,
            0,
            &plno,
            &prazdno
    };

    pthread_t plantaznik_vlakno[n];
    pthread_t kontrolor_vlakno;

    pthread_create(&kontrolor_vlakno, NULL, &kontrolor, &kd);

    for (int i = 0; i < n; ++i) {
        int trieda;
        int r = rand() % 100;
        if(r < 40){
            trieda = 1;
        } else if(r < 75){
            trieda = 2;
        }else {
            trieda = 3;
        }
        PLANTAZNIKDATA pd = {
                &plantaz,
                &sklad,
                i,
                trieda,
                0,
                &nenachadzaSa,
                &plno,
                &prazdno
        };
        pthread_create(&plantaznik_vlakno[i], NULL, &plantaznik, &pd);
    }

    pthread_join(kontrolor_vlakno, NULL);
    for (int i = 0; i < n; ++i) {
        pthread_join(plantaznik_vlakno[i], NULL);
    }

    free(data);
    pthread_mutex_destroy(&mutexPlantaz);
    pthread_mutex_destroy(&mutexSklad);
    pthread_cond_destroy(&nenachadzaSa);
    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);


    return 0;
}
