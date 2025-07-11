#include <stdio.h>  
#include <unistd.h>  
#include <pthread.h>
#include <stdlib.h>  
#include <time.h>  
#include <string.h>  
#include <sys/wait.h>

// Cores ANSI para o terminal  
#define RED "\033[1;31m"  
#define GREEN "\033[1;32m"  
#define YELLOW "\033[1;33m"  
#define BLUE "\033[1;34m"  
#define RESET "\033[0m"  

// Dados compartilhados (threads)  
typedef struct {  
    int sector_id;  
    int sensor_id;  
    int threat_level;  
    int row; // nova linha aleatória
} SensorData;  

pthread_mutex_t lock;  

// Mapa ASCII dos setores  
char battlefield_map[10][50]; // 10 linhas x 50 colunas  

void init_map() {  
    for (int i = 0; i < 10; i++) {  
        snprintf(battlefield_map[i], 50, "|...........................|...........................|");  
    }  
}  

void print_map() {  
    printf("\n\n=== MAPA DO CAMPO DE BATALHA ===\n");  
    for (int i = 0; i < 10; i++) {  
        printf("%s\n", battlefield_map[i]);  
    }  
    printf("=================================================\n");  
}  

// Atualiza o mapa com ameaças  
void update_map(int sector, int sensor, int threat, int row) {
    int pos;
    if (sector == 1) {
        // Setor esquerdo: posições 1, 9, 17 (por exemplo)
        pos = 1 + sensor * 8; // 1, 9, 17
    } else {
        // Setor direito: posições 26, 34, 42
        pos = 30 + sensor * 8; // 26, 34, 42
    }
    char marker = (threat > 70) ? 'X' : 'o';
    battlefield_map[row][pos] = marker;
}  

// Função da thread (análise de sensor)  
void* analyze_sensor(void* arg) {  
    SensorData* data = (SensorData*)arg;  
    pthread_mutex_lock(&lock);  

    update_map(data->sector_id, data->sensor_id, data->threat_level, data->row);
    printf("[SENSOR %d-%d] Nível de ameaça: ", data->sector_id, data->sensor_id);  
    if (data->threat_level > 70) {
        printf(RED "%d%% (ALERTA CRÍTICO!\a)" RESET, data->threat_level);  
    } else if (data->threat_level > 40) {  
        printf(YELLOW "%d%% (ATENÇÃO)" RESET, data->threat_level);  
    } else {  
        printf(GREEN "%d%% (SEGURO)" RESET, data->threat_level);  
    }  
    printf("\n");  
    fflush(stdout); // Garante que a saída seja exibida imediatamente

    pthread_mutex_unlock(&lock);  
    return NULL;  
}  

// Função do processo (análise de setor)  
void analyze_sector(int sector_id) {  
    printf(BLUE "\n[PROCESSO] Iniciando análise do Setor %d..." RESET "\n", sector_id);  
    pthread_t threads[3];  
    SensorData sensors[3];  

    for (int i = 0; i < 3; i++) {
        sensors[i].sector_id = sector_id;
        sensors[i].sensor_id = i;
        sensors[i].threat_level = rand() % 100;
        sensors[i].row = 1 + rand() % 8; // linhas de 1 a 8 (ajuste conforme seu mapa)
        pthread_create(&threads[i], NULL, analyze_sensor, &sensors[i]);
    }  

    for (int i = 0; i < 3; i++) {  
        pthread_join(threads[i], NULL);  
    }  

    print_map();  
}  

int main() {  
    srand(time(NULL));  
    pthread_mutex_init(&lock, NULL);  
    init_map();  

    printf(GREEN "=== BATTLEFIELD ANALYZER (C - Processos/Threads) ===" RESET "\n");  
    print_map();  // Mapa vazio

    pid_t pid = fork();  

    if (pid == 0) {
        srand(time(NULL) + getpid());
        printf(BLUE "\n--- Análise do Setor 1 (Processo Filho) ---\n" RESET);
        analyze_sector(1); // Esquerda
        exit(0);
    } else {
        srand(time(NULL) + getpid());
        waitpid(pid, NULL, 0);
        printf(BLUE "\n--- Análise do Setor 2 (Processo Pai) ---\n" RESET);
        analyze_sector(2); // Direita
    }  

    pthread_mutex_destroy(&lock);  
    return 0;  
}