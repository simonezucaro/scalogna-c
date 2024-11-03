#include "gamelib.h"

// ####### Global variables #######

// MAP
Zona_segrete *firstZonaSegreta = NULL;
Zona_segrete *lastZonaSegreta = NULL;
static bool isGameInitialized = false;

// GAME
Giocatore *giocatori = NULL;
short int playersNumber = 0;
unsigned short int actualTurn = 0;
Zona_segrete **playersCurrentZone = NULL;
bool *turnsArray = NULL;
bool *playersDead = NULL;
Giocatore *gameMaster = NULL;
Giocatore *gameWinner = NULL;

// Utils
static void resetGame();
static int getNumberByProbability(const double probability[], int numElements);
static int diceThrow(int min, int max, char *message, bool abitante);
static Abitante_segrete *figureAbitanteSegrete();
static void regenerateZoneSegrete();
static void visualizzaBarraVita(char *nome, int vita_attuale, int vita_massima);
static void setDeadPlayer();

// Players
static int srandomNumberGenerator(int min, int max);
static void getPlayersNumber(short int *playersNumber);
static void initPlayers();
static Giocatore *setGameMaster();

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
static bool getAllPlayersPlayed();
static void printMenuGiocatore();
static void selectTurn();
// static void passTurn();

static bool apri_porta(enum tipo_porta tipoPorta);
static bool combatti(Abitante_segrete *abitante);
static void avanza();
static void indietreggia();

static void stampa_giocatore();
static void stampa_zona();

static void prendi_tesoro();
static void scappa();
static bool gioca_potere_speciale();

// MENU DATA
const char *menuGiocatoreName = "Menu Giocatore";
const char *optionsMenuGiocatore[] = {
    "Avanza",
    "Indietreggia",
    "Stampa Giocatore",
    "Stampa Zona",
    "Prendi Tesoro",
    "Passa il turno"};
int numOptionsGiocatore = sizeof(optionsMenuGiocatore) / sizeof(optionsMenuGiocatore[0]);

const char *menuMappaName = "Menu Mappa";
const char *optionsMenuMappa[] = {
    "Genera Mappa",
    "Inserisci Zona",
    "Cancella Zona",
    "Stampa Mappa",
    "Chiudi Menu Mappa"};
int numOptionsMappa = sizeof(optionsMenuMappa) / sizeof(optionsMenuMappa[0]);

const char *menuCombattimentoName = "Menu Combattimento";
const char *optionsMenuCombattimento[] = {
    "Combatti",
    "Scappa",
    "Gioca Potere Speciale"};
int numOptionsCombattimento = sizeof(optionsMenuCombattimento) / sizeof(optionsMenuCombattimento[0]);

const char *menuAzioneName = "Menu Azione";
const char *optionsMenuAzione[] = {
    "Attacca",
    "Difenditi",
    "Curati"};
