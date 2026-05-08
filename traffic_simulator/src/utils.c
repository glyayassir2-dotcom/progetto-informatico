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
    
    // Disegna gli ostacoli prima del giocatore
    for (int i = 0; i < MAX_OSTACOLI; i++) {
        if (ostacoli[i].riga >= 0 && ostacoli[i].riga < ALTEZZA_SCHERMO) {
            int pos = ostacoli[i].corsia * 4 + 1;
            switch (ostacoli[i].tipo) {
                case AUTO:
                case BUCA:
                    schermo[ostacoli[i].riga][pos] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 1] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 2] = ostacoli[i].simbolo;
                    break;
                case CAMION:
                    schermo[ostacoli[i].riga][pos] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 1] = ostacoli[i].simbolo;
                    schermo[ostacoli[i].riga][pos + 2] = ostacoli[i].simbolo;
                    if (ostacoli[i].riga + 1 < ALTEZZA_SCHERMO) {
                        schermo[ostacoli[i].riga + 1][pos] = ostacoli[i].simbolo;
                        schermo[ostacoli[i].riga + 1][pos + 1] = ostacoli[i].simbolo;
                        schermo[ostacoli[i].riga + 1][pos + 2] = ostacoli[i].simbolo;
                    }
                    break;
            }
        }
    }

    // Disegna il giocatore dopo gli ostacoli, per mostrare sempre il veicolo
    if (g->vivo) {
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 1] = '[';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 2] = 'O';
        schermo[RIGA_GIOCATORE][g->corsia * 4 + 3] = ']';
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
        for (int j = 0; j < NUM_CORSIE * 4 + 1; j++) {
            char c = schermo[i][j];
            int isGiocatore = (i == RIGA_GIOCATORE && j >= g->corsia * 4 + 1 && j <= g->corsia * 4 + 3 &&
                               (c == '[' || c == 'O' || c == ']'));
            if (isGiocatore) {
                printf("\033[1;31m%c\033[0m", c);
            } else if (c == '#' || c == 'H' || c == 'O') {
                printf("\033[1;33m%c\033[0m", c);
            } else {
                printf("%c", c);
            }
        }
        printf("\n");
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
