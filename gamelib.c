
#include "gamelib.h"

void imposta_gioco() {

    // Ottiene il numero di giocatori
    short int playersNumber;
    do {
        printf("Inserisci il numero di giocatori (min 1, max 4): ");
        scanf("%hd", &playersNumber);
        while ((getchar()) != '\n');
    } while (playersNumber < MIN_PLAYER || playersNumber > MAX_PLAYER);

    struct Giocatore giocatori[playersNumber];
    for (int i = 0; i < playersNumber; i++) {
        printf("Ciao Giocatore %d, configura il tuo personaggio :D\n", i+1);

    }
}
