#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ERR_INPUT 1
#define ERR_KONVEX 2

#define LEFT 1
#define RIGHT 2

typedef struct {
	long int x;
	long int y;
} POINT;

typedef struct {
	long int a;
	long int b;
} VECTOR;

POINT p;
int pop = 0;
int inside = 1;
int underRotation = -1;

void error( int err )
{
	switch( err ) {
		case 1:
			printf( "Nespravny vstup.\n" );
			break;
		case 2:
			printf( "Zadany n-uhelnik neni konvexni.\n" );
			break;
		default:
			printf( "Neznama chyba.\n" );
			break;
	}
	exit( 1 );
}

/* MATH WITH VECTORS */

VECTOR calcVector( POINT a, POINT b )
{
	VECTOR u;
	u.a = b.x - a.x;
	u.b = b.y - a.y;
	return u;
}

double calcSizeOfVector( VECTOR u )
{
	return sqrt( pow( u.a, 2 ) + pow( u.b, 2 ) );
}

double calcAngle( POINT a, POINT b, POINT c )
{
	double foo = atan2( a.x - b.x, a.y - b.y ) - atan2( c.x - b.x, c.y - b.y );
	return foo * 180 / 3.1415926535897932384;
}

/* POLYGON CONVEX HELPERS */

int isUnderRotation( double angle )
{
	if( ( 0 < angle && angle < 180 ) || angle < -180 ) {
		return 1;
	} else if( ( -180 < angle && angle < 0 ) || 180 < angle ) {
		return 0;
	}
	return -1;
}

int isConvex( POINT a, POINT b, POINT c )
{		
	double angle = calcAngle( a, b, c );
	if( underRotation == -1 ) underRotation = isUnderRotation( angle );
	if( underRotation == -1 ) return 0;
	
	return isUnderRotation( angle ) == underRotation;
}

int positionOfPoint( POINT a, POINT b )
{
	long int c, y;
	VECTOR u = calcVector( a, b );
	VECTOR n; /* normalovy vektor */
	
	n.a = -u.b;
	n.b = u.a;
	
	c = -( a.x*n.a + a.y*n.b );
	y = p.x*n.a + p.y*n.b + c;
	
	/*printf( "c = ( %d*%d + %d*%d ) = %d\n", a.x, n.a, a.y, n.b, c );
	printf( "y = %d*%d + %d*%d + %d = %d\n", p.x, n.a, p.y, n.b, c, y );*/
	
	if( y > 0 ) {
		return LEFT;
	} else if( y < 0 ) {
		return RIGHT;
	}
	return -1;
}

/* GETTERS */

POINT getPoint( void )
{
	POINT point;
	printf( "Zadejte souradnice bodu:\n" );
	if( scanf( "%ld %ld", &point.x, &point.y ) == 2 )
		return point;
	error( ERR_INPUT );
	exit( 1 );
}

void getPolygon( void )
{
	POINT point, first, second, a, b, c;
	int x = 0;

	int polygonConvex = 1;
	int position = 0;
	
	printf( "Zadejte body n-uhelniku:\n" );
	do {
		if( scanf( "%ld %ld", &point.x, &point.y ) != 2 ) {
			if( feof( stdin ) )
				break;
			error( ERR_INPUT );
		}
		if( x == 0 ) first = point;
		if( x == 1 ) second = point;
		a = b;
		b = c;
		c = point;
		
		if( x > 0 ) {
			position = positionOfPoint( b, c );
			if( pop == 0 ) pop = position;
			if( pop != position ) inside = 0;
		}
		
		if( x > 1 ) {
			if( !isConvex( a, b, c ) || !polygonConvex ) polygonConvex = 0;
		}
		
		if( x < 5 ) x++;
	} while( 1 );
	
	if( x < 3 ) {
		error( ERR_INPUT );
		exit( 1 );
	}
	
	if( !isConvex( b, c, first ) || !polygonConvex ) polygonConvex = 0;
	if( !isConvex( c, first, second ) || !polygonConvex ) polygonConvex = 0;
	
	if( !polygonConvex ) {
		error( ERR_KONVEX );
		exit( 1 );
	}
	
	if( inside == 1 && underRotation == 1 && position == RIGHT ) inside = 1;
	else if( inside == 1 && underRotation == 0 && position == LEFT ) inside = 1;
	else inside = 0;
	
	if( pop != positionOfPoint( c, first ) ) inside = 0;
}

/* MAIN */

int main( void )
{
	p = getPoint();
	getPolygon();
	
	if( inside == 1 ) {
		printf( "Bod lezi uvnitr n-uhelniku.\n" );
	} else {
		printf( "Bod nelezi uvnitr n-uhelniku.\n" );
	}
	
	return EXIT_SUCCESS;
}

