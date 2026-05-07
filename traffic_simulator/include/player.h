#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

// Funzioni per la gestione del giocatore
void inizializza_giocatore(Giocatore *giocatore);
void gestisciInput(Giocatore *giocatore);
void disegna_giocatore(const Giocatore *giocatore);

#endif // PLAYER_H
