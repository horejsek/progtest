#ifndef __PROGTEST__
#include <pthread.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>

#define MAX_TERRORISTS   32

using namespace std;

#define TERRORIST_SIZE  100
#define IMAGE_SIZE      400

typedef struct TTerrorist
 {
   int              m_W;
   int              m_H;
   unsigned char    m_Data[TERRORIST_SIZE][TERRORIST_SIZE];
 } TTERRORIST;

typedef struct TImage
 {
   int              m_W;
   int              m_H;
   unsigned char    m_Data[IMAGE_SIZE][IMAGE_SIZE];
 } TIMAGE;

void  TerroristHunter    ( int               databaseSize,
                           TTERRORIST      * database,
                           int               processes,
                           TIMAGE        *(* scanner) ( void ),
                           void           (* officer) ( TIMAGE *, int ) );

/* DBG */

int DBG_CIM = 0;

#endif /* __PROGTEST__ */

/******************************************************************************/
/* HEADERS */
/******************************************************************************/

void fillQueueByScanner(void);
void myThread(int);

void copyImage(TIMAGE*, TIMAGE*);
int compareImage(TTERRORIST*, TIMAGE*);

/******************************************************************************/
/* GLOBALS */
/******************************************************************************/

int DB_SIZE;
TTERRORIST* DB_IMAGES;

void (*OFFICER)(TIMAGE*, int);
TIMAGE* (*SCANNER)(void);

int PROCESSES;
int* END;

int IM_MAX = 20;
int QUEUE_MAX = 20;

int* QUEUE_FIRST;
int* QUEUE_LAST;

/* GLOBALS of PROCESSES */

sem_t* MUTEX;
sem_t* SEMAPHORE;
sem_t* SEMAPHORE2;

/* STRUCTS */

struct IMAGE_NODE {
    TIMAGE image;
    int foundFlags;
    int needCompares; // 0 mean "done, send to officer"
    int done;
};
IMAGE_NODE* IM;

struct QUEUE_NODE {
    int terrorist;
    int image;
};
QUEUE_NODE* QUEUE;

/******************************************************************************/
/* MAIN FUNCTION */
/******************************************************************************/

