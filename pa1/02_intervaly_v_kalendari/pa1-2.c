#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FALSE 0
#define TRUE 1

typedef struct {
	int day;
	int month;
	long int year;
} DATE;

int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int isLeap( int year )
{
	if( year < 1584 && year % 4 == 0 )
		return TRUE;
	if( year % 4 == 0 && ( year % 100 != 0 || year % 400 == 0 ) && year % 4000 != 0 )
		return TRUE;
	return FALSE;
}

int isDateOk( DATE date )
{
	if( date.day > daysInMonth[ date.month-1 ] && !( date.day==29 && date.month==2 && isLeap( date.year ) ) ) {
		return FALSE;
	} else if( date.day < 1 ) {
		return FALSE;
	} else if( date.month < 1 || date.month > 12 ) {
		return FALSE;
	} else if( date.year <= 0 ) {
		return FALSE;
	} else if( date.year == 1584 && date.month == 1 && date.day > 6 && date.day < 17 ) {
		return FALSE;
	}
	return TRUE;
}

DATE getDate( char* name )
{
	DATE date;
	printf( "Zadejte %s datum - den mesic rok:\n", name );
	if( scanf( "%d %d %ld", &date.day, &date.month, &date.year ) == 3 ) {
		if( isDateOk( date ) ) {
			return date;
		}
	}
	printf( "Nespravny vstup.\n" );
	exit( 1 );
}

long int countOfLeapYears( long int year )
{
	long int count = year / 4;
	
	if( year > 1584 ) {
		count += year / 400 - year / 100 - year / 4000;
		count -= 1584 / 400 - 1584 / 100 - 1584 / 4000;
	}
	
	return count;
}

long int calcDays( DATE date )
{
	long int days;
	int x;
	
	days = ( date.year - 1 ) * 365; /* pocet dni za roky */
	days += countOfLeapYears( date.year - 1 ); /* pricist prestupne roky */
	if( isLeap( date.year ) && date.month > 2 ) days++; /* pricist den za aktualne prestupny rok */
	for( x = 0; x < date.month-1; x++ ) days += daysInMonth[x]; /* pocet dni za mesice */
	days += date.day; /* pocet dni za dny */
	
	if( date.year > 1584 || ( date.year == 1584 && ( date.month > 1 || date.day >16 ) ) ) days -= 10; /* odecteni neexistujicich dnu */
	else if( date.year == 1584 && date.month == 1 && date.day > 6 && date.day < 17 ) days -= date.day - 6; /* odecteni neexistujicich dnu */
	
	return days;
}

int main( void )
{
	DATE first = getDate( "prvni" );
	DATE second = getDate( "druhe" );
	
	long int daysOfFirst = calcDays( first );
	long int daysOfSecond = calcDays( second );
	
	if( daysOfFirst > daysOfSecond ) {
		printf( "Prve datum je vetsi nez datum druhe.\n" );
	} else {
		printf(
			"Mezi %d.%d.%ld a %d.%d.%ld ubehlo %ld dni.\n",
			first.day, first.month, first.year,
			second.day, second.month, second.year,
			daysOfSecond - daysOfFirst
		);
	}
	
	return EXIT_SUCCESS;
}

