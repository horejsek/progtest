#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ERR_INPUT 1
#define SIZE_OF_TEXT 1024

int clsm = 0; /* count of lines of search matrix */
int ccsm = 0; /* count of chars of search matrix */
int ccm = 0; /* count of chars of matrix */

int countOfMatrix = 0;

char **searchMatrix;
char **matrix;

/* malloc & free */

char **myMalloc( int x, int y )
{
	int i;
	char **array = (char **)malloc( x*sizeof( char* ) );
	for( i = 0; i < x; i++ )
		array[i] =  (char *)malloc( y*sizeof( char ) );
	return array;
}

void myFree( char **array )
{
	free( array );
}

/* clean up & error */

void cleanUp( void )
{
	myFree( searchMatrix );
	myFree( matrix );
}

void error( int err )
{
	switch( err ) {
		case 1:
			printf( "Nespravny vstup.\n" );
			break;
		default:
			printf( "Neznama chyba.\n" );
			break;
	}
	cleanUp();
	exit( 1 );
}

/* search */

void search( void )
{
	int i, j, line, match;
	
	for( i = 0; i <= ccm-ccsm; i++ ) {
		match = 1;
		for( line = 0; line < clsm; line++ ) {
			for( j = 0; j < ccsm; j++ ) {
				if( searchMatrix[line][j] != matrix[line][i+j] ) {
					match = 0;
					break;
				}
			}
			if( !match ) break;
		}
		if( match )
			countOfMatrix++;
	}
}

/* getters */

void getLinesOfSearchMatrix( void )
{
	printf( "Zadejte pocet radek hledane matice:\n" );
	if( scanf( "%d", &clsm ) != 1 ) {
		error( 1 );
	}
	if( clsm < 1 ) error( 1 );
}

void getSearchMatrix( void )
{
	int i, chars;
	char ch;
	
	searchMatrix = myMalloc( clsm, SIZE_OF_TEXT );
	matrix = myMalloc( clsm, SIZE_OF_TEXT );
	
	fgetc( stdin );
	printf( "Zadejte hledanou matici:\n" );
	for( i = 0; i < clsm; i++ ) {
		chars = 0;
		do {
			ch = fgetc( stdin );
			if( ch == '\n' ) break;
			if( ch < '0' || '9' < ch ) error( 1 );
			searchMatrix[i][chars++] = ch;
		} while ( 1 );
		
		if( ccsm == 0 ) ccsm = chars;
		if( ccsm != chars ) error( 1 );	
	}
}

void getMatrix( void )
{
	int i, j, line = 0, chars, eof = 0;
	char ch;
	
	printf( "Matice, ktera bude prohledana:\n" );
	do {
		for( i = 0; i+1 < clsm; i++ ) {
			for( j = 0; j < SIZE_OF_TEXT; j++ )
				matrix[i][j] = matrix[i+1][j];
		}
	
		chars = 0;
		do {
			ch = fgetc( stdin );
			if( feof( stdin ) ) {
				eof = 1;
				break;
			}
			if( ch == '\n' ) break;
			if( ch < '0' || '9' < ch ) error( 1 );
			matrix[clsm-1][chars++] = ch;
		} while ( 1 );
		if( eof ) break;
		
		if( ccm == 0 ) ccm = chars;
		if( ccm != chars ) error( 1 );
		
		line++;
		if( line >= clsm ) {
			search();
		}
	} while( 1 );
}

int main( void )
{
	getLinesOfSearchMatrix();
	getSearchMatrix();
	getMatrix();
	
	printf( "Matice nalezena %i krat.\n", countOfMatrix );
	
	cleanUp();
	return EXIT_SUCCESS;
}

