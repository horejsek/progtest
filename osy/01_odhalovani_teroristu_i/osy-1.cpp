#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

#define MAX_TERRORISTS   32

using namespace std;

typedef struct TImage
 {
   int              m_W;
   int              m_H;
   unsigned char ** m_Data;
 } TIMAGE;

void  TerroristHunter    ( int        databaseSize,
                           TIMAGE  ** database,
                           int         threads,
                           TIMAGE *(* scanner) ( void ),
                           void    (* officer) ( TIMAGE *, int ) );
#endif /* __PROGTEST__ */

/******************************************************************************/
/* HEADERS */
/******************************************************************************/

void createThreads(void);
void joinThreads(void);
void fillQueueByScanner(void);

void* myThread(void*);
//void compareImages(TIMAGE*);
int compareImage(TIMAGE*, TIMAGE*);

/******************************************************************************/
/* GLOBALS */
/******************************************************************************/

int DB_SIZE;
TIMAGE** DB_IMAGES;

void (*OFFICER)(TIMAGE*, int);
TIMAGE* (*SCANNER)(void);

int THREADS;
int END;

/* GLOBALS of THREADS */

pthread_t* pthreads;
pthread_mutex_t MUTEX;
sem_t SEMAPHORE;

/* STRUCTS */

struct IMAGE_NODE {
    TIMAGE* image;
    IMAGE_NODE* prev;
    IMAGE_NODE* next;
    int foundFlags;
    int needCompares; // 0 mean "done, send to officer"
};
IMAGE_NODE* IM_FIRST;
IMAGE_NODE* IM_LAST;

struct QUEUE_NODE {
    TIMAGE* cmpImage1;
    TIMAGE* cmpImage2;
    QUEUE_NODE* next;
    IMAGE_NODE* image;
    int index;
};
QUEUE_NODE* QUEUE_FIRST;
QUEUE_NODE* QUEUE_LAST;

/******************************************************************************/
/* MAIN FUNCTION */
/******************************************************************************/

void TerroristHunter(int databaseSize, TIMAGE** database, int threads, TIMAGE* (*scanner)(void), void (*officer)(TIMAGE*, int))
{
    DB_SIZE = databaseSize;
    DB_IMAGES = database;
    
    OFFICER = officer;
    SCANNER = scanner;
    
    THREADS = threads;
    END = 0;
    
    IM_FIRST = NULL;
    IM_LAST = NULL;
    
    QUEUE_FIRST = NULL;
    QUEUE_LAST = NULL;
    
    pthread_mutex_init(&MUTEX, NULL);
    sem_init(&SEMAPHORE, 0, 1);
    
    createThreads();
    fillQueueByScanner();
    joinThreads();
    
    pthread_mutex_destroy(&MUTEX);
    sem_destroy(&SEMAPHORE);
}

/* CREATE & JOIN THREADS */

void createThreads(void)
{
    pthreads = (pthread_t*)malloc(THREADS * sizeof(pthread_t));
    for(int i = 0; i < THREADS; i++) {
        pthread_create(&pthreads[i], NULL, myThread, (void*)((long)i));
    }
}

void joinThreads(void)
{
    void* retval;
    for(int i = 0; i < THREADS; i++) {
        pthread_join(pthreads[i], &retval);
    }
}

/* FILL QUEUE BY IMAGES FROM SCANNER */

void fillQueueByScanner(void)
{
    TIMAGE* image = NULL;
    do {
        image = SCANNER();
        
        for(int i = 0; i < THREADS; i++)
            sem_post(&SEMAPHORE);
        
        if(image == NULL) {
            END = 1;
            break;
        }
        
        struct IMAGE_NODE* imageNode = new(struct IMAGE_NODE);
        imageNode->image = image;
        imageNode->next = NULL;
        imageNode->prev = IM_LAST;
        imageNode->foundFlags = 0;
        imageNode->needCompares = DB_SIZE;
        
        if(IM_FIRST == NULL) IM_FIRST = imageNode;
        if(IM_LAST != NULL) IM_LAST->next = imageNode;
        IM_LAST = imageNode;

        for(int x = 0; x < DB_SIZE; x++) {
            struct QUEUE_NODE* queueNode = new(struct QUEUE_NODE);
            queueNode->cmpImage1 = DB_IMAGES[x];
            queueNode->cmpImage2 = image;
            queueNode->next = NULL;
            queueNode->image = imageNode;
            queueNode->index = x;
            
            // {{{{ LOCK
            pthread_mutex_lock(&MUTEX);
                if(QUEUE_FIRST == NULL) QUEUE_FIRST = queueNode;
                if(QUEUE_LAST != NULL) QUEUE_LAST->next = queueNode;
                QUEUE_LAST = queueNode;
            pthread_mutex_unlock(&MUTEX);
            // }}}} UNLOCK
        }
    } while(true);
        
    for(int i = 0; i < THREADS; i++)
        sem_post(&SEMAPHORE);
}

