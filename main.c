#pragma once
#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>

#pragma comment(lib,"pthreadVC2.lib")

#define LINHAS_MATRIZ 20000
#define COLUNAS_MATRIZ 20000
#define LINHAS_MACROBLOCOS 200
#define COLUNAS_MACROBLOCOS 200
#define SEMENTE 2
#define QTD_THREADS 12

pthread_mutex_t mutex_p;
pthread_mutex_t mutex_cont;

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

void liberarMatriz() {
    if (matriz == NULL) exit(1);
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
    printf("Quantidade de primos: %d\n", cont);
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
    return NULL;
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

    printf("Quantidade de primos: %d\n", contParalela);

    pthread_mutex_destroy(&mutex_p);
    pthread_mutex_destroy(&mutex_cont);
}

void* threadSemMutex(void* args) {
    int local_p = 0;
    int total_macroblocos = qtd_blocos_linha * qtd_blocos_coluna;

    local_p = p;
    p++;

    while (local_p < total_macroblocos) {
        int local_cont = 0;

        const int y_i = (local_p / qtd_blocos_linha) * LINHAS_MACROBLOCOS;
        const int x_i = (local_p % qtd_blocos_linha) * COLUNAS_MACROBLOCOS;

        for (int i = y_i; i < y_i + LINHAS_MACROBLOCOS; i++) {
            for (int j = x_i; j < x_i + COLUNAS_MACROBLOCOS; j++) {
                if (calcularPrimo(matriz[i][j])) local_cont++;
            }
        }

        contParalela = contParalela + local_cont;

        local_p = p;
        p++;
    }
    pthread_exit(NULL);
    return NULL;
}

void buscaParalelaSemMutex() {
    pthread_t threads[QTD_THREADS];
    int thread_ids[QTD_THREADS];

    contParalela = 0;
    p = 0;

    for (int t = 0; t < QTD_THREADS; t++) {
        thread_ids[t] = t;
        pthread_create(&threads[t], NULL, thread, &thread_ids[t]);
    }

    for (int t = 0; t < QTD_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    printf("Quantidade de primos: %d\n", contParalela);
}

double speedup(double tempo_serial, double tempo_paralelo) {
    return tempo_serial / tempo_paralelo;
}

int menu() {
    int opcao;
    printf("Escolha a opcao de execucao:\n 1. Serial\n 2. Paralela\n 3. Paralela sem protecao\n 4. Ambos (com calculo de speedup)\n 0. Sair\nDigite sua opcao: ");
    scanf(" %d", &opcao);
	return opcao;
}

int main() {
    int opcao = menu();

    LARGE_INTEGER frequencia;
    LARGE_INTEGER inicio, fim;
    double tempo;
    if (opcao != 0) {
        printf("Criando matriz aleatoria...\n");
        srand(SEMENTE);
        matriz = criarMatrizAleatoria();
        if (matriz == NULL) {
            printf("Encerrando o programa devido a erro de memoria.\n");
            return 1;
        }
    }

    printf("Matriz de %dx%d criada.\n", LINHAS_MATRIZ, COLUNAS_MATRIZ);

    while (opcao != 0) {
        printf("Aguarde, processando a opcao %d...\n", opcao);
        switch(opcao) {
            case 1:
                //Busca Serial
                QueryPerformanceFrequency(&frequencia);
                QueryPerformanceCounter(&inicio);
                buscaSerial();
                QueryPerformanceCounter(&fim);
                tempo = (double)(fim.QuadPart - inicio.QuadPart) / frequencia.QuadPart;
                printf("Tempo de execucao Serial: %f segundos\n\n", tempo);
                break;
            case 2:
                //Busca Paralela
                printf("Tamanho dos macroblocos: %dx%d\n", LINHAS_MACROBLOCOS, COLUNAS_MACROBLOCOS);
                QueryPerformanceFrequency(&frequencia);
                QueryPerformanceCounter(&inicio);
                buscaParalela();
                QueryPerformanceCounter(&fim);
                tempo = (double)(fim.QuadPart - inicio.QuadPart) / frequencia.QuadPart;
                printf("Tempo de execucao Paralela: %f segundos\n\n", tempo);
                break;
            case 3:
                //Busca paralela sem proteção
                printf("Tamanho dos macroblocos: %dx%d\n", LINHAS_MACROBLOCOS, COLUNAS_MACROBLOCOS);
                QueryPerformanceFrequency(&frequencia);
                QueryPerformanceCounter(&inicio);
                buscaParalelaSemMutex();
                QueryPerformanceCounter(&fim);
                tempo = (double)(fim.QuadPart - inicio.QuadPart) / frequencia.QuadPart;
                printf("Tempo de execucao Paralela: %f segundos\n\n", tempo);
                break;
            case 4:
                //Busca Serial
                QueryPerformanceFrequency(&frequencia);
                QueryPerformanceCounter(&inicio);
                buscaSerial();
                QueryPerformanceCounter(&fim);
                double tempoS = (double)(fim.QuadPart - inicio.QuadPart) / frequencia.QuadPart;
                printf("Tempo de execucao Serial: %f segundos\n\n", tempoS);

                //Busca Paralela
                printf("Tamanho dos macroblocos: %dx%d\n", LINHAS_MACROBLOCOS, COLUNAS_MACROBLOCOS);
                QueryPerformanceCounter(&inicio);
                buscaParalela();
                QueryPerformanceCounter(&fim);
                double tempoP = (double)(fim.QuadPart - inicio.QuadPart) / frequencia.QuadPart;
                printf("Tempo de execucao Paralela: %f segundos\n\n", tempoP);
                break;

                //speedup
                printf("O speedup foi de %f", speedup(tempoS, tempoP));
        }
		opcao = menu();
    }
    liberarMatriz();
    return 0;
}
