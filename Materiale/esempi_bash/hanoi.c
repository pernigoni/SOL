#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

	int *A,*B,*C;
	int topA, topB, topC;
	int H = 3;
	int SLEEP = 50000;
	int flush = 0;

void stampa(){
	printf("==================\n");
	printf("A: [");
	for(int i = 0; i < H; i++){
		printf("%d ", A[i]);
	}
	printf("] \n");

	printf("B: [");
	for(int i = 0; i < H; i++){
		printf("%d ", B[i]);
	}
	printf("] \n");

	printf("C: [");
	for(int i = 0; i < H; i++){
		printf("%d ", C[i]);
	}
	printf("] \n");
	printf("==================\n");

	if(flush) fflush(stdout);

	if(SLEEP <= 0){
		getchar();
	}
	else{
	    usleep(SLEEP);
	}
}



void move(int * from, int * topFrom, int * to, int * topTo){

	fprintf(stderr, "[stderr] (moving tile %d)\n", (*from));
	if(flush) fflush(stderr);

	if(*topFrom <= 0 || *topTo >= H || *topFrom > H || *topTo < 0){
	 printf("ERRORE: tf = %d , tt = %d \n", *topFrom, *topTo);
	 exit(0);
	}
	if(*topTo > 0 && from[*topFrom-1] > to[*topTo - 1]){
		printf("ERRORE: tfv = %d , ttv = %d \n", from[*topFrom-1], to[*topTo-1]);
	 	exit(0);	
	}

	to[*topTo] = from[*topFrom - 1];
	from[*topFrom -1] = 0;

	(*topFrom)--;
	(*topTo)++;

	stampa();
}


void spostatorre(int n, int * P1, int* tp1, int * P2, int * tp2, int * P3, int * tp3){

	if(n == 1){
		move(P1, tp1, P2, tp2);
	}
	else{
		spostatorre(n-1, P1, tp1, P3, tp3, P2, tp2);

		move(P1, tp1, P2, tp2);

		spostatorre(n-1, P3, tp3, P2, tp2, P1, tp1);
	}


}

/**
* ./hanoi [height] [millis] (millis < 0 : getchar())
*/
int main(int argc, char const *argv[])
{
	if(argc > 1 && strcmp("-flush", argv[1])==0){
		//flushes streams
		flush = 1;
	}	else{
		if(argc > 1) H = atoi(argv[1]);
		if(argc > 2) SLEEP = 1000*atoi(argv[2]);		
	}

	A = calloc(H, sizeof(int));
	B = calloc(H, sizeof(int));
	C = calloc(H, sizeof(int));

	for(int i = 0; i < H; i++) A[i] = H -i;

	topA = H; // first free
	topB = 0;
	topC = 0;

	stampa();

	spostatorre(H, A, &topA, B, &topB, C, &topC);

	return 0;
}

