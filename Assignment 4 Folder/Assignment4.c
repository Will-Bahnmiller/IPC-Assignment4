/* Will Bahnmiller
   COP 3223-0002
   10/22/2011
   Assignment 4
   Performs 3 picture operations: brighten,
   subtract two, or outline */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define  BUFFER_SIZE  70
#define  TRUE          1
#define  FALSE         0

int**  img;
int**  img2;
int**  img3;
int**  temppicx, temppicy;
int**  img, sobelout;
int    numRows;
int    numCols;
int    maxVal;
FILE*  fo1;

void addtopixels(int** imgtemp, int value);
void  subtopixels(int** img, int** img2, int** img3);
void  writeoutpic(char* fileName, int** imgtemp);
int** readpic(char* fileName);
void  readHeader(FILE* imgFin);
int   isComment(char* line);
void  readImgID(char* line);
void  readImgSize(char* line);
void  readMaxVal(char* line);
int** setImage();
void  readBinaryData(FILE* imgFin, int** imgtemp);
void sobelfunc(int** pic, int** edges, int** tempx, int** tempy);

int main()
{
        char fileName[BUFFER_SIZE];
        int i, j, rows, cols, MENU, VALUE;
        char ci;
        
  // MENU text
  printf("\n~ Welcome to the picture program! ~\n\n");
  printf("   [1] - Brighten a picture\n   [2] - Subtract two pictures\n   [3] - Highlight edges in a picture\n");
  scanf("%d", &MENU);
  
  
      if (MENU == 1) {
        
        // Entering in a picture
        printf("\nEnter image filename: ");
        scanf("%s", fileName);
        printf("\n");
        
        // Entering in a value
        printf("Enter a brightness value: ");
        scanf("%d", &VALUE);
        printf("\n");

        // Using readpic on the file, then putting the
        // returned value into the 2D array "img"
        img = readpic(fileName);


        printf("Successfully read image file '%s'\n", fileName);

        // Using addtopixels to brighten img by VALUE
        addtopixels(img,VALUE);

        // Outputting the resultant picture
        printf("\n\nEnter image filename for output: ");
        scanf("%s", fileName);
        printf("\n");

        writeoutpic(fileName,img);

        // Reset the data for img
        free(img);
        img = NULL;

        system("pause");
        return(EXIT_SUCCESS);

          
        }
///////////////////////////////////////

  else if (MENU == 3) {
       
        // Reading in the picture
        printf("\nEnter image filename: ");
        scanf("%s", fileName);
        printf("\n");

        img = readpic(fileName);

        printf("Successfully read image file '%s'\n", fileName);
       
        // Code from sobelcallfrag.c
        sobelout= setImage();

        temppicx= setImage();
        temppicy= setImage();

        sobelfunc(img,sobelout,temppicx,temppicy); 

        // Outputting the resultant picture
        printf("\nEnter image filename for output: ");
        scanf("%s", fileName);

        writeoutpic(fileName,sobelout);
        
        printf("\nGoodbye!\n");

       }
/////////////////////////////////////////////////////

  else if (MENU == 2) {
       
        // Entering in a picture
        printf("\nEnter image1 filename: ");
        scanf("%s", fileName);
        printf("\n");
        
        // Process picture 1
        img = readpic(fileName);
        
        printf("Successfully read image file '%s'\n", fileName);

        // Entering in a second picture
        printf("\nEnter image2 filename: ");
        scanf("%s", fileName);
        printf("\n");
        
        // Process picture 2
        img2 = readpic(fileName);
        
        printf("Successfully read image file '%s'\n", fileName);
        
        // setimage to output picture to set it up
        // for processing
        img3 = setImage();
        
        // Subract the pictures with the function
        subtopixels(img, img2, img3);

        // Outputting the resultant picture
        printf("\n\nEnter image filename for output: ");
        scanf("%s", fileName);
        printf("\n");

        writeoutpic(fileName,img3);
        
        printf("\nGoodbye!\n");
  
     }
//////////////////////////////////////
        
  
  system("pause");
  return 0;
} // End of int main

void addtopixels(int** imgtemp, int value)
{  
        int i,j;
        
        for (i=0;i<numRows;i++)
        { for (j=0;j<numCols;j++)
                {
                  imgtemp[i][j] += value;
                }
        }
}

void writeoutpic(char* fileName, int** imgtemp)
{
        int i,j;
        char ci;
        FILE* fo1;
        
        if((fo1 = fopen(fileName, "wb")) == NULL)
        {
                printf("Unable to open out image file '%s'\n", fileName);
                exit(EXIT_FAILURE);
        }

        fprintf(fo1,"P5\n");
        fprintf(fo1,"%d %d\n", numRows, numCols);
        fprintf(fo1,"255\n");

        for (i=0;i<numRows;i++)
        { for (j=0;j<numCols;j++)
                {
                  ci   =  (char) (imgtemp[i][j]);
                  fprintf(fo1,"%c", ci);
                }
        }
}




