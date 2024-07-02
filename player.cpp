//=================================================================
// Implementation for the player module
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================
#include "graphics.h"
#include "player_public.h"
#include <cstdio>




PLAYER player; // structure of player

int missile[] = {
    0,   RED, RED, 0,
    0,   RED, RED, 0,
    RED, WHITE, WHITE, RED,
    0, BLUE, BLUE, 0,
    0, BLUE, BLUE, 0,
    0, BLUE, BLUE, 0,
    0, BLUE, BLUE, 0
};

int ship[] = {      0,         0,     0,     0,     0, WHITE,     0,     0,     0,     0, 0,
                    0,         0,     0,     0,     0, WHITE,     0,     0,     0,     0, 0,
                    0,         0,   RED,     0, WHITE, WHITE, WHITE,     0,   RED,     0, 0,
                    0,         0,   RED,     0, WHITE, WHITE, WHITE,     0,   RED,     0, 0,
                    0,         0, WHITE,     0, WHITE, WHITE, WHITE,     0, WHITE,     0, 0,
                    0,         0, WHITE,     0, WHITE, WHITE, WHITE,     0, WHITE,     0, 0,
                    RED,       0, WHITE,  BLUE, WHITE, WHITE, WHITE,  BLUE, WHITE,      0, RED,
                    RED,       0,  BLUE, WHITE, WHITE,   RED, WHITE, WHITE,  BLUE,      0, RED,
                    WHITE,     0, WHITE, WHITE,   RED,   RED,   RED, WHITE, WHITE,     0, WHITE,
                    WHITE,     0, WHITE, WHITE,   RED, WHITE,   RED, WHITE, WHITE,     0, WHITE,
                    WHITE,     0, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,     0, WHITE,
                    WHITE, WHITE, WHITE,   RED, WHITE, WHITE, WHITE,   RED, WHITE, WHITE, WHITE,
                    WHITE, WHITE,   RED,   RED, WHITE, WHITE, WHITE,   RED,   RED, WHITE, WHITE,
                    WHITE,     0,   RED,   RED, WHITE, WHITE, WHITE,   RED,   RED,     0, WHITE,
                    WHITE,     0,     0,     0,     0, WHITE,     0,     0,     0,     0, WHITE};

// Complete the function below
PLAYER player_get_info(void){
    PLAYER clone;
    clone.x = player.x;
    clone.y = player.y;
    clone.score = player.score;
    clone.height = player.height;
    clone.missile_length = player.missile_length;
    clone.missile_boundingBox = player.missile_boundingBox;
    clone.player_has_missile = player.player_has_missile;

    return clone;

}

void player_init(int x, int y) {  
   player.player_has_missile = true;
   player.height = 15;
   player.x = x;
   player.y = y;
   player.score = 0;
   boundingBox b;
   point topleft;
   point bottomright;
   topleft.x = 50;
   topleft.y = 50;
   bottomright.x = 54;
   bottomright.y = 56;
   b.bottomRight = bottomright;
   b.topLeft = topleft;
   player.missile_boundingBox = b;
   printf("Creating player\n");
   player_draw(0);
}

// player_draw in main loop
void player_moveUp(void) {
    int movementamt = player.height;
    if (player.y - player.height < 8) {
        movementamt = player.y - 8;
    }
    uLCD.filled_rectangle(player.x, player.y, player.x + PLAYER_SIZE, player.y + player.height, 0);
    player.y -= movementamt;
}

void player_moveDown(void) { 
    int movementamt = 5;
    if (player.y + player.height + 5 > SIZE_Y) {
        movementamt = SIZE_Y - player.y - player.height;
    }
    uLCD.filled_rectangle(player.x, player.y, player.x + PLAYER_SIZE, player.y + player.height, 0);
    player.y += movementamt;
}

void player_moveLeft(void) { 
    int movementamt = 5;
    if (player.x - 5 <= 0) {
        movementamt = player.x;
    }
    uLCD.filled_rectangle(player.x, player.y, player.x + PLAYER_SIZE, player.y + player.height, 0);
    player.x -= movementamt;
}

void player_moveRight(void) { 
    int movementamt = 5;
    if (player.x + PLAYER_SIZE + movementamt > SIZE_X) {
        movementamt = SIZE_X - player.x - PLAYER_SIZE;
    }
    uLCD.filled_rectangle(player.x, player.y, player.x + 11, player.y + player.height, 0);
    player.x += movementamt;
}

void player_throw(void) {
    if (player.player_has_missile) {
        player_missile_return();
        player.player_has_missile = false;
    }
}

void player_missile_draw(bool b) {
   if (!b) {
       uLCD.BLIT(player.missile_boundingBox.topLeft.x, player.missile_boundingBox.topLeft.y, MISSILE_WIDTH, MISSILE_LENGTH, missile);
   } else {
       uLCD.filled_rectangle(player.missile_boundingBox.bottomRight.x, 
                             player.missile_boundingBox.bottomRight.y, 
                             player.missile_boundingBox.topLeft.x, 
                             player.missile_boundingBox.topLeft.y, 
                             0);
   }
}

void player_missile_return(void) {
    uLCD.filled_rectangle(player.missile_boundingBox.bottomRight.x, 
                             player.missile_boundingBox.bottomRight.y, 
                             player.missile_boundingBox.topLeft.x, 
                             player.missile_boundingBox.topLeft.y, 
                             0);
    player.missile_boundingBox.topLeft.x = player.x + 3;
    player.missile_boundingBox.topLeft.y = player.y;
    player.missile_boundingBox.bottomRight.x = player.x + MISSILE_WIDTH + 3;
    player.missile_boundingBox.bottomRight.y = player.y + MISSILE_LENGTH;
    player.player_has_missile = true;
}

void player_draw(int color) {
    uLCD.BLIT(player.x, player.y, 11, player.height, ship); 
}

void player_score_inc(void) {
    player.score++;
}

void player_move_missile(void) {
    uLCD.filled_rectangle(player.missile_boundingBox.bottomRight.x, 
                             player.missile_boundingBox.bottomRight.y, 
                             player.missile_boundingBox.topLeft.x, 
                             player.missile_boundingBox.topLeft.y, 
                             0);
    int movementamt = PLAYER_MISSILE_SPEED;
    if (player.missile_boundingBox.topLeft.y - PLAYER_MISSILE_SPEED <= 8) {
        movementamt = player.missile_boundingBox.topLeft.y - 8;
    }
    player.missile_boundingBox.topLeft.y -= movementamt;
    player.missile_boundingBox.bottomRight.y -= movementamt;
}
