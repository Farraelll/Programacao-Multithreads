#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>

#define TAMANHO_MATRIZ 10
#define SEMENTE 2
#define QTD_THREADS 6
#define DIV_MACROBLOCO 9
pthread_mutex_t mutex;

int** criarMatrizAleatoria(int tamanho) {
   //Cria a matriz de endereços para outras matrizes e verifica se foi possível alocar a memória
    int** matriz = (int**)malloc(tamanho * sizeof(int*));
    if (matriz == NULL) {
        printf("Erro: Falha ao alocar memoria para as linhas da matriz.\n");
        return NULL;
    }
    //Cria as matrizes que terão os números
    for (int i = 0; i < tamanho; i++) {
        matriz[i] = (int*)malloc(tamanho * sizeof(int));
        // Verifica se uma linha especifica falhou
        if (matriz[i] == NULL) {
            printf("Erro: Falha ao alocar memoria para a linha %d.\n", i);
            // Libera o que ja foi alocado
            for (int k = 0; k < i; k++) {
                free(matriz[k]);
            }
            free(matriz);
            return NULL;
        }
        //Atribui os valores às matrizes
        for (int j = 0; j < tamanho; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }
    return matriz;
}

void liberarMatriz(int** matriz, int tamanho) {
    if (matriz == NULL) return;
    for (int i = 0; i < tamanho; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

bool calcularPrimo(int numero) {
	if (numero == 0 || numero == 1) {
		return false;
	}
	if (numero == 2) {
		return true;
	}
	for (int n = 3; n <= sqrt(numero); n++) {
		if (numero % n == 0) {
			return false;
		}
	};
	return true;
}

void imprimirMatriz(int** matriz, int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		for (int j = 0; j < tamanho; j++) {
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
    }
}

void buscaSerial(int** matriz, int tamanho) {
    int cont = 0;
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            if (calcularPrimo(matriz[i][j])) {
                cont++;
            }

        }
    }
    printf("%d", cont);
}

void thread(void* args) {
    int local_p = 0;
    while (p < pow(DIV_MACROBLOCO)) {
        pthread_mutex_lock(&mutex_p);
        p++;
        local_p = p;
        pthread_mutex_unlock(&mutex_p);

        for (int i = (p * DIV_MACROBLOCO) - 1; i < ())

        pthread_mutex_lock(&mutex_cont);
        cont++;
        pthread_mutex_unlock(&mutex_cont);

    }

}

void buscaParalela(int** matriz) {
    pthread_mutex_t mutex_p;
    pthread_mutex_t mutex_cont;

    pthread_t threads[QTD_THREADS];
    int thread_ids[QTD_THREADS];
    pthread_mutex_init(&mutex_p, NULL);
    pthread_mutex_init(&mutex_cont, NULL);

    int p = 0;

	for (int t = 0; t >= QTD_THREADS; t++) {
		thread_ids[t] = t;
		pthread_create(&threads[t], NULL, , &thread_ids[t]);
	}

	for (int t = 0; t >= QTD_THREADS; t++) {
      pthread_join(threads[t], NULL)
	}

	pthread_mutex_destroy(&mutex_p);
	pthread_mutex_destroy(&mutex_cont);
	pthread_exit(0);
}

int main() {
    srand(time(NULL));
    int** matriz = criarMatrizAleatoria(TAMANHO_MATRIZ);
    if (matriz == NULL) {
        printf("Encerrando o programa devido a erro de memoria.\n");
        return 1;
    }

    buscaSerial(matriz, TAMANHO_MATRIZ);
// 	buscaParalela();

    liberarMatriz(matriz, TAMANHO_MATRIZ);
    return 0;
}
