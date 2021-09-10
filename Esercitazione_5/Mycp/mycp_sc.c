#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../Utils/utils.h"
#define BUF_SIZE 256

int main(int argc, char *argv[])
{
      if(argc < 3 || argc > 4)
      {
            fprintf(stderr, "Usa: %s filein fileout [buffersize]\n", argv[0]);
            return -1;
      }

      int buf_size;
      if(argc == 3)
            buf_size = BUF_SIZE;
      else if(argc == 4)
      {
            buf_size = isNumber(argv[3]);
            if(buf_size <= 0)
            {
                  fprintf(stderr, "%s è minore o uguale a 0 oppure non è un numero\n", argv[3]);
                  return -1;
            }
      }

      char *src_name = argv[1], *dest_name = argv[2];
      int src_fd, dest_fd;
      SYSCALL(open, src_fd, open(src_name, O_RDONLY), "aprendo il file di input %s, errno = %d\n", src_name, errno);
      SYSCALL(open, dest_fd, open(dest_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH), "creando o aprendo il file di output %s, errno = %d\n", dest_name, errno);

      char *buf = malloc(buf_size * sizeof(char));
      if(buf == NULL)
      {
            perror("malloc");
            return -1;
      }

      int n_bytes;
      do
      {     // read e write ritornano rispettivamente il numero di byte letti e scritti
            SYSCALL(read, n_bytes, read(src_fd, buf, buf_size), "leggendo il file di input %s, errno = %d\n", src_name, errno);
            SYSCALL(write, n_bytes, write(dest_fd, buf, n_bytes), "scrivendo il file di output %s, errno = %d\n", dest_name, errno);
      } while(n_bytes > 0);
      
      SYSCALL(close, n_bytes, close(src_fd), "chiudendo il file di input %s, errno = %d\n", src_name, errno);
      SYSCALL(close, n_bytes, close(dest_fd), "chiudendo il file di output %s, errno = %d\n", dest_name, errno);
      free(buf);
      return 0;
}