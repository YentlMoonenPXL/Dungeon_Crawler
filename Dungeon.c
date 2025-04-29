#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Dungeon.h"

Room* generateDungeon(int aantalKamers) {
    srand(time(NULL));

    Room* kamers = malloc(aantalKamers * sizeof(Room));
    if (!kamers) {
        printf("Memory allocatie mislukt!\n");
        exit(1);
    }

    // Initieer kamers
    for (int i = 0; i < aantalKamers; i++) {
        kamers[i].id = i;
        kamers[i].neighborCount = 0;
        kamers[i].neighbors = malloc(4 * sizeof(Room*)); // Maximaal 4 buren
        kamers[i].visited = 0;
        kamers[i].content = EMPTY;
    }

    // Verbind kamers met elkaar
    for (int i = 0; i < aantalKamers - 1; i++) {
        // Elke kamer krijgt minstens één verbinding naar de volgende
        kamers[i].neighbors[kamers[i].neighborCount++] = &kamers[i + 1];
        kamers[i + 1].neighbors[kamers[i + 1].neighborCount++] = &kamers[i];

        // Extra verbindingen toevoegen
        int extraDoors = rand() % 3; // 0, 1 of 2 extra deuren
        for (int j = 0; j < extraDoors; j++) {
            int target = rand() % aantalKamers;
            if (target != i && kamers[i].neighborCount < 4 && kamers[target].neighborCount < 4) {
                kamers[i].neighbors[kamers[i].neighborCount++] = &kamers[target];
                kamers[target].neighbors[kamers[target].neighborCount++] = &kamers[i];
            }
        }
    }

    // Inhoud toevoegen (monsters, items, schat)
    int treasurePlaced = 0;
    for (int i = 1; i < aantalKamers; i++) { // Startkamer blijft leeg
        int roll = rand() % 10;
        if (roll < 3) {
            // 30% kans op monster
            kamers[i].content = MONSTER;
            kamers[i].details.monster = malloc(sizeof(Monster));
            kamers[i].details.monster->type = rand() % 2 == 0 ? GOBLIN : TROLL;
            kamers[i].details.monster->hp = (kamers[i].details.monster->type == GOBLIN) ? 8 : 20;
            kamers[i].details.monster->damage = (kamers[i].details.monster->type == GOBLIN) ? 2 : 5;
        } else if (roll < 6) {
            // 30% kans op item
            kamers[i].content = ITEM;
            kamers[i].details.item = malloc(sizeof(Item));
            kamers[i].details.item->type = rand() % 2 == 0 ? HEAL_POTION : DAMAGE_BOOST;
            kamers[i].details.item->value = (kamers[i].details.item->type == HEAL_POTION) ? 5 : 2;
        } else if (!treasurePlaced) {
            // 10% kans eerste keer schat
            kamers[i].content = TREASURE;
            treasurePlaced = 1;
        }
    }

    // Indien geen schat is geplaatst, forceer het op een random kamer
    if (!treasurePlaced) {
        int treasureRoom = (rand() % (aantalKamers - 1)) + 1;
        kamers[treasureRoom].content = TREASURE;
    }

    return kamers;
}

