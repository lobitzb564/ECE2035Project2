//=================================================================
// The main program file.
//
// Copyright 2020 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================

// External libs
#include <cstdio>
#include <ctime>
#include <stdlib.h>

// Use these macros if you wanna read timer values
#define timer_read_f(x)     chrono::duration<float>((x).elapsed_time()).count()
#define timer_read_s(x)     chrono::duration_cast<chrono::seconds>((x).elapsed_time()).count()
#define timer_read_ms(x)    chrono::duration_cast<chrono::milliseconds>((x).elapsed_time()).count()
#define timer_read_us(x)    (x).elapsed_time().count()

// Project includes
//#include "enemy_private.h"
#include "globals.h"
#include "hardware.h"
#include "enemy_public.h"
#include "player_public.h"

#define MENU_OPTIONS 5



// ===User implementations start===
int enemy_contact(void); // iterate through enemys and see if any collided
int projectile_contact(void);   // see if any missiles have hit
void set_random_seed(Timer);
void game_over(char*);
void start_menu(int, int, int);
void pause_menu(int);
void playSound(char*);
int start_game();
void load_game(char*);
void save_game(int, int, int);

int espeed = 4;
int pspeed = 5;
int gm = 0;
int timeinc = 0;
int d = 1;
FILE* save;



int main()
{
    GameInputs inputs; 
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    printf("Program Starting\n");
    printf("mounting %d\n", fs.mount(&sd_block));

    // ===User implementations start===
    // Game state variables
    // Timer to measure game update speed (secondarily used to generate random seed)

    Timer gametime;
    Timer t;
    
    int dt; // delta time
    // set seed for random enemy generation
    set_random_seed(t);

    // REMEMBER TO CHANGE THIS FUNCTION WHEN CONTROLS ARE UPDATED-
    int gamemode = start_game();       
    printf("Initialization complete\n");
    // uLCD.baudrate(3000000);
    
    /*

    You will need to make helper methods to help out with the game objective
    You should have methods that deal with drawing the missile, checking for missile contact,
    checking for projectile contact, and any others as you see fit.

    The shell code is just a guideline. You can edit the code in any way you see fir   
       
    
    */

    gametime.start();
    int lasttime = 10;
    uLCD.printf("Score: 0, Time:");
    char str [24];
    while(1)
    {
        t.start();

        // Draw enemys first
        enemy_generator(espeed, pspeed);

        //Put score on screen 
        
        // NOTE: Text is 8 pixels tall
        PLAYER player = player_get_info();
        player_missile_draw(player.player_has_missile);
        player_draw(0x0000FF);
        
        // read inputs
        inputs = read_inputs();
        if (inputs.down) {
            player_moveDown();
        } else if (inputs.up) {
            player_moveUp();
        } else if (inputs.left) {
            player_moveLeft();
        } else if (inputs.right) {
            player_moveRight();
        } else if (inputs.fire) {
            player_throw();
        }
        // fill the rest of it out           
        if (!player.player_has_missile) {
            if (player.missile_boundingBox.topLeft.y > 8) {
                player_move_missile();
            } else {
                player_missile_return();
            }
        }

        // TODO: Write helper methods as needed  

        
        // TODO: Check if missile hit enemy
        bool gameover = false;
        LLNode* curr = get_enemy_list()->head;
        while (curr != NULL) {
            ENEMY* e = (ENEMY*) curr->data;
            // check if this enemy is in contact with the player
            if ((e->box.bottomRight.x >= player.x && e->box.bottomRight.y >= player.y &&
                e->box.topLeft.x <= player.x && e->box.topLeft.y <= player.y) ||
                (e->box.bottomRight.x >= player.x + PLAYER_SIZE && e->box.bottomRight.y >= player.y + player.height &&
                e->box.topLeft.x <= player.x + PLAYER_SIZE && e->box.topLeft.y <= player.y + player.height) ||
                (e->box.bottomRight.x >= player.x + PLAYER_SIZE && e->box.bottomRight.y >= player.y &&
                e->box.topLeft.x <= player.x + PLAYER_SIZE && e->box.topLeft.y <= player.y) ||
                (e->box.bottomRight.x >= player.x && e->box.bottomRight.y >= player.y + player.height &&
                e->box.topLeft.x <= player.x && e->box.topLeft.y <= player.y + player.height)) {
                    gameover = true;
                    break;
            }

            if (!player.player_has_missile) {
                // check if this enemy is in contact with the missile
                if ((e->box.bottomRight.x >= player.missile_boundingBox.bottomRight.x && e->box.bottomRight.y >= player.missile_boundingBox.bottomRight.y &&
                e->box.topLeft.x <= player.missile_boundingBox.bottomRight.x && e->box.topLeft.y <= player.missile_boundingBox.bottomRight.y) ||
                (e->box.bottomRight.x >= player.missile_boundingBox.topLeft.x && e->box.bottomRight.y >= player.missile_boundingBox.topLeft.y &&
                e->box.topLeft.x <= player.missile_boundingBox.topLeft.x && e->box.topLeft.y <= player.missile_boundingBox.topLeft.y) ||
                (e->box.bottomRight.x >= player.missile_boundingBox.bottomRight.x && e->box.bottomRight.y >= player.missile_boundingBox.topLeft.y &&
                e->box.topLeft.x <= player.missile_boundingBox.bottomRight.x && e->box.topLeft.y <= player.missile_boundingBox.topLeft.y) ||
                (e->box.bottomRight.x >= player.missile_boundingBox.topLeft.x && e->box.bottomRight.y >= player.missile_boundingBox.bottomRight.y &&
                e->box.topLeft.x <= player.missile_boundingBox.topLeft.x && e->box.topLeft.y <= player.missile_boundingBox.bottomRight.y)) {
                    e->status = ENEMY_DESTROYED;
                    get_enemy_list();
                    player_score_inc();
                    int timer = 20-timer_read_s(gametime)-timeinc;
                    if (gamemode == 0) {
                        timer = timer_read_s(gametime)+timeinc;
                    }
                    snprintf(str, 24, "Score: %i Time: %i ", player_get_info().score, timer);
                    uLCD.text_string(str, 0, 0, 0, 0x00FF00);
                }
            }
            curr = curr->next;
        }
        if (timer_read_s(gametime) != lasttime) { 
            int timer = 20-timer_read_s(gametime)-timeinc;
            if (gamemode == 0) {
                timer = timer_read_s(gametime)+timeinc;
            }
            snprintf(str, 24, "Score: %i Time: %i ", player_get_info().score, timer);
            uLCD.text_string(str, 0, 0, 0, 0x00FF00);
            lasttime = timer_read_s(gametime);
        }

        curr = get_projectile_list()->head;
        while (curr != NULL) {
            PROJECTILE* p = (PROJECTILE*) curr->data;
            // check if this projectile is in contact with the player
            if ((player.x + PLAYER_SIZE >= p->box.topLeft.x && player.y + player.height >= p->box.topLeft.y &&
                player.x <= p->box.topLeft.x && player.y <= p->box.topLeft.y) ||
                (player.x + PLAYER_SIZE >= p->box.bottomRight.x && player.y + player.height >= p->box.bottomRight.y &&
                player.x <= p->box.topLeft.x && player.y <= p->box.bottomRight.y) ||
                (player.x + PLAYER_SIZE >= p->box.bottomRight.x && player.y + player.height >= p->box.topLeft.y &&
                player.x <= p->box.bottomRight.x && player.y <= p->box.topLeft.y) ||
                (player.x + PLAYER_SIZE >= p->box.topLeft.x && player.y + player.height >= p->box.topLeft.y &&
                player.x <= p->box.topLeft.x && player.y <= p->box.bottomRight.y)) {
                    gameover = true;
                    break;
            }
            curr = curr->next;
        }
        

        // TODO: Check for projectile contact
        if (gamemode == 1 && (lasttime + timeinc) >= 20) {
            gameover = true;
        }
        if (player.score > 4 && gamemode == 1) {
            game_over("GAME OVER, YOU WIN !!!!!!");
            playSound("/fs/Win.wav");
            while(!inputs.fire) {
                inputs = read_inputs();
            }
            while(inputs.fire) {
                inputs=read_inputs();
            }
            gamemode = start_game();
            gametime.reset();
            uLCD.printf("Score: 0");
            continue;
        } else if (gameover) {
            playSound("/fs/Player_die.wav");
            game_over("YOU LOSE !!!!!! GAME OVER");
            while(!inputs.fire) {
                inputs = read_inputs();
            }
            while(inputs.fire) {
                inputs=read_inputs();
            }
            gamemode = start_game();
            gametime.reset();
            uLCD.printf("Score: 0");
            continue;
        }

        

        // Compute update time
        t.reset();

        // draw player and missile again to increase smoothness
        player_missile_draw(player.player_has_missile);
        player_draw(0x0000FF);
        if (!inputs.b1) {
            int selected = 0;

            bool resume = false;
            while (!resume) {
                pause_menu(selected);
                gametime.stop();
                while(1) {
                    inputs = read_inputs();
                    if (inputs.down) {
                        selected = (selected + 1) % 3;
                        break;
                    } else if (inputs.up) {
                        selected = (selected + 2) % 3;
                        break;
                    } else if (inputs.fire) {
                        if (selected == 0) {
                            resume = true;
                            break;
                        } else if (selected == 1) {
                            // print a status screen
                            uLCD.cls();
                            
                            uLCD.printf("Score........... %i\n", player_get_info().score);
                            uLCD.printf("Enemies beaten.. %i\n", player_get_info().score);
                            uLCD.printf("Time played. %i sec\n\n", timer_read_s(gametime));
                            
                            uLCD.printf("Press FIRE to\nreturn to menu");
                            // Wait for nav switch to be un-pressed
                            while (inputs.fire) {
                                inputs = read_inputs();
                            }
                            // wait for nav switch to be pressed
                            while (!inputs.fire) {
                                inputs = read_inputs();
                            }
                            // wait for nav switch to be un-pressed
                            while (inputs.fire) {
                                inputs = read_inputs();
                            }
                            uLCD.cls();
                            wait_ns(100000);
                            break;
                        } else if (selected == 2) {
                            // Save game
                            save_game(timer_read_s(gametime), gamemode, d);
                            break;
                        }
                    }
                }
                gametime.start();
            }
            uLCD.cls();
            char str [16]; 
            snprintf(str, 16, "Score: %i", player_get_info().score);
            uLCD.text_string(str, 0, 0, 0, 0xFFFFFF);
        }
        //uLCD.filled_rectangle(54, 54, 64, 64, RED);

    }
    printf("out of main loop\n");

    destroyList(get_enemy_list());
    // destroy more lists as needed
    
    return 0;
    // ===User implementations end===
}

