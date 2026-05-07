#include "utils.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

void disegnaSchermo(Giocatore *g, const Ostacolo ostacoli[]) {
    // Per ridurre lo sfarfallio (flickering), disegnamo l'intera schermata
    // in un buffer di caratteri in memoria e poi stampiamo il buffer
    // tutto in una volta. Questa tecnica è una forma semplificata di 
    // "double buffering".

    char schermo[ALTEZZA_SCHERMO][NUM_CORSIE * 4 + 2]; // 3 corsie * 4 caratteri + 2 bordi

    // Inizializza lo schermo con spazi vuoti
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        for (int j = 0; j < NUM_CORSIE * 4 + 1; j++) {
            schermo[i][j] = ' ';
        }
        schermo[i][NUM_CORSIE * 4 + 1] = '\0'; // Termina la stringa
        
    }

    // Disegna i bordi della strada
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        schermo[i][0] = '|';
        schermo[i][4] = '|';
        schermo[i][8] = '|';
        schermo[i][12] = '|';
    }
    
    // Disegna il giocatore
    if (g->vivo) {
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 1] = '[';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 2] = 'O';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 3] = ']';
    }

    // Disegna gli ostacoli
    for (int i = 0; i < MAX_OSTACOLI; i++) {
        if (ostacoli[i].riga >= 0 && ostacoli[i].riga < ALTEZZA_SCHERMO) {
            char simbolo_ostacolo = ostacoli[i].simbolo == '#' ? '#' : 'X';
            schermo[ostacoli[i].riga][ostacoli[i].corsia * 4 + 1] = simbolo_ostacolo;
            schermo[ostacoli[i].riga][ostacoli[i].corsia * 4 + 2] = simbolo_ostacolo;
            schermo[ostacoli[i].riga][ostacoli[i].corsia * 4 + 3] = simbolo_ostacolo;
        }
    }

    // Pulisce lo schermo
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[H\033[J");
    #endif

    // Stampa le statistiche e la schermata
    printf("KM: %.2f | Velocita: %.1f km/h\n", g->km, g->velocita);
    for (int i = 0; i < ALTEZZA_SCHERMO; i++) {
        printf("%s\n", schermo[i]);
    }
}

#ifndef _WIN32
int _kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int _getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