/******************************************************************************/
/* THREAD */
/******************************************************************************/

void* myThread(void* index)
{
    struct IMAGE_NODE* imageNode = NULL;
    struct QUEUE_NODE* queueNode = NULL;

    struct IMAGE_NODE* tmp = NULL;
    
    while(true) {
        if(!END && QUEUE_FIRST == NULL) {
            sem_wait(&SEMAPHORE);
        }
        
        if(END && QUEUE_FIRST == NULL) break;
        
        // {{{{ LOCK
        pthread_mutex_lock(&MUTEX);
            if(QUEUE_FIRST != NULL) {
                queueNode = QUEUE_FIRST;
                if(QUEUE_FIRST->next) {
                    QUEUE_FIRST = QUEUE_FIRST->next;
                } else {
                    QUEUE_FIRST = NULL;
                    QUEUE_LAST = NULL;
                }
            } else {
                queueNode = NULL;
            }
        pthread_mutex_unlock(&MUTEX);
        // }}}} UNLOCK
        
        if(queueNode) {
            int found = compareImage(queueNode->cmpImage1, queueNode->cmpImage2);
            imageNode = queueNode->image;
            
            // {{{{ LOCK
            pthread_mutex_lock(&MUTEX);
                if(found) imageNode->foundFlags |= (int)pow(2, queueNode->index);
                imageNode->needCompares--;
                
                if(imageNode->needCompares == 0) {
                    if(imageNode == IM_LAST) IM_LAST = imageNode->prev;
                    if(imageNode == IM_FIRST) IM_FIRST = imageNode->next;
            
                    tmp = imageNode->prev;
                    if(tmp) tmp->next = imageNode->next;
                    tmp = imageNode->next;
                    if(tmp) tmp->prev = imageNode->prev;
                    
                    //SEMAPHORE--;
                }
            pthread_mutex_unlock(&MUTEX);
            // }}}} UNLOCK
            
            if(imageNode->needCompares == 0) {
                OFFICER(imageNode->image, imageNode->foundFlags);
                queueNode->image = NULL;
                //delete imageNode;
            }
        }
    }
    if(queueNode) delete queueNode;
    
    pthread_exit((void*) 0);
}

/******************************************************************************/
/* IMAGES */
/******************************************************************************/

/*void compareImages(TIMAGE* image)
{
    int cmp = 0;
    
    for(int i = 0; i < DB_SIZE; i++) {
        if(compareImage(DB_IMAGES[i], image)) {
            cmp = cmp | (int)pow(2, i);
        }
    }
    
    OFFICER(image, cmp);
}*/

int compareImage(TIMAGE* terrorist, TIMAGE* scanner)
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


#ifndef __PROGTEST__
unsigned char t0r0[] = { 255, 255, 255, 255, 255, 255, 255, 255 };
unsigned char t0r1[] = { 255,  50,  60,  70, 255, 255, 255, 255 };
unsigned char t0r2[] = { 255,  50,  60,  70, 255, 255, 255, 255 };
unsigned char t0r3[] = { 255, 255,  50,  60,  70,  80, 255, 255 };
unsigned char t0r4[] = { 255, 255,  50,  60,  70,  80, 255, 255 };
unsigned char t0r5[] = { 255, 255, 255, 255, 255, 255, 255, 255 };
unsigned char t0r6[] = { 255, 255, 255, 255, 255, 255, 255, 255 };
unsigned char * t0all[] = { t0r0, t0r1, t0r2, t0r3, t0r4, t0r5, t0r6 };
TIMAGE  t0 = { 8, 7, t0all };  /* one sample */

unsigned char t1r0[] = {  10,  20,  30 };
unsigned char t1r1[] = {  20,  50, 255 };
unsigned char t1r2[] = {  80,  50, 255 };
unsigned char * t1all[] = { t1r0, t1r1, t1r2 };
TIMAGE  t1 = { 3, 3, t1all };  /* another sample */

TIMAGE * terrorists[20] = { &t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1,&t0, &t1 };