// ===User implementations start===

// Write your helper methods here to deal with game loop objectives
// Feel free to do whatever you need to implement the game

void game_over(char* message) {
    uLCD.cls();
    uLCD.printf(message);
    uLCD.printf("\nPress Fire to\nplay again");
    return;
}


// /* Wanna hear some annoying sounds?*/
// void playNotes(void)
// {
//     int i;
// // generate a 500Hz tone using PWM hardware output
//     speaker.period(1.0/500.0); // 500hz period
//     speaker =0.5; //50% duty cycle - max volume
//     wait(3);
//     speaker=0.0; // turn off audio
//     wait(2);
// // generate a short 150Hz tone using PWM hardware output
// // something like this can be used for a button click effect for feedback
//     for (i=0; i<10; i++) {
//         speaker.period(1.0/150.0); // 500hz period
//         speaker =0.25; //25% duty cycle - mid range volume
//         wait(.02);
//         speaker=0.0; // turn off audio
//         wait(0.5);
//     }

// // sweep up in frequency by changing the PWM period
//     for (i=0; i<8000; i=i+100) {
//         speaker.period(1.0/float(i));
//         speaker=0.25;
//         wait(.1);
//     }
//     wait(2);

// // two tone police siren effect -  two periods or two frequencies
// // increase volume - by changing the PWM duty cycle
//     for (i=0; i<26; i=i+2) {
//         speaker.period(1.0/969.0);
//         speaker = float(i)/50.0;
//         wait(.5);
//         speaker.period(1.0/800.0);
//         wait(.5);
//     }
//     // decrease volume
//     for (i=25; i>=0; i=i-2) {
//         speaker.period(1.0/969.0);
//         speaker = float(i)/50.0;
//         wait(.5);
//         speaker.period(1.0/800.0);
//         wait(.5);
//     }
//     speaker =0.0;
//     wait(2);
// }

