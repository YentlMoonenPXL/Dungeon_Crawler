#ifndef DUNGEON_H
#define DUNGEON_H

// Utility
#define CLEAR_SCREEN() printf("\e[1;1H\e[2J")

// Enumeraties
typedef enum { EMPTY, MONSTER, ITEM, TREASURE } RoomContent;
typedef enum { GOBLIN, TROLL } MonsterType;
typedef enum { HEAL_POTION, DAMAGE_BOOST } ItemType;

// Structs
typedef struct Monster {
    MonsterType type;
    int hp;
    int damage;
} Monster;

typedef struct Item {
    ItemType type;
    int value; // hoeveel heal of hoeveel damage boost
} Item;

typedef struct Room {
    int id;
    struct Room** neighbors; // array van pointers naar buren (max 4)
    int neighborCount;
    RoomContent content;
    union {
        Monster* monster;
        Item* item;
    } details;
    int visited;
} Room;

typedef struct Player {
    int hp;
    int damage;
    Room* currentRoom;
} Player;

// Functiedeclaraties
// Dungeonbeheer
Room* generateDungeon(int aantalKamers);
void playGame(Player* speler, Room* kamers, int aantalKamers);
void freeDungeon(Room* startRoom, int aantalKamers, Player* loadspeler);
int alVerbonden(Room* kamer, Room* target);

// Opslaan & Laden
void saveGameJson(Player* player, Room* Kamers, int aantalKamers, const char* filename);
Player* loadGameJson(Room** kamersOut, int* aantalKamersOut, const char* filename);

// UI functies
void PRINTINTRO();
void PRINTHOME();
void PRINTSTATS(Player* speler, Room* kamers, int aantalKamers);
void PRINTMONSTER(Monster* monster);
void PRINTTREASURE();

#endif