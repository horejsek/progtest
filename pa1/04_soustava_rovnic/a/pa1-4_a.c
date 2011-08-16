#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

#define ERR_INPUT 1
#define ERR_MAX_EQUATION 2
#define ERR_GET_MATRIX 3
#define INFINITELY_SOLUTIONS 4
#define NO_SOLUTIONS 5

int countOfEquation = 0;
int rows = 0;
int cols = 0;
double matrix[20][21];

void debug_show( void )
{
	int x, y;
	return;
	
	for( x = 0; x < countOfEquation; x++ ) {
		for( y = 0; y < countOfEquation+1; y++ ) {
			printf( "%.2f ", matrix[x][y] );
		}
		printf( "\n" );
	}
	printf( "\n" );
}

void error( int err )
{
	switch( err ) {
		case ERR_INPUT:
		case ERR_MAX_EQUATION:
		case ERR_GET_MATRIX:
			printf( "Nespravny vstup.\n" );
			break;
		case INFINITELY_SOLUTIONS:
			printf( "Existuje nekonecne mnoho reseni.\n" );
			break;
		case NO_SOLUTIONS:
			printf( "Soustava nema reseni.\n" );
			break;
		default:
			printf( "Neznama chyba.\n" );
			break;
	}
	exit( 1 );
}

void getCountOfEquation( void )
{
	printf( "Zadejte pocet promennych:\n" );
	if( scanf( "%d", &countOfEquation ) != 1 ) {
		error( ERR_INPUT );
	}
	if( countOfEquation < 1 || 20 < countOfEquation )
		error( ERR_MAX_EQUATION );
}

void getMatrix( void )
{
	int i, j, foo, isNull;
	printf( "Zadejte %.0f prvku matice:\n", pow( countOfEquation, 2 ) + countOfEquation );
	
	for( i = 0; i < countOfEquation; i++ ) {
		for( j = 0; j < countOfEquation + 1; j++ ) {
			if( scanf( "%d", &foo ) != 1 )
				error( ERR_GET_MATRIX );
			matrix[i][j] = foo;
		}
	}
	
	for( i = 0; i < countOfEquation; i++ ) {
		isNull = TRUE;
		for( j = 0; j < countOfEquation; j++ ) {
			if( matrix[i][j] != 0 ) isNull = FALSE;
		}
		if( isNull && matrix[i][countOfEquation] != 0 ) {
			error( NO_SOLUTIONS );
		}
	}
}

void gaussianElimination( void )
{
	int i, x, y, z, c, n1, n2;
	double mul, foo, bar;
	
	n1 = countOfEquation;
	for( c = 0; c < countOfEquation; c++ ) {
		for( x = 0; x < countOfEquation; x++ ) {
			n2 = 0;
			for( y = countOfEquation-n1; y < countOfEquation; y++ ) {
				if( matrix[y][c] < 0.00000000001 ) {
					for( z = 0; z < countOfEquation+1; z++ ) {
						bar = matrix[y+1][z];
						matrix[y+1][z] = matrix[y][z];
						matrix[y][z] = bar;
					}
					n2++;
				}
			}
			n1 = n2;
		}
	}
	
	debug_show();
	
	for( x = 0; x < countOfEquation-1; x++ ) {
		for( y = x+1; y <= countOfEquation; y++ ) {
			if( matrix[y][x] != 0 ) {
				mul = matrix[x][x] / (double) matrix[y][x];
				for( i = 0; i <= countOfEquation; i++ ) {
					matrix[y][i] = matrix[x][i] - ( matrix[y][i] * mul );
				}
			} else {
				
			}
		}
	}
	
	debug_show();
	
	for( x = countOfEquation-1; x >= 0; x-- ) {
		foo = 0;
		for( y = countOfEquation-1; y > x; y-- ) {
			foo = foo + ( matrix[x][y] * matrix[y][countOfEquation] );
			matrix[x][y] = 0;
		}
		
		if( fabs( matrix[x][x] ) < 0.00000000001 && fabs( matrix[x][countOfEquation] ) < 0.00000000001 ) {
			error( INFINITELY_SOLUTIONS );
		} else if( fabs( matrix[x][x] ) < 0.00000000001 ) {
			error( NO_SOLUTIONS );
		} else {
			foo = ( matrix[x][countOfEquation] - foo ) / (double) matrix[x][x];
		}
		
		matrix[x][x] = 1;
		matrix[x][countOfEquation] = foo;
	}
	
	debug_show();
}

void showResult( void )
{
	int x;
	printf( "Jedno reseni:\n" );
	for( x = 0; x < countOfEquation; x++ ) {
		printf( "x%d = %f\n", x+1, matrix[x][countOfEquation] );
	}
}

int main( void )
{
	getCountOfEquation();
	getMatrix();
	gaussianElimination();
	showResult();
	
	return EXIT_SUCCESS;
}