int** readpic(char* fileName)
{

        FILE* imgFin;
        int** imgtemp;

        if((imgFin = fopen(fileName, "rb")) == NULL)
        {
                printf("Unable to open image file '%s'\n", fileName);
                exit(EXIT_FAILURE);
        }

        readHeader(imgFin);


        imgtemp  = setImage();

        readBinaryData(imgFin, imgtemp);

        fclose(imgFin);
        
        return  imgtemp;

}

void readHeader(FILE* imgFin)
{
        int  haveReadImgID   = FALSE;
        int  haveReadImgSize = FALSE;
        int  haveReadMaxVal  = FALSE;
        char line[BUFFER_SIZE];

        while(!(haveReadImgID && haveReadImgSize && haveReadMaxVal))
        {
                fgets(line, BUFFER_SIZE, imgFin);

                if((strlen(line) == 0) || (strlen(line) == 1))
                        continue;

                if(isComment(line))
                        continue;

                if(!(haveReadImgID))
                {
                        readImgID(line);
                        haveReadImgID = TRUE;
                }
                else if(!(haveReadImgSize))
                {
                        readImgSize(line);
                        haveReadImgSize = TRUE;
                }
                else if(!(haveReadMaxVal))
                {
                        readMaxVal(line);
                        haveReadMaxVal = TRUE;
                }
        }

}

int isComment(char *line)
{
        if(line[0] == '#')
                return(TRUE);

        return(FALSE);
}

void readImgID(char* line)
{
        if(strcmp(line, "P5\n") != 0)
        {
                printf("Invalid image ID\n");
                exit(EXIT_FAILURE);
        }
}

void readImgSize(char* line)
{
        unsigned i;

        for(i = 0; i < strlen(line); ++i)
        {
                if(!((isdigit((int) line[i])) || (isspace((int) line[i]))))
                {
                        printf("Invalid image size\n");
                        exit(EXIT_FAILURE);
                }
        }

        sscanf(line, "%d %d", &numRows, &numCols);
}

void readMaxVal(char* line)
{
        unsigned i;

        for(i = 0; i < strlen(line); ++i)
        {
                if(!((isdigit((int) line[i])) || (isspace((int) line[i]))))
                {
                        printf("Invalid image max value\n");
                        exit(EXIT_FAILURE);
                }
        }

        maxVal = atoi(line);
}

int** setImage()
{
        int** imgtemp;
        unsigned i;

        imgtemp = (int**) calloc(numRows, sizeof(int*));

        for(i = 0; i < numRows; ++i)
        {
                imgtemp[i] = (int*) calloc(numCols, sizeof(int));
        }
        return imgtemp;
}

void readBinaryData(FILE* imgFin, int** imgtemp)
{
        unsigned  i;
        unsigned  j;
        for(i = 0; i < numRows; ++i)
        {
                for(j = 0; j < numCols; ++j)
                {
                            imgtemp[i][j] = 
                            fgetc(imgFin);
                }
        }
}

void sobelfunc(int** pic, int** edges, int** tempx, int** tempy){

        int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
        int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        int maxival;


        
 
        int i,j,p,q,mr,sum1,sum2;
        double threshold;
         

        mr = 1;


        for (i=mr;i<numRows-mr;i++)
        { for (j=mr;j<numCols-mr;j++)
          {
             sum1 = 0;
             sum2 = 0;
             for (p=-mr;p<=mr;p++)
             {
                for (q=-mr;q<=mr;q++)
                {
                   sum1 += pic[i+p][j+q] * maskx[p+mr][q+mr];
                   sum2 += pic[i+p][j+q] * masky[p+mr][q+mr];
                }
             }
             tempx[i][j] = sum1;
             tempy[i][j] = sum2;
          }
        }

        maxival = 0;
        for (i=mr;i<numRows-mr;i++)
        { for (j=mr;j<numCols-mr;j++)
          {
             edges[i][j]=(int) (sqrt((double)((tempx[i][j]*tempx[i][j]) +
                                      (tempy[i][j]*tempy[i][j]))));
             if (edges[i][j] > maxival)
                maxival = edges[i][j];

           }
        }



        for (i=0;i<numRows;i++)
          { for (j=0;j<numCols;j++)
            {
             edges[i][j] = ((edges[i][j]*1.0) / maxival) * 255;            
             
            }
          }
}

void subtopixels(int** img, int** img2, int** img3){
     
     int i, j;
     
     for (i=0; i<numRows; i++){
         for (j=0; j<numCols; j++){
             img3[i][j] = abs(img[i][j] - img2[i][j]);
             }
         } 
     
     }
