/*
Authors: Shreyas Patil, Parth Sarthi Sharma
Affiliation: Cornell University
Course: ECE 4760
Purpose: Week 1 of Final Project
*/

#include "config_1_3_2.h" //Contains the main configurations like clock freq etc.
#include "pt_cornell_1_3_2_python.h" //Threading library
#include "tft_master.h" //Library used to use the TFT screen
#include "tft_gfx.h" //Library used to draw on the TFT screen
#include <stdlib.h> //Standard C library
#include <stdfix.h> //Library for importing the fixed-point datatype
#include <math.h> //Library with all math functions

#define GROUND_SIZE 10 //Number of elements in the ground array
#define GROUND_HEIGHT 60 //Height of the ground above the base
#define HEIGHT 240 //Height of the TFT screen
#define WIDTH 320 //Width of the TFT screen
#define HARD_COLOR 0xFFFF //Display white
#define SOFT_COLOR 0x2945 //Display grayish color
#define SPEED 5 //Speed with which the ground moves

int exTime; //Time to yield the animation protothread

char buffer[60]; //Buffer to display the strings on the TFT

struct Ground{ //Ground structure to store each small ground element with x-coordinate, y-coordinate and width
    int x, y, w;
};

inline int randomRange(int min, int max){ //Function to generate a random number between min and max
    return (rand() % (max - min)) + min;
}

struct Ground ground[GROUND_SIZE]; //Ground array


static PT_THREAD (pt_anim(struct pt *pt)){ //Animation protothread
    PT_BEGIN(pt); //Begin the protothread
    static unsigned int begin_time, i; //Variable to keep track of start time and counter i
    while(1){ //Infinite while loop
        begin_time = PT_GET_TIME(); //Get start time
        tft_drawFastHLine(0, HEIGHT - GROUND_HEIGHT, WIDTH, SOFT_COLOR); //Draw a white line to represent ground
        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
            tft_drawFastHLine(ground[i].x, ground[i].y, ground[i].w, ILI9340_BLACK); //Clear the old display of the ground by drawing it with black 
            ground[i].x -= SPEED; //Change the x-coordinate based on speed
            if(ground[i].x + ground[i].w <= 0){ //If the ground element is out of screen
                ground[i].x = WIDTH; //Reset the ground element's x-coordinate
                ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Get a new y-coordinate for the ground element
                ground[i].w = randomRange(3, 10); //Get a new width for the ground element
            }
        }

        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
            tft_drawFastHLine(ground[i].x, ground[i].y, ground[i].w, HARD_COLOR); //Draw the new display of the ground elements
        }
        exTime = 33 - (PT_GET_TIME() - begin_time); //Get remaining time to achieve 30 frames per second
        printf("%d\n", exTime); //Print out the remaining time
        PT_YIELD_TIME_msec(exTime); //Yield for the remaining time
    }
    PT_END(pt); //End the protothread
}

int main(){ //The main function
    int i; //Counter variable i
    for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
        ground[i].x = randomRange(0, WIDTH); //Assign them a random x-coordinate
        ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Assign them a random y-coordinate
        ground[i].w = randomRange(3, 10); //Assign them a random width
    }

    tft_init_hw(); //Inititialise the TFT hardware
    tft_begin(); //Begin the TFT
    tft_fillScreen(ILI9340_BLACK); //Fill the TFT screen with black colour
    tft_setRotation(1); //Set the TFT screen orientation

    INTEnableSystemMultiVectoredInt(); //Enable system wide interrupts

    PT_setup(); //Setup threads
    pt_add(&pt_anim, 0); //Add animation thread to the chain of protothreads
    PT_INIT(&pt_sched); //Initialize the scheduler
    pt_sched_method = SCHED_ROUND_ROBIN; //Configure the scheduling method to be Round Robin 
    PT_SCHEDULE(protothread_sched(&pt_sched)); //Schedule the protothreads
}