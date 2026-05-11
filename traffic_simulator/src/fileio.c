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

int leggi_record_personale(const char *nome) {
    if (nome == NULL || nome[0] == '\0') {
        return 0;
    }

    FILE *file = fopen("data/records.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char line[128];
    char record_nome[20];
    double km;
    int tempo;
    int record_personale = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%19[^,],%lf,%d", record_nome, &km, &tempo) == 3) {
            if (strcmp(record_nome, nome) == 0) {
                int km_intero = (int)km;
                if (km_intero > record_personale) {
                    record_personale = km_intero;
                }
            }
        }
    }

    fclose(file);
    return record_personale;
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
