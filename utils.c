#include "utils.h"

void printCustomHeader(const char *headerText)
{
    int total_width = 27; // Aggiornato per corrispondere all'esempio
    int len_title = strlen(headerText);
    int inner_width = total_width - 2; // Sottraiamo i due asterischi ai bordi
    int spaces_total = inner_width - len_title;
    int left_padding = spaces_total / 2;
    int right_padding = spaces_total - left_padding;

    // Stampa la linea superiore
    for (int i = 0; i < total_width; i++)
    {
        printf("*");
    }
    printf("\n");

    // Stampa la linea centrale con il testo centrato
    printf("*");
    for (int i = 0; i < left_padding; i++)
    {
        printf(" ");
    }
    printf("%s", headerText);
    for (int i = 0; i < right_padding; i++)
    {
        printf(" ");
    }
    printf("*\n");

    // Stampa la linea inferiore
    for (int i = 0; i < total_width; i++)
    {
        printf("*");
    }
    printf("\n");
}

void printCustomMenu(const char *menuName, const char *options[], int numOptions)
{
    int i;
    int maxLength = strlen(menuName); // Lunghezza massima iniziale: il titolo del menu

    // Determina la lunghezza massima tra il titolo e le opzioni, includendo lo spazio per il numero
    for (i = 0; i < numOptions; i++)
    {
        int optionLength = strlen(options[i]) + 3; // +3 per "N. " davanti all'opzione
        if (optionLength > maxLength)
        {
            maxLength = optionLength;
        }
    }

    maxLength += 4; // Aggiunge spazio per il padding e la cornice

    // Stampa la linea superiore del menu
    printf("\n");
    for (i = 0; i < maxLength + 6; i++)
    {
        printf("*");
    }
    printf("\n");

    // Stampa il titolo del menu centrato
    printf("** ");
    int paddingLeft = (maxLength - strlen(menuName)) / 2;

    for (i = 0; i < paddingLeft; i++)
    {
        printf(" ");
    }
    printf("%s", menuName);
    for (i = 0; i < (maxLength - strlen(menuName) - paddingLeft); i++)
    {
        printf(" ");
    }
    printf(" **\n");

    // Stampa una linea di separazione
    for (i = 0; i < maxLength + 6; i++)
    {
        printf("*");
    }
    printf("\n");

    // Stampa le opzioni del menu
    for (i = 0; i < numOptions; i++)
    {
        printf("** %d. %s", i + 1, options[i]);
        int padding = maxLength - (strlen(options[i]) + 3); // +3 per l'indice numerico
        for (int j = 0; j < padding; j++)
        {
            printf(" ");
        }
        printf(" **\n");
    }

    // Stampa la linea inferiore del menu
    for (i = 0; i < maxLength + 6; i++)
    {
        printf("*");
    }
    printf("\n\n");
}

void clearScreen()
{
    printf("\033[2J\033[H");
}

void printGameEvent(const char *str, Color color)
{
#ifdef _WIN32
    // Implementazione specifica per Windows
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    // Salva gli attributi correnti della console
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    // Mappa l'enum Color agli attributi della console di Windows
    WORD wColor = 0;
    switch (color)
    {
    case BLACK:
        wColor = 0;
        break;
    case RED:
        wColor = FOREGROUND_RED;
        break;
    case GREEN:
        wColor = FOREGROUND_GREEN;
        break;
    case YELLOW:
        wColor = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    case BLUE:
        wColor = FOREGROUND_BLUE;
        break;
    case MAGENTA:
        wColor = FOREGROUND_RED | FOREGROUND_BLUE;
        break;
    case CYAN:
        wColor = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case WHITE:
        wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case RESET:
        wColor = saved_attributes;
        break;
    }

    // Imposta il colore del testo (aggiunge intensità per colori brillanti)
    SetConsoleTextAttribute(hConsole, wColor | FOREGROUND_INTENSITY);

    // Stampa la stringa
    printf("%s", str);

    // Ripristina gli attributi originali della console
    SetConsoleTextAttribute(hConsole, saved_attributes);
#else
    // I sistemi Unix-like utilizzano i codici di escape ANSI
    const char *color_codes[] = {
        "\033[0;30m", // BLACK
        "\033[0;31m", // RED
        "\033[0;32m", // GREEN
        "\033[0;33m", // YELLOW
        "\033[0;34m", // BLUE
        "\033[0;35m", // MAGENTA
        "\033[0;36m", // CYAN
        "\033[0;37m", // WHITE
        "\033[0m"     // RESET
    };

    // Stampa la stringa con il colore selezionato
    printf("%s%s\033[0m", color_codes[color], str);
#endif
    puts("\n");
}

void clearInputBuffer()
{
#ifdef _WIN32
    while (getchar() != '\n')
        ;
#else
    fflush(stdin);
#endif
}

