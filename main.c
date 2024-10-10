#include <stdio.h>

void imposta_gioco();

void gioca();

void function3()
{
    printf("Hai scelto la funzione 3\n");
};

void printGameHeader() {
    
  _________             .__                                        ________                          __   
 /   _____/ ____ _____  |  |   ____   ____   ____ _____            \_____  \  __ __   ____   _______/  |_ 
 \_____  \_/ ___\\__  \ |  |  /  _ \ / ___\ /    \\__  \    ______  /  / \  \|  |  \_/ __ \ /  ___/\   __\
 /        \  \___ / __ \|  |_(  <_> ) /_/  >   |  \/ __ \_ /_____/ /   \_/.  \  |  /\  ___/ \___ \  |  |  
/_______  /\___  >____  /____/\____/\___  /|___|  (____  /         \_____\ \_/____/  \___  >____  > |__|  
        \/     \/     \/           /_____/      \/     \/                 \__>           \/     \/        

}

int main()
{
    

    int choice = 0;
    void (*f[3])() = {imposta_gioco, gioca, function3};

    do
    {
        printf("Inserisci la tua scelta: ");

        // If the user enters a non-integer value, the choice value will be set to -1
        // and the while loop will be executed again
        if (scanf("%d", &choice) != 1)
        {
            choice = -1;
            printf("Attenzione! Inserire un numero intero!\n");
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
            printf("Attenzione! Inserire un numero compreso tra 1 e 3!\n");
        }

        while ((getchar()) != '\n')
            ;
    } while ((choice >= 1 && choice <= 3) || (choice == -1));

    return 0;
}