int numOptionsAzione = sizeof(optionsMenuAzione) / sizeof(optionsMenuAzione[0]);

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
    clearScreen();
    printCustomHeader("CREAZIONE GIOCATORI");

    getPlayersNumber(&playersNumber);
    turnsArray = (bool *)calloc(playersNumber, sizeof(int));
    playersDead = (bool *)calloc(playersNumber, sizeof(int));
    for (int i = 0; i < playersNumber; i++)
    {
        turnsArray[i] = false;
        playersDead[i] = false;
    }
    playersCurrentZone = (Zona_segrete **)calloc(playersNumber, sizeof(Zona_segrete *));

    if (turnsArray == NULL)
    {
        fprintf(stderr, "Errore nell'allocazione della memoria per l'array dei turni :/\n");
        exit(EXIT_FAILURE);
    }

    if (playersDead == NULL)
    {
        fprintf(stderr, "Errore nell'allocazione della memoria per l'array dei giocatori morti :/\n");
        exit(EXIT_FAILURE);
    }

    if (playersCurrentZone == NULL)
    {
        fprintf(stderr, "Errore nell'allocazione della memoria per l'array delle zone dei giocatori :/\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < playersNumber; i++)
    {
        turnsArray[i] = false;
    }

    // Get the players' names, assign them a class, and initialize their attributes
    giocatori = (Giocatore *)calloc(playersNumber, sizeof(Giocatore));
    if (giocatori == NULL)
    {
        fprintf(stderr, "Errore nella creazione dei giocatori :/\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        initPlayers();
    }

    clearScreen();
    gameMaster = setGameMaster();
    char message[100];
    sprintf(message, "\n%s e' evoluto a Game Master! E' arrivato il momento di creare la mappa di gioco!", gameMaster->nome_giocatore);
    printGameEvent(message, BLUE);

    // ######################## MAP ########################
    printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
    printMenuMappa();
}

/**
 * @brief Selects an index based on a given probability distribution.
 *
 * This function takes an array of probabilities and the number of elements in the array,
 * and returns an index based on the probability distribution. The sum of the probabilities
 * must be approximately 1.0.
 *
 * @param probability An array of probabilities.
 * @param numElements The number of elements in the probability array.
 * @return The selected index based on the probability distribution.
 */
static int getNumberByProbability(const double probability[], int numElements)
{
    // Calculate the sum of probabilities to ensure it is approximately 1.0
    double sumProbability = 0.0;
    for (int i = 0; i < numElements; i++)
    {
        sumProbability += probability[i];
    }

    // Verify that the sum of probabilities is close to 1.0
    if (sumProbability < 0.9999 || sumProbability > 1.0001)
    {
        fprintf(stderr, "Error: The sum of probabilities must be 1.0 (current: %f)\n", sumProbability);
        exit(EXIT_FAILURE);
    }

    // Create the cumulative distribution
    double cumulativeDistribution[numElements];
    cumulativeDistribution[0] = probability[0];
    for (int i = 1; i < numElements; i++)
    {
        cumulativeDistribution[i] = cumulativeDistribution[i - 1] + probability[i];
    }

    // Generate a random number between 0 and 1
    double randomFraction = rand() / (RAND_MAX + 1.0);

    // Find the corresponding index in the cumulative distribution
    for (int i = 0; i < numElements; i++)
    {
        if (randomFraction < cumulativeDistribution[i])
        {
            return i;
        }
    }

    // In case no index is found (due to rounding), return the last index
    return numElements - 1;
}

/**
 * @brief Resets the game state by freeing allocated memory and resetting pointers.
 *
 * This function performs the following actions:
 * - Frees the memory allocated for the linked list of `Zona_segrete` structures.
 * - Sets the `firstZonaSegreta` and `lastZonaSegreta` pointers to NULL.
 * - Frees the memory allocated for the `giocatori` array, if it is not NULL.
 * - Sets the `giocatori` pointer to NULL.
 * - Sets the `isGameInitialized` flag to false.
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
    isGameInitialized = false;
}

static int diceThrow(int min, int max, char *message, bool abitante)
{
    printGameEvent(message, BLUE);
    if (abitante)
    {
        for (int i = 0; i < 3; i++)
        {
            printf(".");
            fflush(stdout);
#ifdef _WIN32
            Sleep(500);
#else
            sleep(1);
#endif
        }
    }
    else
    {
        // printf("Premi qualsiasi tasto per lanciare il dado...\n");
        clearInputBuffer();
        getchar();
    }
    return srandomNumberGenerator(min, max);
}

static Abitante_segrete *figureAbitanteSegrete()
{
    // Define the possible names for the inhabitants
    const char *names[] = {"Goblin", "Orco", "Troll", "Scheletro", "Zombie", "Yuri"};
    int numNames = sizeof(names) / sizeof(names[0]);

    // Create an Abitante_segrete struct and populate it with random characteristics
    Abitante_segrete *abitante = (Abitante_segrete *)malloc(sizeof(Abitante_segrete));
    strncpy(abitante->nome, names[srandomNumberGenerator(0, numNames - 1)], sizeof(abitante->nome) - 1);
    abitante->nome[sizeof(abitante->nome) - 1] = '\0';     // Ensure null-termination
    abitante->dadi_attacco = srandomNumberGenerator(1, 3); // Attack dice between 1 and 3
    abitante->dadi_difesa = srandomNumberGenerator(1, 3);  // Defense dice between 1 and 3
    abitante->punti_vita = giocatori[actualTurn].p_vita + srandomNumberGenerator(1, 3);
    abitante->punti_vita_max = abitante->punti_vita;

    // Print the generated characteristics
    char message[256];
    snprintf(message, sizeof(message), "E' apparso un abitante delle segrete, un %s! Punti vita: %d, Dadi Attacco: %d, Dadi Difesa: %d",
             abitante->nome, abitante->punti_vita, abitante->dadi_attacco, abitante->dadi_difesa);
    printGameEvent(message, YELLOW);
    return abitante;
}

static void regenerateZoneSegrete()
{
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL)
    {
        current->tipoTesoro = getNumberByProbability((double[]){0.25, 0.25, 0.25, 0.25}, 4);
        current->tipoPorta = getNumberByProbability((double[]){0.33333, 0.33333, 0.33333}, 3);
        current = current->zona_successiva;
    }
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
        printf("\nInserisci il numero di giocatori (min 1, max 4): ");
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
static void initPlayers()
{
    for (int i = 0; i < playersNumber; i++)
    {
        clearScreen();
        printCustomHeader("CREAZIONE GIOCATORI");
        printf("\nGiocatore %d, configura il tuo personaggio :D\n\n", i + 1);

        printf("Inserisci il tuo nickname (MAX. %d caratteri): ", nicknameLength);
        fgets(giocatori[i].nome_giocatore, nicknameLength, stdin);
        size_t len = strlen(giocatori[i].nome_giocatore);
        if (len > 0 && giocatori[i].nome_giocatore[len - 1] == '\n')
        {
            giocatori[i].nome_giocatore[len - 1] = '\0';
        }

        clearScreen();
        printCustomHeader("CREAZIONE GIOCATORI");
        printf("\nScegli la tua classe:\n\n");
        printf("1. Barbaro (Attacco: 3, Difesa: 2, Vita: 8, Mente: 1-2, Potere Speciale: 0)\n");
        printf("2. Nano (Attacco: 2, Difesa: 2, Vita: 7, Mente: 2-3, Potere Speciale: 1)\n");
        printf("3. Elfo (Attacco: 2, Difesa: 2, Vita: 6, Mente: 3-4, Potere Speciale: 1)\n");
        printf("4. Mago (Attacco: 1, Difesa: 2, Vita: 4, Mente: 4-5, Potere Speciale: 3)\n");
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
            printf("\nHai scelto la classe Barbaro!\n");
            giocatori[i].dadi_attacco = 3;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 8;
            giocatori[i].punti_vita_max = 8;
            giocatori[i].mente = srandomNumberGenerator(1, 2);
            giocatori[i].potere_speciale = 0;
            break;
        case 2:
            printf("\nHai scelto la classe Nano!\n");
            giocatori[i].dadi_attacco = 2;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 7;
            giocatori[i].punti_vita_max = 7;
            giocatori[i].mente = srandomNumberGenerator(2, 3);
            giocatori[i].potere_speciale = 1;
            break;
        case 3:
            printf("\nHai scelto la classe Elfo!\n");
            giocatori[i].dadi_attacco = 2;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 6;
            giocatori[i].punti_vita_max = 6;
            giocatori[i].mente = srandomNumberGenerator(3, 4);
            giocatori[i].potere_speciale = 1;
            break;
        case 4:
            printf("\nHai scelto la classe Mago!\n");
            giocatori[i].dadi_attacco = 1;
            giocatori[i].dadi_difesa = 2;
            giocatori[i].p_vita = 4;
            giocatori[i].punti_vita_max = 4;
            giocatori[i].mente = srandomNumberGenerator(4, 5);
            giocatori[i].potere_speciale = 3;
            break;
        default:
            printf("\nErrore nella scelta della classe. :/ Riavvia il gioco\n");
            break;
        }

        clearScreen();
        printCustomHeader("CREAZIONE GIOCATORI");
        printf("\nFai una scelta!\n\n");
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

        printf("\n");
        switch (choice)
        {
        case 1:
            giocatori[i].p_vita++;
            giocatori[i].mente--;
            printf("Hai sacrificato un punto mente per +1 punto vita\n");
            break;
        case 2:
            if (giocatori[i].p_vita > 0)
            {
                giocatori[i].p_vita--;
            }
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
static Giocatore *setGameMaster()
{
    clearScreen();
    printCustomHeader("CREAZIONE GIOCATORI");
    printf("\nGiocatori! E' arrivato il momento di scegliere il game master:\n\n");
    size_t i;
    for (i = 0; i < playersNumber; i++)
    {
        printf("%zu. %s\n", i + 1, giocatori[i].nome_giocatore);
    }
    short int gameMaster;
    puts("\n");
    do
    {
        scanf("%hd", &gameMaster);
        while ((getchar()) != '\n')
            ;
    } while (gameMaster < 1 || gameMaster > playersNumber);

    return &giocatori[gameMaster - 1];
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
    double tipoZonaProbability[10] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    double tipoTesoroProbability[4] = {0.25, 0.25, 0.25, 0.25};
    double tipoPortaProbability[3] = {0.33333, 0.33333, 0.33333};

    zonaSegreta->tipoZona = getNumberByProbability(tipoZonaProbability, 10);
    zonaSegreta->tipoTesoro = getNumberByProbability(tipoTesoroProbability, 4);
    zonaSegreta->tipoPorta = getNumberByProbability(tipoPortaProbability, 3);
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
            fprintf(stderr, "Errore nell'allocazione della memoria per le zone segrete :/\n");
            exit(EXIT_FAILURE);
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
        printGameEvent("\nMappa generata con successo!\n", GREEN);
    }
    else
    {
        createZoneSegrete(&firstZonaSegretaLocal, &lastZonaSegretaLocal);
        lastZonaSegreta->zona_successiva = firstZonaSegretaLocal;
        printGameEvent("Sono state aggiunte ulteriori 15 zone alla mappa!\n", GREEN);
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
        printGameEvent("Posizione non valida.\n", RED);
        return;
    }

    if (firstZonaSegreta == NULL || lastZonaSegreta == NULL)
    {
        genera_mappa();
        return;
    }

    // Allocate memory for the new secret zone
    Zona_segrete *newZonaSegreta = (Zona_segrete *)malloc(sizeof(Zona_segrete));
    if (newZonaSegreta == NULL)
    {
        fprintf(stderr, "Errore nella creazione della zona segreta/\n");
        exit(EXIT_FAILURE);
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
        for (unsigned int j = 1; j < i - 1; j++)
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

    printGameEvent("Zona segreta inserita con successo!\n", GREEN);
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
    if (i > numberOfZonaSegrete)
    {
        printGameEvent("Posizione non valida.\n", RED);
        return;
    }

    if (firstZonaSegreta == NULL || lastZonaSegreta == NULL)
    {
        printGameEvent("La mappa e' vuota.\n", RED);
        return;
    }

    Zona_segrete *toDelete = firstZonaSegreta;
    for (unsigned int j = 1; j < i; j++)
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
    printGameEvent("Zona segreta cancellata con successo!\n", GREEN);
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
    int zoneCount = 1;
    printf("\n==================== MAPPA DEL GIOCO ====================\n");
    while (current != NULL)
    {
        printf("Zona %d:\n", zoneCount++);
        printf("----------------------------------------\n");
        printf("- Indirizzo Zona Attuale: %p\n", (void *)current);
        printf("- Tipo Zona: %s\n", tipo_zona_to_string(current->tipoZona));
        printf("- Tipo Tesoro: %s\n", tipo_tesoro_to_string(current->tipoTesoro));
        printf("- Tipo Porta: %s\n", tipo_porta_to_string(current->tipoPorta));
        printf("- Zona Precedente: %p\n", (void *)current->zona_precedente);
        printf("- Zona Successiva: %p\n", (void *)current->zona_successiva);
        printf("----------------------------------------\n");
        current = current->zona_successiva;
    }
    printf("========================================================\n");
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
    Zona_segrete *current = firstZonaSegreta;
    while (current != NULL)
    {
        count++;
        current = current->zona_successiva;
    }
    if (count < numberOfZonaSegrete)
    {
        printGameEvent("Genera altre zone segrete prima di chiudere la mappa.\n", RED);
        printMenuMappa();
        return;
    }
    else
    {
        isGameInitialized = true;
        clearScreen();
    }
}

/**
 * @brief Displays the menu for map operations.
 *
 * This function displays a menu of options for map operations, including generating the map,
 * inserting a new secret zone, deleting a secret zone, printing the map, and closing the 1
 * The user can choose an option by entering the corresponding number.
 */
void printMenuMappa()
{
    int choice;
    do
    {
        if (scanf("%d", &choice) != 1)
        {
            choice = -1;
            printGameEvent("Attenzione! Inserire un numero intero!", RED);
            clearInputBuffer();
        }
        else
        {
            switch (choice)
            {
            case 1:
                clearScreen();
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                genera_mappa();
                break;
            case 2:
                clearScreen();
                printf("Inserisci la posizione della zona: ");
                unsigned short int posizione;
                scanf("%hu", &posizione);
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                inserisci_zona(posizione);
                break;
            case 3:
                clearScreen();
                printf("Inserisci la posizione della zona da cancellare: ");
                unsigned short int posizioneCancella;
                scanf("%hu", &posizioneCancella);
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                cancella_zona(posizioneCancella);
                break;
            case 4:
                // clearScreen();
                stampa_mappa();
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                break;
            case 5:
                clearScreen();
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                chiudi_mappa();
                break;
            default:
                clearScreen();
                printCustomMenu(menuMappaName, optionsMenuMappa, numOptionsMappa);
                printGameEvent("Scelta non valida!", RED);
                break;
            }
        }
    } while (choice != 5 || choice == -1);
}

// ######################## GAME ########################

static bool getAllPlayersPlayed()
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

void printMenuGiocatore()
{
    int choice;
    bool passTurn = false;
    do
    {
        if (scanf("%d", &choice) != 1)
        {
            choice = -1;
            printGameEvent("Attenzione! Inserire un numero intero!", RED);
            clearInputBuffer();
        }
        else
        {
            switch (choice)
            {
            case 1:
                avanza();
                passTurn = true;
                break;
            case 2:
                clearScreen();
                printCustomMenu(menuGiocatoreName, optionsMenuGiocatore, numOptionsGiocatore);
                indietreggia();
                break;
            case 3:
                clearScreen();
                printCustomMenu(menuGiocatoreName, optionsMenuGiocatore, numOptionsGiocatore);
                stampa_giocatore();
                break;
            case 4:
                clearScreen();
                printCustomMenu(menuGiocatoreName, optionsMenuGiocatore, numOptionsGiocatore);
                stampa_zona();
                break;
            case 5:
                prendi_tesoro();
                passTurn = true;
                break;
            case 6:
                passTurn = true;
                break;
            default:
                printGameEvent("Scelta non valida!", RED);
                break;
            }
        }
    } while (!passTurn || choice == -1);
    if (passTurn)
        selectTurn();
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
static void selectTurn()
{
    unsigned short int turn = srandomNumberGenerator(0, playersNumber - 1);
    bool allPlayersPlayed = getAllPlayersPlayed();

    if (allPlayersPlayed)
    {
        regenerateZoneSegrete();
        for (int i = 0; i < playersNumber; i++)
        {
            turnsArray[i] = false;
        }
        turn = srandomNumberGenerator(0, playersNumber - 1);
    }
    else
    {
        while (turnsArray[turn] || playersDead[turn])
        {
            turn = srandomNumberGenerator(0, playersNumber - 1);
        }
    }
    turnsArray[turn] = true;
    actualTurn = turn;
    printCustomMenu(menuGiocatoreName, optionsMenuGiocatore, numOptionsGiocatore);
    char eventMessage[100];
    snprintf(eventMessage, sizeof(eventMessage), "%s e' il tuo turno!", giocatori[turn].nome_giocatore);
    printGameEvent(eventMessage, BLUE);
    printMenuGiocatore();
}

// static void passTurn()
// {
//     unsigned short int nextTurn = (actualTurn + 1) % playersNumber;
//     bool allPlayersPlayed = getAllPlayersPlayed();

//     if (allPlayersPlayed)
//     {
//         selectTurn();
//     }
//     else
//     {
//         while (turnsArray[nextTurn])
//         {
//             nextTurn = (nextTurn + 1) % playersNumber;
//         }
//         actualTurn = nextTurn;
//         turnsArray[actualTurn] = true;
//         printf("E' il turno di %s\n", giocatori[actualTurn].nome_giocatore);
//     }
// }

static void avanza()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    if (currentZone->zona_successiva == NULL)
    {
        printGameEvent("Non ci sono piu stanze in cui avanzare.", RED);
    }

    bool avanzato = apri_porta(currentZone->tipoPorta);

    if (!avanzato)
    {
        return;
    }

    playersCurrentZone[currentPlayer] = currentZone->zona_successiva;

    double probability[3] = {0.33, 0.67, 0.0}; // 33% probability for the first element
    if (getNumberByProbability(probability, 3) == 0 || playersCurrentZone[currentPlayer]->zona_successiva == NULL)
    {
        Abitante_segrete *abitante = figureAbitanteSegrete();
        combatti(abitante);
    }

        if(playersCurrentZone[currentPlayer] == lastZonaSegreta) {
        printGameEvent("Hai raggiunto l'ultima stanza delle segrete! Complimenti!", GREEN);
        termina_gioco();
        return;
    }
}

static void indietreggia()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    if (currentZone->zona_precedente == NULL)
    {
        printGameEvent("Non ci sono più stanze in cui avanzare.", RED);
        return;
    }

    playersCurrentZone[currentPlayer] = currentZone->zona_precedente;
    printGameEvent("Sei tornato nella stanza precedente.\n", BLUE);

    double probability[3] = {0.33, 0.67, 0.0}; // 33% probability for the first element
    if (getNumberByProbability(probability, 3) == 0)
    {
        Abitante_segrete *abitante = figureAbitanteSegrete();
        combatti(abitante);
    }
}

static void stampa_giocatore()
{
    unsigned short int currentPlayer = actualTurn;
    Giocatore *player = &giocatori[currentPlayer];

    printf("\n==================== GIOCATORE ====================\n");
    printf("Nome Giocatore: %s\n", player->nome_giocatore);
    printf("Classe: %s\n", player->classe);
    printf("Punti Vita: %d\n", player->p_vita);
    printf("Punti Mente: %d\n", player->mente);
    printf("Dadi Attacco: %d\n", player->dadi_attacco);
    printf("Dadi Difesa: %d\n", player->dadi_difesa);
    printf("Poteri Speciali: %d\n", player->potere_speciale);
    printf("===================================================\n");
}

static void stampa_zona()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    printf("\n==================== ZONA SEGRETA ====================\n");
    printf("Indirizzo Zona Attuale: %p\n", (void *)currentZone);
    printf("------------------------------------------------------\n");
    printf("Tipo Zona: %s\n", tipo_zona_to_string(currentZone->tipoZona));
    printf("Tesoro Presente: %s\n", currentZone->tipoTesoro != nessun_tesoro ? "Si" : "No");
    printf("Porta Presente: %s\n", currentZone->tipoPorta != nessuna_porta ? "Si" : "No");
    printf("------------------------------------------------------\n");
    printf("Zona Precedente: %p\n", (void *)currentZone->zona_precedente);
    printf("Zona Successiva: %p\n", (void *)currentZone->zona_successiva);
    printf("======================================================\n");
}

