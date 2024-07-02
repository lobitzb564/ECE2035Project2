//=================================================================
// The header file for general settings for the project
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================

#ifndef GLOBAL_H
#define GLOBAL_H
#include "mbed.h"
#include "wave_player.h"
#include "uLCD_4DGL.h"
#include <math.h>
#include "Nav_Switch.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"


// === [global object] ===
extern uLCD_4DGL uLCD;
extern wave_player waver;
extern PwmOut speaker;
extern SDBlockDevice sd_block;
extern FATFileSystem fs;


typedef struct {
    int x;
    int y;
} point;
typedef struct {
    point topLeft;
    point bottomRight;
} boundingBox;

// === [global settings] ===
#define BACKGROUND_COLOR 0x000000 //black

// === [define the macro of error handle function] ===
// when the condition (c) is not true, assert the program and show error code
#define ASSERT_P(c,e) do { \
    if(!(c)){ \
        uLCD.printf("\nERROR:%d\n",e); \
        printf("\nERROR:%d\n",e); \
        while(1); \
    } \
} while (0)

// === [error code] ===
#define ERROR_NONE 0 // All good in the hood
#define ERROR_ENEMY_INDEX_GET_INFO      -1  // make sure your code gives the valid index for enemy_get_info()
#define ERROR_ENEMY_INDEX_UPDATE_STATUS -2  // make sure your code gives the valid index for enemy_update_status()
#define ERROR_ENEMY_SPEED               -3  // enemy speed has to be between 1 and 8
#define ERROR_ENEMY_INTERVAL            -4  // enemy interval has to be between 1 and 100
//otehr enemy error code

// DLL
#define ERROR_DLL_INSERT_HEAD             -14 // inserting into doubly linked list at head failed
#define ERROR_DLL_DELETE                  -15 // deleting node from doubly linked list failed
//other error codes


#endif //GLOBAL_H
