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
void efficient_routine2();


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
  efficient_routine2();
  //efficient_routine();
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
    unsigned char out_compute;
    int x, y, k;
    const unsigned short int total = 235;

    //Merging problem probs due to x_image dependency/dependencies
    /*  Part A */
    //x dimension of image (n = absolute horizontal size)
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2) {
                x_compute[x][y][0] = 0;
                //xy_compute[x][y][0] = 0; //Merged code

                //Unrolled loop
                /*x_compute[x][y][1] = x_compute[x][y][0] + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;*/

                //Original loop
                for (k = -1; k <= 1; k++)
                {
                    x_compute[x][y][k + 2] = x_compute[x][y][k + 1] + input[x + k][y] * Filter[abs(k)];
                    //xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)]; //Merged code, old
                }
                x_image[x][y] = x_compute[x][y][3] / 235;

                //for (k = -1; k <= 1; k++)
                //{
                    //xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)]; //Merged code
                //}
                
                //xy_image[x][y] = xy_compute[x][y][3] / total; //Merged code

                //Second Degree Merged code vv
                /*xy_compute[x][y][0] = 0;
                for (k = -1; k <= 1; k++)
                    xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)];

                xy_image[x][y] = xy_compute[x][y][3] / total;*/
                //Merged code ^^
            }
            else //this is for image border pixels only
                x_image[x][y] = 0;
                //xy_image[x][y] = 0; //Merged code
            
            //First Degree Merged code (breaks)
            /*if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2) {
                xy_compute[x][y][0] = 0;
                for (k = -1; k <= 1; k++)
                    xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)];

                xy_image[x][y] = xy_compute[x][y][3] / total;
            }
            else
                xy_image[x][y] = 0;*/

    //Unmerged code
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2) {
                xy_compute[x][y][0] = 0;
                for (k = -1; k <= 1; k++)
                    xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)];

                xy_image[x][y] = xy_compute[x][y][3] / total;
            }
            else
                xy_image[x][y] = 0;

    /*  Part B */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= 1 && x <= N - 2 && y >= 1 && y <= M - 2) {
                diff_compute[x][y][0] = 0;

                //Removed function calling & unrolled loop (still need to unroll)
                /*if ((xy_image[x + x_offset[k]][y + y_offset[k]] - xy_image[x][y]) > diff_compute[x][y][k] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][k + 1] = xy_image[x + x_offset[k]][y + y_offset[k]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][k + 1] = diff_compute[x][y][k];
                }*/

                //Iteration 1
                /*
                if (abs(xy_image[x + x_offset[0]][y + y_offset[0]] - xy_image[x][y]) > diff_compute[x][y][0] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][1] = xy_image[x + x_offset[0]][y + y_offset[0]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][1] = diff_compute[x][y][0];
                }

                //Iteration 2
                if (abs(xy_image[x + x_offset[1]][y + y_offset[1]] - xy_image[x][y]) > diff_compute[x][y][1] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][2] = xy_image[x + x_offset[1]][y + y_offset[1]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][2] = diff_compute[x][y][1];
                }

                //Iteration 3
                if (abs(xy_image[x + x_offset[2]][y + y_offset[2]] - xy_image[x][y]) > diff_compute[x][y][2] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][3] = xy_image[x + x_offset[2]][y + y_offset[2]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][3] = diff_compute[x][y][2];
                }

                //Iteration 4
                if (abs(xy_image[x + x_offset[3]][y + y_offset[3]] - xy_image[x][y]) > diff_compute[x][y][3] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][4] = xy_image[x + x_offset[3]][y + y_offset[3]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][4] = diff_compute[x][y][3];
                }

                //Iteration 5
                if (abs(xy_image[x + x_offset[4]][y + y_offset[4]] - xy_image[x][y]) > diff_compute[x][y][4] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][5] = xy_image[x + x_offset[4]][y + y_offset[4]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][5] = diff_compute[x][y][4];
                }

                //Iteration 6
                if (abs(xy_image[x + x_offset[5]][y + y_offset[5]] - xy_image[x][y]) > diff_compute[x][y][5] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][6] = xy_image[x + x_offset[5]][y + y_offset[5]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][6] = diff_compute[x][y][5];
                }

                //Iteration 7
                if (abs(xy_image[x + x_offset[6]][y + y_offset[6]] - xy_image[x][y]) > diff_compute[x][y][6] == true)
                {
                    //Equiv of returning 'a'
                    diff_compute[x][y][7] = xy_image[x + x_offset[6]][y + y_offset[6]] - xy_image[x][y];
                }
                else
                {
                    //Equiv of returning 'b'
                    diff_compute[x][y][7] = diff_compute[x][y][6];
                }
                */

                /* From now unused function
                unsigned char maximum(unsigned char a, unsigned char b)
                {
                    return a > b ? a : b;
                }
                */

                //Unrolled loop (still function calling)
                diff_compute[x][y][1] = maximum(abs(xy_image[x + x_offset[0]][y + y_offset[0]] - xy_image[x][y]), diff_compute[x][y][0]);
                diff_compute[x][y][2] = maximum(abs(xy_image[x + x_offset[1]][y + y_offset[1]] - xy_image[x][y]), diff_compute[x][y][1]);
                diff_compute[x][y][3] = maximum(abs(xy_image[x + x_offset[2]][y + y_offset[2]] - xy_image[x][y]), diff_compute[x][y][2]);
                diff_compute[x][y][4] = maximum(abs(xy_image[x + x_offset[3]][y + y_offset[3]] - xy_image[x][y]), diff_compute[x][y][3]);
                diff_compute[x][y][5] = maximum(abs(xy_image[x + x_offset[4]][y + y_offset[4]] - xy_image[x][y]), diff_compute[x][y][4]);
                diff_compute[x][y][6] = maximum(abs(xy_image[x + x_offset[5]][y + y_offset[5]] - xy_image[x][y]), diff_compute[x][y][5]);
                diff_compute[x][y][7] = maximum(abs(xy_image[x + x_offset[6]][y + y_offset[6]] - xy_image[x][y]), diff_compute[x][y][6]);
                diff_compute[x][y][8] = maximum(abs(xy_image[x + x_offset[7]][y + y_offset[7]] - xy_image[x][y]), diff_compute[x][y][7]);

                //Original loop
                /*for (k = 0; k <= 7; ++k)
                {
                    diff_compute[x][y][k + 1] = maximum(abs(xy_image[x + x_offset[k]][y + y_offset[k]] - xy_image[x][y]), diff_compute[x][y][k]);
                }*/

                edge_image[x][y] = diff_compute[x][y][8];
            }
            else
                edge_image[x][y] = 0;

    /* Part C */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= 1 && x <= N - 2 && y >= 1 && y <= M - 2) {
                out_compute = 255;
                //k = 0;

                //Unrolled loop
                if (edge_image[x + x_offset[0]][y + y_offset[0]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[1]][y + y_offset[1]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[2]][y + y_offset[2]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[3]][y + y_offset[3]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[4]][y + y_offset[4]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[5]][y + y_offset[5]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[6]][y + y_offset[6]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[7]][y + y_offset[7]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }

                //Original loop
                /*while ((out_compute == 255) && (k <= 7)) {
                    if (edge_image[x + x_offset[k]][y + y_offset[k]] < edge_image[x][y]) 
                        out_compute = 0;
                    ++k;
                }*/
                output[x][y] = out_compute;
            }
            else
                output[x][y] = 0;
}