static bool apri_porta(enum tipo_porta tipoPorta)
{
    unsigned short int currentPlayer = actualTurn;

    if (tipoPorta == porta_da_scassinare)
    {
        unsigned short int mindRoll = diceThrow(1, 6, "Hai trovato una porta! Tira il dado per scassinarla...", false);

        char message[100];
        snprintf(message, sizeof(message), "E' uscito %hu.\n", mindRoll);
        printGameEvent(message, BLUE);

        if (mindRoll <= giocatori[currentPlayer].mente)
        {
            printGameEvent("Hai scassinato la porta con successo!", GREEN);
            return true;
        }
        else
        {
            unsigned short int outcome = getNumberByProbability((double[]){0.1, 0.5, 0.4}, 3);
            Abitante_segrete *abitante;
            switch (outcome)
            {
            case 0:
                printGameEvent("Non sei riuscito a scassinare la porta! Sfortuna! Devi ricominciare dalla prima stanza delle segrete.", RED);
                playersCurrentZone[currentPlayer] = firstZonaSegreta;
                break;
            case 1:
                printGameEvent("Non sei riuscito a scassinare la porta! Hai perso un punto vita.", RED);
                if (giocatori[currentPlayer].p_vita > 0)
                {
                    giocatori[currentPlayer].p_vita--;
                }
                if (giocatori[currentPlayer].p_vita <= 0)
                {
                    setDeadPlayer();
                }
                break;
            case 2:
                abitante = figureAbitanteSegrete();
                combatti(abitante);
                break;
            }
            return false;
        }
    }
    else if (tipoPorta == nessuna_porta)
    {
        printGameEvent("Non e' presente nessuna porta in questa stanza! Sei avanzato nella zona successiva!", GREEN);
        return true;
    }
    {
        printGameEvent("Hai aperto la porta e sei avanzato nella zona successiva!", GREEN);
        return true;
    }
}

