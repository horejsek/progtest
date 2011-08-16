#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	int x;
	int y;
} POINT;

typedef struct {
	int a;
	int b;
} VECTOR;

POINT getPoint( char name )
{
	POINT point;
	printf( "Zadejte souradnice bodu %c:\n", name );
	if( scanf( "%d %d", &point.x, &point.y ) == 2 )
		return point;
	printf( "Nespravny vstup.\n" );
	exit( 1 );
}

VECTOR calcVector( POINT a, POINT b )
{
	VECTOR u;
	u.a = b.x - a.x;
	u.b = b.y - a.y;
	return u;
}

double calcDistance( POINT a, POINT b )
{
	return sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) );
}

int isPointSame( POINT a, POINT b )
{
	return ( a.x == b.x && a.y == b.y );
}

int isVectorNull( VECTOR u )
{
	return ( u.a == 0 && u.b == 0 );
}

int isMultiple( VECTOR u, VECTOR v )
{
	if( v.a != 0 ) {
		float foo = u.a / (float) v.a;
		if( v.b * foo == u.b )
			return 1;
	} else if( v.b != 0 ) {
		float foo = u.b / (float) v.b;
		if( v.a * foo == u.a )
			return 1;
	}
	return 0;
}

int main( void )
{
	char p;
		
	POINT a = getPoint( 'A' );
	POINT b = getPoint( 'B' );
	POINT c = getPoint( 'C' );
	
	VECTOR u = calcVector( a, b );
	VECTOR v = calcVector( b, c );
	VECTOR w = calcVector( a, c );
	
	float distanceAB = calcDistance( a, b );
	float distanceBC = calcDistance( b, c );
	float distanceAC = calcDistance( a, c );
	
	int oneLine = 0;
	if( !isVectorNull( u ) && !isVectorNull( v ) ) oneLine = isMultiple( u, v );
	else if( !isVectorNull( v ) && !isVectorNull( w ) ) oneLine = isMultiple( v, w );
	else if( !isVectorNull( u ) && !isVectorNull( w ) ) oneLine = isMultiple( u, w );
	else oneLine = 1;
	
	if( oneLine ) {
		printf( "Body lezi na jedne primce.\n" );
		
		if( isPointSame( a, b ) || isPointSame( b, c ) || isPointSame( a, c ) ) {
			printf( "Nektere body splyvaji - zadny neni uprostred.\n" );
		} else {
			if( distanceBC > distanceAB && distanceBC > distanceAC ) p = 'A';
			else if( distanceAC > distanceAB && distanceAC > distanceBC ) p = 'B';
			else if( distanceAB > distanceBC && distanceAB > distanceAC ) p = 'C';
			printf( "Prostredni je bod %c.\n", p );
		}
	} else {
		printf( "Body nelezi na jedne primce.\n" );
	}
	
	return EXIT_SUCCESS;
}

