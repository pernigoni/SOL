#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../Utils/utils.h"
#define BUF_SIZE 256

int main(int argc, char *argv[])
{
      if(argc < 3 || argc > 4)
      {
            fprintf(stderr, "Usa: %s filein fileout [buffersize]\n", argv[0]);
            return -1;
      }

      int buf_size = BUF_SIZE;
      if(argc == 4)
      {
            buf_size = isNumber(argv[3]);
            if(buf_size <= 0)
            {
                  fprintf(stderr, "%s è minore o uguale a 0 oppure non è un numero\n", argv[3]);
                  return -1;
            }
      }

      char *src_name = argv[1], *dest_name = argv[2];
      FILE *ifp, *ofp;
      FOPEN_ERR(ifp, src_name, "r");
      FOPEN_ERR(ofp, dest_name, "w");

      char *buf = calloc(buf_size, sizeof(char));
      if(buf == NULL)
      {
            perror("calloc");
            return -1;
      }

      while(fread(buf, sizeof(char), buf_size, ifp) > 0)
            fwrite(buf, sizeof(char), buf_size, ofp);
      // con buf_size non piccolissimi scrive robaccia alla fine del file

      fclose(ifp);
      fclose(ofp);
      free(buf);
      return 0;
}