static void prendi_tesoro()
{
    unsigned short int currentPlayer = actualTurn;
    Zona_segrete *currentZone = playersCurrentZone[currentPlayer];

    switch (currentZone->tipoTesoro)
    {
    case veleno:
        if (giocatori[currentPlayer].p_vita > 2)
        {
            giocatori[currentPlayer].p_vita -= 2;
        }
        else
        {
            giocatori[currentPlayer].p_vita = 0;
            setDeadPlayer();
        }
        printGameEvent("Hai preso veleno! Hai perso 2 punti vita.\n", RED);
        break;
    case guarigione:
        giocatori[currentPlayer].p_vita += 1;
        printGameEvent("Hai preso guarigione! Hai guadagnato 1 punto vita.\n", GREEN);
        break;
    case doppia_guarigione:
        giocatori[currentPlayer].p_vita += 2;
        printGameEvent("Hai preso doppia guarigione! Hai guadagnato 2 punti vita.\n", GREEN);
        break;
    default:
        printGameEvent("Non c'e' nessun tesoro in questa stanza.\n", YELLOW);
        return;
    }

    // Reset the treasure type to regenerate it for the next entry
    currentZone->tipoTesoro = nessun_tesoro;
}

static void scappa()
{
    unsigned short int currentPlayer = actualTurn;
    unsigned short int mindRoll = diceThrow(1, 6, "Hai trovato una via di fuga! Tira il dado per scappare...", false);

    char message[100];
    snprintf(message, sizeof(message), "Hai tirato %hu per scappare.\n", mindRoll);
    printGameEvent(message, BLUE);

    if (mindRoll <= giocatori[currentPlayer].mente)
    {
        printGameEvent("Sei riuscito a scappare! Sei tornato nella stanza precedente!\n", GREEN);
        playersCurrentZone[currentPlayer] = playersCurrentZone[currentPlayer]->zona_precedente;
    }
    else
    {
        printGameEvent("Non sei riuscito a scappare. Stai subendo un attacco!\n", RED);
        unsigned short int attackRoll = srandomNumberGenerator(1, 6);
        unsigned short int defenseRoll = diceThrow(1, 6, "Tira il dado per difenderti dall'attacco...", false);

        // snprintf(message, sizeof(message), "Hai tirato %hu per la difesa.\n", defenseRoll);
        // printGameEvent(message, BLUE);
        int damage = attackRoll - defenseRoll;

        if (damage > 0)
        {
            if (giocatori[currentPlayer].p_vita >= damage)
            {
                giocatori[currentPlayer].p_vita -= damage;
            }
            else
            {
                giocatori[currentPlayer].p_vita = 0;
                setDeadPlayer();
            }
            snprintf(message, sizeof(message), "Durante l'attacco hai perso %d punti vita.\n", damage);
            printGameEvent(message, RED);
        }
        else
        {
            printGameEvent("Non hai subito danni durante l'attacco.\n", GREEN);
        }
    }
}

