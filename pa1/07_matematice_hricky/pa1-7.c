#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ERR_INPUT 1
#define DEBUG 0

int result, countOfNumbers;
int countOfSolution = 0;
char sign[6] = " +-*/";

/* clean up & error */

void error(int err)
{
	switch(err) {
		case ERR_INPUT:
			printf("Nespravny vstup.\n");
			break;
		default:
			printf("Neznama chyba.\n");
			break;
	}
	exit(1);
}

/* getters */

void getResult(void)
{
	printf("Zadejte pozadovany vysledek:\n");
	if(scanf("%d", &result) != 1) {
		error(ERR_INPUT);
	}
}

void getCountOfNumbers(void)
{
	printf("Zadejte pocet cifer:\n");
	if(scanf("%d", &countOfNumbers) != 1) {
		error(ERR_INPUT);
	}
	if(countOfNumbers < 1) error(ERR_INPUT);
}

/* binary */

void dec2bin(int a, char *number)
{
	int rest, i;
	char temp;
	
	for(i = countOfNumbers-1; i >= 0; i--) {
		rest = a % 2;
		a /= 2;
		sprintf(&temp, "%d", rest);
		number[i] = temp;
	}
}

int bin2dec(char *a, int size)
{
	int i, multiple = 1, result = 0, foo;
	char temp;
	for(i = size-1; i >= 0; i--) {
		temp = a[i];
		foo = atoi(&temp);
		result += foo * multiple;
		multiple *= 2;
	}
	return result;
}

/* find solution */

int control(char *string)
{
	char *temp = (char *)malloc(countOfNumbers*sizeof(char));
	char *signs = (char *)malloc(countOfNumbers*sizeof(char));
	float *numbers = (float *)malloc(countOfNumbers*sizeof(float));
	int i, tx = 0, sx = 0, nx = 0, ret;
	
	for(i = 0; i < countOfNumbers*2; i++) {
		if(string[i] == ' ') {
			continue;
		} else if(string[i] == '0' || string[i] == '1') {
			temp[tx++] = string[i];
		} else {
			numbers[nx++] = bin2dec(temp, tx);
			signs[sx++] = string[i];
			tx = 0;
		}
	}
	numbers[nx++] = bin2dec(temp, tx);
	
	if(DEBUG) {
		for(i = 0; i < nx; i++) {
			printf("%f ", numbers[i]);
		}
		printf("\n%s\n", signs);
	}
	
	for(i = 0; i < sx; i++) {
		if(signs[i] == '*') {
			numbers[i+1] = numbers[i] * numbers[i+1];
			numbers[i] = 0;
			if(i>0) signs[i] = signs[i-1];
			else signs[i] = '+';
		} else if(signs[i] == '/') {
			if(fabs(numbers[i+1]) < 0.000001) {
				ret = 0;
				goto endControl;
			}
			numbers[i+1] = numbers[i] / numbers[i+1];
			numbers[i] = 0;
			if(i>0) signs[i] = signs[i-1];
			else signs[i] = '+';
		}
	}
	
	if(DEBUG) {
		for(i = 0; i < nx; i++) {
			printf("%f ", numbers[i]);
		}
		printf("\n%s\n", signs);
	}
	
	for(i = 0; i < sx; i++) {
		if(signs[i] == '+') {
			numbers[i+1] = numbers[i] + numbers[i+1];
			numbers[i] = 0;
			signs[i] = ' ';
		} else if(signs[i] == '-') {
			numbers[i+1] = numbers[i] - numbers[i+1];
			numbers[i] = 0;
			signs[i] = ' ';
		}
	}
	
	if(DEBUG) {
		printf("------------\n%f\n",numbers[nx-1]);
		return 1;
	}
	
	if(fabs(numbers[nx-1]-result) < 0.000001) {
		ret = 1;
		goto endControl;
	}
	ret = 0;
	
endControl:
	free(temp);
	free(signs);
	free(numbers);
	return ret;
}

void cycleSign(char *foo, int position)
{
	int i, j;
	for(i = 0; i < 5; i++) {
		foo[position] = sign[i];
		if(position+3 >= countOfNumbers*2) {
			if(control(foo)) {
				for(j = 0; j < countOfNumbers*2; j++) {
					if(foo[j] == ' ') continue;
					printf("%c", foo[j]);
				}
				printf(" bin = %d dec\n", result);
				countOfSolution++;
			}
		} else {
			cycleSign(foo, position+2);
		}
	}
}

void findSolution(void)
{
	int i, x, maxNumber;
	char *number = (char *)malloc((countOfNumbers)*sizeof(char));
	char *foo = (char *)malloc((countOfNumbers*2)*sizeof(char));
	
	for(i = 0; i<countOfNumbers; i++) {
		number[i] = '1';
	}
	maxNumber = bin2dec(number, countOfNumbers);
	
	for(x = 0; x <= maxNumber; x++) {
		dec2bin(x, number);
		for(i = 0; i < countOfNumbers; i++) {
			foo[i*2] = number[i];
			foo[i*2+1] = ' ';
		}
		cycleSign(foo, 1);
	}
	
	free(number);
	free(foo);
}

/* main */

int main( void )
{
	getResult();
	getCountOfNumbers();
	
	findSolution();
	printf( "Celkem reseni: %d\n", countOfSolution );
	
	return EXIT_SUCCESS;
}

