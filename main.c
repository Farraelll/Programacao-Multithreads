#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>

#define LINHAS_MATRIZ 100
#define COLUNAS_MATRIZ 100
#define LINHAS_MACROBLOCOS 10
#define COLUNAS_MACROBLOCOS 10

#define SEMENTE 2
#define QTD_THREADS 6

pthread_mutex_t mutex_p;
pthread_mutex_t mutex_cont;

// Ajustado para mapear corretamente linhas e colunas
int qtd_blocos_linha = COLUNAS_MATRIZ / COLUNAS_MACROBLOCOS;
int qtd_blocos_coluna = LINHAS_MATRIZ / LINHAS_MACROBLOCOS;

int p = 0;
int contParalela = 0;
int** matriz;

int** criarMatrizAleatoria() {
    int** m = (int**)malloc(LINHAS_MATRIZ * sizeof(int*));
    if (m == NULL) {
        printf("Erro: Falha ao alocar memoria para as linhas da matriz.\n");
        return NULL;
    }

    for (int i = 0; i < LINHAS_MATRIZ; i++) {
        m[i] = (int*)malloc(COLUNAS_MATRIZ * sizeof(int));
        if (m[i] == NULL) {
            printf("Erro: Falha ao alocar memoria para a linha %d.\n", i);
            for (int k = 0; k < i; k++) {
                free(m[k]);
            }
            free(m);
            return NULL;
        }
        for (int j = 0; j < COLUNAS_MATRIZ; j++) {
            m[i][j] = rand() % 32000;
        }
    }
    return m;
}

void liberarMatriz(int** matriz) {
    if (matriz == NULL) return;
    for (int i = 0; i < LINHAS_MATRIZ; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

bool calcularPrimo(int numero) {
    if (numero == 0 || numero == 1) return false;
    if (numero == 2) return true;
    for (int n = 3; n <= sqrt(numero); n++) {
       if (numero % n == 0) return false;
    }
    return true;
}

void buscaSerial() {
    int cont = 0;
    for (int i = 0; i < LINHAS_MATRIZ; i++) {
        for (int j = 0; j < COLUNAS_MATRIZ; j++) {
            if (calcularPrimo(matriz[i][j])) {
                cont++;
            }
        }
    }
    printf("Serial: %d\n", cont);
}

void* thread(void* args) {
    int local_p = 0;
    int total_macroblocos = qtd_blocos_linha * qtd_blocos_coluna;

    pthread_mutex_lock(&mutex_p);
    local_p = p;
    p++;
    pthread_mutex_unlock(&mutex_p);

    while (local_p < total_macroblocos) {
        int local_cont = 0;

        // Fórmulas corrigidas para conversão de ID sequencial para coordenadas 2D
        const int y_i = (local_p / qtd_blocos_linha) * LINHAS_MACROBLOCOS;
        const int x_i = (local_p % qtd_blocos_linha) * COLUNAS_MACROBLOCOS;

        for (int i = y_i; i < y_i + LINHAS_MACROBLOCOS; i++) {
            for (int j = x_i; j < x_i + COLUNAS_MACROBLOCOS; j++) {
                if (calcularPrimo(matriz[i][j])) local_cont++;
            }
        }

        pthread_mutex_lock(&mutex_cont);
        contParalela = contParalela + local_cont;
        pthread_mutex_unlock(&mutex_cont);

        pthread_mutex_lock(&mutex_p);
        local_p = p;
        p++;
        pthread_mutex_unlock(&mutex_p);
    }
    pthread_exit(NULL);
}

void buscaParalela() {
    pthread_t threads[QTD_THREADS];
    int thread_ids[QTD_THREADS];

    pthread_mutex_init(&mutex_p, NULL);
    pthread_mutex_init(&mutex_cont, NULL);
    contParalela = 0;
    p = 0;

    for (int t = 0; t < QTD_THREADS; t++) {
       thread_ids[t] = t;
       pthread_create(&threads[t], NULL, thread, &thread_ids[t]);
    }

    for (int t = 0; t < QTD_THREADS; t++) {
      pthread_join(threads[t], NULL);
    }

    printf("Paralela: %d\n", contParalela);

    pthread_mutex_destroy(&mutex_p);
    pthread_mutex_destroy(&mutex_cont);
}

int main() {
    srand(SEMENTE); // Ajustado para usar apenas a semente fixa
    matriz = criarMatrizAleatoria();
    if (matriz == NULL) {
        printf("Encerrando o programa devido a erro de memoria.\n");
        return 1;
    }

    buscaSerial();
    buscaParalela();

    liberarMatriz(matriz);
    return 0;
}