// Funzione per mostrare una barra di vita in ASCII
void visualizzaBarraVita(char *nome, int vita_attuale, int vita_massima)
{
    int lunghezza_barra = 20;                                             // Lunghezza della barra di vita
    int lunghezza_vita = (vita_attuale * lunghezza_barra) / vita_massima; // Percentuale della barra

    printf("%s [", nome);
    for (int i = 0; i < lunghezza_barra; i++)
    {
        if (i < lunghezza_vita)
            printf("\033[0;32m#\033[0m");
        else
            printf(" "); // Parte della barra vuota
    }
    printf("] %d/%d\n", vita_attuale, vita_massima);
}

static bool getIfAllPlayersDead()
{
    for (int i = 0; i < playersNumber; i++)
    {
        if (giocatori[i].p_vita > 0)
        {
            return false;
            break;
        }
    }
    return true;
}

static void setDeadPlayer()
{
    char message[100];
    sprintf(message, "\n%s e' MORTO! Non puo piu partecipare al gioco!", giocatori[actualTurn].nome_giocatore);
    printGameEvent(message, RED);
    playersDead[actualTurn] = true;

    if (getIfAllPlayersDead())
    {
        printGameEvent("Tutti i giocatori sono morti. Il gioco e' finito!", RED);
        termina_gioco();
    }
}

