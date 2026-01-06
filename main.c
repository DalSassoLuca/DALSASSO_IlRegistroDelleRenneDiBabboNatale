#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

typedef struct {
    char nome[30];
    int velocita;
    int resistenza;
} Renna;

#define R 0
#define W 1

void elfo(int w) {
    srand(time(NULL) ^ getpid());
    int n = (rand() % 8) + 5;

    for (int i = 1; i <= n; i++) {
        Renna r;
        snprintf(r.nome, sizeof(r.nome), "Renna_%d", i);
        r.velocita = (rand() % 51) + 50;
        r.resistenza = (rand() % 10) + 1;

        printf("Elfo: invio %s con velocità=%d e resistenza=%d\n", r.nome, r.velocita, r.resistenza);
        write(w, &r, sizeof(r));
    }

    Renna end = {"SENTINELLA", -1, 0};
    write(w, &end, sizeof(end));
    close(w);
    _exit(0);
}

int main() {

    int fd[2];
    pipe(fd);

    pid_t p = fork();
    if (p == 0) {
        close(fd[R]);
        elfo(fd[W]);
    }

    close(fd[W]);

    Renna r, *arr = NULL;
    int cnt = 0, cap = 0;
    int vmax = -1;
    char best[30] = "";

    while (read(fd[R], &r, sizeof(r)) > 0) {
        if (r.velocita == -1) break;

        if (cnt == cap) {
            cap = (cap == 0) ? 5 : cap * 2;
            arr = realloc(arr, cap * sizeof(Renna));
            if (!arr) exit(1);
        }

        arr[cnt++] = r;
        if (r.velocita > vmax) {
            vmax = r.velocita;
            strcpy(best, r.nome);
        }

        printf("Babbo Natale: ricevuta %s\n", r.nome);
    }

    close(fd[R]);
    waitpid(p, NULL, 0);

    for (int i = 0; i < cnt; i++)
        printf("%s  velocità=%d  resistenza=%d\n", arr[i].nome, arr[i].velocita, arr[i].resistenza);

    if (cnt > 0) printf("%s guiderà la slitta con velocita %d km/h\n", best, vmax);
    else printf("Nessuna renna ricevuta\n");

    free(arr);
    return 0;
}
