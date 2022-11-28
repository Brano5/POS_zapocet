#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct buffer{
    int* data;
    int pocet;
    int kapacita;
    pthread_mutex_t* mutex;
} BUFFER;

typedef struct hlavneData{
    BUFFER* buffer;
    int n;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} HLAVNEDATA;

typedef struct vedlajsieData{
    BUFFER* buffer;
    int n;
    int a;
    int b;
    pthread_cond_t* plno;
    pthread_cond_t* prazdno;
} VEDLAJSIEDATA;

void* hlavne(void* data){
    printf("Zaciatok hlavneho vlakna!\n");
    HLAVNEDATA* d = (HLAVNEDATA*)data;

    for (int i = 0; i < d->n; ++i) {
        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->pocet <= 0){
            pthread_cond_wait(d->plno, d->buffer->mutex);
        }

        int r = d->buffer->data[d->buffer->pocet - 1];
        d->buffer->pocet--;

        printf("Citam cislo: %d!\n", r);

        pthread_cond_broadcast(d->prazdno);

        pthread_mutex_unlock(d->buffer->mutex);

        printf("Prvociselny rozklad cisla: %d! \t", r);

        int i = 2;
        while (r > 1)
        {
            while(r % i == 0){
                printf("%d ", i);
                r = r / i;
            }

            i++;
        }
        printf("\n");
    }
    printf("Koniec hlavneho vlakna!\n");
}

void* vedlajsie(void* data){
    printf("Zaciatok vedlajsieho vlakna!\n");
    VEDLAJSIEDATA* d = (VEDLAJSIEDATA*)data;

    for (int i = 0; i < d->n; ++i) {
        int r = (rand() % (d->b - d->a)) + d->a;

        pthread_mutex_lock(d->buffer->mutex);

        while(d->buffer->pocet >= d->buffer->kapacita){
            pthread_cond_wait(d->prazdno, d->buffer->mutex);
        }

        printf("Vkladam cislo: %d!\n", r);

        d->buffer->data[d->buffer->pocet] = r;
        d->buffer->pocet++;

        pthread_cond_broadcast(d->plno);

        pthread_mutex_unlock(d->buffer->mutex);
    }
    printf("Koniec vedlajsieho vlakna!\n");
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    printf("Hello, World!\n");

    if(argc != 4){
        printf("Nespravny pocet argumentov! Správny pocet: 4, tvoj pocet: %d\n", argc);
        return 0;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int n = atoi(argv[3]);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t plno;
    pthread_cond_t prazdno;

    pthread_cond_init(&plno, NULL);
    pthread_cond_init(&prazdno, NULL);

    int* data = malloc(sizeof (int) * 10);

    BUFFER buffer = {
            data,
            0,
            10,
            &mutex
    };

    HLAVNEDATA hd = {
            &buffer,
            n,
            &plno,
            &prazdno
    };

    VEDLAJSIEDATA vd = {
            &buffer,
            n,
            a,
            b,
            &plno,
            &prazdno
    };

    pthread_t hlavne_vlakno;
    pthread_t vedlajsie_vlakno;

    pthread_create(&hlavne_vlakno, NULL, &hlavne, &hd);
    pthread_create(&vedlajsie_vlakno, NULL, &vedlajsie, &vd);



    pthread_join(hlavne_vlakno, NULL);
    pthread_join(vedlajsie_vlakno, NULL);

    free(data);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&plno);
    pthread_cond_destroy(&prazdno);


    return 0;
}
