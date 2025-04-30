#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "Dungeon.h"

int main(int argc, char* argv[]) {

    Player speler;
    Room* dungeon;
    int aantalKamers;
    char again, save, load;
    bool back, forward;

    while (1) {
        char buffer[100];

        //PRINTINTRO();
        PRINTHOME();
        sleep(1);

        printf("\n\n                          1. New game\n");
        printf("                          2. Load game\n");
        printf("                          3. Exit\n\n>");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            
            buffer[strcspn(buffer, "\n")] = '\0'; // Verwijder newline karakter
            
            if (strcmp(buffer, "1") == 0) {
                while (1)
                {                  
                    char buffer2[100];
                    CLEAR_SCREEN();
                    PRINTHOME();
                    printf("\nSelecteer een moeilijkheidsgraad:\n\n1. Apprentice dungeon (10 rooms)\n2. Journeyman dungeon (25 rooms)\n3. Master dungeon (50 rooms)\n\n4. Back\n\n> ");
                    if (fgets(buffer2, sizeof(buffer2), stdin) != NULL) {
                        buffer2[strcspn(buffer2, "\n")] = '\0';
                        if (strcmp(buffer2, "1") == 0) {
                            aantalKamers = 10;
                            forward = true;
                            break;
                        } else if (strcmp(buffer2, "2") == 0) {
                            aantalKamers = 25;
                            forward = true;
                            break;
                        } else if (strcmp(buffer2, "3") == 0) {
                            aantalKamers = 50;
                            forward = true;
                            break;
                        } else if (strcmp(buffer2, "4") == 0) {
                            break;
                        } else {
                            printf("\nOngeldige keuze. Probeer opnieuw.");
                            sleep(2);
                        }
                    }
                }
                if (forward) {
                    break;
                }
            }
            else if (strcmp(buffer, "2") == 0) {
                loadGameJson(&dungeon, &aantalKamers, "SAVEFILES/savegame.json");
                load = true;
                break;
            } else if (strcmp(buffer, "3") == 0) {
                freeDungeon(dungeon, aantalKamers);
                exit(0);
            } else {
                if (forward) {
                    forward = false;
                    break;
                }
                else {
                    printf("\nOngeldige keuze. Probeer opnieuw.");
                    sleep(2);
                }
            }
        }
    }

    if (load) {
        !load;
    } else {
        dungeon = generateDungeon(aantalKamers);

        speler.hp = 30;
        speler.damage = 5;
        speler.currentRoom = &dungeon[0];
    }


    playGame(&speler, dungeon,  aantalKamers);

    freeDungeon(dungeon, aantalKamers);

    return 0;
}