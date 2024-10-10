#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "gamelib.h"

// ####### Global variables #######

// MAP
Zona_segrete *firstZonaSegreta = NULL;
Zona_segrete *lastZonaSegreta = NULL;
static bool mappaCreata = false;

// GAME
Giocatore *giocatori = NULL;
short int playersNumber = 0;
unsigned short int actualTurn = 0;
Zona_segrete **playersCurrentZone = NULL;
bool *turnsArray = NULL;

// Misc
void resetGame();

// Players
static int srandomNumberGenerator(int min, int max);
static void getPlayersNumber(short int *playersNumber);
static void initPlayers(Giocatore *giocatori, short int playersNumber);
static short int setGameMaster(Giocatore *giocatori, short int playersNumber);

// Map
static void fillZonaSegreta(Zona_segrete *zonaSegreta);
static void createZoneSegrete(Zona_segrete **firstZonaSegretaLocal, Zona_segrete **lastZonaSegretaLocal);
static void genera_mappa();
static void inserisci_zona(unsigned short int i);
static void cancella_zona(unsigned short int i);
static void stampa_mappa();
static void chiudi_mappa();
static void printMenuMappa();

// Game
bool getAllPlayersPlayed();
void selectTurn();
void passTurn();

bool apri_porta(enum tipo_porta tipoPorta);
bool combatti();
void avanza();
void indietreggia();

void stampa_giocatore();
void stampa_zona();

void prendi_tesoro();
void scappa();
void gioca_potere_speciale();

/**
 * @brief Sets up the game environment.
 *
 * This function initializes the game by performing the following steps:
 * 1. Retrieves the number of players and allocates memory for player structures.
 * 2. Initializes each player with a name, class, and attributes.
 * 3. Selects one player to be the Game Master and announces it.
 * 4. Generates the game map with secret zones.
 * 5. Prints details of all secret zones including their type, treasure type, door type,
 *    and pointers to previous and next zones.
 *
 * If memory allocation for players fails, the function prints an error message and exits the program.
 */
void imposta_gioco()
{
    resetGame();
    getPlayersNumber(&playersNumber);
    turnsArray = (bool *)calloc(playersNumber, sizeof(int));
    playersCurrentZone = (Zona_segrete **)calloc(playersNumber, sizeof(Zona_segrete *));

    if (turnsArray == NULL)
    {
        printf("Errore nell'allocazione della memoria per l'array dei turni :/\n");
        exit(1);
    }

    if(playersCurrentZone == NULL)
    {
        printf("Errore nell'allocazione della memoria per l'array delle zone dei giocatori :/\n");
        exit(1);
    }

    for (int i = 0; i < playersNumber; i++)
    {
        turnsArray[i] = false;
    }

    // Get the players' names, assign them a class, and initialize their attributes
    giocatori = (Giocatore *)calloc(playersNumber, sizeof(Giocatore));
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
    printf("%s è evoluto a Game Master, complimenti! :D\n", giocatori[gameMasterIndex].nome_giocatore);

    // ######################## MAP ########################
    printMenuMappa();
    return;
}

/**
 * @brief Resets the game state by freeing allocated memory and resetting pointers.
 *
 * This function performs the following actions:
 * - Frees the memory allocated for the linked list of `Zona_segrete` structures.
 * - Sets the `firstZonaSegreta` and `lastZonaSegreta` pointers to NULL.
 * - Frees the memory allocated for the `giocatori` array, if it is not NULL.
 * - Sets the `giocatori` pointer to NULL.
 * - Sets the `mappaCreata` flag to false.
 */
void resetGame()
{
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL)
    {
        Zona_segrete *next = current->zona_successiva;
        free(current);
        current = next;
    }
    firstZonaSegreta = NULL;
    lastZonaSegreta = NULL;

    // Free memory allocated for players
    if (giocatori != NULL)
    {
        free(giocatori);
        giocatori = NULL;
    }
    mappaCreata = false;
}

