#ifndef OBSTACLES_H
#define OBSTACLES_H

// La definizione di Ostacolo e Giocatore si trova in game.h
#include "game.h"

#define MAX_OSTACOLI 10
#define NUM_CORSIE 3
#define ALTEZZA_SCHERMO 24
#define RIGA_GIOCATORE 20 // Riga fissa dove si controlla la collisione con il giocatore

// Funzioni per la gestione degli ostacoli
void inizializza_ostacoli();
void genera_ostacolo();
void aggiorna_ostacoli();
void disegna_ostacoli();

// Fornisce accesso in sola lettura all'array di ostacoli per il controllo collisioni
const Ostacolo* get_ostacoli();

#endif // OBSTACLES_H
