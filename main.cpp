/*
------------------DR VASILIOS KELEFOURAS-----------------------------------------------------
------------------COMP2004 ------------------------------------------------------------------
------------------EMBEDDED PROGRAMMING AND THE INTERNET OF THINGS-------------------------------------------------
------------------UNIVERSITY OF PLYMOUTH, SCHOOL OF ENGINEERING, COMPUTING AND MATHEMATICS---
*/

// --------------- COMP2004 Coursework - Set of Exercises -------------------

/*
In this element of assessment you need to reduce the execution time of the ineffcient_routine(). 
This is an image processing routine written in an inefficient way.
First, read the code and understand what it does. Then, try to simplify the code (reduce its complexity), by eliminating redundant operations. Last, apply code optimizations such as scalar replacement, register blocking, loop merge and others. 
*/

/* SUBMISSION DETAILS COMMENTING
SPEEDUP VALUES
	- Nucleo Board:
        - Debug: 6ms (6 msecs)
        - Release: 6ms (6 msecs)

OPTIMISATIONS (IN ORDER, STARTING TOP FROM BOTTOM)
    OVERVIEW
        - Replaced all constants with literals
        - Removed use of redundant indexing
        - Simplified logical conditions
        - Loop merging
        - Register blocking
        - Full unrolling of short loops
        - Partial unrolling of extensive loops
        - Removed functional calls & rewrote them inline
        - Replaced array indexing with literals

    DETAILED
        - Replaced all constants with literals
        - Removed use of redundant indexing of compute arrays, & by extension altered code that utilises them
            - x_compute[x][y][0] = 0 in Part A
            - xy_compute[x][y][0] = 0 in Part A
            - diff_compute[x][y][0] = 0 in Part B
            - edge_image[x][y] = 0 in Part B
            - out_compute = 255 in Part C
            - xy_image[x][y] = 0 in Parts A, B & C
        - Simplified if statement conditions in Parts A, B & C: removed x conditions as y implies x, so x conditions are redundant
        - Loop merge; Part A second y loop nested into first x loop after first y loop
        - Applied register blocking to frequently used array indexing
            - xYImageIndexedValue in Part B for xy_image[x][y]
            - edgeImageIndexedValue in Part C for edge_image[x][y]
        - Fully unrolled the k loops in both Parts A, B & C
            - Applied register blocking to frequent indexes
        - Unrolled the y loops by a factor of 4 in Parts A, B & C
        - Removed the abs() function call in Part A: 'rewrote inline;' that is, hardcoded the values
        - Replaced Filter[] array access with hardcoded literals in both (unrolled) k loops in Part A
        - Removed the maximum() function call in Part B: rewrote inline & simplified
*/

#include "mbed.h"


using namespace std::chrono;


#include "string.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


#define N 80 //image size 
#define M 80 //image size 

void initialization();
bool compare_images();
void inefficient_routine();
void efficient_routine();


//the following variables never change their values. You could take this into advantage
const int C0=1;
const int C1=8;
const int x_offset[C1]={1,1,1,0,0,-1,-1,-1};
const int y_offset[C1]={1,0,-1,1,-1,1,0,-1};
const int Filter[]={99,68,35,10};

unsigned char input[N][M];
unsigned char output[N][M];
unsigned char x_image[N][M];
unsigned char xy_image[N][M];
unsigned char edge_image[N][M];
unsigned char diff_compute[N][M][C1+1];
unsigned short x_compute[N][M][(2 * C0) + 2];
unsigned short xy_compute[N][M][(2*C0)+2];
	
unsigned char maximum(unsigned char a, unsigned char b);





Timer timer;

int main() {
  

  printf("\n\r Programmed started \n\r");
	initialization();
	
	timer.start();
    
//YOU WILL OPTIMIZE THE FOLLOWING function
  efficient_routine();
  //inefficient_routine();
//------------------------------------------------------------------------------
	
 timer.stop();
printf("\nThe time taken was %llu msecs\n",duration_cast<milliseconds>(timer.elapsed_time()).count());

	bool outcome=compare_images();
	
	if (outcome==true)
		printf("\n\n\r -----  output is correct -----\n\r");
	else 
		printf("\n\n\r ----- output is INcorrect -----\n\r");

	return 0;
}

