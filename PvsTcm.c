#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>

#define LINHAS 5      // Número de linhas do campo
#define COLUNAS 5     // Número de colunas do campo
#define MINAS 5       // Quantidade de minas no campo

int campo[LINHAS][COLUNAS];      // Matriz que representa o campo minado
pthread_mutex_t lock;            // Mutex para controlar o acesso ao console

// Função executada por cada thread para varrer uma linha do campo
void* varrer_linha(void* arg) {
    int linha = *(int*)arg; // Recupera o número da linha que a thread vai varrer
    for (int coluna = 0; coluna < COLUNAS; coluna++) {
        pthread_mutex_lock(&lock); // Garante que apenas uma thread escreva no console por vez
        if (campo[linha][coluna] == 1) {
            printf("Processo %d, Thread varrendo linha %d: Mina encontrada em (%d,%d)!\n", getpid(), linha, linha, coluna);
        } else {
            printf("Processo %d, Thread varrendo linha %d: Seguro em (%d,%d)\n", getpid(), linha, linha, coluna);
        }
        pthread_mutex_unlock(&lock); // Libera o console para outras threads
        usleep(100000); // Espera 0,1 segundo para simular trabalho
    }
    return NULL;
}

// Função para posicionar as minas aleatoriamente no campo
void posicionar_minas() {
    int colocadas = 0;
    while (colocadas < MINAS) {
        int l = rand() % LINHAS;
        int c = rand() % COLUNAS;
        if (campo[l][c] == 0) { // Só coloca mina se a posição estiver vazia
            campo[l][c] = 1;
            colocadas++;
        }
    }
}

int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    pthread_mutex_init(&lock, NULL); // Inicializa o mutex

    // Posiciona as minas apenas no processo pai
    posicionar_minas();

    pid_t pid = fork(); // Cria um novo processo

    if (pid < 0) {
        perror("Erro ao criar processo");
        exit(1);
    }

    if (pid == 0) { // Código executado pelo processo filho
        printf("Processo filho (pid=%d) iniciando varredura do campo...\n", getpid());
    } else { // Código executado pelo processo pai
        printf("Processo pai (pid=%d) iniciando varredura do campo...\n", getpid());
    }

    pthread_t threads[LINHAS]; // Vetor para armazenar as threads
    int linhas[LINHAS];        // Vetor para passar o número da linha para cada thread
    for (int i = 0; i < LINHAS; i++) {
        linhas[i] = i;
        // Cria uma thread para varrer cada linha do campo
        pthread_create(&threads[i], NULL, varrer_linha, &linhas[i]);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < LINHAS; i++) {
        pthread_join(threads[i], NULL);
    }

    if (pid > 0) { // Somente o processo pai executa este trecho
        wait(NULL); // Espera o processo filho terminar
        printf("Simulação finalizada.\n");
    }

    pthread_mutex_destroy(&lock); // Destroi o mutex
    return 0;
}