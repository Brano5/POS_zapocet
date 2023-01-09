#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct suradnice{
    int x;
    int y;
} SURADNICE;

typedef struct buffer{
    SURADNICE *data;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    int n;
    int r;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int n;
    int r;
    double odhad;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} KONZUMENTDATA;

void* producent(void* data){
    PRODUCENTDATA* d = (PRODUCENTDATA*)data;

    printf("Zaciatok vlakna producent!\n");

    for (int i = 0; i < d->n; ++i) {
        int x = (rand() % (2 * d->r));
        int y = (rand() % (2 * d->r));

        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index >= d->buffer->kapacita){
            printf("Cakanie na uvolnenie bufferu!\n");
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        printf("Vkladanie do bufferu!\n");

        d->buffer->data[d->buffer->index].x = x;
        d->buffer->data[d->buffer->index].y = y;
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Koniec vlakna producent!\n");
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    int nach = 0;
    int nenach = 0;

    printf("Zaciatok vlakna konzument!\n");

    for (int i = 0; i < d->n; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Cakanie na zaplnenie bufferu!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        printf("Vyberanie z bufferu!\n");

        int x = d->buffer->data[d->buffer->index - 1].x;
        int y = d->buffer->data[d->buffer->index - 1].y;
        d->buffer->index--;

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);

        double v = ((d->r - x) * (d->r - x)) + ((d->r - y) * (d->r - y)) * 1.0;
        if(sqrt(v) < d->r){
            nach++;
            printf("Suradnice %d a %d, sa nachadzaju v kruhu!\n", x, y);
        } else {
            nenach++;
            printf("Suradnice %d a %d, sa nenachadzaju v kruhu!\n", x, y);
        }
    }
    d->odhad = 4 * (nenach * 1.0 / nach);
    printf("Koniec vlakna konzument!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int r, n;
    if(argc == 3){
        r = atoi(argv[1]);
        n = atoi(argv[2]);
        if(r <= 0 || n <= 0){
            fprintf(stderr, "Argumenty musia byt kladne cislo!\n");
            return 0;
        }
    } else {
        fprintf(stderr, "Zadal si malo parametrov!\n");
        return 0;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (SURADNICE) * 20);

    BUFFER buffer = {
            data,
            0,
            20,
            &mutex
    };

    //Producent
    PRODUCENTDATA pd = {
            &buffer,
            n,
            r,
            &plno,
            &prazdno
    };

    pthread_t producent_vlakno;

    pthread_create(&producent_vlakno, NULL, &producent, &pd);

    //Konzument
    KONZUMENTDATA kd = {
            &buffer,
            n,
            r,
            0.0,
            &plno,
            &prazdno
    };

    pthread_t konzument_vlakno;

    pthread_create(&konzument_vlakno, NULL, &konzument, &kd);

    //Spojenie vlakien
    pthread_join(producent_vlakno, NULL);
    pthread_join(konzument_vlakno, NULL);

    printf("Odhad cisla pi je: %f!\n", kd.odhad);

    //Cistenie pamate
    free(data);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}