// Funzione per lanciare il dado (animazione di lancio)
int lanciaDado()
{
    int result;
    printf("Lancio del dado...\n");
#ifdef _WIN32
    Sleep(500);
#else
    sleep(1);
#endif
    result = (rand() % 6) + 1; // Dado a 6 facce
    printf("Il dado ha mostrato: %d\n", result);
    return result;
}

// Funzione di combattimento dinamico
static bool combatti(Abitante_segrete *abitante)
{
    int choice;
    bool skipCombattimento = false;
    printCustomMenu(menuCombattimentoName, optionsMenuCombattimento, numOptionsCombattimento);
    do
    {
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            choice = -928;
            continue;
        case 2:
            scappa();
            return true;
        case 3:
            skipCombattimento = gioca_potere_speciale();
            if (skipCombattimento)
            {

                return true;
            }
            else
            {

                choice = !skipCombattimento ? -928 : 3;
            }
            break;
        default:
            printGameEvent("Scelta non valida.", RED);
            break;
        }
    } while ((choice > 1 && choice < 3));

    int scelta, teschiGiocatore, teschiAbitante, scudiGiocatore, scudiAbitante;
    bool pozioneCurativa = false;

    // Visualizzazione iniziale delle vite
    while (giocatori[actualTurn].p_vita > 0 && abitante->punti_vita > 0)
    {
        // Visualizzazione delle barre vita aggiornate all'inizio di ogni turno
        printf("\n=== Stato del Combattimento ===\n");
        visualizzaBarraVita("Giocatore", giocatori[actualTurn].p_vita, giocatori[actualTurn].punti_vita_max);
        visualizzaBarraVita("Abitante", abitante->punti_vita, abitante->punti_vita_max);
        printf("===============================\n");

        // Turno del giocatore: Scelta dell'azione
        printCustomMenu(menuAzioneName, optionsMenuAzione, numOptionsAzione);
        clearInputBuffer();
        scanf("%d", &scelta);
        teschiGiocatore = 0;
        scudiGiocatore = 0;

        switch (scelta)
        {
        case 1: // Attacco del giocatore
            printf("Hai scelto di attaccare!\n");
            for (int i = 0; i < giocatori[actualTurn].dadi_attacco; i++)
            {
                int dado = lanciaDado();
                if (dado <= 3)
                    teschiGiocatore++; // 3 facce teschio
            }
            printf("\nHai colpito %d volte!\n", teschiGiocatore);
            break;

        case 2: // Difesa del giocatore
            printf("Hai scelto di difenderti!\n");
            for (int i = 0; i < giocatori[actualTurn].dadi_difesa; i++)
            {
                int dado = lanciaDado();
                if (dado == 5 || dado == 6)
                    scudiGiocatore++; // Scudi bianchi
            }
            printf("\nHai parato %d colpi!\n", scudiGiocatore);
            break;
        case 3:
            if (!pozioneCurativa)
            {
                giocatori[actualTurn].p_vita += 4;
                pozioneCurativa = true;
            }
            printGameEvent("Hai ottenuto 4 punti vita aggiuntivi! Sfruttali a dovere!", GREEN);
            break;
        }

        // Turno dell'abitante
        printf("\n>>> Turno dell'Abitante <<<\n");
        teschiAbitante = 0;
        scudiAbitante = 0;

        for (int i = 0; i < abitante->dadi_attacco; i++)
        {
            int dado = lanciaDado();
            if (dado <= 3)
                teschiAbitante++; // 3 facce teschio
        }
        for (int i = 0; i < abitante->dadi_difesa; i++)
        {
            int dado = lanciaDado();
            if (dado == 6)
                scudiAbitante++; // Scudi neri
        }

        printf("\nL'abitante ha colpito %d volte e parato %d colpi!\n", teschiAbitante, scudiAbitante);
        puts("\n");
        // Calcolo danni
        if (teschiGiocatore > scudiAbitante)
        {
            abitante->punti_vita -= (teschiGiocatore - scudiAbitante);
            printf("L'abitante ha perso %d punti vita!\n", teschiGiocatore - scudiAbitante);
        }
        if (teschiAbitante > scudiGiocatore)
        {
            giocatori[actualTurn].p_vita -= (teschiAbitante - scudiGiocatore);
            printf("Hai perso %d punti vita!\n", teschiAbitante - scudiGiocatore);
        }

        // Verifica della fine del combattimento
        if (giocatori[actualTurn].p_vita <= 0)
        {
            printGameEvent("Hai perso il combattimento. Sei morto.", RED);
            setDeadPlayer();
            return false;
        }

        if (abitante->punti_vita <= 0)
        {
            printGameEvent("L'abitante delle segrete  morto!", GREEN);
            return true;
        }

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    return false;
}