void set_random_seed(Timer t) {
    t.start();
    uLCD.printf("Push the nav switch to start.\n");
    
    uLCD.cls();
    t.stop();
    int seed = timer_read_ms(t);    
    srand(seed);
}


void start_menu(int selected, int difficulty, int gamemode) {
    uLCD.cls();
    uLCD.printf("\n\n\n\n");
    char* diff;
    char* md;
    if (difficulty == 0) {
        diff = "Easy";
    } else if (difficulty == 1) {
        diff = "Med";
    } else {
        diff = "Hard";
    }
    if (gamemode == 0) {
        md = "Infinite";
    } else {
        md = "Timed";
    }
    switch (selected) {
        case 0:
            uLCD.printf(">> New Game\n\n");
            uLCD.printf("   Load Game\n\n");
            uLCD.printf("   See Controls\n\n");
            uLCD.printf("   Level: %s\n\n", diff);
            uLCD.printf("   Mode: %s\n\n", md);
            break;
        case 1:
            uLCD.printf("   New Game\n\n");
            uLCD.printf(">> Load Game\n\n");
            uLCD.printf("   See Controls\n\n");
            uLCD.printf("   Level: %s\n\n", diff);
            uLCD.printf("   Mode: %s\n\n", md);
            break;
        case 2:
            uLCD.printf("   New Game\n\n");
            uLCD.printf("   Load Game\n\n");
            uLCD.printf(">> See Controls\n\n");
            uLCD.printf("   Level: %s\n\n", diff);
            uLCD.printf("   Mode: %s\n\n", md);
            break;
        case 3:
            uLCD.printf("   New Game\n\n");
            uLCD.printf("   Load Game\n\n");
            uLCD.printf("   See Controls\n\n");
            uLCD.printf(">> Level: %s\n\n", diff);
            uLCD.printf("   Mode: %s\n\n", md);
            break;
        case 4:
            uLCD.printf("   New Game\n\n");
            uLCD.printf("   Load Game\n\n");
            uLCD.printf("   See Controls\n\n");
            uLCD.printf("   Level: %s\n\n", diff);
            uLCD.printf(">> Mode: %s\n\n", md);
            break;
        default:
            break;
    }
    draw_logo();
}

