#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "fileio.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static const int MAX_RECORDS_FILE = 100;

void salva_record(const Record *record) {
    FILE *file = fopen("data/records.txt", "a");
    if (file == NULL) {
        return;
    }

    fprintf(file, "%s,%.2f,%d\n", record->nome, record->km, record->tempo);
    fclose(file);
}

void leggi_record(Record *records, int *num_records) {
    FILE *file = fopen("data/records.txt", "r");
    if (file == NULL) {
        *num_records = 0;
        return;
    }

    char line[128];
    int count = 0;
    while (count < MAX_RECORDS_FILE && fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%19[^,],%lf,%d", records[count].nome, &records[count].km, &records[count].tempo) == 3) {
            count++;
        }
    }

    fclose(file);
    *num_records = count;
}

void salva_cronologia(const Giocatore *giocatore) {
    FILE *file = fopen("data/history.csv", "a");
    if (file == NULL) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info);

    fprintf(file, "%s,%d,%.2f,%d,%.1f,%d\n",
            timestamp,
            giocatore->corsia,
            giocatore->km,
            giocatore->punteggio,
            giocatore->velocita,
            giocatore->vivo);
    fclose(file);
}
