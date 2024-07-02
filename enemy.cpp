//=================================================================
// Implementation of ENEMY module.
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================

#include "enemy_private.h"
#include "doubly_linked_list.h"
#include "enemy_public.h"

int ENEMY_tick=0;

//Create a DLL for ENEMYs
DLinkedList* ENEMYDLL = NULL;
DLinkedList* projectileDLL = NULL;  //List for projectiles launched by enemies
//Reminder that Player fires missiles. Enemies fire projectiles



void enemy_init(void)
{
//Create a new doubly linked list of ENEMYs
    ENEMYDLL = create_dlinkedlist();
    projectileDLL = create_dlinkedlist();
}

void enemy_generator(int rate, int proj){
    ENEMY_tick++;
    // only fire the ENEMY at certain ticks
    if((ENEMY_tick % 18-(rate*2))==0 || ENEMY_tick==0){
        //printf("ENEMY_create()");
        enemy_create();
    } 
    
    if ((ENEMY_tick % 30-(proj*3))==0) {
        enemy_fire(proj);
    }
    // update the ENEMYs and draw them
    enemy_update_position(rate);
    projectile_update_position();
}

void enemy_create(void){
    ENEMY* M = (ENEMY*)malloc(sizeof(ENEMY));
    // M->y = 0;
    //each ENEMY has its own tick
    M->type = rand() % 3;
    switch (M->type)
    {
    case 0:
        M->draw = draw_red_thing;
        break;
    case 1:
        M->draw = draw_bee;
        break;
    case 2:
        M->draw = draw_green_bug;
        break;
    default:
        break;
    }
    M->tick = rand();
    //set a random source for the ENEMY
    M->direction = rand() % 3;
    if (M->direction == 0){
        M->source = rand() % (SIZE_X - ENEMY_SIZE - PLAYER_SPACE);
    //set a random target for the ENEMY
        M->target = rand() % (SIZE_X - ENEMY_SIZE - PLAYER_SPACE);
    //the ENEMY starts at its source
        M->box.topLeft.x = M->source + PLAYER_SPACE;
        M->box.topLeft.y = 8; // = {M->source + PLAYER_SPACE, 0};
        M->box.bottomRight.x = M->source + ENEMY_SIZE + PLAYER_SPACE;
        M->box.bottomRight.y = ENEMY_SIZE + 8;
        //M->box.bottomRight = {M->source + ENEMY_SIZE + PLAYER_SPACE, ENEMY_SIZE};
        double diagnal = sqrt((M->source - M->target)*(M->source - M->target) + SIZE_Y*SIZE_Y);
        M->delta_x = (M->target - M->source) / diagnal;
        M->delta_y = fabs(SIZE_Y / diagnal);
    }
    else if(M->direction == 1){
        M->source = (rand() % (SIZE_Y - ENEMY_SIZE - 58)) + 8; // 50 is bound size
    //set a random target for the ENEMY
        M->target = rand() % (SIZE_Y - ENEMY_SIZE - 50);
        M->box.topLeft.x = PLAYER_SPACE;
        M->box.topLeft.y = M->source;
        //M->box.topLeft = {PLAYER_SPACE, M->source};
        M->box.bottomRight.x = PLAYER_SPACE + ENEMY_SIZE;
        M->box.bottomRight.y = M->source + ENEMY_SIZE;
        //M->box.bottomRight = {PLAYER_SPACE + ENEMY_SIZE, M->source + ENEMY_SIZE};
        double diagnal = sqrt((M->source - M->target)*(M->source - M->target) + (SIZE_X - PLAYER_SPACE)*(SIZE_X - PLAYER_SPACE));
        M->delta_x = (SIZE_X - PLAYER_SPACE) / diagnal; 
        M->delta_y = fabs((M->target - M->source) / diagnal);
    }else{
        M->source = (rand() % (SIZE_Y - ENEMY_SIZE - 58)) + 8;
    //set a random target for the ENEMY
        M->target = rand() % (SIZE_Y - ENEMY_SIZE - 50);
        M->box.topLeft.x = PLAYER_SPACE + SIZE_X - ENEMY_SIZE;
        M->box.topLeft.y = M->source;
        //M->box.topLeft = {PLAYER_SPACE + SIZE_X - ENEMY_SIZE, M->source};
        M->box.bottomRight.x = PLAYER_SPACE + SIZE_X;
        M->box.bottomRight.y = M->source + ENEMY_SIZE;
        //M->box.bottomRight = {PLAYER_SPACE + SIZE_X, M->source + ENEMY_SIZE};
        double diagnal = sqrt((M->source - M->target)*(M->source - M->target) + (SIZE_X - PLAYER_SPACE)*(SIZE_X - PLAYER_SPACE));
        M->delta_x = (PLAYER_SPACE - SIZE_X) / diagnal; 
        M->delta_y = fabs((M->target - M->source) / diagnal);
    }
        
    
    M->status = ENEMY_ACTIVE;
    
    insertHead(ENEMYDLL, M);
}

