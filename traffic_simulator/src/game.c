#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "../include/game.h"
#include "../include/obstacles.h"
#include "../include/utils.h"
#include "../include/fileio.h"
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
int usleep(unsigned int usec);
#endif

void menuPrincipale(StatoGioco *statoCorrente, Giocatore *giocatore) {
    char scelta = ' ';
    while (scelta != 'P' && scelta != 'p' && scelta != 'R' && scelta != 'r' && scelta != 'Q' && scelta != 'q') {
        #ifdef _WIN32
            system("cls");
        #else
            printf("\033[H\033[J");
        #endif

        printf("=== TRAFFIC SIMULATOR ===\n");
        printf("[P] Gioca\n");
        printf("[R] Record\n");
        printf("[Q] Esci\n");
        printf("Scegli un'opzione: ");
        fflush(stdout);

        scelta = getchar();
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if (scelta == 'P' || scelta == 'p') {
        printf("\nInserisci il tuo gamertag: ");
        fflush(stdout);
        if (fgets(giocatore->nome, sizeof(giocatore->nome), stdin) != NULL) {
            size_t len = strlen(giocatore->nome);
            if (len > 0 && giocatore->nome[len - 1] == '\n') {
                giocatore->nome[len - 1] = '\0';
            }
        }
        if (giocatore->nome[0] == '\0') {
            strncpy(giocatore->nome, "Anonimo", sizeof(giocatore->nome) - 1);
            giocatore->nome[sizeof(giocatore->nome) - 1] = '\0';
        }
        *statoCorrente = GIOCO;
    } else if (scelta == 'R' || scelta == 'r') {
        *statoCorrente = RECORD;
    } else {
        *statoCorrente = ESCI;
    }
}

// Funzione per la schermata di Game Over
void mostraGameOver(StatoGioco *statoCorrente, const Giocatore *giocatore) {
    Record record;
    record.km = giocatore->km;
    strncpy(record.nome, giocatore->nome, sizeof(record.nome) - 1);
    record.nome[sizeof(record.nome) - 1] = '\0';
    record.tempo = giocatore->tempo;

    salva_record(&record);
    salva_cronologia(giocatore);

    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[H\033[J");
    #endif

    printf("\n\n");
    printf(" ==================================================== \n");
    printf("||                                                     ||\n");
    printf("||  GGG   AAA   M   M  EEE   OOO   V   V   EEE   RRRR  ||\n");
    printf("|| G      A  A  MM MM  E    O   O  V   V   E     R   R ||\n");
    printf("|| G GG   AAAA  M M M  EE   O   O   V V    EE    RRRR  ||\n");
    printf("|| G  G  A   A  M   M  E    O   O   V V    E     R R   ||\n");
    printf("||  GGG  A   A  M   M  EEE   OOO     V     EEE   R  R  ||\n");
    printf("||                                                     ||\n");
    printf("||                   GAME OVER                         ||\n");
    printf("||                                                     ||\n");
    printf(" ====================================================\n\n"); 
    printf("Giocatore: %s\n", giocatore->nome[0] ? giocatore->nome : "Anonimo");
    printf("Km percorsi: %.2f km\n", giocatore->km);
    printf("Tempo di gioco: %d secondi\n\n", giocatore->tempo);
    printf("Premi 'R' per rigiocare oppure 'M' per tornare al menu principale.\n\n");

    char scelta = ' ';
    while (scelta != 'r' && scelta != 'R' && scelta != 'm' && scelta != 'M') {
        if (_kbhit()) {
            scelta = _getch();
        }
    }

    if (scelta == 'r' || scelta == 'R') {
        *statoCorrente = GIOCO;
    } else {
        *statoCorrente = MENU;
    }
}

void gameLoop(StatoGioco *statoCorrente, Giocatore *giocatore) {
    inizializza_giocatore(giocatore);
    inizializza_ostacoli();

    static double prossimo_traguardo_km = 10.0; // Variabile statica per l'aumento di velocità

    while (*statoCorrente == GIOCO) {
        if (giocatore->vivo) {
            gestisciInput(giocatore);

            // Aggiorna gli ostacoli
            aggiorna_ostacoli(1.0f);

            // Controllo collisioni
            Ostacolo *ostacoli = get_ostacoli();
            for (int i = 0; i < MAX_OSTACOLI; i++) {
                if (ostacoli[i].corsia != giocatore->corsia || ostacoli[i].riga < 0) {
                    continue;
                }

                int collisione = 0;
                if (ostacoli[i].riga == RIGA_GIOCATORE) {
                    collisione = 1;
                } else if (ostacoli[i].tipo == CAMION && ostacoli[i].riga + 1 == RIGA_GIOCATORE) {
                    collisione = 1; // Il camion occupa due righe
                }

                if (collisione) {
                    if (ostacoli[i].tipo == BUCA) {
                        // Se colpisco una buca, riduco la velocità di 15.0
                        giocatore->velocita -= 15.0f;
                        if (giocatore->velocita < 30.0f) {
                            giocatore->velocita = 30.0f;
                        }
                        ostacoli[i].riga = -1; // Rimuove la buca dopo l'impatto
                    } else {
                        // Auto e camion uccidono il giocatore
                        giocatore->vivo = 0;
                        *statoCorrente = GAMEOVER;
                        break;
                    }
                }
            }

            // Incrementa i KM in base alla velocità
            giocatore->km += (giocatore->velocita / 1000.0);

            // Ogni 10 km aumenta la velocità di 15.0 (una sola volta)
            if (giocatore->km >= prossimo_traguardo_km) {
                giocatore->velocita += 15.0f;
                if (giocatore->velocita > 150.0f) {
                    giocatore->velocita = 150.0f; // Cap massimo alla velocità
                }
                prossimo_traguardo_km += 10.0;
            }

            giocatore->tempo++;
        }

        disegnaSchermo(giocatore, get_ostacoli());

        // Calcola il tempo di attesa in base alla velocità
        int pausa = 150000 - (int)(giocatore->velocita * 500);
        if (pausa < 30000) {
            pausa = 30000; // Minimo per non far diventare il gioco troppo veloce
        }

        // Ritardo dinamico basato sulla velocità
        #ifdef _WIN32
            Sleep(pausa / 1000);
        #else
            usleep(pausa);
        #endif
    }
}
