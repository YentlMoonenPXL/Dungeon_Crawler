#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "Dungeon.h"

int main(int argc, char* argv[]) {
    Player speler;
    Player* loadSpeler = NULL;
    Room* dungeon = NULL;
    int aantalKamers = 0;
    char again, save;
    bool load = false, back = false, forward = false;

    do //lus voor terug naar home te gaan of spel af te sluiten na het einde van een run.
    {    
        forward = false;
        load = false;

        while (1) {
            char buffer[100];
            char buffer2[100];

            //PRINTINTRO();
            PRINTHOME();
            sleep(1);

            printf("\n\n                          1. New game\n");
            printf("                          2. Load game\n");
            printf("                          3. Exit\n\n>");

            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                buffer[strcspn(buffer, "\n")] = '\0'; // Verwijder newline karakter (buffer leegmaken)
                
                if (strcmp(buffer, "1") == 0) {
                    while (1)
                    {                  
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
                    loadSpeler = loadGameJson(&dungeon, &aantalKamers, "SAVEFILES/savegame.json");
                    load = true;
                    break;
                } else if (strcmp(buffer, "3") == 0) {
                    exit(0);
                } else {
                    printf("\nOngeldige keuze. Probeer opnieuw.");
                    sleep(2);
                }
            }
        }
        
        if (load) {
            playGame(loadSpeler, dungeon, aantalKamers);
        } else {
            dungeon = generateDungeon(aantalKamers);

            speler.hp = 30;
            speler.damage = 5;
            speler.currentRoom = &dungeon[0];

            playGame(&speler, dungeon, aantalKamers);
        }

        printf("Ga terug naar het hoofdmenu? (y/n)\n\n>");
        scanf(" %c", &again);
        int c;
        while ((c = getchar()) != '\n' && c != EOF); //clear input buffer

        freeDungeon(dungeon, aantalKamers, loadSpeler);
        dungeon = NULL;
        loadSpeler = NULL;

    } while (again == 'y' || again == 'Y');

    return 0;
}