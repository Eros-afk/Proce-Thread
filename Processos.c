#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Exibe o PID do processo pai ao iniciar
    printf("Processo pai (PID: %d) iniciando.\n", getpid());

    // Cria um novo processo (filho)
    pid = fork();

    // Verifica se houve erro ao criar o processo
    if (pid < 0) {
        fprintf(stderr, "Falha no fork.\n");
        return 1;
    } 
    // Código executado pelo processo filho
    else if (pid == 0) {
        printf("Processo filho (PID: %d) criado. Meu pai é (PID: %d).\n", 
               getpid(), getppid());
        sleep(2); // Simula algum trabalho por 2 segundos
        printf("Processo filho (PID: %d) terminando.\n", getpid());
        exit(0); // Encerra o processo filho
    } 
    // Código executado pelo processo pai
    else {
        printf("Processo pai (PID: %d) criou o filho (PID: %d).\n", getpid(), pid);
        wait(NULL); // Processo pai espera o filho terminar
        printf("Processo pai (PID: %d) detectou que o filho terminou. Terminando também.\n", getpid());
    }

    return 0;
}