void TerroristHunter(int databaseSize, TTERRORIST* database, int processes, TIMAGE* (*scanner)(void), void (*officer)(TIMAGE*, int))
{
    DB_SIZE = databaseSize;
    DB_IMAGES = database;
    
    OFFICER = officer;
    SCANNER = scanner;
    
    PROCESSES = processes;
    END = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    *END = 0;
    
    QUEUE_FIRST = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    QUEUE_LAST = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    *QUEUE_FIRST = -1;
    *QUEUE_LAST = -1;
    
    IM = (IMAGE_NODE*)mmap(0, IM_MAX*sizeof(IMAGE_NODE), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    QUEUE = (QUEUE_NODE*)mmap(0, QUEUE_MAX*sizeof(QUEUE_NODE), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    
    MUTEX = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    SEMAPHORE = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    SEMAPHORE2 = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    sem_init(MUTEX, 1, 1);
    sem_init(SEMAPHORE, 1, 20);
    sem_init(SEMAPHORE2, 1, 0);
    
    for(int i = 0; i < IM_MAX; i++) {
        IM[i].done = 1;
    }

    int id;
	for (id = 0; id < PROCESSES; ++id) {
		int pid = fork();
		if (pid == 0) break; /* potomek */
	}

    if(id == PROCESSES) {
        fillQueueByScanner();
    } else {
        myThread(id);
    }
    
    sem_destroy(MUTEX);
    sem_destroy(SEMAPHORE);
    sem_destroy(SEMAPHORE2);
    
    munmap((void *)MUTEX, sizeof(sem_t));
    munmap((void *)SEMAPHORE, sizeof(sem_t));
    munmap((void *)SEMAPHORE2, sizeof(sem_t));
    
    munmap((void *)QUEUE_FIRST, sizeof(int));
    munmap((void *)QUEUE_LAST, sizeof(int));
    munmap((void *)END, sizeof(int));
    
    munmap((void *)IM, IM_MAX*sizeof(IMAGE_NODE));
    munmap((void *)QUEUE, QUEUE_MAX*sizeof(QUEUE_NODE));
}

/******************************************************************************/
/* PARENT */
/******************************************************************************/

void fillQueueByScanner(void)
{
    TIMAGE* image = NULL;
    int indexOfImage;
    
    do {
        image = SCANNER();
        
        if(image == NULL) {
            *END = 1;
            break;
        }
        
        // FIND FREE IMAGE
        indexOfImage = -1;
        while(true) {
            for(int i = 0; i < IM_MAX; i++) {
                if(IM[i].done == 1) {
                    indexOfImage = i;
                    break;
                }
            }
            if(indexOfImage >= 0) break;
        }
        
        // SET IMAGE
        struct IMAGE_NODE* imageNode = &IM[indexOfImage];
        copyImage(image, &imageNode->image);
        imageNode->foundFlags = 0;
        imageNode->needCompares = DB_SIZE;
        imageNode->done = 0;

        // FILL QUEUE
        for(int indexOfTerrorist = 0; indexOfTerrorist < DB_SIZE; indexOfTerrorist++) {
            sem_wait(SEMAPHORE);
            
            /*        
            int foo2;
            sem_getvalue(SEMAPHORE, &foo2);
            int bar;
            sem_getvalue(SEMAPHORE2, &bar);
            printf("##### end: %d / first: %d / last: %d / sem: %d / sem2: %d \n", *END, *QUEUE_FIRST, *QUEUE_LAST, foo2, bar);
            //*/
            
            *QUEUE_LAST += 1;
            
            struct QUEUE_NODE* queueNode = &QUEUE[*QUEUE_LAST%QUEUE_MAX];
            queueNode->terrorist = indexOfTerrorist;
            queueNode->image = indexOfImage;
            
            sem_post(SEMAPHORE2);
        }
    } while(true);
    
    for(int i = 0; i < PROCESSES; i++) {
        sem_post(SEMAPHORE2);
        wait((void*)i);
    }
}

/******************************************************************************/
/* THREAD */
/******************************************************************************/

void myThread(int ID)
{
    struct IMAGE_NODE* imageNode = NULL;
    struct QUEUE_NODE* queueNode = NULL;
    
    while(true) {
        //if(*QUEUE_LAST == -1) continue;
        
        //if(*END && *QUEUE_FIRST > *QUEUE_LAST) break;
        sem_wait(SEMAPHORE2);
        
        // {{{{ LOCK
        sem_wait(MUTEX);
            *QUEUE_FIRST += 1;
            int index = *QUEUE_FIRST;
        sem_post(MUTEX);
        // }}}} UNLOCK
        
        if(*END && index > *QUEUE_LAST) break;
        
        /*
        int foo2;
        sem_getvalue(SEMAPHORE, &foo2);
        int bar;
        sem_getvalue(SEMAPHORE2, &bar);
        printf("  (%d) end: %d / first: %d / last: %d / sem: %d / sem2: %d \n", ID, *END, index, *QUEUE_LAST, foo2, bar);
        //*/
        
        queueNode = &QUEUE[index%QUEUE_MAX];
        
        if(queueNode) {
            imageNode = &IM[queueNode->image];
            int found = compareImage(&DB_IMAGES[queueNode->terrorist], &imageNode->image);
            
            // {{{{ LOCK
            sem_wait(MUTEX);
                if(found) imageNode->foundFlags |= (int)pow(2, queueNode->terrorist);
                imageNode->needCompares--;
            sem_post(MUTEX);
            // }}}} UNLOCK
            
            if(imageNode->needCompares == 0) {
                OFFICER(&imageNode->image, imageNode->foundFlags);
                imageNode->done = 1;
            }
        }
        
        sem_post(SEMAPHORE);
    }
    
    exit(0);
}

/******************************************************************************/
/* IMAGES */
/******************************************************************************/

void copyImage(TIMAGE* from, TIMAGE* to)
{
    to->m_W = from->m_W;
    to->m_H = from->m_H;
    for(int i = 0; i < from->m_H; i++) {
        for(int j = 0; j < from->m_W; j++) {
            to->m_Data[i][j] = from->m_Data[i][j];
        }
    }
}

int compareImage(TTERRORIST* terrorist, TIMAGE* scanner)
{
    if(terrorist->m_H > scanner->m_H || terrorist->m_W > scanner->m_W) {
        return 0;
    }
    
    int found = 0;
    
    for(int i = 0; i < scanner->m_H-terrorist->m_H+1; i++) {
        for(int j = 0; j < scanner->m_W-terrorist->m_W+1; j++) {
            for(int x = 0; x < terrorist->m_H; x++) {
                for(int y = 0; y < terrorist->m_W; y++) {
                    if(terrorist->m_Data[x][y] == 255) {
                        continue; // 255 mean whatever, i don't care what scanner gave
                    }
                    if(terrorist->m_Data[x][y] != scanner->m_Data[x+i][y+j]) {
                        goto next_cykl;
                    }
                }
            }
            found = 1;
            goto exit_cykl;
            next_cykl:;
        }
    }
    exit_cykl:;

    return found;
}


/******************************************************************************/
/******************************************************************************/


#ifndef __PROGTEST__
TTERRORIST  terrorists[2] = 
 {
   { 8, 7,  /* terrorist t0 */
     {
       { 255, 255, 255, 255, 255, 255, 255, 255 },
       { 255,  50,  60,  70, 255, 255, 255, 255 },
       { 255,  50,  60,  70, 255, 255, 255, 255 },
       { 255, 255,  50,  60,  70,  80, 255, 255 },
       { 255, 255,  50,  60,  70,  80, 255, 255 },
       { 255, 255, 255, 255, 255, 255, 255, 255 },
       { 255, 255, 255, 255, 255, 255, 255, 255 }
     }  
   },
   
   { 3, 3,  /* terrorist t1 */
     {
       {  10,  20,  30 },
       {  20,  50, 255 },
       {  80,  50, 255 }
     }
   }
 };  

TIMAGE i0 =
 { 8, 8,
   {
     {   1,   2,   3,   4,   5,   6,   7,   8 },
     {   2,  50,  60,  70,   6,   7,   8,   1 },
     {   3,  50,  60,  70,   7,  10,  20,  30 },
     {   4,  50,  60,  70,   8,  20,  50,   3 },
     {   5,   6,  50,  60,  70,  80,  50,   4 },
     {   6,   7,  50,  60,  70,  80,   4,   5 },
     {   7,   8,   1,   2,   3,   4,   5,   6 },
     {   8,   1,   2,   3,   4,   5,   6,   7 }
   }
 }; /* t0 and t1 here */

TIMAGE i1 =
 { 8, 7,
   {
     {   1,   2,   3,   4,   5,   6,   7,   8 },
     {   2,   3,   4,   5,   6,   7,   8,   1 },
     {   3,   4,   5,   6,   7,   8,   1,   2 },
     {  10,  20,  30,   7,   8,   1,   2,   3 },
     {  20,  50,   7,   8,   1,   2,   3,   4 },
     {  80,  50,   8,   1,   2,   3,   4,   5 },
     {   7,   8,   1,   2,   3,   4,   5,   6 }
   }
 }; /* t1 here */

TIMAGE i2 =
 { 6, 5,
   {
     {   1,  50,  60,  70,   5,   6 },
     {   2,  50,  60,  70,   6,   7 },
     {   3,   4,  50,  60,  70,  80 },
     {  10,  20,  50,  50,  70,  81 },
     {  20,  50,   7,   8,   1,   2 }
   }
 }; /* no terrorists here */

//TIMAGE * scans[3] = { &i0, &i1, &i2 };
TIMAGE * scans[120] = { &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2, &i0, &i1, &i2 };

TIMAGE   * dummyScanner ( void )
 {
   static int idx = 0;

 sleep(1);
   if ( idx < 10 )
    {
      TIMAGE * res = (TIMAGE *) malloc ( sizeof ( *res ) );
      *res = *scans[idx ++];
      return ( res );
    }
   return ( NULL );
 }

void       dummyOfficer  ( TIMAGE * img, int found )
 {
 
 return;
   int i;

   printf ( "Image: %d x %d  ", img -> m_W, img -> m_H );
   if ( found )
    {
      printf ( "TERRORISTS:" );
      for ( i = 0; i < (int)sizeof ( found ) * 8; i ++ )
       if ( found & ( 1 << i ) )
        printf ( " %d", i );
      printf ( "\n" );
    }
   else
    printf ( "no terrorists found\n" );
 }

int main ( int argc, char * argv[] )
 {
   TerroristHunter ( 2, terrorists, 20, dummyScanner, dummyOfficer );
   
   
   return ( 0 );
 }
#endif /* __PROGTEST__ */

