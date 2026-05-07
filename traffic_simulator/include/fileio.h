#ifndef FILEIO_H
#define FILEIO_H

#include "game.h"

// Funzioni per la gestione dei file
void salva_record(const Record *record);
void leggi_record(Record *records, int *num_records);
void salva_cronologia(const Giocatore *giocatore);

#endif // FILEIO_H