void efficient_routine2()
{
    unsigned char out_compute;
    int x, y, k;
    const unsigned short int total = 235;

    //Optimisations
    int y1, y2, y3;

    //Merging problem probs due to x_image dependency/dependencies
    /*  Part A */
    //x dimension of image (n = absolute horizontal size)
    for (x = 0; x < N; x++)
    {
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;

            if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2)
            {
                x_compute[x][y][0] = 0;

                //Unrolled loop
                /*x_compute[x][y][1] = x_compute[x][y][0] + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;*/

                //Original loop
                for (k = -1; k <= 1; k++)
                {
                    x_compute[x][y][k + 2] = x_compute[x][y][k + 1] + input[x + k][y] * Filter[abs(k)];
                }
                x_image[x][y] = x_compute[x][y][3] / 235;
            }
            else
            {
                //this is for image border pixels only
                x_image[x][y] = 0;
            }

            //y1
            if (y1 >= 1 && y1 <= M - 2 && x >= 1 && x <= N - 2)
            {
                x_compute[x][y1][0] = 0;

                //Unrolled loop
                /*x_compute[x][y][1] = x_compute[x][y][0] + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;*/

                //Original loop
                for (k = -1; k <= 1; k++)
                {
                    x_compute[x][y1][k + 2] = x_compute[x][y1][k + 1] + input[x + k][y1] * Filter[abs(k)];
                }
                x_image[x][y1] = x_compute[x][y1][3] / 235;
            }
            else
            {
                //this is for image border pixels only
                x_image[x][y1] = 0;
            }

            //y2
            if (y2 >= 1 && y2 <= M - 2 && x >= 1 && x <= N - 2)
            {
                x_compute[x][y2][0] = 0;

                //Unrolled loop
                /*x_compute[x][y][1] = x_compute[x][y][0] + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;*/

                //Original loop
                for (k = -1; k <= 1; k++)
                {
                    x_compute[x][y2][k + 2] = x_compute[x][y2][k + 1] + input[x + k][y2] * Filter[abs(k)];
                }
                x_image[x][y2] = x_compute[x][y2][3] / 235;
            }
            else
            {
                //this is for image border pixels only
                x_image[x][y2] = 0;
            }

            //y3
            if (y3 >= 1 && y3 <= M - 2 && x >= 1 && x <= N - 2)
            {
                x_compute[x][y3][0] = 0;

                //Unrolled loop
                /*x_compute[x][y][1] = x_compute[x][y][0] + input[x + -1][y] * 68;
                x_compute[x][y][2] = x_compute[x][y][1] + input[x][y] * 99;
                x_compute[x][y][3] = x_compute[x][y][2] + input[x + 1][y] * 68;*/

                //Original loop
                for (k = -1; k <= 1; k++)
                {
                    x_compute[x][y3][k + 2] = x_compute[x][y3][k + 1] + input[x + k][y3] * Filter[abs(k)];
                }
                x_image[x][y3] = x_compute[x][y3][3] / 235;
            }
            else
            {
                //this is for image border pixels only
                x_image[x][y3] = 0;
            }
        }
    }
    for (x = 0; x < N; x++)
    {
        for (y = 0; y < M; y+=4)
        {
            //Register blocking
            y1 = y + 1;
            y2 = y + 2;
            y3 = y + 3;
            
            if (y >= 1 && y <= M - 2 && x >= 1 && x <= N - 2)
            {
                xy_compute[x][y][0] = 0;
                for (k = -1; k <= 1; k++)
                {
                    xy_compute[x][y][k + 2] = xy_compute[x][y][k + 1] + x_image[x][y + k] * Filter[abs(k)];
                }
                xy_image[x][y] = xy_compute[x][y][3] / total;
            }
            else
            {
                xy_image[x][y] = 0;
            }

            //y1
            if (y1 >= 1 && y1 <= M - 2 && x >= 1 && x <= N - 2)
            {
                xy_compute[x][y1][0] = 0;
                for (k = -1; k <= 1; k++)
                {
                    xy_compute[x][y1][k + 2] = xy_compute[x][y1][k + 1] + x_image[x][y1 + k] * Filter[abs(k)];
                }
                xy_image[x][y1] = xy_compute[x][y1][3] / total;
            }
            else
            {
                xy_image[x][y1] = 0;
            }

            //y2
            if (y2 >= 1 && y2 <= M - 2 && x >= 1 && x <= N - 2)
            {
                xy_compute[x][y2][0] = 0;
                for (k = -1; k <= 1; k++)
                {
                    xy_compute[x][y2][k + 2] = xy_compute[x][y2][k + 1] + x_image[x][y2 + k] * Filter[abs(k)];
                }
                xy_image[x][y2] = xy_compute[x][y2][3] / total;
            }
            else
            {
                xy_image[x][y2] = 0;
            }

            //y3
            if (y3 >= 1 && y3 <= M - 2 && x >= 1 && x <= N - 2)
            {
                xy_compute[x][y3][0] = 0;
                for (k = -1; k <= 1; k++)
                {
                    xy_compute[x][y3][k + 2] = xy_compute[x][y3][k + 1] + x_image[x][y3 + k] * Filter[abs(k)];
                }
                xy_image[x][y3] = xy_compute[x][y3][3] / total;
            }
            else
            {
                xy_image[x][y3] = 0;
            }
        }
    }

    /*  Part B */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= 1 && x <= N - 2 && y >= 1 && y <= M - 2) {
                diff_compute[x][y][0] = 0;

                //Unrolled loop (still function calling)
                diff_compute[x][y][1] = maximum(abs(xy_image[x + x_offset[0]][y + y_offset[0]] - xy_image[x][y]), diff_compute[x][y][0]);
                diff_compute[x][y][2] = maximum(abs(xy_image[x + x_offset[1]][y + y_offset[1]] - xy_image[x][y]), diff_compute[x][y][1]);
                diff_compute[x][y][3] = maximum(abs(xy_image[x + x_offset[2]][y + y_offset[2]] - xy_image[x][y]), diff_compute[x][y][2]);
                diff_compute[x][y][4] = maximum(abs(xy_image[x + x_offset[3]][y + y_offset[3]] - xy_image[x][y]), diff_compute[x][y][3]);
                diff_compute[x][y][5] = maximum(abs(xy_image[x + x_offset[4]][y + y_offset[4]] - xy_image[x][y]), diff_compute[x][y][4]);
                diff_compute[x][y][6] = maximum(abs(xy_image[x + x_offset[5]][y + y_offset[5]] - xy_image[x][y]), diff_compute[x][y][5]);
                diff_compute[x][y][7] = maximum(abs(xy_image[x + x_offset[6]][y + y_offset[6]] - xy_image[x][y]), diff_compute[x][y][6]);
                diff_compute[x][y][8] = maximum(abs(xy_image[x + x_offset[7]][y + y_offset[7]] - xy_image[x][y]), diff_compute[x][y][7]);

                edge_image[x][y] = diff_compute[x][y][8];
            }
            else
                edge_image[x][y] = 0;

    /* Part C */
    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (x >= 1 && x <= N - 2 && y >= 1 && y <= M - 2) {
                out_compute = 255;
                //k = 0;

                //Unrolled loop
                if (edge_image[x + x_offset[0]][y + y_offset[0]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[1]][y + y_offset[1]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[2]][y + y_offset[2]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[3]][y + y_offset[3]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[4]][y + y_offset[4]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[5]][y + y_offset[5]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[6]][y + y_offset[6]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }
                else if (edge_image[x + x_offset[7]][y + y_offset[7]] < edge_image[x][y]) 
                {
                    out_compute = 0;
                }

                output[x][y] = out_compute;
            }
            else
                output[x][y] = 0;
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
                x_compute[x][y][0] = 0;
                for (k = -C0; k <= C0; k++)
                    x_compute[x][y][1 + k + C0] = x_compute[x][y][C0 + k] + input[x + k][y] * Filter[abs(k)];

                x_image[x][y] = x_compute[x][y][(2 * C0) + 1] / total;
            }
            else //this is for image border pixels only
                x_image[x][y] = 0;

    for (x = 0; x < N; x++)
        for (y = 0; y < M; y++)
            if (y >= C0 && y <= M - 1 - C0 && x >= C0 && x <= N - 1 - C0) {
                xy_compute[x][y][0] = 0;
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
                diff_compute[x][y][0] = 0;
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
      diff_compute[x][y][0] = 0;
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
	