void playGame(Player* speler, int aantalKamers) {
    CLEAR_SCREEN();
    printf("Welkom held! Je start in kamer %d.\n", speler->currentRoom->id);

    while (1) {
        Room* current = speler->currentRoom;

        PRINTSTATS(speler);

        printf("\nJe bent in kamer %d.\n", current->id);

        // Als de kamer nog niet bezocht is, verwerk inhoud
        if (!current->visited) {
            current->visited = 1;

            if (current->content == MONSTER) {
                printf("Er is een monster in de kamer!\n");

                Monster* monster = current->details.monster;
                while (monster->hp > 0 && speler->hp > 0) {
                    int aanval = rand() % 16; // Random getal tussen 0-15
                    printf("Aanval volgorde: ");
                    for (int i = 3; i >= 0; i--) {
                        int bit = (aanval >> i) & 1;
                        printf("%d", bit);
                        if (bit == 0) {
                            // Monster valt speler aan
                            speler->hp -= monster->damage;
                            printf(" (Monster doet %d damage) ", monster->damage);
                        } else {
                            // Speler valt monster aan
                            monster->hp -= speler->damage;
                            printf(" (Speler doet %d damage) ", speler->damage);
                        }
                    }
                    printf("\n");

                    // Toon actuele status
                    printf("Speler HP: %d\n", speler->hp > 0 ? speler->hp : 0);
                    printf("Monster HP: %d\n", monster->hp > 0 ? monster->hp : 0);

                    if (speler->hp <= 0) {
                        printf("Je bent gestorven...\n");
                        return;
                    } else if (monster->hp <= 0) {
                        printf("Je hebt het monster verslagen!\n");
                        free(current->details.monster);
                        current->content = EMPTY;
                    }
                }
            } else if (current->content == ITEM) {
                Item* item = current->details.item;
                printf("Je vindt een item: ");

                if (item->type == HEAL_POTION) {
                    printf("Heal Potion! (+%d HP)\n", item->value);
                    speler->hp += item->value;
                } else if (item->type == DAMAGE_BOOST) {
                    printf("Damage Boost! (+%d Damage)\n", item->value);
                    speler->damage += item->value;
                }
                free(current->details.item);
                current->content = EMPTY;
            } else if (current->content == TREASURE) {
                printf("Je hebt de schat gevonden! Je wint het spel!\n");
                return;
            } else {
                printf("De kamer is leeg.\n");
            }
        } else {
            printf("Je bent hier al geweest.\n");
        }

        // Toon beschikbare deuren
        printf("Deuren naar kamers: ");
        for (int i = 0; i < current->neighborCount; i++) {
            printf("%d ", current->neighbors[i]->id);
        }
        printf("\n");

        // Vraag speler welke deur
        int keuze;
        printf("Kies een kamer om naar toe te gaan: ");
        scanf("%d", &keuze);

        // Controleer of de keuze geldig is
        int geldig = 0;
        for (int i = 0; i < current->neighborCount; i++) {
            if (current->neighbors[i]->id == keuze) {
                speler->currentRoom = current->neighbors[i];
                geldig = 1;
                break;
            }
        }

        if (!geldig) {
            printf("Ongeldige keuze! Probeer opnieuw.\n");
        }
    }
}

void saveGameJson(Player* speler, Room* kamers, int aantalKamers, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Kan bestand niet openen om op te slaan.\n");
        return;
    }

    fprintf(file, "{\n");
    
    // Speler
    fprintf(file, "  \"player\": {\n");
    fprintf(file, "    \"hp\": %d,\n", speler->hp);
    fprintf(file, "    \"damage\": %d,\n", speler->damage);
    fprintf(file, "    \"currentRoom\": %d\n", speler->currentRoom->id);
    fprintf(file, "  },\n");

    // Kamers
    fprintf(file, "  \"rooms\": [\n");
    for (int i = 0; i < aantalKamers; i++) {
        Room* r = &kamers[i];
        fprintf(file, "    {\n");
        fprintf(file, "      \"id\": %d,\n", r->id);

        // Buren
        fprintf(file, "      \"neighbors\": [");
        for (int j = 0; j < r->neighborCount; j++) {
            fprintf(file, "%d", r->neighbors[j]->id);
            if (j != r->neighborCount - 1) fprintf(file, ", ");
        }
        fprintf(file, "],\n");

        // Content
        fprintf(file, "      \"content\": \"%s\",\n",
            r->content == EMPTY ? "EMPTY" :
            r->content == MONSTER ? "MONSTER" :
            r->content == ITEM ? "ITEM" : "TREASURE");

        if (r->content == MONSTER) {
            fprintf(file, "      \"monster\": {\n");
            fprintf(file, "        \"type\": \"%s\",\n", r->details.monster->type == GOBLIN ? "GOBLIN" : "TROLL");
            fprintf(file, "        \"hp\": %d,\n", r->details.monster->hp);
            fprintf(file, "        \"damage\": %d\n", r->details.monster->damage);
            fprintf(file, "      },\n");
        } else if (r->content == ITEM) {
            fprintf(file, "      \"item\": {\n");
            fprintf(file, "        \"type\": \"%s\",\n", r->details.item->type == HEAL_POTION ? "HEAL_POTION" : "DAMAGE_BOOST");
            fprintf(file, "        \"value\": %d\n", r->details.item->value);
            fprintf(file, "      },\n");
        }

        fprintf(file, "      \"visited\": %d\n", r->visited);
        fprintf(file, "    }%s\n", (i == aantalKamers - 1) ? "" : ",");
    }
    fprintf(file, "  ]\n");

    fprintf(file, "}\n");

    fclose(file);
    printf("Spel opgeslagen als JSON naar %s.\n", filename);
}