void initialization(){
	int i,j;
	
	for (i=0;i<N;i++)
	 for (j=0;j<M;j++)
	  input[i][j]=rand()%255;
	
	for (i=0;i<N;i++)
	   for (j=0;j<M;j++)
	    output[i][j]=0;
	
}


unsigned char maximum(unsigned char a, unsigned char b)
{
    return a > b ? a : b;
}

void efficient_routine()
{
    int x, y, k;

    //Loop unroll register blocking
    int y1, y2, y3;

    /* Part A1 */
    for (x = 0; x < N; x++)
    {
        //Loop unrolled by factor of 4
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;

            //Originally: if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2)
            //y implies x, so x condition redundant (unless M & N values changed)
            if (y >= 1 && y <= M - 2)
            {
                /*
                Fully unrolled k loop
                Removed abs() function call, rewrote inline & simplified
                Replaced Filter[] array access with hardcoded literals
                */
                x_compute[x][y][1] = 0 + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;

                x_image[x][y] = x_compute[x][y][3] / 235;
            }

            //y1
            if (y1 >= 1 && y1 <= M - 2)
            {
                x_compute[x][y1][1] = 0 + input[x + -1][y1] * 68;
                x_compute[x][y1][2] = x_compute[x][y1][1] + input[x][y1] * 99;
                x_compute[x][y1][3] = x_compute[x][y1][2] + input[x + 1][y1] * 68;

                x_image[x][y1] = x_compute[x][y1][3] / 235;
            }

            //y2
            if (y2 >= 1 && y2 <= M - 2)
            {
                //Unrolled loop
                x_compute[x][y2][1] = 0 + input[x + -1][y2] * 68;
                x_compute[x][y2][2] = x_compute[x][y2][1] + input[x][y2] * 99;
                x_compute[x][y2][3] = x_compute[x][y2][2] + input[x + 1][y2] * 68;

                x_image[x][y2] = x_compute[x][y2][3] / 235;
            }

            //y3
            if (y3 >= 1 && y3 <= M - 2)
            {
                //Unrolled loop
                x_compute[x][y3][1] = 0 + input[x + -1][y3] * 68;
                x_compute[x][y3][2] = x_compute[x][y3][1] + input[x][y3] * 99;
                x_compute[x][y3][3] = x_compute[x][y3][2] + input[x + 1][y3] * 68;

                x_image[x][y3] = x_compute[x][y3][3] / 235;
            }
        }

        /* PART A2 */
        //Loop merge; Part A2 y loop nested into Part A1 x loop
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            
            if (x >= 1 && x <= N - 2)
            {
                /*
                Fully unrolled k loop
                Removed abs() function call, rewrote inline & simplified
                Replaced Filter[] array access with hardcoded literals
                */
                xy_compute[x][y][1] = xy_compute[x][y][0] + x_image[x][y + -1] * 68;
                xy_compute[x][y][2] = xy_compute[x][y][1] + x_image[x][y + 0] * 99;
                xy_compute[x][y][3] = xy_compute[x][y][2] + x_image[x][y + 1] * 68;

                xy_image[x][y] = xy_compute[x][y][3] / 235;
            }

            //y1
            if (x >= 1 && x <= N - 2)
            {
                
                xy_compute[x][y1][1] = xy_compute[x][y1][0] + x_image[x][y1 + -1] * 68;
                xy_compute[x][y1][2] = xy_compute[x][y1][1] + x_image[x][y1 + 0] * 99;
                xy_compute[x][y1][3] = xy_compute[x][y1][2] + x_image[x][y1 + 1] * 68;

                xy_image[x][y1] = xy_compute[x][y1][3] / 235;
            }

            //y2
            if (x >= 1 && x <= N - 2)
            {
                
                xy_compute[x][y2][1] = xy_compute[x][y2][0] + x_image[x][y2 + -1] * 68;
                xy_compute[x][y2][2] = xy_compute[x][y2][1] + x_image[x][y2 + 0] * 99;
                xy_compute[x][y2][3] = xy_compute[x][y2][2] + x_image[x][y2 + 1] * 68;

                xy_image[x][y2] = xy_compute[x][y2][3] / 235;
            }

            //y3
            if (x >= 1 && x <= N - 2)
            {
                
                xy_compute[x][y3][1] = xy_compute[x][y3][0] + x_image[x][y3 + -1] * 68;
                xy_compute[x][y3][2] = xy_compute[x][y3][1] + x_image[x][y3 + 0] * 99;
                xy_compute[x][y3][3] = xy_compute[x][y3][2] + x_image[x][y3 + 1] * 68;

                xy_image[x][y3] = xy_compute[x][y3][3] / 235;
            }
        }
    }

    /* Part B */
    //Register blocking
    unsigned char xyImageIndexedValue;
    for (x = 0; x < N; x++)
    {
        //Unrolled loop by factor of 4
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;

            //y implies x, so x condition redundant (unless M & N values changed)
            if (y >= 1 && y <= M - 2)
            {
                xyImageIndexedValue = xy_image[x][y];

                /*
                Fully unrolled k loop
                Removed maximum() function call, rewrote inline & simplified
                Left abs() function call
                */
                if (abs(xy_image[x + 1][y + 1] - xyImageIndexedValue) > 0)
                {
                    diff_compute[x][y][1] = abs(xy_image[x + 1][y + 1] - xyImageIndexedValue);
                }

                if (abs(xy_image[x + 1][y] - xyImageIndexedValue) > diff_compute[x][y][1])
                {
                    diff_compute[x][y][2] = abs(xy_image[x + 1][y] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][2] = diff_compute[x][y][1];
                }

                if (abs(xy_image[x + 1][y + -1] - xyImageIndexedValue) > diff_compute[x][y][2])
                {
                    diff_compute[x][y][3] = abs(xy_image[x + 1][y + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][3] = diff_compute[x][y][2];
                }

                if (abs(xy_image[x][y + 1] - xyImageIndexedValue) > diff_compute[x][y][3])
                {
                    diff_compute[x][y][4] = abs(xy_image[x][y + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][4] = diff_compute[x][y][3];
                }

                if (abs(xy_image[x][y + -1] - xyImageIndexedValue) > diff_compute[x][y][4])
                {
                    diff_compute[x][y][5] = abs(xy_image[x][y + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][5] = diff_compute[x][y][4];
                }

                if (abs(xy_image[x + -1][y + 1] - xyImageIndexedValue) > diff_compute[x][y][5])
                {
                    diff_compute[x][y][6] = abs(xy_image[x + -1][y + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][6] = diff_compute[x][y][5];
                }

                if (abs(xy_image[x + -1][y] - xyImageIndexedValue) > diff_compute[x][y][6])
                {
                    diff_compute[x][y][7] = abs(xy_image[x + -1][y] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][7] = diff_compute[x][y][6];
                }

                if (abs(xy_image[x + -1][y + -1] - xyImageIndexedValue) > diff_compute[x][y][7])
                {
                    diff_compute[x][y][8] = abs(xy_image[x + -1][y + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y][8] = diff_compute[x][y][7];
                }

                edge_image[x][y] = diff_compute[x][y][8];
            }

            //y1
            if (y1 >= 1 && y1 <= M - 2)
            {
                xyImageIndexedValue = xy_image[x][y1];

                if (abs(xy_image[x + 1][y1 + 1] - xyImageIndexedValue) > diff_compute[x][y1][0])
                {
                    diff_compute[x][y1][1] = abs(xy_image[x + 1][y1 + 1] - xyImageIndexedValue);
                }

                if (abs(xy_image[x + 1][y1] - xyImageIndexedValue) > diff_compute[x][y1][1])
                {
                    diff_compute[x][y1][2] = abs(xy_image[x + 1][y1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][2] = diff_compute[x][y1][1];
                }

                if (abs(xy_image[x + 1][y1 + -1] - xyImageIndexedValue) > diff_compute[x][y1][2])
                {
                    diff_compute[x][y1][3] = abs(xy_image[x + 1][y1 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][3] = diff_compute[x][y1][2];
                }

                if (abs(xy_image[x][y1 + 1] - xyImageIndexedValue) > diff_compute[x][y1][3])
                {
                    diff_compute[x][y1][4] = abs(xy_image[x][y1 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][4] = diff_compute[x][y1][3];
                }

                if (abs(xy_image[x][y1 + -1] - xyImageIndexedValue) > diff_compute[x][y1][4])
                {
                    diff_compute[x][y1][5] = abs(xy_image[x][y1 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][5] = diff_compute[x][y1][4];
                }

                if (abs(xy_image[x + -1][y1 + 1] - xyImageIndexedValue) > diff_compute[x][y1][5])
                {
                    diff_compute[x][y1][6] = abs(xy_image[x + -1][y1 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][6] = diff_compute[x][y1][5];
                }

                if (abs(xy_image[x + -1][y1] - xyImageIndexedValue) > diff_compute[x][y1][6])
                {
                    diff_compute[x][y1][7] = abs(xy_image[x + -1][y1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][7] = diff_compute[x][y1][6];
                }

                if (abs(xy_image[x + -1][y1 + -1] - xyImageIndexedValue) > diff_compute[x][y1][7])
                {
                    diff_compute[x][y1][8] = abs(xy_image[x + -1][y1 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y1][8] = diff_compute[x][y1][7];
                }

                edge_image[x][y1] = diff_compute[x][y1][8];
            }

            //y2
            if (y2 >= 1 && y2 <= M - 2)
            {
                xyImageIndexedValue = xy_image[x][y2];

                if (abs(xy_image[x + 1][y2 + 1] - xyImageIndexedValue) > diff_compute[x][y2][0])
                {
                    diff_compute[x][y2][1] = abs(xy_image[x + 1][y2 + 1] - xyImageIndexedValue);
                }

                if (abs(xy_image[x + 1][y2] - xyImageIndexedValue) > diff_compute[x][y2][1])
                {
                    diff_compute[x][y2][2] = abs(xy_image[x + 1][y2] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][2] = diff_compute[x][y2][1];
                }

                if (abs(xy_image[x + 1][y2 + -1] - xyImageIndexedValue) > diff_compute[x][y2][2])
                {
                    diff_compute[x][y2][3] = abs(xy_image[x + 1][y2 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][3] = diff_compute[x][y2][2];
                }

                if (abs(xy_image[x][y2 + 1] - xyImageIndexedValue) > diff_compute[x][y2][3])
                {
                    diff_compute[x][y2][4] = abs(xy_image[x][y2 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][4] = diff_compute[x][y2][3];
                }

                if (abs(xy_image[x][y2 + -1] - xyImageIndexedValue) > diff_compute[x][y2][4])
                {
                    diff_compute[x][y2][5] = abs(xy_image[x][y2 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][5] = diff_compute[x][y2][4];
                }

                if (abs(xy_image[x + -1][y2 + 1] - xyImageIndexedValue) > diff_compute[x][y2][5])
                {
                    diff_compute[x][y2][6] = abs(xy_image[x + -1][y2 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][6] = diff_compute[x][y2][5];
                }

                if (abs(xy_image[x + -1][y2] - xyImageIndexedValue) > diff_compute[x][y2][6])
                {
                    diff_compute[x][y2][7] = abs(xy_image[x + -1][y2] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][7] = diff_compute[x][y2][6];
                }

                if (abs(xy_image[x + -1][y2 + -1] - xyImageIndexedValue) > diff_compute[x][y2][7])
                {
                    diff_compute[x][y2][8] = abs(xy_image[x + -1][y2 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y2][8] = diff_compute[x][y2][7];
                }

                edge_image[x][y2] = diff_compute[x][y2][8];
            }

            //y3
            if (y3 >= 1 && y3 <= M - 2)
            {
                xyImageIndexedValue = xy_image[x][y3];

                if (abs(xy_image[x + 1][y3 + 1] - xyImageIndexedValue) > diff_compute[x][y3][0])
                {
                    diff_compute[x][y3][1] = abs(xy_image[x + 1][y3 + 1] - xyImageIndexedValue);
                }

                if (abs(xy_image[x + 1][y3] - xyImageIndexedValue) > diff_compute[x][y3][1])
                {
                    diff_compute[x][y3][2] = abs(xy_image[x + 1][y3] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][2] = diff_compute[x][y3][1];
                }

                if (abs(xy_image[x + 1][y3 + -1] - xyImageIndexedValue) > diff_compute[x][y3][2])
                {
                    diff_compute[x][y3][3] = abs(xy_image[x + 1][y3 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][3] = diff_compute[x][y3][2];
                }

                if (abs(xy_image[x][y3 + 1] - xyImageIndexedValue) > diff_compute[x][y3][3])
                {
                    diff_compute[x][y3][4] = abs(xy_image[x][y3 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][4] = diff_compute[x][y3][3];
                }

                if (abs(xy_image[x][y3 + -1] - xyImageIndexedValue) > diff_compute[x][y3][4])
                {
                    diff_compute[x][y3][5] = abs(xy_image[x][y3 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][5] = diff_compute[x][y3][4];
                }

                if (abs(xy_image[x + -1][y3 + 1] - xyImageIndexedValue) > diff_compute[x][y3][5])
                {
                    diff_compute[x][y3][6] = abs(xy_image[x + -1][y3 + 1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][6] = diff_compute[x][y3][5];
                }

                if (abs(xy_image[x + -1][y3] - xyImageIndexedValue) > diff_compute[x][y3][6])
                {
                    diff_compute[x][y3][7] = abs(xy_image[x + -1][y3] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][7] = diff_compute[x][y3][6];
                }

                if (abs(xy_image[x + -1][y3 + -1] - xyImageIndexedValue) > diff_compute[x][y3][7])
                {
                    diff_compute[x][y3][8] = abs(xy_image[x + -1][y3 + -1] - xyImageIndexedValue);
                }
                else
                {
                    diff_compute[x][y3][8] = diff_compute[x][y3][7];
                }

                edge_image[x][y3] = diff_compute[x][y3][8];
            }
        }
    }

    /* Part C */
    for (x = 0; x < N; x++)
    {
        //Unrolled loop by factor of 4
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            unsigned char edgeImageIndexedValue;

            //y implies x, so x condition redundant (unless M & N values changed)
            if (y >= 1 && y <= M - 2)
            {
                edgeImageIndexedValue = edge_image[x][y];

                //Unrolled while loop
                if (edge_image[x + 1][y + 1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x + 1][y] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x + 1][y + -1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x][y + 1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x][y + -1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x + -1][y + 1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x + -1][y] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else if (edge_image[x + -1][y + -1] < edgeImageIndexedValue) 
                {
                    output[x][y] = 0;
                }
                else {
                    output[x][y] = 255;
                }
            }

            //y1
            if (y1 >= 1 && y1 <= M - 2)
            {
                edgeImageIndexedValue = edge_image[x][y1];

                if (edge_image[x + 1][y1 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x + 1][y1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x + 1][y1 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x][y1 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x][y1 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x + -1][y1 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x + -1][y1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else if (edge_image[x + -1][y1 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y1] = 0;
                }
                else
                {
                    output[x][y1] = 255;
                }
            }

            //y2
            if (y2 >= 1 && y2 <= M - 2)
            {
                edgeImageIndexedValue = edge_image[x][y2];

                if (edge_image[x + 1][y2 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x + 1][y2] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x + 1][y2 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x][y2 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x][y2 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x + -1][y2 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x + -1][y2] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else if (edge_image[x + -1][y2 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y2] = 0;
                }
                else
                {
                    output[x][y2] = 255;
                }
            }

            //y3
            if (y3 >= 1 && y3 <= M - 2)
            {
                edgeImageIndexedValue = edge_image[x][y3];

                if (edge_image[x + 1][y3 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x + 1][y3] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x + 1][y3 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x][y3 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x][y3 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x + -1][y3 + 1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x + -1][y3] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else if (edge_image[x + -1][y3 + -1] < edgeImageIndexedValue) 
                {
                    output[x][y3] = 0;
                }
                else
                {
                    output[x][y3] = 255;
                }
            }
        }
    }
}

void inefficient_routine()
{
    unsigned char out_compute;
    int x, y, k;
    const unsigned short int total = 235;

    /*  Part A */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (y >= C0 && y <= M - 1 - C0 && x >= C0 && x <= N - 1 - C0) {
                
                for (k = -C0; k <= C0; k++)
                    x_compute[x][y][1 + k + C0] = x_compute[x][y][C0 + k] + input[x + k][y] * Filter[abs(k)];

                x_image[x][y] = x_compute[x][y][(2 * C0) + 1] / total;
            }
            else //this is for image border pixels only
                x_image[x][y] = 0;

    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (y >= C0 && y <= M - 1 - C0 && x >= C0 && x <= N - 1 - C0) {
                
                for (k = -C0; k <= C0; k++)
                    xy_compute[x][y][C0 + k + 1] = xy_compute[x][y][C0 + k] + x_image[x][y + k] * Filter[abs(k)];

                xy_image[x][y] = xy_compute[x][y][(2 * C0) + 1] / total;
            }
            else
                xy_image[x][y] = 0;

    /*  Part B */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= C0 && x <= N - 1 - C0 && y >= C0 && y <= M - 1 - C0) {
                //0 = 0;
                for (k = 0; k <= C1 - 1; ++k)
                    diff_compute[x][y][k + 1] = maximum(abs(xy_image[x + x_offset[k]][y + y_offset[k]] - xy_image[x][y]), diff_compute[x][y][k]);

                edge_image[x][y] = diff_compute[x][y][C1];
            }
            else
                edge_image[x][y] = 0;

    /* Part C */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= C0 && x <= N - 1 - C0 && y >= C0 && y <= M - 1 - C0) {
                out_compute = 255;
                k = 0;
                while ((out_compute == 255) && (k <= C1 - 1)) {
                    if (edge_image[x + x_offset[k]][y + y_offset[k]] < edge_image[x][y]) 
                        out_compute = 0;
                    ++k;
                }
                output[x][y] = out_compute;
            }
            else
                output[x][y] = 0;
}


//returns false/true, when the output image is incorrect/correct, respectively
bool compare_images(){
	

  unsigned char out_compute;
  int x,y,k;
  unsigned short total=0;


/* start layer 2 code */

  /*  GaussBlur(in_image, g_image); */
  for (k=-C0; k<=C0; ++k)  total += Filter[abs(k)];

  for (x=0; x<N; x++)
    for (y=0; y<M; y++)
     if (x>=C0 && x<=N-1-C0 && y>=C0 && y<=M-1-C0) {
      x_compute[x][y][0]=0;
      for (k=-C0; k<=C0; ++k)
        x_compute[x][y][C0+k+1] = x_compute[x][y][C0+k]
           + input[x+k][y]*Filter[abs(k)];
      x_image[x][y]= x_compute[x][y][(2*C0)+1]/total;
      }
     else
      x_image[x][y] = 0;

  for (x=0; x<N; x++)
    for (y=0; y<M; y++)
     if (x>=C0 && x<=N-1-C0 && y>=C0 && y<=M-1-C0) {
      xy_compute[x][y][0]=0;
      for (k=-C0; k<=C0; ++k)
        xy_compute[x][y][C0+k+1] = xy_compute[x][y][C0+k] +
           x_image[x][y+k]*Filter[abs(k)];
      xy_image[x][y]= xy_compute[x][y][(2*C0)+1]/total;
      }
     else
      xy_image[x][y] = 0;


 /*  ComputeEdges(g_image, c_image); */
  for (x=0; x<N; x++)
    for (y=0; y<M; y++)
     if (x>=C0 && x<=N-1-C0 && y>=C0 && y<=M-1-C0) {
      //0 = 0;
      for (k=0; k<=C1-1; ++k)
        diff_compute[x][y][k+1] =
          maximum(abs(xy_image[x+x_offset[k]][y+y_offset[k]]
                    - xy_image[x][y]), diff_compute[x][y][k]);
      edge_image[x][y] = diff_compute[x][y][C1];
      }
     else
      edge_image[x][y] = 0;
  
  /*  DetectRoots(c_image, out_image); */
  for (x=0; x<N; x++)
    for (y=0; y<M; y++)
     if (x>=C0 && x<=N-1-C0 && y>=C0 && y<=M-1-C0) {
        out_compute = 255; 
        k = 0;
        while ((out_compute == 255) && (k <= C1-1)) {
          if (edge_image[x+x_offset[k]][y+y_offset[k]] <
              edge_image[x][y]) out_compute = 0;
          ++k; }
        if (output[x][y] != out_compute)
					return false;
        }
     else
          if (output[x][y] != 0)
					  return false;
		
					
					return true;
	}
	