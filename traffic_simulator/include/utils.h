#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "player.h"
#include "obstacles.h"

// Funzioni di utilità generiche
int random_int(int min, int max);
void disegnaSchermo(Giocatore *g, const Ostacolo ostacoli[]);

// Funzioni di input non bloccante per piattaforme POSIX
int _kbhit(void);
int _getch(void);

#endif // UTILS_H
