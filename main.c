#include "gamelib.h"

const char *menuName = "Main Menu";
const char *options[] = {
    "Imposta gioco",
    "Gioca",
    "Termina gioco"};
int numOptions = sizeof(options) / sizeof(options[0]);

int main()
{
    printString("SCALOGNA");
    printString("QUEST");

    int choice = 0;
    void (*f[3])() = {imposta_gioco, gioca, termina_gioco};

    do
    {
        printCustomMenu(menuName, options, numOptions);

        // If the user enters a non-integer value, the choice value will be set to -1
        // and the while loop will be executed again
        if (scanf("%d", &choice) != 1)
        {
            choice = -1;
            printGameEvent("Attenzione! Inserire un numero intero!", RED);
        }
        else if (choice >= 1 && choice <= 3)
        {
            (*f[choice - 1])();
        }
        else
        {
            // If the user enters a value that is not between 1 and 3, the choice value will be set to -1
            // and the while loop will be executed again
            choice = -1;
            printGameEvent("Attenzione! Inserire un numero compreso tra 1 e 3!", RED);
        }

        while ((getchar()) != '\n')
            ;
    } while ((choice >= 1 && choice <= 3) || (choice == -1));

    return 0;
}
