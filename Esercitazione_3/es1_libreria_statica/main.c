#include <string.h>
#include "tokenizer.h"

int main(int argc, char *argv[])
{
      if(argc == 1)
            return 1;
      if(strncmp(argv[1], "0", strlen(argv[1])) == 0) 
            for(int i = 2; i < argc; i++)
                  tokenizer(argv[i]);
      else if(strncmp(argv[1], "1", strlen(argv[1])) == 0)
            for(int i = 2; i < argc; i++)
                  tokenizer_r(argv[i]);
      else
            return 2;
      return 0;
}