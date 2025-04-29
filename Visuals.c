#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "Dungeon.h"

void PRINTINTRO() {
    CLEAR_SCREEN();
    sleep(1);
    printf("\n\n\n\n                      \033[90mA 1Fps Production\x1b[0m");
    sleep(1);
    CLEAR_SCREEN();
    printf("\n\n\n\n                      A 1Fps Production");
    sleep(2);
    CLEAR_SCREEN();
    printf("\n\n\n\n                      \033[90mA 1Fps Production\x1b[0m");
    sleep(1);
    CLEAR_SCREEN();
    sleep(1);
    printf("\033[90m ___                                 ___                 _         \n|   \\ _  _ _ _  __ _ ___ ___ _ _    / __|_ _ __ ___ __ _| |___ _ _ \n| |) | || | ' \\/ _` / -_) _ \\ ' \\  | (__| '_/ _` \\ V  V / / -_) '_|\n|___/ \\_,_|_||_\\__, \\___\\___/_||_|  \\___|_| \\__,_|\\_/\\_/|_\\___|_|  \n               |___/\n");
    printf("                                                ~by Yentl Moonen\x1b[0m\n\n");
    sleep(1);
}

void PRINTHOME() {
    CLEAR_SCREEN();
    printf(" ___                                 ___                 _         \n|   \\ _  _ _ _  __ _ ___ ___ _ _    / __|_ _ __ ___ __ _| |___ _ _ \n| |) | || | ' \\/ _` / -_) _ \\ ' \\  | (__| '_/ _` \\ V  V / / -_) '_|\n|___/ \\_,_|_||_\\__, \\___\\___/_||_|  \\___|_| \\__,_|\\_/\\_/|_\\___|_|  \n               |___/\n");
    printf("                                                ~by Yentl Moonen");
}

void PRINTSTATS(Player* speler) {
    CLEAR_SCREEN();
    if (speler->hp == 0) {
        printf("\nHP:     \033[31m0\x1b[0m/30 [ ☠ ☠ ☠ ☠ ☠ ☠ ☠ ☠ ☠ ☠ ]", speler->hp);
    }
    else {
        printf("\nHP:          %d/30 ", speler->hp);
        if(speler->hp < 10) {
            printf(" [");
        } else {
            printf("[");
        }
        for (int i = 0; i < 10; i++)
        {   
            if ((speler->hp / 3) > i) {
                printf("\033[31m ❤ \x1b[0m");
            } else {
                printf("   ");
            }
        }
        printf(" ]\n");
    }

    printf("DAMAGE:          %d [", speler->damage);
    for (int i = 0; i < 10; i++)
    {   
        if (i < speler->damage){
            printf(" 🗡 ");
        } else {
            printf("   ");
        }
    }
    printf(" ]\n");
    printf("CURRENTROOM:       [%d]\n\n", speler->currentRoom->id);
    
    
}