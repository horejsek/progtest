#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#endif __PROGTEST__

int maxDiagonal ( int ** mat, int size, int * maxSum )
{
	int i, j, row, col, count, max = 0, countMax = 0;
	for(i = 0; i < size; i++) {
		count = 0;
		for(j = 0; j < size; j++) {
			row = j;
			col = i+j;
			if(col >= size) {
				col -= size;
			}
			count += mat[row][col];
		}
		if(count > max) {
			max = count;
			countMax = 1;
		} else if(count == max) {
			countMax++;
		}
	}
	*maxSum = max;
	if(countMax > 1) {
		return 0;
	} else {
		return 1;
	}
}
 
#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
  int ret, val;

  int m0r0[4] = {   1,   2,   3,   4 };
  int m0r1[4] = {   3,   6,   7,   8 };
  int m0r2[4] = {   9,  10,  11,  12 };
  int m0r3[4] = {  16,  15,  14,  13 };
  int * m0[4] = { m0r0, m0r1, m0r2, m0r3 };

  int m1r0[3] = {   0,   1,   5 };
  int m1r1[3] = {  -2,   3,  10 };
  int m1r2[3] = {  21,   0,  13 };
  int * m1[3] = { m1r0, m1r1, m1r2 };

  int m2r0[2] = {   1,   2 };
  int m2r1[2] = {   3,   4 };
  int * m2[2] = { m2r0, m2r1 };

  int m3r0[1] = { 100 };
  int * m3[1] = { m3r0 };

  ret = maxDiagonal ( m0, 4, &val );
  printf ( "m0: ret = %d (ref: 1), val=%d (ref: 37)\n", ret, val );

  ret = maxDiagonal ( m1, 3, &val );
  printf ( "m1: ret = %d (ref: 1), val=%d (ref: 32)\n", ret, val );

  ret = maxDiagonal ( m2, 2, &val );
  printf ( "m2: ret = %d (ref: 0)\n", ret );

  ret = maxDiagonal ( m3, 1, &val );
  printf ( "m3: ret = %d (ref: 1), val=%d (ref: 100)\n", ret, val );

  return ( 0 );
}
#endif __PROGTEST__
