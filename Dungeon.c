#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

    for (int i = 0; i < aantalKamers; i++) {
        kamers[i].id = i;
        kamers[i].neighborCount = 0;
        kamers[i].neighbors = calloc(4, sizeof(Room*));
        for (int j = 0; j < 4; j++) kamers[i].neighbors[j] = NULL;
        kamers[i].visited = 0;
        kamers[i].content = EMPTY;
    }

    // Verbonden kamers bijhouden
    int* verbonden = calloc(aantalKamers, sizeof(int));
    verbonden[0] = 0;
    int verbondenCount = 1;

    // Verbind alle kamers startend van 0
    for (int i = 1; i < aantalKamers; i++) {
        int bronIndex = rand() % verbondenCount;
        int bronID = verbonden[bronIndex];

        if (
            kamers[i].neighborCount < 4 &&
            kamers[bronID].neighborCount < 4 &&
            !alVerbonden(&kamers[i], &kamers[bronID])
        ) {
            kamers[i].neighbors[kamers[i].neighborCount++] = &kamers[bronID];
            kamers[bronID].neighbors[kamers[bronID].neighborCount++] = &kamers[i];
        }

        verbonden[verbondenCount++] = i;
    }

    // Extra verbindingen toevoegen
    for (int i = 0; i < aantalKamers; i++) {
        int extra = rand() % 3; // 0–2 extra verbindingen
        for (int j = 0; j < extra; j++) {
            int target = rand() % aantalKamers;
            if (target != i &&
                kamers[i].neighborCount < 4 &&
                kamers[target].neighborCount < 4 &&
                !alVerbonden(&kamers[i], &kamers[target])
            ) {
                kamers[i].neighbors[kamers[i].neighborCount++] = &kamers[target];
                kamers[target].neighbors[kamers[target].neighborCount++] = &kamers[i];
            }
        }
    }

    // Inhoud toevoegen
    for (int i = 1; i < aantalKamers; i++) {
        int roll = rand() % 10;
        if (roll < 3) {
            kamers[i].content = MONSTER;
            kamers[i].details.monster = malloc(sizeof(Monster));
            Monster* m = kamers[i].details.monster;
            m->type = rand() % 2 == 0 ? GOBLIN : TROLL;
            m->hp = (m->type == GOBLIN) ? 9 : 21;
            m->damage = (m->type == GOBLIN) ? 3 : 5;
        } else if (roll < 6) {
            kamers[i].content = ITEM;
            kamers[i].details.item = malloc(sizeof(Item));
            Item* item = kamers[i].details.item;
            item->type = rand() % 2 == 0 ? HEAL_POTION : DAMAGE_BOOST;
            item->value = (item->type == HEAL_POTION) ? 5 : 2;
        }
    }

    // Schat in willekeurige kamer (≠ 0)
    int treasureRoom = (rand() % (aantalKamers - 1)) + 1;
    kamers[treasureRoom].content = TREASURE;

    free(verbonden);
    return kamers;
}

int alVerbonden(Room* kamer, Room* target) {
    for (int i = 0; i < kamer->neighborCount; i++) {
        if (kamer->neighbors[i] == target) {
            return 1;
        }
    }
    return 0;
}

