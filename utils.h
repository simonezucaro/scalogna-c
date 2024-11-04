
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#endif

void printCustomMenu(const char *menuName, const char *options[], int numOptions);
void printCustomHeader(const char *headerText);
void clearScreen();
void clearInputBuffer();

typedef enum {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    RESET
} Color;
void printGameEvent(const char* str, Color color);
void printChar(char c, char righe[][200], int larghezza);
void printString(const char *str);
