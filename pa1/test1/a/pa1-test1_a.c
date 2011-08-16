#include <stdio.h>
#include <stdlib.h>

#define ERR_INPUT 1
#define EVEN 0
#define ODD 1

int count = 0;

int even = 0;
int odd = 0;

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

void getCount(void)
{
	printf("Zadejte n:\n");
	if(scanf("%d", &count) != 1) {
		error(ERR_INPUT);
	}
}

void getNumbers(void)
{
	int i, number, this_, last = -1;
	
	printf("Zadejte cisla:\n");
	for(i = 0; i < count; i++) {
		if(scanf("%d", &number) != 1) {
			error(ERR_INPUT);
		}
		this_ = ODD;
		if(number % 2 == 0) {
			this_ = EVEN;
		}
		if(this_ != last) {
			if(last == EVEN) even++;
			if(last == ODD) odd++;
		}
		last = this_;
	}
	if(last == EVEN) even++;
	if(last == ODD) odd++;
}

int main(void)
{
	getCount();
	getNumbers();
	
	printf("Posloupnosti sudych cisel: %d\n", even);
	printf("Posloupnosti lichych cisel: %d\n", odd);
	
	return EXIT_SUCCESS;
}
