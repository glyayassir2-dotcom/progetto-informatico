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
    printf("============================================================\n");
    printf("||                                                        ||\n");
    printf("||  GGGG   AAAAA  M   M  EEEEE    OOOOO   V   V  EEEEE     ||\n");
    printf("|| G      A   A  MM MM  E       O     O  V   V  E         ||\n");
    printf("|| G  GG  AAAAA  M M M  EEEE    O     O   V V   EEEE      ||\n");
    printf("|| G   G  A   A  M   M  E       O     O   V V   E         ||\n");
    printf("||  GGGG  A   A  M   M  EEEEE    OOOOO     V    EEEEE     ||\n");
    printf("||                                                        ||\n");
    printf("||                      GAME OVER                         ||\n");
    printf("||                                                        ||\n");
    printf("============================================================\n\n");
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

    struct timespec last_time, current_time;
    double tempo_gioco = 0.0;
    clock_gettime(CLOCK_MONOTONIC, &last_time);

    while (*statoCorrente == GIOCO) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double delta_sec = (current_time.tv_sec - last_time.tv_sec) +
                           (current_time.tv_nsec - last_time.tv_nsec) / 1e9;
        if (delta_sec < 0.0) {
            delta_sec = 0.0;
        }
        if (delta_sec > 0.1) {
            delta_sec = 0.1; // Limita delta per evitare salti troppo grandi dopo pause o freeze
        }
        last_time = current_time;

        if (giocatore->vivo) {
            tempo_gioco += delta_sec;
            giocatore->tempo = (int)tempo_gioco;
            gestisciInput(giocatore);
            aggiorna_ostacoli();

            // Controllo collisioni
            const Ostacolo *ostacoli = get_ostacoli();
            for (int i = 0; i < MAX_OSTACOLI; i++) {
                if (ostacoli[i].riga == RIGA_GIOCATORE && ostacoli[i].corsia == giocatore->corsia) {
                    giocatore->vivo = 0; // Il giocatore è morto!
                    *statoCorrente = GAMEOVER;
                    break;
                }
            }

            // Fisica base: v [km/h] = d [km] / t [h], quindi d = v * t.
            // Usiamo delta_sec in secondi e un fattore arcade per rendere la distanza perceptibile.
            const float arcade_scale = 120.0f;
            double distance_increment = ((double)giocatore->velocita * delta_sec / 3600.0) * arcade_scale;
            giocatore->km += distance_increment;

            // Aumenta la velocità in modo graduale in base alla distanza effettivamente percorsa.
            // 0.5 km/h ogni 200 km percorsi.
            float speed_increment = 0.5f * (float)(distance_increment / 200.0);
            giocatore->velocita += speed_increment;
            if (giocatore->velocita > 20.0f) {
                giocatore->velocita = 20.0f; // Limite massimo di velocità
            }
        }

        disegnaSchermo(giocatore, get_ostacoli());

        // Delay fisso per tenere il loop a un frame rate stabile,
        // ma la distanza è calcolata sul vero tempo trascorso.
        #ifdef _WIN32
            Sleep(50);
        #else
            usleep(50 * 1000);
        #endif
    }
}
