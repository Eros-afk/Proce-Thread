#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Função que será executada por cada thread criada
void *minhaThread(void *arg) {
    int id = *((int *)arg); // Recupera o identificador da thread
    printf("Thread %d: Olá do mundo das threads!\n", id);
    sleep(1); // Simula algum trabalho por 1 segundo
    printf("Thread %d: Terminando.\n", id);
    pthread_exit(NULL); // Encerra a thread
}

int main() {
    pthread_t thread1, thread2; // Variáveis para identificar as threads
    int id1 = 1;
    int id2 = 2;

    // Cria a primeira thread
    printf("Processo principal: Criando thread 1.\n");
    if (pthread_create(&thread1, NULL, minhaThread, (void *)&id1) != 0) {
        fprintf(stderr, "Erro ao criar thread 1.\n");
        return 1;
    }

    // Cria a segunda thread
    printf("Processo principal: Criando thread 2.\n");
    if (pthread_create(&thread2, NULL, minhaThread, (void *)&id2) != 0) {
        fprintf(stderr, "Erro ao criar thread 2.\n");
        return 1;
    }

    // Espera as duas threads terminarem antes de encerrar o programa
    printf("Processo principal: Esperando as threads terminarem.\n");
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Processo principal: Todas as threads terminaram. Encerrando.\n");

    return 0;
}