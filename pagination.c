#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSES 10

typedef struct {
    int process_id;
    int process_size;
    int *page_table;
    int num_pages;
} Process;

typedef struct {
    int *memory;
    int size;
    int page_size;
    bool *free_frames;
    int num_frames;
} PhysicalMemory;

typedef struct {
    Process processes[MAX_PROCESSES];
    int num_processes;
    int max_process_size;
    PhysicalMemory physical_memory;
} MemoryManager;

// Function declarations
void initialize_memory(MemoryManager *mm, int physical_size, int page_size, int max_process_size);
void create_process(MemoryManager *mm, int process_id, int process_size);
void view_memory(MemoryManager *mm);
void view_page_table(MemoryManager *mm, int process_id);
void display_menu();

int main() {
    MemoryManager mm;
    int physical_size, page_size, max_process_size;
    int choice, process_id, process_size;

    printf("Digite o tamanho da memória física: ");
    scanf("%d", &physical_size);
    printf("Digite o tamanho da página: ");
    scanf("%d", &page_size);
    printf("Digite o tamanho máximo do processo: ");
    scanf("%d", &max_process_size);

    initialize_memory(&mm, physical_size, page_size, max_process_size);

    while (1) {
        display_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                view_memory(&mm);
                break;
            case 2:
                printf("Digite o ID do processo: ");
                scanf("%d", &process_id);
                printf("Digite o tamanho do processo: ");
                scanf("%d", &process_size);
                create_process(&mm, process_id, process_size);
                break;
            case 3:
                printf("Digite o ID do processo: ");
                scanf("%d", &process_id);
                view_page_table(&mm, process_id);
                break;
            case 4:
                printf("Sair\n");
                return 0;
            default:
                printf("Escolha inválida. Tente novamente\n");
        }
    }

    return 0;
}

void initialize_memory(MemoryManager *mm, int physical_size, int page_size, int max_process_size) {
    mm->physical_memory.size = physical_size;
    mm->physical_memory.page_size = page_size;
    mm->physical_memory.num_frames = physical_size / page_size; // Número de quadro é o tamanho da memória/tamanho da página
    mm->physical_memory.memory = (int *)malloc(physical_size * sizeof(int));
    mm->physical_memory.free_frames = (bool *)malloc(mm->physical_memory.num_frames * sizeof(bool));
    mm->num_processes = 0;
    mm->max_process_size = max_process_size;

    for (int i = 0; i < mm->physical_memory.num_frames; i++) {
        mm->physical_memory.free_frames[i] = true;
        mm->physical_memory.memory[i * page_size] = -1; // -1 indica que está livre
    }
}

void create_process(MemoryManager *mm, int process_id, int process_size) {
    if (process_size > mm->max_process_size) {
        printf("Processo muito grande.\n");
        return;
    }

    int num_pages = (process_size + mm->physical_memory.page_size - 1) / mm->physical_memory.page_size;
    int free_frames_needed = num_pages;
    int free_frames_count = 0;

    for (int i = 0; i < mm->physical_memory.num_frames; i++) {
        if (mm->physical_memory.free_frames[i]) {
            free_frames_count++;
        }
        if (free_frames_count >= free_frames_needed) {
            break;
        }
    }

    if (free_frames_count < free_frames_needed) {
        printf("Memória insuficiente para alocar processo.\n");
        return;
    }

    Process *new_process = &mm->processes[mm->num_processes++];
    new_process->process_id = process_id;
    new_process->process_size = process_size;
    new_process->num_pages = num_pages;
    new_process->page_table = (int *)malloc(num_pages * sizeof(int));

    int pages_allocated = 0;
    for (int i = 0; i < mm->physical_memory.num_frames && pages_allocated < num_pages; i++) {
        if (mm->physical_memory.free_frames[i]) {
            mm->physical_memory.free_frames[i] = false;
            new_process->page_table[pages_allocated++] = i;
            // Simulating random initialization of the page with process ID
            mm->physical_memory.memory[i * mm->physical_memory.page_size] = process_id;
        }
    }

    printf("Processo %d criado com sucesso.\n", process_id);
}

void view_memory(MemoryManager *mm) {
    int used_memory = 0;
    for (int i = 0; i < mm->physical_memory.num_frames; i++) {
        if (!mm->physical_memory.free_frames[i]) {
            used_memory += mm->physical_memory.page_size;
        }
    }

    double used_memory_percentage = (double)used_memory / mm->physical_memory.size * 100;
    printf("Uso da memória: %.2f%%\n", used_memory_percentage);

    for (int i = 0; i < mm->physical_memory.num_frames; i++) {
        // TO DO: Show process ID in the place of Ocupado
        printf("Quadro %d: %s\n", i, mm->physical_memory.free_frames[i] ? "Livre" : "Ocupado");
    }
}

void view_page_table(MemoryManager *mm, int process_id) {
    for (int i = 0; i < mm->num_processes; i++) {
        if (mm->processes[i].process_id == process_id) {
            printf("ID do processo: %d\n", process_id);
            printf("Tamanho do processo: %d bytes\n", mm->processes[i].process_size);
            printf("Tabela de páginas:\n");
            for (int j = 0; j < mm->processes[i].num_pages; j++) {
                printf("Página %d -> Quadro %d\n", j, mm->processes[i].page_table[j]);
            }
            return;
        }
    }

    printf("Processo %d não encontrado.\n", process_id);
}

void display_menu() {
    printf("\nGerenciamento de memória usando paginação\n");
    printf("1. Ver memória física\n");
    printf("2. Criar processo\n");
    printf("3. Ver tabela de páginas do processo\n");
    printf("4. Sair\n");
    printf("Selecione uma opção: ");
}
