#include <stdio.h>
#include <stdlib.h>

void imposta_gioco();

void function1()
{
    printf("Hai scelto la funzione 1\n");
};

void function2()
{
    printf("Hai scelto la funzione 2\n");
};

void function3()
{
    printf("Hai scelto la funzione 3\n");
};


int main()
{

    int choice = 0;
    void (*f[3])() = {imposta_gioco, function2, function3};

    do
    {
        printf("Inserisci la tua scelta: ");

        // If the user enters a non-integer value, the choice value will be set to -1
        // and the while loop will be executed again
        if (scanf("%d", &choice) != 1)
        {
            choice = -1;
            printf("Attenzione! Inserire un numero intero!\n");
        } else if(choice >= 1 && choice <= 3) {
            (*f[choice - 1])();
        } else {
            // If the user enters a value that is not between 1 and 3, the choice value will be set to -1
            // and the while loop will be executed again
            choice = -1;
            printf("Attenzione! Inserire un numero compreso tra 1 e 3!\n");
        
        }

        while ((getchar()) != '\n');
    } while ((choice >= 1 && choice <= 3 ) || (choice == -1));

    return 0;
}
