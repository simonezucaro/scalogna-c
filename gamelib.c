#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

static int srandomNumberGenerator(int min, int max);
static void getPlayersNumber(short int *playersNumber);
static void initPlayers(struct Giocatore *giocatori, short int playersNumber);
static short int setGameMaster(struct Giocatore *giocatori, short int playersNumber);


void imposta_gioco()
{
    short int playersNumber;
    getPlayersNumber(&playersNumber);

    // Ottiene il nome dei giocatori, assegna loro una classe e inizializza i loro attributi
    struct Giocatore *giocatori = malloc(playersNumber * sizeof(struct Giocatore));
    if (giocatori == NULL)
    {
        printf("Errore nella creazione dei giocatori :/\n");
        exit(1);
    }
    else
    {
        initPlayers(giocatori, playersNumber);
    }

    short int gameMasterIndex = setGameMaster(giocatori, playersNumber);
}

static int srandomNumberGenerator(int min, int max)
{
    srand(time(0));
    return rand() % (max - min + 1) + min;
}

static void getPlayersNumber(short int *playersNumber)
{
    do
    {
        printf("Inserisci il numero di giocatori (min 1, max 4): ");
        scanf("%hd", playersNumber);
        while ((getchar()) != '\n')
            ;
    } while (*playersNumber < MIN_PLAYER || *playersNumber > MAX_PLAYER);
}

static void initPlayers(struct Giocatore *giocatori, short int playersNumber)
{
    for (int i = 0; i < playersNumber; i++)
    {
        printf("Ciao Giocatore %d, configura il tuo personaggio :D\n", i + 1);
        puts("---------------------------------------------");

        printf("Inserisci il tuo nickname (MAX. %d caratteri): ", nicknameLength);
        fgets(giocatori[i].nome_giocatore, nicknameLength, stdin);
        system("cls");

        printf("Benvenuto ");
        printf("%s", giocatori[i].nome_giocatore);
        printf(" su Scalogna Quest!\n\n");

        printf("Scegli la tua classe:\n");
        printf("1. Barbaro\n");
        printf("2. Nano\n");
        printf("3. Elfo\n");
        printf("4. Mago\n");
        short int classe;
        do
        {
            scanf("%hd", &classe);
            while ((getchar()) != '\n')
                ;
        } while (classe < 1 || classe > 4);

        switch (classe)
        {
        case 1:
            printf("Hai scelto la classe Barbaro!\n");
            giocatori[i].dadi_attacco = 3;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 8;
            giocatori[i].mente = srandomNumberGenerator(1, 2);
            giocatori[i].potere_speciale = 0;
            break;
        case 2:
            printf("Hai scelto la classe Nano!\n");
            giocatori[i].dadi_attacco = 2;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 7;
            giocatori[i].mente = srandomNumberGenerator(2, 3);
            giocatori[i].potere_speciale = 1;
            break;
        case 3:
            printf("Hai scelto la classe Elfo!\n");
            giocatori[i].dadi_attacco = 2;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 6;
            giocatori[i].mente = srandomNumberGenerator(3, 4);
            giocatori[i].potere_speciale = 1;
            break;
        case 4:
            printf("Hai scelto la classe Mago!\n");
            giocatori[i].dadi_attacco = 1;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 4;
            giocatori[i].mente = srandomNumberGenerator(4, 5);
            giocatori[i].potere_speciale = 3;
            break;
        default:
            printf("Errore nella scelta della classe. :/ Riavvia il gioco\n");
            break;
        }

        printf("%s fai una scelta!\n", giocatori[i].nome_giocatore);
        printf("1. Sacrificare un punto mente per +1 punto vita\n");
        printf("2. Sacrificare un punto vita per +1 punto mente\n");
        printf("3. Nessuna delle due\n");
        short int choice;
        do
        {
            scanf("%hd", &choice);
            while ((getchar()) != '\n')
                ;
        } while (choice < 1 || choice > 4);

        switch (choice)
        {
        case 1:
            giocatori[i].p_vita++;
            giocatori[i].mente--;
            printf("Hai sacrificato un punto mente per +1 punto vita\n");
            break;
        case 2:
            giocatori[i].p_vita--;
            giocatori[i].mente++;
            printf("Hai sacrificato un punto vita per +1 punto mente\n");
            break;
        default:
            printf("Hai scelto di non sacrificare punti vita o mente\n");
            break;
        }
    }
}

static short int setGameMaster(struct Giocatore *giocatori, short int playersNumber)
{
    printf("La partita sta per iniziare! Scegliete il game master:\n");
    size_t i;
    for (i = 0; i < playersNumber; i++)
    {
        printf("%zu. %s", i + 1, giocatori[i].nome_giocatore);
    }
    short int gameMaster;
    do
    {
        scanf("%hd", &gameMaster);
        while ((getchar()) != '\n')
            ;
    } while (gameMaster < 1 || gameMaster > playersNumber);
}

