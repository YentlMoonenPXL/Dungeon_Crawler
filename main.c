#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "Dungeon.h"

#ifdef _WIN32
#include <windows.h> // Speciaal voor output met emojis te krijgen bij run met ./main.exe + arg
#endif

int kiesAantalKamers() {
    char keuze[100];
    while (1) {
        PRINTHOME();
        printf("\n\nSelecteer een moeilijkheidsgraad:\n\n");
        printf("1. Apprentice dungeon (10 rooms)\n");
        printf("2. Journeyman dungeon (25 rooms)\n");
        printf("3. Master dungeon (50 rooms)\n");
        printf("\n4. Back\n\n> ");

        if (fgets(keuze, sizeof(keuze), stdin)) {
            keuze[strcspn(keuze, "\n")] = '\0';

            if (strcmp(keuze, "1") == 0) return 10;
            if (strcmp(keuze, "2") == 0) return 25;
            if (strcmp(keuze, "3") == 0) return 50;
            if (strcmp(keuze, "4") == 0) return 0;

            printf("\nOngeldige keuze. Probeer opnieuw.");
            sleep(2);
        }
    }
}

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    Player speler;
    Player* loadSpeler = NULL;
    Room* dungeon = NULL;
    int aantalKamers = 0;
    char again;
    bool loadGame = false;

    if (argc == 2) { // Als er een argument (nummer of savefile) mee gegeven wordt met ./main.exe
        // In powershell terminal voer eerst "[Console]::OutputEncoding = [System.Text.UTF8Encoding]::UTF8" uit
        int mogelijkAantal = atoi(argv[1]);

        if (mogelijkAantal > 0) {
            // Genereer nieuwe dungeon met gegeven aantal kamers
            aantalKamers = mogelijkAantal;
            dungeon = generateDungeon(aantalKamers);

            speler.hp = 30;
            speler.damage = 5;
            speler.currentRoom = &dungeon[0];
            
            playGame(&speler, dungeon, aantalKamers);
            freeDungeon(dungeon, aantalKamers, NULL);
            return 0;
        } else {
            // Probeer te laden van savebestand
            loadSpeler = loadGameJson(&dungeon, &aantalKamers, argv[1]);
            if (loadSpeler) {
                playGame(loadSpeler, dungeon, aantalKamers);
                freeDungeon(dungeon, aantalKamers, loadSpeler);
                return 0;
            } else {
                printf("❌ Kon bestand \"%s\" niet laden.\n\n", argv[1]);
                return 1;
            }
        }
    }

    //PRINTINTRO();
    startmenu:
    do // Lus voor terug naar home te gaan of spel af te sluiten na het einde van een run.
    {    
        loadGame = false;
        PRINTHOME();
        sleep(1);

        char keuze[100];
        printf("\n\n                          1. New game\n");
        printf("                          2. Load game\n");
        printf("                          3. Exit\n\n> ");

        if (fgets(keuze, sizeof(keuze), stdin)) {
            keuze[strcspn(keuze, "\n")] = '\0';

            if (strcmp(keuze, "1") == 0) {
                aantalKamers = kiesAantalKamers();
                if (aantalKamers == 0) goto startmenu;
            } else if (strcmp(keuze, "2") == 0) {
                loadSpeler = loadGameJson(&dungeon, &aantalKamers, "SAVEFILES/savegame.json");
                loadGame = true;
            } else if (strcmp(keuze, "3") == 0) {
                return 0;
            } else {
                printf("\nOngeldige keuze. Probeer opnieuw.\n");
                sleep(2);
                continue;
            }
        }
        
        if (loadGame) {
            playGame(loadSpeler, dungeon, aantalKamers);
        } else {
            dungeon = generateDungeon(aantalKamers);
            speler.hp = 30;
            speler.damage = 5;
            speler.currentRoom = &dungeon[0];
            playGame(&speler, dungeon, aantalKamers);
        }

        printf("Ga terug naar het hoofdmenu? (y/n)\n\n> ");
        scanf(" %c", &again);
        int c;
        while ((c = getchar()) != '\n' && c != EOF); // Clear input buffer

        freeDungeon(dungeon, aantalKamers, loadGame ? loadSpeler : NULL);

    } while (again == 'y' || again == 'Y');

    return 0;
}