void pause_menu(int selected) {
    uLCD.cls();
    uLCD.printf("    PAUSED\n\n\n");
    switch (selected) {
        case 0:
            uLCD.printf(">> Resume Game\n\n");
            uLCD.printf("   View Status\n\n");
            uLCD.printf("   Save the Game");
            break;
        case 1:
            uLCD.printf("   Resume Game\n\n");
            uLCD.printf(">> View Status\n\n");
            uLCD.printf("   Save the Game\n");
            break;
        case 2:
            uLCD.printf("   Resume Game\n\n");
            uLCD.printf("   View Status\n\n");
            uLCD.printf(">> Save the Game\n\n");
            break;
        default:
            break;
    }
}

void playSound(char* wav)
{
    //open wav file
    FILE *wave_file;

    int count = 0;

try_open:
    wave_file=fopen(wav,"r");

    if(wave_file != NULL) {
        printf("File opened successfully\n");

        //play wav file
        printf("Sound playing...\n");
        waver.play(wave_file);

        //close wav file
        printf("Sound stopped...\n");
        fclose(wave_file);
        return;
    }
    else {
        printf("Could not open file for reading - %s\n", wav);
        count++;
        if (count == 4) {
            printf("File could not be opened. Skipping audio\n");
            return;
        }
        else
            goto try_open;

    }


    
    return;
}


