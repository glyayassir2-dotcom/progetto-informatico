#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Enum per rappresentare i diversi stati del gioco
typedef enum { MENU, GIOCO, PAUSA, GAMEOVER, RECORD, ESCI } StatoGioco;
typedef enum { AUTO, CAMION, BUCA } TipoOstacolo;

typedef struct {
    int corsia;
    double km;
    int tempo;
    int punteggio;
    int vivo;
    float velocita;
    float penalita; // riduce la velocità dopo aver preso una buca
    char nome[20];
    time_t tempo_inizio; // Timestamp di inizio della partita (per misurare il tempo reale)
} Giocatore;


typedef struct {
    int corsia;
    int riga;
    char simbolo;
    TipoOstacolo tipo;
} Ostacolo;

typedef struct {
    double km;
    char nome[20];
    int tempo;
} Record;

// Funzioni di gestione del gioco
void aggiorna_logica(StatoGioco *statoCorrente, Giocatore *giocatore);
void menuPrincipale(StatoGioco *statoCorrente, Giocatore *giocatore);
void mostraGameOver(StatoGioco *statoCorrente, const Giocatore *giocatore);
void gameLoop(StatoGioco *statoCorrente, Giocatore *giocatore);

// Funzioni dei moduli
void inizializza_giocatore(Giocatore *giocatore);
void gestisciInput(Giocatore *giocatore);
void inizializza_ostacoli();
void aggiorna_ostacoli(float moltiplicatore_velocita);
Ostacolo* get_ostacoli();
void disegnaSchermo(Giocatore *g, const Ostacolo ostacoli[], int recordPersonale, int recordSuperato);

#endif // GAME_H
