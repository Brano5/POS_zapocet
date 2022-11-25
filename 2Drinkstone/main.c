#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


typedef struct suradnice{
    int x;
    int y;
} SURADNICE;

typedef struct nastenka{
    SURADNICE* suradnice;
    int* pocet;
    int kapacita;
    pthread_mutex_t* mutex;
} NASTENKA;

typedef struct prospektor_data{
    NASTENKA* nastenka;
    int pocetBanikov;
    pthread_cond_t* prazdna_nastaneka;
    pthread_cond_t* nastenka_nie_je_prazdna;
} PROSPEKTOR_DATA;

typedef struct bannik_data{
    NASTENKA* nastenka;
    int id;
    pthread_cond_t* nastenka_nie_je_prazdna;
    pthread_cond_t* prazdna_nastaneka;
} BANNIK_DATA;

//propektor
void *prospektor(void *data){
    PROSPEKTOR_DATA* d = (PROSPEKTOR_DATA*)data;

    //konci, ked vygeneroval pocet_banikov*20
    for (int i = 0; i < d->pocetBanikov * 20; ++i) {
        //vygeneruje suradnice (z rozsahu x: -20, 20 a y: -50, 50)
        usleep((rand() % 2000) + 1000);

        int x = (rand() % 40) - 20;
        int y = (rand() % 100) - 50;

        pthread_mutex_lock(d->nastenka->mutex);
        // ak je nastenka plna, ide do miestneho lokalu
        while(*d->nastenka->pocet >= d->nastenka->kapacita){
            //caka, kym sa nastenka uplne nevyprazdni
            printf("Nastenka je plna indem do lokalu! \n");
            pthread_cond_wait(d->prazdna_nastaneka, d->nastenka->mutex);
        }
        printf("Pridavam suradnice %d %d na nastenku! \n", x, y);
        // zavesi ich na nastenku
        d->nastenka->suradnice[*d->nastenka->pocet].x = x;
        d->nastenka->suradnice[*d->nastenka->pocet].y = y;
        ++*d->nastenka->pocet;

        pthread_cond_broadcast(d->nastenka_nie_je_prazdna);

        pthread_mutex_unlock(d->nastenka->mutex);
    }
    printf("Koncim cinnost! \n");
}

//bannik
void *bannik(void *data) {
    BANNIK_DATA *d = (BANNIK_DATA *) data;
    int zarobok = 0;

    //vykona celkova 20 tazieb
    for (int i = 0; i < 20; ++i) {
        //zobrerie suradnice z nastenky
        pthread_mutex_lock(d->nastenka->mutex);
        //ak je nastenka prazdna, tak caka pred nou
        while(*d->nastenka->pocet <= 0){
            printf("ID: %d, cakam na suradnice pred nastenkou! \n", d->id);
            pthread_cond_wait(d->nastenka_nie_je_prazdna, d->nastenka->mutex);
        }
        SURADNICE s = d->nastenka->suradnice[*d->nastenka->pocet-1];
        --*d->nastenka->pocet;
        if(*d->nastenka->pocet == 0) {
            pthread_cond_signal(d->prazdna_nastaneka);
        }
        pthread_mutex_unlock(d->nastenka->mutex);

        //vyberie sa na miesto rychlostou 5dm/s
        int cas = (s.x == 0 && s.y == 0) ? 0 : trunc((sqrt(pow(s.x, 2)+pow(s.y, 2))*1000)/5);

        printf("ID: %d, zobral som suradnice a idem na miesto vzdielene: %d! \n", d->id, cas);
        usleep(cas);

        //nahodny pocet kusov jednej z rud
        double r = rand() / RAND_MAX;
        int typ;
        int pocet;
        //zelezo
        if (r < 0.45){
            typ = 5;
            pocet = (rand() % 10) + 1;
        }
        //zlato
        else if(r < 0.75){
            typ = 15;
            pocet = (rand() % 6) + 1;
        }
        //diamanty
        else if(r < 0.9){
            typ = 50;
            pocet = (rand() % 3) + 1;
        }
        //nic
        else{
            typ = 0;
            pocet = 0;
        }
        printf("ID: %d, vytazil som %d kusov rudy s hodnotou %d \n", d->id, pocet, typ);

        //cesta spat
        usleep(cas);

        //stretnutie s creepsterom s pravdepodobonostou 25%
        if(rand() / RAND_MAX < 0.25){
            printf("ID: %d, stretol som creepera! \n", d->id);
            //ak ano, tak strati rudu
            pocet = 0;
        }
        printf("ID: %d, zarobil som %d! \n", d->id, pocet*typ);
        zarobok += pocet * typ;
    }
    //vypis celkovy zarobok
    printf("Bannik s ID: %d ma celkovy zarobok: %d \n", d->id, zarobok);
}


int main(int argc, char**argv) {
    srand(time(NULL));

    int velkostNastenky = 5;//atoi(argv[1]);
    int pocetBanikov = 20;//atoi(argv[2]);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_t prazdna_nastenka, nastenka_nie_je_prazdna;
    pthread_cond_init(&prazdna_nastenka, NULL);
    pthread_cond_init(&nastenka_nie_je_prazdna, NULL);

    SURADNICE* suradnice = malloc(sizeof (suradnice) * velkostNastenky);
    int pocet = 0;
    NASTENKA nastenka = {
            suradnice,
            &pocet,
            velkostNastenky,
            &mutex
    };

    PROSPEKTOR_DATA pd = {
            &nastenka,
            pocetBanikov,
            &prazdna_nastenka,
            &nastenka_nie_je_prazdna
    };

    pthread_t prospektor_vlakno;
    pthread_t bannici_vlakno[pocetBanikov];
    pthread_create(&prospektor_vlakno, NULL, &prospektor, &pd);
    for (int i = 0; i < pocetBanikov; ++i) {
        BANNIK_DATA bd = {
                &nastenka,
                i,
                &nastenka_nie_je_prazdna,
                &prazdna_nastenka
        };
        pthread_create(&bannici_vlakno[i], NULL, &bannik, &bd);
    }

    pthread_join(prospektor_vlakno, NULL);
    for (int i = 0; i < pocetBanikov; ++i) {
        pthread_join(bannici_vlakno[i], NULL);
    }

    free(suradnice);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&prazdna_nastenka);
    pthread_cond_destroy(&nastenka_nie_je_prazdna);

    return 0;
}