int start_game() {
    GameInputs inputs;
    int selected = 0;
    int difficulty = d;
    int gamemode = gm;
    bool startgame = false;
    while (!startgame) {
        uLCD.cls();
        start_menu(selected, difficulty, gamemode);
        while(1) {
            inputs = read_inputs();
            if (inputs.down) {
                selected = (selected + 1) % MENU_OPTIONS;
                break;
            } else if (inputs.up) {
                selected = (selected + 4) % MENU_OPTIONS;
                break;
            } else if (inputs.fire) {
                if (selected == 0) {
                    startgame = true;
                    enemy_init();
                    player_init(SIZE_X / 2, PLAYER_INIT_Y);
                    timeinc = 0;
                    break;
                } else if (selected == 1) {
                    // LOAD GAME FROM SAVE FILE
                    enemy_init();
                    player_init(SIZE_X / 2, PLAYER_INIT_Y);
                    startgame = true;
                    load_game("/fs/Save.txt");
                    gamemode = gm;
                    break;
                } else if (selected == 2) {
                    uLCD.cls();
                    uLCD.printf("Up........  Nav up\n");
                    uLCD.printf("Down..... Nav down\n");
                    uLCD.printf("Left..... Nav left\n");
                    uLCD.printf("Right... Nav right\n");
                    uLCD.printf("Fire..... Nav push\n");
                    uLCD.printf("Pause.. 4th button\n\n");
                    uLCD.printf("Press FIRE to\nreturn home");
                    // Wait for nav switch to be un-pressed
                    while (inputs.fire) {
                        inputs = read_inputs();
                    }
                    // wait for nav switch to be pressed
                    while (!inputs.fire) {
                        inputs = read_inputs();
                    }
                    // wait for nav switch to be un-pressed
                    while (inputs.fire) {
                        inputs = read_inputs();
                    }
                    uLCD.cls();
                    break;
                }
            } else if (inputs.left && selected == 3) {
                difficulty = (difficulty + 2) % 3;
                d = difficulty;
                espeed = 2 + difficulty*2;
                pspeed = 3 + difficulty*2;
                break;
            } else if (inputs.right && selected == 3) {
                difficulty = (difficulty + 1) % 3;
                d = difficulty;
                espeed = 2 + difficulty*2;
                pspeed = 3 + difficulty*2;
                break;
            } else if ((inputs.right || inputs.left) && selected == 4) {
                if (gamemode == 0) {
                    gamemode = 1;
                } else {
                    gamemode = 0;
                }
                break;
            }
        }
        wait_ns(100000);
    }
    uLCD.cls();
    //initialize functions
    playSound("/fs/Start.wav");
    gm = gamemode;
    return gamemode;
}


