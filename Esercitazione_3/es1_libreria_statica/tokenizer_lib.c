#define _POSIX_C_SOURCE  200112L
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"

void tokenizer(char *stringa) {
  char* token = strtok(stringa, " ");
  while (token) {
    printf("%s\n", token);
    token = strtok(NULL, " ");
  }
}

void tokenizer_r(char *stringa) {
    char *tmpstr;
    char *token = strtok_r(stringa, " ", &tmpstr);
    while (token) {
      printf("%s\n", token);
      token = strtok_r(NULL, " ", &tmpstr);
    }
}