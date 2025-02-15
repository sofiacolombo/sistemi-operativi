#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Funzione di gestione del segnale
void handle_sigint(int sig) {
    printf("\nHai premuto Ctrl+C! Ignoro il segnale SIGINT (%d).\n", sig);
}

int main() {
    // Registra la funzione handle_sigint per gestire SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Errore nella registrazione del gestore di segnale");
        exit(EXIT_FAILURE);
    }

    printf("Premi Ctrl+C per testare il gestore di segnale...\n");

    // Loop infinito per mantenere il programma in esecuzione
    while (1) {
        printf("In attesa...\n");
        sleep(1);
    }

    return 0;
}