static bool gioca_potere_speciale()
{
    unsigned short int currentPlayer = actualTurn;

    if (giocatori[currentPlayer].potere_speciale > 0)
    {
        giocatori[currentPlayer].potere_speciale--;
        printGameEvent("Hai usato il potere speciale per uccidere immediatamente l'abitante!", GREEN);
        return true;
    }
    else
    {
        printCustomMenu(menuCombattimentoName, optionsMenuCombattimento, numOptionsCombattimento);
        printGameEvent("Non hai piu' poteri speciali disponibili. Devi continuare il combattimento!", RED);
        return false;
    }
}

void gioca()
{
    if (!isGameInitialized)
    {
        printGameEvent("La mappa non e' stata creata. Impossibile iniziare il gioco.", RED);
        return;
    }

    for (int i = 0; i < playersNumber; i++)
    {
        playersCurrentZone[i] = firstZonaSegreta;
    }

    selectTurn();
}

void termina_gioco()
{
    // Banner di fine gioco con asterischi
    printGameEvent("**************************************************", GREEN);
    printGameEvent("*                                                *", GREEN);
    printGameEvent("*     Il gioco e' terminato!                      *", GREEN);
    printGameEvent("*     Grazie per aver giocato a Scalogna-Quest!  *", GREEN);
    printGameEvent("*                                                *", GREEN);
    printGameEvent("**************************************************", GREEN);

    if (gameWinner)
    {
        // Larghezza totale del riquadro
        int width = 50;
        // Lunghezza del testo "VINCITORE"
        int titleLength = strlen("VINCITORE");
        // Calcola il padding per centrare il titolo
        int titlePadding = (width - titleLength) / 2;

        // Stampa la sezione del vincitore con trattini
        printGameEvent("\n", RESET);
        printGameEvent("--------------------------------------------------", YELLOW);

        // Crea e stampa il titolo centrato
        char title[100];
        sprintf(title, "%*sVINCITORE%*s", titlePadding, "", titlePadding, "");
        printGameEvent(title, YELLOW);

        printGameEvent("--------------------------------------------------", YELLOW);

        // Calcola il padding per centrare il nome del vincitore
        int nameLength = strlen(giocatori[actualTurn].nome_giocatore);
        int namePadding = (width - nameLength) / 2;

        // Assicura che il padding sia almeno zero
        if (namePadding < 0)
            namePadding = 0;

        // Crea e stampa il nome del vincitore centrato
        char winnerMessage[100];
        sprintf(winnerMessage, "%*s%s%*s", namePadding, "", giocatori[actualTurn].nome_giocatore, namePadding, "");
        printGameEvent(winnerMessage, CYAN);

        printGameEvent("--------------------------------------------------", YELLOW);
    }
    return;
}