void load_game(char* loc) {
    save = fopen(loc, "r");
    if (!save) {
        uLCD.cls();
        uLCD.printf("Could not find save file\n");
        wait_us(500000);
        return;
    }
    int inp = 0;
    int i2 = 0;
    int i3 = 0;
    double inp1 = 0;
    double inp2 = 0;
    // player loc
    fscanf(save, "%d %d", &inp, &i2);
    player_init(inp, i2);
    fscanf(save, "%d", &inp);
    for (int i = 0; i < inp; i++) {
        player_score_inc();
    }
    fscanf(save, "%d %d %d", &inp, &i2, &i3);
    timeinc = inp;
    gm = i2;
    d = i3;
    espeed = 2 + d*2;
    pspeed = 3 + d*2;
    int numenemies = 0;
    fscanf(save, "%d", &numenemies);
    while(numenemies > 0) {
        fscanf(save, "%d %d ", &inp, &i2);
        boundingBox b1;
        b1.topLeft.x = inp;
        b1.topLeft.y = i2;
        fscanf(save, "%d %d ", &inp, &i2);
        b1.bottomRight.x = inp;
        b1.bottomRight.y = i2;
        ENEMY* e1 = (ENEMY*)malloc(sizeof(ENEMY));
        e1->box = b1;
        fscanf(save, "%lf %lf ", &inp1, &inp2);
        e1->delta_x = inp1;
        e1->delta_y = inp2;
        fscanf(save, "%lf %lf ", &inp1, &inp2);
        e1->source = inp1;
        e1->target = inp2;
        fscanf(save, "%d %d ", &inp, &i2);
        e1->direction = inp;
        e1->type = i2;
        fscanf(save, "%d", &inp, &i2);
        e1->tick = inp;
        e1->status = ENEMY_ACTIVE;
        switch (e1->type)
        {
            case 0:
                e1->draw = draw_red_thing;
                break;
            case 1:
                e1->draw = draw_bee;
                break;
            case 2:
                e1->draw = draw_green_bug;
                break;
            default:
                break;
        }
        insertTail(get_enemy_list(), e1);
        numenemies--;
    }
    int numproj = 0;
    fscanf(save, "%d", &numproj);
    while(numproj > 0) {
        fscanf(save, "%d %d ", &inp, &i2);
        boundingBox b1;
        b1.topLeft.x = inp;
        b1.topLeft.y = i2;
        fscanf(save, "%d %d ", &inp, &i2);
        b1.bottomRight.x = inp;
        b1.bottomRight.y = i2;
        PROJECTILE* e1 = (PROJECTILE*)malloc(sizeof(PROJECTILE));
        e1->box = b1;
        fscanf(save, "%lf", &inp1);
        e1->delta_y = inp1;
        insertTail(get_projectile_list(), e1);
        numproj--;
    }
    fclose(save);
}
void save_game(int time, int mode, int difficulty) {
    save = fopen("/fs/Save.txt", "w");
    fprintf(save, "\n%d %d", player_get_info().x, player_get_info().y);
    fprintf(save, "\n%d", player_get_info().score);
    fprintf(save, "\n%d %d %d", time+timeinc, mode, d);
    fprintf(save, "\n%d", get_enemy_list()->size);
    LLNode* curr = get_enemy_list()->head;
    ENEMY* E;
    while (curr != NULL) {
        E = (ENEMY*) curr->data;
        fprintf(save, "\n%d %d ", E->box.topLeft.x, E->box.topLeft.y);
        fprintf(save, "\n%d %d ", E->box.bottomRight.x, E->box.bottomRight.y);
        fprintf(save, "\n%lf %lf ", E->delta_x, E->delta_y);
        fprintf(save, "\n%lf %lf ", E->source, E->target);
        fprintf(save, "\n%d %d ", E->direction, E->type);
        fprintf(save, "\n%d", E->tick);
        curr = curr->next;
    }

    fprintf(save, "\n%d", get_projectile_list()->size);
    curr = get_projectile_list()->head;
    PROJECTILE* P;
    while (curr != NULL) {
        P = (PROJECTILE*) curr->data;
        fprintf(save, "\n%d %d ", P->box.topLeft.x, P->box.topLeft.y);
        fprintf(save, "\n%d %d ", P->box.bottomRight.x, P->box.bottomRight.y);
        fprintf(save, "\n%lf", P->delta_y);
        curr = curr->next;
    }
    /*
    int tick;                  ///< The enemy's internal tick
    ENEMY_STATUS status;   ///< The enemy status, see ENEMY_STATUS
    DrawFunc draw;
    */
    fclose(save);
}
// ===User implementations end===