Player* loadGameJson(Room** kamersOut, int* aantalKamersOut, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Kan JSON-bestand niet openen.\n");
        return NULL;
    }

    Player* speler = malloc(sizeof(Player));
    int kamerTeller = 0;
    int maxKamers = 100;
    Room* kamers = malloc(maxKamers * sizeof(Room));

    Room* currentRoom = NULL;
    char line[512];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "\"hp\":")) {
            sscanf(line, " %*[^0-9]%d", &speler->hp);
        } else if (strstr(line, "\"damage\":")) {
            sscanf(line, " %*[^0-9]%d", &speler->damage);
        } else if (strstr(line, "\"currentRoom\":")) {
            int id;
            sscanf(line, " %*[^0-9]%d", &id);
            speler->currentRoom = &kamers[id];
        } else if (strstr(line, "\"id\":")) {
            int id;
            sscanf(line, " %*[^0-9]%d", &id);
            kamers[id].id = id;
            kamers[id].neighborCount = 0;
            kamers[id].neighbors = malloc(4 * sizeof(Room*));
            kamers[id].content = EMPTY;
            kamers[id].visited = 0;
            kamerTeller++;
            currentRoom = &kamers[id];
        } else if (strstr(line, "\"neighbors\":")) {
            char* ptr = strchr(line, '[') + 1;
            while (*ptr != ']') {
                int neighborID;
                sscanf(ptr, "%d", &neighborID);
                currentRoom->neighbors[currentRoom->neighborCount++] = &kamers[neighborID];
                while (*ptr && *ptr != ',' && *ptr != ']') ptr++;
                if (*ptr == ',') ptr++;
            }
        } else if (strstr(line, "\"content\":")) {
            if (strstr(line, "EMPTY")) currentRoom->content = EMPTY;
            else if (strstr(line, "MONSTER")) currentRoom->content = MONSTER;
            else if (strstr(line, "ITEM")) currentRoom->content = ITEM;
            else if (strstr(line, "TREASURE")) currentRoom->content = TREASURE;
        } else if (strstr(line, "\"monster\":")) {
            currentRoom->details.monster = malloc(sizeof(Monster));
        } else if (strstr(line, "\"item\":")) {
            currentRoom->details.item = malloc(sizeof(Item));
        } else if (currentRoom && currentRoom->content == MONSTER) {
            if (strstr(line, "\"type\":")) {
                if (strstr(line, "GOBLIN")) currentRoom->details.monster->type = GOBLIN;
                else currentRoom->details.monster->type = TROLL;
            } else if (strstr(line, "\"hp\":")) {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->hp);
            } else if (strstr(line, "\"damage\":")) {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->damage);
            }
        } else if (currentRoom && currentRoom->content == ITEM) {
            if (strstr(line, "\"type\":")) {
                if (strstr(line, "HEAL_POTION")) currentRoom->details.item->type = HEAL_POTION;
                else currentRoom->details.item->type = DAMAGE_BOOST;
            } else if (strstr(line, "\"value\":")) {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.item->value);
            }
        } else if (strstr(line, "\"visited\":")) {
            sscanf(line, " %*[^0-9]%d", &currentRoom->visited);
        }
    }

    fclose(file);

    *kamersOut = kamers;
    *aantalKamersOut = kamerTeller;

    printf("Spel succesvol geladen uit JSON.\n");
    return speler;
}

void freeDungeon(Room* kamers, int aantalKamers) {
    for (int i = 0; i < aantalKamers; i++) {
        // Eerst de inhoud van de kamer vrijmaken
        if (kamers[i].content == MONSTER && kamers[i].details.monster != NULL) {
            free(kamers[i].details.monster);
        } else if (kamers[i].content == ITEM && kamers[i].details.item != NULL) {
            free(kamers[i].details.item);
        }

        // Dan de array van neighbor pointers vrijmaken
        if (kamers[i].neighbors != NULL) {
            free(kamers[i].neighbors);
        }
    }

    // En als laatste de kamers zelf
    free(kamers);
}