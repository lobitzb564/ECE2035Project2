//=================================================================
// The header file defining the enemy module
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================
/** @file enemy_public.h */
#ifndef ENEMY_PUBLIC_H
#define ENEMY_PUBLIC_H

#include "doubly_linked_list.h"
#include "globals.h"
#include "graphics.h"

#define ENEMY_SIZE 11


typedef enum {
    ENEMY_DESTROYED=0,
    ENEMY_ACTIVE=1,
} ENEMY_STATUS;

typedef void (*DrawFunc)(boundingBox b);

/// The structure to store the information of a enemy
typedef struct {
    boundingBox box;
    double delta_x;         // moving unit in x dir
    double delta_y;         // moving unit in y dir
    double source;           ///< The x or y-coordinate of the enemy's origin
    double target;           ///< The x or y-coordinate of the enemy's target
    int direction;              // 0: N->S, 1: E->W, 2: W->E
    int type;                   // Orange, Banana, etc
    int tick;                  ///< The enemy's internal tick
    ENEMY_STATUS status;   ///< The enemy status, see ENEMY_STATUS
    DrawFunc draw;
} ENEMY;

typedef struct {
    boundingBox box;
    double delta_y;         // moving unit in y dir
} PROJECTILE;

/** Call enemy_init() only once at the begining of your code */
void enemy_init(void);

/** This function draw the enemys onto the screen
    Call enemy_generator() repeatedly in your game-loop. ex: main()
*/
void enemy_generator(int, int);


DLinkedList* get_enemy_list();
DLinkedList* get_projectile_list();

#endif //ENEMY_PUBLIC_H