void enemy_update_position(int r){

    
    //controls how fast the ENEMY will move
    int rate = r;
    //delta_x and delta_y account for the slope of the ENEMY
    DrawFunc draw = NULL;
    LLNode* current = ENEMYDLL->head;
    ENEMY* newENEMY;
    //iterate over all ENEMYs
    while(current)
    {   newENEMY = (ENEMY*) current->data;
        if(newENEMY->status == ENEMY_DESTROYED ||
            newENEMY->box.topLeft.x > 127 ||
            newENEMY->box.topLeft.y > 127 ||
            newENEMY->box.bottomRight.x < 0   ||
            newENEMY->box.bottomRight.y < 8)
        {
            //cover the last ENEMY location
            draw_nothing(newENEMY->box);
            // clear the ENEMY on the screen
            draw = NULL;           
            // Remove it from the list
            //printf("deleting ENEMY node...\n");
            deleteNode(ENEMYDLL, current);
            //printf("ENEMY node deleted.\n");
        }
        else 
        {
            //cover the last ENEMY location
            draw_nothing(newENEMY->box);

            // update ENEMY position
            
            //printf("%f, %f\n", newENEMY->delta_x, newENEMY->delta_y);
            newENEMY->box.topLeft.x += rate*newENEMY->delta_x;
            newENEMY->box.topLeft.y += rate*newENEMY->delta_y;
            newENEMY->box.bottomRight.x += rate*newENEMY->delta_x;
            newENEMY->box.bottomRight.y += rate*newENEMY->delta_y;
            //printf(" %f, %f", newENEMY->delta_x, newENEMY->delta_y);
            // draw ENEMY
            draw = newENEMY->draw;
            //update ENEMY's internal tick
            newENEMY->tick++;
    
        }       
        // Advance the loop
        if(draw) draw(newENEMY->box);
        current = current->next;
    }
}




/*
In this file, you need to create functions that deal with enemies and their projectiles.
You need to make methods that make enemies shoot projectiles randomly,
    Update projectile positions
    Delete Projectiles and Enemies as needed
    and other things as needed
    
You can choose to implement the below methods or make your own new methods
*/




//TODO: make a function that make a new Projectile that fires from the enemy. Make sure that you add it to the list of PROJECTILES
// You should only call this to just launch the projectile. You can use enemy.tick or anything else to simulate randomness
// This method also should insert the projectile into the list after initializng a projectile
void enemy_fire(int speed) {
    if (ENEMYDLL == NULL || ENEMYDLL->head == NULL) {
        return;
    }
    int ene = rand() % get_enemy_list()->size;
    
    LLNode* curr = get_enemy_list()->head;
    while (ene > 0) {
        curr = curr->next;
        ene--;
    }
    ENEMY* e = (ENEMY*)curr->data;
    if (e == NULL || 
            e->box.topLeft.x > 127 ||
            e->box.topLeft.y > 127 ||
            e->box.bottomRight.x < 0   ||
            e->box.bottomRight.y < 8) {
        return;
    }
    PROJECTILE* p = (PROJECTILE*) malloc(sizeof(PROJECTILE));
    p->delta_y = speed;
    boundingBox b;
    point p1;
    point p2;
    p1.y = e->box.bottomRight.y - PROJ_LEN;
    p1.x = e->box.topLeft.x + (ENEMY_SIZE - PROJ_WID) / 2;
    p2.y = e->box.bottomRight.y;
    p2.x = p1.x + PROJ_WID;

    b.bottomRight = p2;
    b.topLeft = p1;
    p->box = b;
    insertTail(projectileDLL, p);
    return;


}

// TODO: Update projectile position  for all projectiles. May require an iterative approach. Make sure you are checking for LCD bounds.
// This method also updates the Projectile List by updating the projectile parameters
// This is called every iteration of the game loop to update the projectile positions and delete projectiles as needed
void projectile_update_position(void) {
    LLNode* curr = get_projectile_list()->head;
    while(curr != NULL) {
        double dy = ((PROJECTILE*) curr->data)->delta_y;
        draw_nothing(((PROJECTILE*) curr->data)->box);
        ((PROJECTILE*) curr->data)->box.bottomRight.y += dy;
        ((PROJECTILE*) curr->data)->box.topLeft.y += dy;
        int y = ((PROJECTILE*) curr->data)->box.bottomRight.y;
        if (y > SIZE_Y) {
            LLNode* del = curr;
            curr = curr->next;
            deleteNode(projectileDLL, del);
        } else {
            draw_proj(((PROJECTILE*) curr->data)->box);
            curr = curr->next;
        }
    }

    return; 

}


//TODO:  Return Enemy List. This method also updates the enemy list
// Remember that you also need to delete DESTROYED enemies from the list. You can do this here or somewhere else.
DLinkedList* get_enemy_list() {
    LLNode* curr = ENEMYDLL->head;
    while (curr != NULL) {
        if (((ENEMY*)curr->data)->status == ENEMY_DESTROYED) {
            draw_nothing(((ENEMY*)curr->data)->box);
            deleteNode(ENEMYDLL, curr);
        }
        curr = curr->next;
    }
    return ENEMYDLL;

}

//TODO: Return Projectile List.
DLinkedList* get_projectile_list() {
    return projectileDLL;
}



// Remember that you also need to delete enemies