// Players initialization

/**
 * @brief Generates a random number within a specified range.
 *
 * This function initializes the random number generator with the current time
 * and returns a random integer between the specified minimum and maximum values,
 * inclusive.
 *
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return A random integer between min and max, inclusive.
 */
static int srandomNumberGenerator(int min, int max)
{
    srand(time(0));
    return rand() % (max - min + 1) + min;
}

/**
 * @brief Prompts the user to enter the number of players and validates the input.
 *
 * This function repeatedly prompts the user to enter the number of players until a valid
 * number is provided. The valid range for the number of players is defined by the constants
 * MIN_PLAYER and MAX_PLAYER. The input is read as a short integer and stored in the variable
 * pointed to by the parameter `playersNumber`.
 *
 * @param playersNumber Pointer to a short integer where the number of players will be stored.
 */
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

/**
 * @brief Initializes the players in the game.
 *
 * This function configures each player by asking for their nickname and class,
 * and then sets their initial attributes based on the chosen class. Players
 * can also make a choice to sacrifice a point of mind for an extra point of life,
 * or vice versa.
 *
 * @param giocatori Pointer to an array of Giocatore structures representing the players.
 * @param playersNumber The number of players to initialize.
 *
 * The function performs the following steps for each player:
 * 1. Prompts the player to enter their nickname.
 * 2. Welcomes the player to the game.
 * 3. Asks the player to choose a class from the available options (Barbaro, Nano, Elfo, Mago).
 * 4. Sets the player's attributes (combatti dice, defense dice, life points, mind points, special power) based on the chosen class.
 * 5. Asks the player to make a choice to either sacrifice a point of mind for an extra point of life, sacrifice a point of life for an extra point of mind, or make no sacrifice.
 *
 * Note: The function uses fflush(stdin) and fflush(stdout) to clear the input and output buffers.
 */
