# Traffic Simulator

Un semplice simulatore di traffico scritto in C. Il giocatore deve evitare gli ostacoli che cadono dalle corsie.

## Come compilare

Usa il Makefile:

```bash
make
```

Oppure compila manualmente:

```bash
gcc -Iinclude src/*.c -o traffic_simulator
```

## Come eseguire

```bash
./traffic_simulator
```

## Controlli

- `A` / `D`: Muovi a sinistra/destra
- `P`: Pausa
- Nel menu: `P` per giocare, `R` per record, `Q` per uscire
- Nel game over: `R` per rigiocare, `M` per menu

## File

- `records.txt`: Salva i record
- `history.csv`: Cronologia delle partite

## Dipendenze

- Librerie standard C
- Su Linux: termios per input non bloccante