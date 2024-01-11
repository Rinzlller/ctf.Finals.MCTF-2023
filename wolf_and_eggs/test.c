#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
	char** eggs_array[10];
	eggs_array[0] = malloc(0x24);
	free(&eggs_array[0]);
	printf("%016x\n", eggs_array[0]);
}