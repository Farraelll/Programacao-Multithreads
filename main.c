#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>

#define TAMANHO_MATRIZ 10
#define SEMENTE 2

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

int main() {
    srand(time(NULL));
    int** matriz = criarMatrizAleatoria(TAMANHO_MATRIZ);
    if (matriz == NULL) {
        printf("Encerrando o programa devido a erro de memoria.\n");
        return 1;
    }

	imprimirMatriz(matriz, TAMANHO_MATRIZ); // Código aqui

    liberarMatriz(matriz, TAMANHO_MATRIZ);
    return 0;
}
