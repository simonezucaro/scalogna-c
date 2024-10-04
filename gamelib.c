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

// Misc
void resetGame();

// Players
static int srandomNumberGenerator(int min, int max);
static void getPlayersNumber(short int *playersNumber);
static void initPlayers(Giocatore *giocatori, short int playersNumber);
static short int setGameMaster(Giocatore *giocatori, short int playersNumber);

// Map
static void fillZonaSegreta(Zona_segrete *zonaSegreta);
static void createZoneSegrete(Zona_segrete **firstZonaSegretaLocal, Zona_segrete** lastZonaSegretaLocal);
static void genera_mappa();
static void inserisci_zona(unsigned short int i);
static void cancella_zona(unsigned short int i);
static void stampa_mappa();
static void chiudi_mappa();
static void printMenuMappa();

// Game
void selectTurn();

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
    while (current != NULL) {
        Zona_segrete *next = current->zona_successiva;
        free(current);
        current = next;
    }
    firstZonaSegreta = NULL;
    lastZonaSegreta = NULL;

    // Free memory allocated for players
    if (giocatori != NULL) {
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
 * 4. Sets the player's attributes (attack dice, defense dice, life points, mind points, special power) based on the chosen class.
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
static void createZoneSegrete(Zona_segrete** firstZonaSegretaLocal, Zona_segrete** lastZonaSegretaLocal)
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
static void inserisci_zona(unsigned short int i) {
    if (i > numberOfZonaSegrete) {
        printf("Posizione non valida.\n");
        return;
    }

    // Allocate memory for the new secret zone
    Zona_segrete *newZonaSegreta = (Zona_segrete *)malloc(sizeof(Zona_segrete));
    if (newZonaSegreta == NULL) {
        printf("Errore nella creazione della zona segreta :/\n");
        exit(1);
    }

    // Initialize the new secret zone with random values
    fillZonaSegreta(newZonaSegreta);

    // Insert the new zone at the beginning of the list
    if (i == 0) {
        newZonaSegreta->zona_successiva = firstZonaSegreta;
        newZonaSegreta->zona_precedente = NULL;
        if (firstZonaSegreta != NULL) {
            firstZonaSegreta->zona_precedente = newZonaSegreta;
        }
        firstZonaSegreta = newZonaSegreta;
        if (lastZonaSegreta == NULL) {
            lastZonaSegreta = newZonaSegreta;
        }
    } else {
        // Traverse the list to find the insertion point
        Zona_segrete *current = firstZonaSegreta;
        for (unsigned int j = 0; j < i - 1; j++) {
            current = current->zona_successiva;
        }
        // Insert the new zone at the specified position
        newZonaSegreta->zona_successiva = current->zona_successiva;
        newZonaSegreta->zona_precedente = current;
        if (current->zona_successiva != NULL) {
            current->zona_successiva->zona_precedente = newZonaSegreta;
        }
        current->zona_successiva = newZonaSegreta;
        if (newZonaSegreta->zona_successiva == NULL) {
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
static void cancella_zona(unsigned short int i) {
    if (i >= numberOfZonaSegrete) {
        printf("Posizione non valida.\n");
        return;
    }

    Zona_segrete *toDelete = firstZonaSegreta;
    for (unsigned int j = 0; j < i; j++) {
        toDelete = toDelete->zona_successiva;
    }

    if (toDelete->zona_precedente != NULL) {
        toDelete->zona_precedente->zona_successiva = toDelete->zona_successiva;
    } else {
        firstZonaSegreta = toDelete->zona_successiva;
    }

    if (toDelete->zona_successiva != NULL) {
        toDelete->zona_successiva->zona_precedente = toDelete->zona_precedente;
    } else {
        lastZonaSegreta = toDelete->zona_precedente;
    }

    free(toDelete);
}

const char* tipo_zona_to_string(enum tipo_zona zona) {
    switch (zona) {
        case corridoio: return "corridoio";
        case scala: return "scala";
        case scala_banchetto: return "scala_banchetto";
        case magazzino: return "magazzino";
        case giardino: return "giardino";
        case posto_guardia: return "posto_guardia";
        case prigione: return "prigione";
        case cucina: return "cucina";
        case armeria: return "armeria";
        case tempio: return "tempio";
        default: return "unknown";
    }
}

const char* tipo_tesoro_to_string(enum tipo_tesoro tesoro) {
    switch (tesoro) {
        case nessun_tesoro: return "nessun_tesoro";
        case veleno: return "veleno";
        case guarigione: return "guarigione";
        case doppia_guarigione: return "doppia_guarigione";
        default: return "unknown";
    }
}

const char* tipo_porta_to_string(enum tipo_porta porta) {
    switch (porta) {
        case nessuna_porta: return "nessuna_porta";
        case porta_normale: return "porta_normale";
        case porta_da_scassinare: return "porta_da_scassinare";
        default: return "unknown";
    }
}

/**
 * @brief Prints the details of all secret zones in the linked list.
 *
 * This function traverses the doubly linked list of secret zones and prints the details
 * of each zone, including its type, treasure type, door type, and pointers to the previous
 * and next zones. The function prints a separator between each zone for clarity.
 */
static void stampa_mappa() {
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL) {
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
void chiudi_mappa() {
    int count = 0;
    do {
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL) {
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
void printMenuMappa() {
    int choice;
    printf("Game master, e' arrivato il momento di creare la mappa di gioco!\n");
    do {
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

        switch (choice) {
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
void gioca() {
    if (!mappaCreata) {
        printf("La mappa non è stata creata. Impossibile iniziare il gioco.\n");
        return;
    }

    // Game logic goes here
}