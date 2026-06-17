#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>

#define LINHAS_MATRIZ 100
#define COLUNAS_MATRIZ 100
#define LINHAS_MACROBLOCOS 10
#define COLUNAS_MACROBLOCOS 10

#define SEMENTE 2
#define QTD_THREADS 6
#define DIV_MACROBLOCO 9

pthread_mutex_t mutex_p;
pthread_mutex_t mutex_cont;
int linhas_tam_mb = LINHAS_MATRIZ/LINHAS_MACROBLOCOS;
int colunas_tam_mb = COLUNAS_MATRIZ/COLUNAS_MACROBLOCOS;
int p = 0;
int contParalela = 0;
int** matriz;

int** criarMatrizAleatoria() {
   //Cria a matriz de endereços para outras matrizes e verifica se foi possível alocar a memória
    matriz = (int**)malloc(LINHAS_MATRIZ * sizeof(int*));
    if (matriz == NULL) {
        printf("Erro: Falha ao alocar memoria para as linhas da matriz.\n");
        return NULL;
    }
    //Cria as matrizes que terão os números
    for (int i = 0; i < LINHAS_MATRIZ; i++) {
        matriz[i] = (int*)malloc(COLUNAS_MATRIZ * sizeof(int));
        // Verifica se uma linha específica falhou
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
        for (int j = 0; j < COLUNAS_MATRIZ; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }
}

void liberarMatriz(int** matriz) {
    if (matriz == NULL) return;
    for (int i = 0; i < LINHAS_MATRIZ; i++) {
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

void imprimirMatriz(int** matriz) {
	for (int i = 0; i < LINHAS_MATRIZ; i++) {
		for (int j = 0; j < COLUNAS_MATRIZ; j++) {
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
    }
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
    printf("%d\n", cont);
}

void* thread(void* args) {
    int local_p = 0;

    pthread_mutex_lock(&mutex_p);
    local_p = p;
    p++;
    pthread_mutex_unlock(&mutex_p);
    while (local_p < COLUNAS_MACROBLOCOS * LINHAS_MACROBLOCOS) {
        int local_cont = 0;

        const int y_i = local_p / linhas_tam_mb * linhas_tam_mb;


        for (int i = y_i; i < y_i + linhas_tam_mb; i++) {
            const int x_i = local_p % colunas_tam_mb * COLUNAS_MACROBLOCOS;
            for (int j = x_i; j < x_i + colunas_tam_mb; j++)
            {
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



	for (int t = 0; t < QTD_THREADS; t++) {
		thread_ids[t] = t;
		pthread_create(&threads[t], NULL, thread, &thread_ids[t]);
	}

    int value_returned;

	for (int t = 0; t < QTD_THREADS; t++) {
      pthread_join(threads[t], NULL);
	}

    printf("%d\n", contParalela);

	pthread_mutex_destroy(&mutex_p);
	pthread_mutex_destroy(&mutex_cont);
}

int main() {
    srand(time(NULL));
    criarMatrizAleatoria();
    if (matriz == NULL) {
        printf("Encerrando o programa devido a erro de memoria.\n");
        return 1;
    }

    buscaSerial();
	buscaParalela();

    liberarMatriz(matriz);
    return 0;
}