static void initPlayers(Giocatore *giocatori, short int playersNumber)
{
    for (int i = 0; i < playersNumber; i++)
    {
        fflush(stdin);
        fflush(stdout);
        printf("Ciao Giocatore %d, configura il tuo personaggio :D\n", i + 1);
        puts("---------------------------------------------");

        printf("Inserisci il tuo nickname (MAX. %d caratteri): ", nicknameLength);
        fgets(giocatori[i].nome_giocatore, nicknameLength, stdin);

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

/**
 * @brief Prompts the players to choose a game master and returns the selected player's index.
 *
 * This function displays a list of players and asks the user to select a game master by entering a number.
 * It ensures that the input is valid and within the range of available players.
 *
 * @param giocatori An array of Giocatore structures representing the players.
 * @param playersNumber The total number of players.
 * @return The index of the selected game master (0-based).
 */
static short int setGameMaster(Giocatore *giocatori, short int playersNumber)
{
    printf("La partita sta per iniziare! Scegliete il game master:\n");
    size_t i;
    for (i = 0; i < playersNumber; i++)
    {
        printf("%zu. %s\n", i + 1, giocatori[i].nome_giocatore);
    }
    short int gameMaster;
    do
    {
        scanf("%hd", &gameMaster);
        while ((getchar()) != '\n')
            ;
    } while (gameMaster < 1 || gameMaster > playersNumber);

    return gameMaster - 1;
}

// Game map generation

/**
 * @brief Initializes a Zona_segrete structure with random values.
 *
 * This function assigns random values to the fields of a Zona_segrete structure.
 * The values are generated using the rand() function and are within specific ranges:
 * - tipoZona: A random integer between 1 and 10 (inclusive).
 * - tipoTesoro: A random integer between 1 and 4 (inclusive).
 * - tipoPorta: A random integer between 1 and 3 (inclusive).
 *
 * @param zonaSegreta A pointer to the Zona_segrete structure to be initialized.
 */
static void fillZonaSegreta(Zona_segrete *zonaSegreta)
{
    zonaSegreta->tipoZona = rand() % 10;  // 0 to 9
    zonaSegreta->tipoTesoro = rand() % 4; // 0 to 3
    zonaSegreta->tipoPorta = rand() % 3;  // 0 to 2
}

/**
 * @brief Creates a linked list of secret zones (Zona_segrete).
 *
 * This function generates a specified number of secret zones and links them together
 * in a doubly linked list. Each zone is initialized with random values and linked
 * to its predecessor and successor.
 *
 * If memory allocation for a zone fails, the function prints an error message and exits the program.
 *
 * @param firstZonaSegretaLocal Pointer to the first zone in the local list.
 * @param lastZonaSegretaLocal Pointer to the last zone in the local list.
 */
static void createZoneSegrete(Zona_segrete **firstZonaSegretaLocal, Zona_segrete **lastZonaSegretaLocal)
{
    for (int i = 0; i < numberOfZonaSegrete; i++)
    {
        // Allocate memory for a new secret zone
        Zona_segrete *zonaSegreta = (Zona_segrete *)malloc(sizeof(Zona_segrete));
        if (zonaSegreta == NULL)
        {
            // Print error message and exit if memory allocation fails
            printf("Errore nella creazione della zona segreta :/\n");
            exit(1);
        }
        else
        {
            // Initialize the secret zone with random values
            fillZonaSegreta(zonaSegreta);
            zonaSegreta->zona_precedente = NULL;
            zonaSegreta->zona_successiva = NULL;

            // If this is the first zone, set it as the first and last in the list
            if (*firstZonaSegretaLocal == NULL)
            {
                *firstZonaSegretaLocal = zonaSegreta;
                *lastZonaSegretaLocal = zonaSegreta;
            }
            else
            {
                // Link the new zone to the end of the list
                (*lastZonaSegretaLocal)->zona_successiva = zonaSegreta;
                zonaSegreta->zona_precedente = *lastZonaSegretaLocal;
                *lastZonaSegretaLocal = zonaSegreta;
            }
        }
    }
}

/**
 * @brief Generates a linked list of secret zones (Zona_segrete).
 *
 * This function initializes a doubly linked list of secret zones if it has not been created yet.
 * It allocates memory for each secret zone, initializes it using the createZonaSegrete function,
 * and links each zone to its predecessor and successor.
 *
 * If the list is already initialized (i.e., firstZonaSegreta and lastZonaSegreta are not NULL),
 * the function does nothing.
 *
 * @note The function exits the program with an error message if memory allocation fails.
 */
static void genera_mappa()
{
    Zona_segrete *firstZonaSegretaLocal = NULL;
    Zona_segrete *lastZonaSegretaLocal = NULL;
    if (firstZonaSegreta == NULL && lastZonaSegreta == NULL)
    {
        createZoneSegrete(&firstZonaSegretaLocal, &lastZonaSegretaLocal);

        firstZonaSegreta = firstZonaSegretaLocal;
        lastZonaSegreta = lastZonaSegretaLocal;
    }
    else
    {
        createZoneSegrete(&firstZonaSegretaLocal, &lastZonaSegretaLocal);
        lastZonaSegreta->zona_successiva = firstZonaSegretaLocal;
    }
}

/**
 * @brief Inserts a new secret zone at a specified position in the linked list.
 *
 * This function creates a new secret zone and inserts it at the specified position
 * in the doubly linked list of secret zones. If the position is invalid, it prints
 * an error message and returns. The new zone is initialized with random values.
 *
 * @param i The position at which to insert the new secret zone (0-based index).
 */
static void inserisci_zona(unsigned short int i)
{
    if (i > numberOfZonaSegrete)
    {
        printf("Posizione non valida.\n");
        return;
    }

    if(firstZonaSegreta == NULL || lastZonaSegreta == NULL)
    {
        genera_mappa();
        return;
    }

    // Allocate memory for the new secret zone
    Zona_segrete *newZonaSegreta = (Zona_segrete *)malloc(sizeof(Zona_segrete));
    if (newZonaSegreta == NULL)
    {
        printf("Errore nella creazione della zona segreta :/\n");
        exit(1);
    }

    // Initialize the new secret zone with random values
    fillZonaSegreta(newZonaSegreta);

    // Insert the new zone at the beginning of the list
    if (i == 0)
    {
        newZonaSegreta->zona_successiva = firstZonaSegreta;
        newZonaSegreta->zona_precedente = NULL;
        if (firstZonaSegreta != NULL)
        {
            firstZonaSegreta->zona_precedente = newZonaSegreta;
        }
        firstZonaSegreta = newZonaSegreta;
        if (lastZonaSegreta == NULL)
        {
            lastZonaSegreta = newZonaSegreta;
        }
    }
    else
    {
        // Traverse the list to find the insertion point
        Zona_segrete *current = firstZonaSegreta;
        for (unsigned int j = 0; j < i - 1; j++)
        {
            current = current->zona_successiva;
        }
        // Insert the new zone at the specified position
        newZonaSegreta->zona_successiva = current->zona_successiva;
        newZonaSegreta->zona_precedente = current;
        if (current->zona_successiva != NULL)
        {
            current->zona_successiva->zona_precedente = newZonaSegreta;
        }
        current->zona_successiva = newZonaSegreta;
        if (newZonaSegreta->zona_successiva == NULL)
        {
            lastZonaSegreta = newZonaSegreta;
        }
    }
}

/**
 * @brief Deletes a secret zone at the specified index.
 *
 * This function removes a secret zone from the linked list of secret zones
 * based on the provided index. If the index is invalid (i.e., greater than or
 * equal to the number of secret zones), the function prints an error message
 * and returns without making any changes.
 *
 * @param i The index of the secret zone to delete.
 *
 * The function updates the pointers of the neighboring secret zones to maintain
 * the integrity of the linked list. If the secret zone to be deleted is the first
 * or last in the list, the corresponding pointers (`firstZonaSegreta` or `lastZonaSegreta`)
 * are updated accordingly.
 *
 * Memory allocated for the secret zone is freed after it is removed from the list.
 */
static void cancella_zona(unsigned short int i)
{
    if (i >= numberOfZonaSegrete)
    {
        printf("Posizione non valida.\n");
        return;
    }

      if(firstZonaSegreta == NULL || lastZonaSegreta == NULL)
    {
        printf("La mappa non è stata ancora generata.\n");
        return;
    }

    Zona_segrete *toDelete = firstZonaSegreta;
    for (unsigned int j = 0; j < i; j++)
    {
        toDelete = toDelete->zona_successiva;
    }

    if (toDelete->zona_precedente != NULL)
    {
        toDelete->zona_precedente->zona_successiva = toDelete->zona_successiva;
    }
    else
    {
        firstZonaSegreta = toDelete->zona_successiva;
    }

    if (toDelete->zona_successiva != NULL)
    {
        toDelete->zona_successiva->zona_precedente = toDelete->zona_precedente;
    }
    else
    {
        lastZonaSegreta = toDelete->zona_precedente;
    }

    free(toDelete);
}

const char *tipo_zona_to_string(enum tipo_zona zona)
{
    switch (zona)
    {
    case corridoio:
        return "corridoio";
    case scala:
        return "scala";
    case scala_banchetto:
        return "scala_banchetto";
    case magazzino:
        return "magazzino";
    case giardino:
        return "giardino";
    case posto_guardia:
        return "posto_guardia";
    case prigione:
        return "prigione";
    case cucina:
        return "cucina";
    case armeria:
        return "armeria";
    case tempio:
        return "tempio";
    default:
        return "unknown";
    }
}

const char *tipo_tesoro_to_string(enum tipo_tesoro tesoro)
{
    switch (tesoro)
    {
    case nessun_tesoro:
        return "nessun_tesoro";
    case veleno:
        return "veleno";
    case guarigione:
        return "guarigione";
    case doppia_guarigione:
        return "doppia_guarigione";
    default:
        return "unknown";
    }
}

const char *tipo_porta_to_string(enum tipo_porta porta)
{
    switch (porta)
    {
    case nessuna_porta:
        return "nessuna_porta";
    case porta_normale:
        return "porta_normale";
    case porta_da_scassinare:
        return "porta_da_scassinare";
    default:
        return "unknown";
    }
}

/**
 * @brief Prints the details of all secret zones in the linked list.
 *
 * This function traverses the doubly linked list of secret zones and prints the details
 * of each zone, including its type, treasure type, door type, and pointers to the previous
 * and next zones. The function prints a separator between each zone for clarity.
 */
static void stampa_mappa()
{
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL)
    {
        printf("Zona Segreta:\n");
        printf("Indirizzo Zona Attuale: %p\n", (void *)current);
        printf("Tipo Zona: %s\n", tipo_zona_to_string(current->tipoZona));
        printf("Tipo Tesoro: %s\n", tipo_tesoro_to_string(current->tipoTesoro));
        printf("Tipo Porta: %s\n", tipo_porta_to_string(current->tipoPorta));
        printf("Zona Precedente: %p\n", (void *)current->zona_precedente);
        printf("Zona Successiva: %p\n", (void *)current->zona_successiva);
        printf("-----------------------------\n");
        current = current->zona_successiva;
    }
}

/**
 * @brief Closes the map after ensuring there are at least 15 secret zones.
 *
 * This function iterates through the linked list of secret zones and counts the number of zones.
 * It continues to do so until the count reaches at least 15. Once the condition is met, it sets
 * the map creation flag to true.
 */
void chiudi_mappa()
{
    int count = 0;
    do
    {
        Zona_segrete *current = firstZonaSegreta;
        while (current != NULL)
        {
            count++;
            current = current->zona_successiva;
        }
    } while (count < 15);
    mappaCreata = true;
}

/**
 * @brief Displays the menu for map operations.
 *
 * This function displays a menu of options for map operations, including generating the map,
 * inserting a new secret zone, deleting a secret zone, printing the map, and closing the map.
 * The user can choose an option by entering the corresponding number.
 */
void printMenuMappa()
{
    int choice;
    printf("Game master, e' arrivato il momento di creare la mappa di gioco!\n");
    do
    {
        printf("######## Menu Mappa ########\n");
        printf("1. Genera Mappa\n");
        printf("2. Inserisci Zona\n");
        printf("3. Cancella Zona\n");
        printf("4. Stampa Mappa\n");
        printf("5. Chiudi Mappa\n");
        printf("Scelta: ");
        scanf("%d", &choice);
        while ((getchar()) != '\n')
            ;

        switch (choice)
        {
        case 1:
            genera_mappa();
            break;
        case 2:
            printf("Inserisci la posizione della zona: ");
            unsigned short int posizione;
            scanf("%hu", &posizione);
            inserisci_zona(posizione);
            break;
        case 3:
            printf("Inserisci la posizione della zona da cancellare: ");
            unsigned short int posizioneCancella;
            scanf("%hu", &posizioneCancella);
            cancella_zona(posizioneCancella);
            break;
        case 4:
            stampa_mappa();
            break;
        case 5:
            chiudi_mappa();
            break;
        default:
            printf("Scelta non valida.\n");
            break;
        }
    } while ((choice != 5) || (choice == -1));
}

// ######################## GAME ########################

bool getAllPlayersPlayed()
{
    for (int i = 0; i < playersNumber; i++)
    {
        if (!turnsArray[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Selects the next player's turn randomly.
 *
 * This function randomly selects a player to take the next turn. It ensures that each player
 * gets a turn before any player gets a second turn. Once all players have had their turn,
 * the function resets the turn tracking array and starts over.
 *
 * The function performs the following steps:
 * 1. Generates a random index to select a player.
 * 2. Checks if all players have already played their turn.
 * 3. If all players have played, it resets the turn tracking array.
 * 4. If not all players have played, it continues to generate a random index until it finds a player who hasn't played yet.
 * 5. Marks the selected player as having played their turn.
 * 6. Prints the name of the player whose turn it is.
 */
void selectTurn()
{
    unsigned short int turn = srandomNumberGenerator(0, playersNumber - 1);
    bool allPlayersPlayed = getAllPlayersPlayed();

    if (allPlayersPlayed)
    {
        printf("Tutti i giocatori hanno giocato. Resetto i turni.\n");
        for (int i = 0; i < playersNumber; i++)
        {
            turnsArray[i] = false;
        }
        turn = srandomNumberGenerator(0, playersNumber - 1);
    } else {
    while (turnsArray[turn])
    {
        turn = srandomNumberGenerator(0, playersNumber - 1);
    }
    }
    turnsArray[turn] = true;
    printf("E' il turno di %s\n", giocatori[turn].nome_giocatore);
}

void passTurn() {
    unsigned short int nextTurn = (actualTurn + 1) % playersNumber;
    bool allPlayersPlayed = getAllPlayersPlayed();

    if (allPlayersPlayed) {
        selectTurn();
    } else {
        while (turnsArray[nextTurn]) {
            nextTurn = (nextTurn + 1) % playersNumber;
        }
        actualTurn = nextTurn;
        turnsArray[actualTurn] = true;
        printf("E' il turno di %s\n", giocatori[actualTurn].nome_giocatore);
    }
}

void avanza()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    if (currentZone->zona_successiva == NULL)
    {
        printf("Non ci sono più stanze in cui avanzare.\n");
        return;
    }

    if (currentZone->tipoPorta == porta_normale || currentZone->tipoPorta == porta_da_scassinare)
    {
        if (!apri_porta(currentZone->tipoPorta))
        {
            printf("Non sei riuscito ad aprire la porta.\n");
            return;
        }
    }

    playersCurrentZone[currentPlayer] = currentZone->zona_successiva;
    printf("Sei avanzato nella stanza successiva.\n");

    if (rand() % 3 == 0 || playersCurrentZone[currentPlayer]->zona_successiva == NULL)
    {
        printf("Un abitante delle segrete è apparso!\n");
        if (!combatti())
        {
            printf("Hai perso il combattimento. Non puoi avanzare ulteriormente.\n");
            return;
        }
    }
}

void indietreggia()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    if (currentZone->zona_precedente == NULL)
    {
        printf("Non ci sono più stanze in cui indietreggiare.\n");
        return;
    }

    playersCurrentZone[currentPlayer] = currentZone->zona_precedente;
    printf("Sei indietreggiato nella stanza precedente.\n");

    if (rand() % 3 == 0)
    {
        printf("Un abitante delle segrete è apparso!\n");
        if (!combatti())
        {
            printf("Hai perso il combattimento. Non puoi indietreggiare ulteriormente.\n");
            return;
        }
    }
}

void stampa_giocatore()
{
    for (int i = 0; i < playersNumber; i++)
    {
        printf("Giocatore %d:\n", i + 1);
        printf("Nome: %s\n", giocatori[i].nome_giocatore);
        printf("Classe: %d\n", giocatori[i].classe);
        printf("Dadi Attacco: %d\n", giocatori[i].dadi_attacco);
        printf("Dadi Difesa: %d\n", giocatori[i].dadi_difesa);
        printf("Punti Vita: %d\n", giocatori[i].p_vita);
        printf("Mente: %d\n", giocatori[i].mente);
        printf("Potere Speciale: %d\n", giocatori[i].potere_speciale);
        printf("-----------------------------\n");
    }
}

void stampa_zona()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    printf("Zona Segreta:\n");
    printf("Indirizzo Zona Attuale: %p\n", (void *)currentZone);
    printf("Tipo Zona: %s\n", tipo_zona_to_string(currentZone->tipoZona));
    printf("Tesoro Presente: %s\n", currentZone->tipoTesoro != nessun_tesoro ? "Sì" : "No");
    printf("Porta Presente: %s\n", currentZone->tipoPorta != nessuna_porta ? "Sì" : "No");
    printf("Zona Precedente: %p\n", (void *)currentZone->zona_precedente);
    printf("Zona Successiva: %p\n", (void *)currentZone->zona_successiva);
    printf("-----------------------------\n");
}

bool apri_porta(enum tipo_porta tipoPorta)
{
    unsigned short int currentPlayer = actualTurn;
    unsigned short int mindRoll = srandomNumberGenerator(1, 6);

    printf("Hai tirato %hu per aprire la porta.\n", mindRoll);

    if (tipoPorta == porta_da_scassinare)
    {
        if (mindRoll <= giocatori[currentPlayer].mente)
        {
            printf("Hai scassinato la porta con successo!\n");
            return true;
        }
        else
        {
            printf("Non sei riuscito a scassinare la porta.\n");
            int outcome = rand() % 100;
            if (outcome < 10)
            {
                printf("Sfortuna! Devi ricominciare dalla prima stanza delle segrete.\n");
                playersCurrentZone[currentPlayer] = firstZonaSegreta;
            }
            else if (outcome < 60)
            {
                printf("Hai perso un punto vita.\n");
                giocatori[currentPlayer].p_vita--;
                if (giocatori[currentPlayer].p_vita <= 0)
                {
                    printf("Sei morto!\n");
                    // Handle player death if necessary
                }
            }
            else
            {
                printf("Un abitante delle segrete è apparso!\n");
                if (!combatti())
                {
                    printf("Hai perso il combattimento.\n");
                    // Handle combat loss if necessary
                }
            }
            return false;
        }
    }
    else if (mindRoll > 4)
    {
        printf("Hai aperto la porta con successo!\n");
        return true;
    }
    else
    {
        printf("Non sei riuscito ad aprire la porta.\n");
        return false;
    }
}

void prendi_tesoro()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    switch (currentZone->tipoTesoro)
    {
    case veleno:
        giocatori[currentPlayer].p_vita -= 2;
        printf("Hai preso veleno! Hai perso 2 punti vita.\n");
        break;
    case guarigione:
        giocatori[currentPlayer].p_vita += 1;
        printf("Hai preso guarigione! Hai guadagnato 1 punto vita.\n");
        break;
    case doppia_guarigione:
        giocatori[currentPlayer].p_vita += 2;
        printf("Hai preso doppia guarigione! Hai guadagnato 2 punti vita.\n");
        break;
    default:
        printf("Non c'è nessun tesoro in questa stanza.\n");
        return;
    }

    // Reset the treasure type to regenerate it for the next entry
    currentZone->tipoTesoro = srandomNumberGenerator(0, 3); // 0 to 3 corresponds to enum tipo_tesoro
}

void scappa()
{
    unsigned short int currentPlayer = actualTurn;
    unsigned short int mindRoll = srandomNumberGenerator(1, 6);

    printf("Hai tirato %hu per scappare.\n", mindRoll);

    if (mindRoll <= giocatori[currentPlayer].mente)
    {
        printf("Sei riuscito a scappare!\n");
        indietreggia();
    }
    else
    {
        printf("Non sei riuscito a scappare. Subisci un attacco!\n");
        unsigned short int attackRoll = srandomNumberGenerator(1, 6) + giocatori[currentPlayer].dadi_attacco;
        unsigned short int defenseRoll = srandomNumberGenerator(1, 6) + (giocatori[currentPlayer].dadi_difesa / 2);

        printf("Hai tirato %hu per l'attacco e %hu per la difesa.\n", attackRoll, defenseRoll);

        if (attackRoll > defenseRoll)
        {
            printf("Hai perso il combattimento.\n");
            // Handle combat loss if necessary
        }
        else
        {
            printf("Sei riuscito a difenderti dall'attacco.\n");
        }
    }
}

bool combatti()
{
    unsigned short int currentPlayer = actualTurn;
    unsigned short int playerRoll = srandomNumberGenerator(1, 6);
    unsigned short int inhabitantRoll = srandomNumberGenerator(1, 6);

    printf("Il giocatore ha tirato %hu.\n", playerRoll);
    printf("L'abitante ha tirato %hu.\n", inhabitantRoll);

    bool playerStarts = playerRoll >= inhabitantRoll;
    bool playerTurn = playerStarts;

    while (giocatori[currentPlayer].p_vita > 0 && inhabitantRoll > 0)
    {
        unsigned short int attackRolls, defenseRolls;
        unsigned short int playerHits = 0, inhabitantHits = 0;
        unsigned short int playerDefends = 0, inhabitantDefends = 0;

        if (playerTurn)
        {
            attackRolls = giocatori[currentPlayer].dadi_attacco;
            defenseRolls = giocatori[currentPlayer].dadi_difesa;

            for (int i = 0; i < attackRolls; i++)
            {
                unsigned short int roll = srandomNumberGenerator(1, 6);
                if (roll <= 3)
                    playerHits++;
            }

            for (int i = 0; i < defenseRolls; i++)
            {
                unsigned short int roll = srandomNumberGenerator(1, 6);
                if (roll == 6)
                    playerDefends++;
            }

            printf("Il giocatore ha colpito %hu volte e parato %hu volte.\n", playerHits, playerDefends);

            if (playerHits > inhabitantDefends)
            {
                inhabitantRoll -= (playerHits - inhabitantDefends);
                printf("L'abitante ha perso %hu punti vita.\n", playerHits - inhabitantDefends);
            }
        }
        else
        {
            attackRolls = 2; // Assuming inhabitant has 2 attack dice
            defenseRolls = 2; // Assuming inhabitant has 2 defense dice

            for (int i = 0; i < attackRolls; i++)
            {
                unsigned short int roll = srandomNumberGenerator(1, 6);
                if (roll <= 3)
                    inhabitantHits++;
            }

            for (int i = 0; i < defenseRolls; i++)
            {
                unsigned short int roll = srandomNumberGenerator(1, 6);
                if (roll == 5 || roll == 6)
                    inhabitantDefends++;
            }

            printf("L'abitante ha colpito %hu volte e parato %hu volte.\n", inhabitantHits, inhabitantDefends);

            if (inhabitantHits > playerDefends)
            {
                giocatori[currentPlayer].p_vita -= (inhabitantHits - playerDefends);
                printf("Il giocatore ha perso %hu punti vita.\n", inhabitantHits - playerDefends);
            }
        }

        if (giocatori[currentPlayer].p_vita <= 0)
        {
            printf("Il giocatore è morto.\n");
            return false;
        }

        if (inhabitantRoll <= 0)
        {
            printf("L'abitante è morto.\n");
            return true;
        }

        playerTurn = !playerTurn;
    }

    return false;
}

void gioca_potere_speciale()
{
    unsigned short int currentPlayer = actualTurn;

    if (giocatori[currentPlayer].potere_speciale > 0)
    {
        giocatori[currentPlayer].potere_speciale--;
        printf("Hai usato il potere speciale per uccidere immediatamente l'abitante!\n");
    }
    else
    {
        printf("Non hai più poteri speciali disponibili.\n");
    }
}

void gioca()
{
    if (!mappaCreata)
    {
        printf("La mappa non è stata creata. Impossibile iniziare il gioco.\n");
        return;
    }

    for (int i = 0; i < playersNumber; i++) {
        playersCurrentZone[i] = firstZonaSegreta;
    }

    selectTurn();
    avanza();
}
