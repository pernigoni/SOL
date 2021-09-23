#include <stdio.h>

void stampa(const char* orig, const char* up) {

	printf("\"%s\" => \"%s\"\n",orig, up);

	return;
}


void errore(const char* msg) {

	fprintf(stderr, "[ %s ]\n", msg);

	return;
}

