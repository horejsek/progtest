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
float matrix[20][21];

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

void getMatrix( void )
{
	int i, j, foo[420], isNull;
	printf( "Zadejte matici soustavy:\n" );
	
	for( i = 0; i < 420; i++ ) {
		if( scanf( "%d", &foo[i] ) != 1 ) {
			if( feof( stdin ) )
				break;
			error( ERR_INPUT );
		}
	}
	
	countOfEquation = (int) ( -1 + sqrt( 1 - 4*1*-i ) ) / 2;
	
	if( pow( countOfEquation, 2 ) + countOfEquation != i )
		error( ERR_INPUT );
	
	for( i = 0; i < countOfEquation; i++ ) {
		for( j = 0; j < countOfEquation + 1; j++ ) {
			matrix[i][j] = foo[i*(countOfEquation+1)+j];
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
	float mul, foo, bar;
	
	
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
	
	for( x = 0; x < countOfEquation-1; x++ ) {
		for( y = x+1; y <= countOfEquation; y++ ) {
			if( matrix[y][x] != 0 ) {
				mul = matrix[x][x] / (float) matrix[y][x];
				for( i = 0; i <= countOfEquation; i++ ) {
					matrix[y][i] = matrix[x][i] - ( matrix[y][i] * mul );
				}
			}
		}
	}
	
	for( x = countOfEquation-1; x >= 0; x-- ) {
		foo = 0;
		for( y = countOfEquation-1; y > x; y-- ) {
			foo = foo + ( matrix[x][y] * matrix[y][countOfEquation] );
			matrix[x][y] = 0;
		}
		
		if( fabs( matrix[x][x] ) < 0.000001 && fabs( matrix[x][countOfEquation] ) < 0.000001 ) {
			error( INFINITELY_SOLUTIONS );
		} else if( fabs( matrix[x][x] ) < 0.000001 ) {
			error( NO_SOLUTIONS );
		} else {
			mul = ( matrix[x][countOfEquation] - foo ) / (float) matrix[x][x];
		}
		
		matrix[x][x] = 1;
		matrix[x][countOfEquation] = mul;
	}
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
	getMatrix();
	gaussianElimination();
	showResult();
	
	return EXIT_SUCCESS;
}