void printChar(char c, char righe[][200], int larghezza)
{
    switch (c)
    {
    case 'S':
    case 's':
        strcpy(righe[0] + larghezza, "   0000000   ");
        strcpy(righe[1] + larghezza, "  00      00 ");
        strcpy(righe[2] + larghezza, " 00           ");
        strcpy(righe[3] + larghezza, "  0000000     ");
        strcpy(righe[4] + larghezza, "         00  ");
        strcpy(righe[5] + larghezza, "  00      00 ");
        strcpy(righe[6] + larghezza, "   0000000   ");
        break;

    case 'C':
    case 'c':
        strcpy(righe[0] + larghezza, "   0000000   ");
        strcpy(righe[1] + larghezza, "  00      00 ");
        strcpy(righe[2] + larghezza, " 00          ");
        strcpy(righe[3] + larghezza, " 00          ");
        strcpy(righe[4] + larghezza, " 00          ");
        strcpy(righe[5] + larghezza, "  00      00 ");
        strcpy(righe[6] + larghezza, "   0000000   ");
        break;

    case 'A':
    case 'a':
        strcpy(righe[0] + larghezza, "     0000    ");
        strcpy(righe[1] + larghezza, "    00  00   ");
        strcpy(righe[2] + larghezza, "   00    00  ");
        strcpy(righe[3] + larghezza, "  0000000000 ");
        strcpy(righe[4] + larghezza, " 00        00");
        strcpy(righe[5] + larghezza, "00          00");
        strcpy(righe[6] + larghezza, "00          00");
        break;

    case 'L':
    case 'l':
        strcpy(righe[0] + larghezza, "00           ");
        strcpy(righe[1] + larghezza, "00           ");
        strcpy(righe[2] + larghezza, "00           ");
        strcpy(righe[3] + larghezza, "00           ");
        strcpy(righe[4] + larghezza, "00           ");
        strcpy(righe[5] + larghezza, "00           ");
        strcpy(righe[6] + larghezza, " 0000000000  ");
        break;

    case 'O':
    case 'o':
        strcpy(righe[0] + larghezza, "   0000000   ");
        strcpy(righe[1] + larghezza, "  00      00 ");
        strcpy(righe[2] + larghezza, " 00        00");
        strcpy(righe[3] + larghezza, " 00        00");
        strcpy(righe[4] + larghezza, " 00        00");
        strcpy(righe[5] + larghezza, "  00      00 ");
        strcpy(righe[6] + larghezza, "   0000000   ");
        break;

    case 'G':
    case 'g':
        strcpy(righe[0] + larghezza, "   0000000   ");
        strcpy(righe[1] + larghezza, "  00      00 ");
        strcpy(righe[2] + larghezza, " 00          ");
        strcpy(righe[3] + larghezza, " 00    00000 ");
        strcpy(righe[4] + larghezza, " 00       00 ");
        strcpy(righe[5] + larghezza, "  00      00 ");
        strcpy(righe[6] + larghezza, "   0000000   ");
        break;

    case 'N':
    case 'n':
        strcpy(righe[0] + larghezza, "00         00");
        strcpy(righe[1] + larghezza, "0000       00");
        strcpy(righe[2] + larghezza, "00  00     00");
        strcpy(righe[3] + larghezza, "00   00    00");
        strcpy(righe[4] + larghezza, "00    00   00");
        strcpy(righe[5] + larghezza, "00      00 00");
        strcpy(righe[6] + larghezza, "00       0000");
        break;

    case 'Q':
    case 'q':
        strcpy(righe[0] + larghezza, "   0000000   ");
        strcpy(righe[1] + larghezza, "  00      00 ");
        strcpy(righe[2] + larghezza, " 00        00");
        strcpy(righe[3] + larghezza, " 00    00  00");
        strcpy(righe[4] + larghezza, " 00      00  ");
        strcpy(righe[5] + larghezza, "  00     000 ");
        strcpy(righe[6] + larghezza, "   0000000  00");
        break;

    case 'U':
    case 'u':
        strcpy(righe[0] + larghezza, "00          00");
        strcpy(righe[1] + larghezza, "00          00");
        strcpy(righe[2] + larghezza, "00          00");
        strcpy(righe[3] + larghezza, "00          00");
        strcpy(righe[4] + larghezza, "00          00");
        strcpy(righe[5] + larghezza, " 00        00 ");
        strcpy(righe[6] + larghezza, "   0000000    ");
        break;

    case 'E':
    case 'e':
        strcpy(righe[0] + larghezza, " 0000000000  ");
        strcpy(righe[1] + larghezza, "00           ");
        strcpy(righe[2] + larghezza, "00           ");
        strcpy(righe[3] + larghezza, "000000000    ");
        strcpy(righe[4] + larghezza, "00           ");
        strcpy(righe[5] + larghezza, "00           ");
        strcpy(righe[6] + larghezza, " 0000000000  ");
        break;

    case 'T':
    case 't':
        strcpy(righe[0] + larghezza, "000000000000 ");
        strcpy(righe[1] + larghezza, "     00      ");
        strcpy(righe[2] + larghezza, "     00      ");
        strcpy(righe[3] + larghezza, "     00      ");
        strcpy(righe[4] + larghezza, "     00      ");
        strcpy(righe[5] + larghezza, "     00      ");
        strcpy(righe[6] + larghezza, "     00      ");
        break;

    default:
        for (int i = 0; i < 7; i++)
        {
            strcpy(righe[i] + larghezza, "            ");
        }
        break;
    }
}

void printString(const char *str)
{
    char righe[7][200] = {{0}}; // Buffer per contenere tutte le lettere in una singola riga orizzontale
    int larghezza = 0;

    // Itera su ogni carattere della stringa e aggiungi le sue "righe" al buffer
    for (int i = 0; i < strlen(str); i++)
    {
        printChar(str[i], righe, larghezza);
        larghezza += 13; // Spaziatura tra i caratteri
    }

    // Stampa ogni riga
    for (int i = 0; i < 7; i++)
    {
        printf("%s\n", righe[i]);
    }
}