#include "utils.h"

void print_errors(char *msg, ...)
{
	char 	*begin = "ERRORE: ",
		*err = calloc(strlen(begin) + strlen(msg) + MAX_ERROR_MSG_SIZE, sizeof(char));
	if(err == NULL)
	{
		perror("calloc");
		return ;
	}
	
	strncat(err, begin, strlen(begin));
	strncat(err, msg, strlen(msg));

	va_list va;
	va_start(va, msg);
	vfprintf(stderr, err, va);
	va_end(va);
	free(err);
}

long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}