unsigned char i0r0[] = {   1,   2,   3,   4,   5,   6,   7,   8 };
unsigned char i0r1[] = {   2,  50,  60,  70,   6,   7,   8,   1 };
unsigned char i0r2[] = {   3,  50,  60,  70,   7,  10,  20,  30 };
unsigned char i0r3[] = {   4,  50,  60,  70,   8,  20,  50,   3 };
unsigned char i0r4[] = {   5,   6,  50,  60,  70,  80,  50,   4 };
unsigned char i0r5[] = {   6,   7,  50,  60,  70,  80,   4,   5 };
unsigned char i0r6[] = {   7,   8,   1,   2,   3,   4,   5,   6 };
unsigned char i0r7[] = {   8,   1,   2,   3,   4,   5,   6,   7 };
unsigned char * i0all[] = { i0r0, i0r1, i0r2, i0r3, i0r4, i0r5, i0r6, i0r7 };
TIMAGE i0 = { 8, 8, i0all }; /* t0 and t1 here */

unsigned char i1r0[] = {   1,   2,   3,   4,   5,   6,   7,   8 };
unsigned char i1r1[] = {   2,   3,   4,   5,   6,   7,   8,   1 };
unsigned char i1r2[] = {   3,   4,   5,   6,   7,   8,   1,   2 };
unsigned char i1r3[] = {  10,  20,  30,   7,   8,   1,   2,   3 };
unsigned char i1r4[] = {  20,  50,   7,   8,   1,   2,   3,   4 };
unsigned char i1r5[] = {  80,  50,   8,   1,   2,   3,   4,   5 };
unsigned char i1r6[] = {   7,   8,   1,   2,   3,   4,   5,   6 };
unsigned char * i1all[] = { i1r0, i1r1, i1r2, i1r3, i1r4, i1r5, i1r6 };
TIMAGE i1 = { 8, 7, i1all }; /* t1 here */

unsigned char i2r0[] = {   1,  50,  60,  70,   5,   6 };
unsigned char i2r1[] = {   2,  50,  60,  70,   6,   7 };
unsigned char i2r2[] = {   3,   4,  50,  60,  70,  80 };
unsigned char i2r3[] = {  10,  20,  50,  50,  70,  81 };
unsigned char i2r4[] = {  20,  50,   7,   8,   1,   2 };
unsigned char * i2all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i2 = { 6, 5, i2all }; /* no terrorists here */

unsigned char * i3all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i3 = { 6, 5, i3all }; /* no terrorists here */

unsigned char * i4all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i4 = { 6, 5, i4all }; /* no terrorists here */

unsigned char * i5all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i5 = { 6, 5, i5all }; /* no terrorists here */

unsigned char * i6all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i6 = { 6, 5, i5all }; /* no terrorists here */

unsigned char * i7all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i7 = { 6, 5, i7all }; /* no terrorists here */

unsigned char * i8all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i8 = { 6, 5, i8all }; /* no terrorists here */

unsigned char * i9all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i9 = { 6, 5, i9all }; /* no terrorists here */

unsigned char * i10all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i10 = { 6, 5, i10all }; /* no terrorists here */

unsigned char * i11all[] = { i2r0, i2r1, i2r2, i2r3, i2r4 };
TIMAGE i11 = { 6, 5, i11all }; /* no terrorists here */

//TIMAGE * scans[3] = { &i0, &i1, &i2 };
TIMAGE * scans[120] = { &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11, &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11,&i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &i10, &i11 };

TIMAGE   * dummyScanner ( void )
 {
   static int idx = 0;

//sleep(1);
   if ( idx < 119 ) return ( scans[idx ++] );
   return ( NULL );
 }

void       dummyOfficer  ( TIMAGE * img, int found )
 {
   int i;

   //printf ( "Image: %d x %d  ", img -> m_W, img -> m_H );
   if ( found )
    {
      printf (  "Image: %d x %d  TERRORISTS:", img -> m_W, img -> m_H );
      for ( i = 0; i < (int)sizeof ( found ) * 8; i ++ )
       if ( found & ( 1 << i ) )
        printf ( " %d", i );
      printf ( "\n" );
    }
   else {
    printf ( "Image: %d x %d  no terrorists found\n", img -> m_W, img -> m_H );
   }
 }

int main (int argc, char * argv[])
{
    TerroristHunter ( 19, terrorists, 100, dummyScanner, dummyOfficer );
    return ( 0 );
}
#endif /* __PROGTEST__ */

