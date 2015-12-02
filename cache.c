#include <stdio.h>
#include <stdlib.h>

/*	Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

#define CACHE_16 16
#define CACHE_256 256
/* A struct to keep information of the cache */
typedef struct {
   int size; //Cache size can be either 16 or 256
   int assoc; //Cache associativity can be either 1,2 or 4
   int ***numArr; //Array to keep track of addresses
   int **flagArr; //Array to keep track of which cache index is set
   int hit;
   int count;
} Cache;

/* Global variables to keep track of # of read and write to cache */
static Cache cache;
static int readAccess = 0;
static int writeAccess = 0;

/* This function is to initiate a cache */
void InitiateCache(int size, int assoc) {
   int idx, row, col;

   cache.hit = 0;
   cache.count = 0;
   cache.size = size;  //height
   cache.assoc = assoc; //ways
   cache.numArr = calloc(sizeof(int **), size);
   cache.flagArr = calloc(sizeof(int *), size);

   for (idx = 0; idx < size; idx++) {
      cache.numArr[idx] = calloc(sizeof(int *), assoc);
      cache.flagArr[idx] = calloc(sizeof(int), assoc);
   }

   //Set all value in cache 0 to indicate it's not yet set
   for (row = 0; row < size; row++) {
      for (col = 0; col < assoc; col++) {
         cache.flagArr[row][col] = 0;
      }
   }
}

/* Clean cache and free everything */
void CleanCache(Cache *cache) {
	int ndx;
	for (ndx = 0; ndx < cache.size, ndx++) {
		free(cache.numArr[ndx]);
		free(cache.flagArr[ndx]);
	}
	free(cache.numArr);
	free(cache.flagArr);
}

/* If found item, need to move it to the end of cache */
void MoveCache(int row, int column) {
   int idx, *temp = cache.numArr[row][column];
   
   for (idx = column; idx + 1 < cache.assoc && cache.numArr[row][idx + 1]; idx++) {
      cache.numArr[row][idx] = cache.numArr[row][idx + 1];
   }
   cache.numArr[row][idx] = temp;
}

/* This function is to check if a an item was found in the cache */
int CheckHit(int *mp, int idx) {
   int hit = 0, i;

   for (i = 0; i < cache.assoc; i++) {
      if (mp == cache.flagArr[idx]) {
         hit = 1;
         if (cache.size > 1 && cache.flagArr[idx][i + 1]) {
            MoveCache(idx, mp);
         }
         break;
      }
   }

   return hit;
}

/* This function is to read from cache */
void mem_read(int *mp)
{
   int index = ((int) mp % cache.size);
   readAccess++;
   cache.count++;
   if (CheckHit(mp, index)) {
      cache.hit++;
   }
   printf("Memory read from location %p\n", mp);
}

/* This function is to write to cache */
void mem_write(int *mp)
{
   writeAccess++;
   printf("Memory write to location %p\n", mp);
}

int main()
{
   int a[10][10], b[10][10], mult[10][10], r1, c1, r2, c2, i, j, k;

   int *mp1, *mp2, *mp3;
   int size, assoc;

   printf("Please enter cache size (16 or 256): ");
   scanf("%d", &size);
   while (size != CACHE_16  || size != CACHE_256) {
      printf("Please enter a valid cache size (16 or 256): ");
      scanf("%d", &size);
   }

   printf("Please enter cache associativity (1, 2 or 4): ");
   scanf("%d", &assoc);
   while (assoc != 1 || assoc != 2 || assoc != 4) {
      printf("Please enter a valid cache associativity (1, 2 or 4): ");
      scanf("%d", &assoc);
   }

   InitiateCache(size, assoc);

   printf("Size of pointer is: %lu\n\n", sizeof(mp1));
   printf("Enter rows and column for first matrix: ");
   scanf("%d%d", &r1, &c1);
   printf("Enter rows and column for second matrix: ");
   scanf("%d%d",&r2, &c2);

   /* If colum of first matrix in not equal to row of second matrix, asking user to enter the size of matrix again. */
   while (c1!=r2)
   {
      printf("Error! column of first matrix not equal to row of second.\n");
      printf("Enter rows and column for first matrix: ");
      scanf("%d%d", &r1, &c1);
      printf("Enter rows and column for second matrix: ");
      scanf("%d%d",&r2, &c2);
   }

   /* Storing elements of first matrix. */
   //printf("\nEnter elements of matrix 1:\n");
   for(i=0; i<r1; ++i)
      for(j=0; j<c1; ++j)
      {
         a[i][j] = i+j; // build sample data

      }

   /* Storing elements of second matrix. */
   //printf("\nEnter elements of matrix 2:\n");
   for(i=0; i<r2; ++i)
      for(j=0; j<c2; ++j)
      {
         b[i][j] = 10 + i + j;
      }

   /* Initializing elements of matrix mult to 0.*/
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
         mult[i][j]=0;
      }

   /* Multiplying matrix a and b and storing in array mult. */
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
         for(k=0; k<c1; ++k)
         {
            mp1 = &mult[i][j];
            mp2 = &a[i][k];
            mp3 = &b[k][j];

            mem_read(mp1);
            mem_read(mp2);
            mem_read(mp3);
            mult[i][j]+=a[i][k]*b[k][j];
            mem_write(mp1);
         }

   /* Displaying the multiplication of two matrix. */
   printf("\nOutput Matrix:\n");
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
         printf("%d  ",mult[i][j]);
         if(j==c2-1)
            printf("\n\n");
      }

   printf("For a cache size %d, matrix size is %dx%d, associativity %d.\n",
         cache.size, r1, c1, cache.assoc);
   printf("Ratio of read to write accesses is %lf.\n", (double) readAccess/writeAccess);
   printf("\"Hit rate\" for the cache is %lf%c\n", ((double) cache.hit / cache.count) * 100, '%');

   return 0;
}
