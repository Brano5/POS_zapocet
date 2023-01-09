#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct buffer{
    int* data;
    int* data2;
    int index;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct producentData{
    BUFFER* buffer;
    int n, a ,b;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} PRODUCENTDATA;

typedef struct konzumentData{
    BUFFER* buffer;
    int n;
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

        d->buffer->data[d->buffer->index] = (rand() % (d->b - d->a)) + d->a;
        d->buffer->data2[d->buffer->index] = (rand() % (d->b - d->a)) + d->a;
        d->buffer->index++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Koniec vlakna producent!\n");
}

int fact(int n){
    if(n == 1 || n == 0)
        return 1;
    return n * fact(n - 1);
}

void* konzument(void* data){
    KONZUMENTDATA* d = (KONZUMENTDATA*)data;

    printf("Zaciatok vlakna konzument!\n");

    for (int i = 0; i < d->n; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->index <= 0){
            printf("Cakanie na zaplnenie bufferu!\n");
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        printf("Vyberanie z bufferu!\n");

        int a = d->buffer->data[d->buffer->index - 1];
        int b = d->buffer->data2[d->buffer->index - 1];
        d->buffer->index--;

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);

        if(a < b){
            int c = a;
            a = b;
            b = c;
        }

        int vys = fact(a) / (fact(b) * fact(a-b));
        printf("Vysledok: %d z a: %d, b: %d\n", vys, a, b);

    }
    printf("Koniec vlakna konzument!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));

    printf("Zaciatok programu!\n");

    int n, a, b;
    if(argc == 4){
        n = atoi(argv[1]);
        a = atoi(argv[2]);
        b = atoi(argv[3]);
    } else {
        fprintf( stderr, "Malo argumentov, koneic programu!\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 20);
    int* data2 = malloc(sizeof (int) * 20);

    BUFFER buffer = {
            data,
            data2,
            0,
            20,
            &mutex
    };

    //Producent
    PRODUCENTDATA pd = {
            &buffer,
            n,
            a,
            b,
            &plno,
            &prazdno
    };

    pthread_t producent_vlakno;

    pthread_create(&producent_vlakno, NULL, &producent, &pd);

    //Konzument
    KONZUMENTDATA kd = {
            &buffer,
            n,
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
    free(data2);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);

    return 0;
}