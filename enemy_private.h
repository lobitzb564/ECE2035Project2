//=================================================================
// The header file defining the enemy module
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================
#ifndef ENEMY_PRIVATE_H
#define ENEMY_PRIVATE_H

#include "mbed.h"
#include "globals.h"
#include "enemy_public.h"

//==== [private settings] ====
int ENEMY_INTERVAL = 10;
int PROJ_INTERVAL = 17;
int ENEMY_SPEED = 4;
int PLAYER_SPACE = 20;
int PROJ_WID = 3;
int PROJ_LEN = 7;
int PROJ_SPED = 5;


//==== [private type] ====

//==== [private function] ====
/** Generate a random enemy and add it to the enemy DLL
*/
void enemy_create(void);
/** Iterate over all enemy and update their positions using the rate of motion
 *  and the deltaX and deltaY generated by enemy create
*/
void enemy_update_position(int);

void projectile_update_position(void);

void enemy_fire(int);

#endif //ENEMY_PRIVATE_H