void playGame(Player* speler, Room* kamers, int aantalKamers) {
    while (1) {
        Room* current = speler->currentRoom;
        PRINTSTATS(speler, kamers, aantalKamers);

        // Verwerk kamerinhoud bij eerste bezoek
        if (!current->visited) {
            current->visited = 1;

            switch (current->content) {
                case MONSTER: {
                    Monster* monster = current->details.monster;
                    PRINTMONSTER(monster);

                    while (monster->hp > 0 && speler->hp > 0) {
                        sleep(2);
                        PRINTSTATS(speler, kamers, aantalKamers);
                        PRINTMONSTER(monster);

                        int aanval = rand() % 16;
                        printf("Je valt aan! Aanval volgorde:\n");

                        for (int i = 3; i >= 0; i--) {
                            sleep(1);
                            int bit = (aanval >> i) & 1;
                            printf("%d", bit);

                            if (bit == 0) {
                                speler->hp -= monster->damage;
                                printf(" (Monster doet %d damage)\n", monster->damage);
                            } else {
                                monster->hp -= speler->damage;
                                printf(" (Speler doet %d damage)\n", speler->damage);
                            }
                        }

                        sleep(3);
                        PRINTSTATS(speler, kamers, aantalKamers);
                        PRINTMONSTER(monster);
                    }

                    if (speler->hp <= 0) {
                        printf("\nJe bent gestorven...\n\n");
                        return;
                    }

                    if (monster->hp <= 0) {
                        printf("\nJe hebt het monster verslagen!\n\n");
                        free(monster);
                        current->content = EMPTY;
                    }
                    break;
                }
                case ITEM: {
                    Item* item = current->details.item;
                    printf("🎁 Je vindt een item: ");

                    if (item->type == HEAL_POTION) {
                        printf("Heal Potion! (+%d HP)\n\n", item->value);
                        speler->hp += item->value;
                    } else {
                        printf("Damage Boost! (+%d Damage)\n\n", item->value);
                        speler->damage += item->value;
                    }

                    free(item);
                    current->content = EMPTY;
                    break;
                }
                case TREASURE:
                    PRINTTREASURE();
                    printf("\nJe hebt de schat gevonden! Je wint het spel!\n");
                    return;

                case EMPTY:
                default:
                    printf("De kamer is leeg.\n\n");
                    break;
            }
        } else {
            printf("Je bent hier al geweest.\n\n");
        }

        // Toon beschikbare deuren
        printf("Deuren naar kamers: ");
        for (int i = 0; i < current->neighborCount; i++) {
            printf("%d ", current->neighbors[i]->id);
        }

        char input[10];
        printf("\nKies een kamer om naar toe te gaan (of typ 's' om op te slaan):\n\n> ");
        scanf("%s", input);

        if (strcmp(input, "s") == 0) {
            saveGameJson(speler, kamers, aantalKamers, "SAVEFILES/savegame.json");
            sleep(2);
            continue;
        }

        int keuze = atoi(input);
        int geldig = 0;

        for (int i = 0; i < current->neighborCount; i++) {
            if (current->neighbors[i]->id == keuze) {
                speler->currentRoom = current->neighbors[i];
                geldig = 1;
                break;
            }
        }

        if (!geldig) {
            printf("❌ Ongeldige keuze! Probeer opnieuw.\n");
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
    printf("\n✅ Spel opgeslagen als JSON naar %s.", filename);
}

Player* loadGameJson(Room** kamersOut, int* aantalKamersOut, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("❌ Kan JSON-bestand niet openen: %s\n", filename);
        return NULL;
    }

    Player* speler = malloc(sizeof(Player));
    int maxKamers = 50;
    Room* kamers = calloc(maxKamers, sizeof(Room));
    Room* currentRoom = NULL;
    int laatstGelezenID = -1;
    int currentRoomId = -1;

    int inMonster = 0;
    int inItem = 0;

    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "\"hp\":")) {
            if (!inMonster) {
                sscanf(line, " %*[^0-9]%d", &speler->hp);
            } else {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->hp);
            }
        } else if (strstr(line, "\"damage\":")) {
            if (!inMonster) {
                sscanf(line, " %*[^0-9]%d", &speler->damage);
            } else {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->damage);
            }
        } else if (strstr(line, "\"currentRoom\":")) {
            sscanf(line, " %*[^0-9]%d", &currentRoomId);
        } else if (strstr(line, "\"id\":")) {
            inMonster = inItem = 0;
            int id;
            sscanf(line, " %*[^0-9]%d", &id);
            kamers[id].id = id;
            kamers[id].neighborCount = 0;
            kamers[id].neighbors = calloc(4, sizeof(Room*));
            kamers[id].content = EMPTY;
            kamers[id].visited = 0;
            currentRoom = &kamers[id];
            if (id > laatstGelezenID) laatstGelezenID = id;
        } else if (strstr(line, "\"neighbors\":")) {
            char* ptr = strchr(line, '[') + 1;
            while (ptr && *ptr && *ptr != ']') {
                int neighborID;
                if (sscanf(ptr, "%d", &neighborID) == 1) {
                    currentRoom->neighbors[currentRoom->neighborCount++] = &kamers[neighborID];
                }
                while (*ptr && *ptr != ',' && *ptr != ']') ptr++;
                if (*ptr == ',') ptr++;
            }
        } else if (strstr(line, "\"content\":")) {
            inMonster = inItem = 0;
            if (strstr(line, "EMPTY")) currentRoom->content = EMPTY;
            else if (strstr(line, "MONSTER")) {
                currentRoom->content = MONSTER;
                currentRoom->details.monster = malloc(sizeof(Monster));
                inMonster = 1;
            }
            else if (strstr(line, "ITEM")) {
                currentRoom->content = ITEM;
                currentRoom->details.item = malloc(sizeof(Item));
                inItem = 1;
            }
            else if (strstr(line, "TREASURE")) currentRoom->content = TREASURE;
        } else if (inMonster && currentRoom && currentRoom->details.monster) {
            if (strstr(line, "\"type\":")) {
                if (strstr(line, "GOBLIN")) currentRoom->details.monster->type = GOBLIN;
                else currentRoom->details.monster->type = TROLL;
            } else if (strstr(line, "\"hp\":")) {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->hp);
            } else if (strstr(line, "\"damage\":")) {
                sscanf(line, " %*[^0-9]%d", &currentRoom->details.monster->damage);
            }
        } else if (inItem && currentRoom && currentRoom->details.item) {
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

    speler->currentRoom = &kamers[currentRoomId];
    *kamersOut = kamers;
    *aantalKamersOut = laatstGelezenID + 1;

    CLEAR_SCREEN();
    printf("\n✅ Spel succesvol geladen uit %s.", filename);
    sleep(2);
    return speler;
}

void freeDungeon(Room* kamers, int aantalKamers, Player* speler) {
    for (int i = 0; i < aantalKamers; i++) {
        switch (kamers[i].content) {
            case MONSTER:
                if (kamers[i].details.monster) {
                    free(kamers[i].details.monster);
                }
                break;
            case ITEM:
                if (kamers[i].details.item) {
                    free(kamers[i].details.item);
                }
                break;
            default:
                break;
        }
        free(kamers[i].neighbors);
    }
    if (speler != NULL) {
        free(speler);
    }
    free(kamers);
}
