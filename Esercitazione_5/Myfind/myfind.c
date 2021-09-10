#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include "../Utils/utils.h"
#define MAX_LEN 256

int find(char *dir_name, char *file_name)
{
      // chdir() changes the current working directory of the calling process to the directory specified in path.
      if(chdir(dir_name) == -1)
      {
            fprintf(stderr, "ERRORE: impossibile entrare nella directory %s\n", dir_name);
            return -1;
      }
      /*
            The  opendir() function opens a directory stream corresponding to the directory
            name, and returns a pointer to the directory stream.  The stream is  positioned
            at the first entry in the directory.
      */
      DIR *dir;
      if((dir = opendir(".")) == NULL)
      {
            fprintf(stderr, "ERRORE: impossibile entrare nella directory %s\n", dir_name);
            return -1;
      }
      /*
            struct dirent 
            {
                  ino_t          d_ino;       * Inode number *
                  off_t          d_off;       * Not an offset; see below *
                  unsigned short d_reclen;    * Length of this record *
                  unsigned char  d_type;      * Type of file; not supported by all filesystem types *
                  char           d_name[256]; * Null-terminated filename *
            };
      */
      struct dirent *file;
      struct stat statbuf;
      int r;

      while((errno = 0, file = readdir(dir)) != NULL)
      {
            SYSCALL(stat, r, stat(file->d_name, &statbuf), "chiamando stat su %s, errno = %d\n", file->d_name, errno);
      
            if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) // senza questo if entro in un loop infinito o comunque in qualcosa che non voglio 
            {
                  if(S_ISDIR(statbuf.st_mode))
                  {
                        find(file->d_name, file_name);

                        if(chdir("..") == -1)
                        {
                              fprintf(stderr, "ERRORE: impossibile tornare nella directory %s/..\n", file->d_name);
                              return -1;
                        }
                  }

                  if(S_ISREG(statbuf.st_mode)) // regular file
                  {     
                        char *buf = malloc(MAX_LEN * sizeof(char));
                        if(buf == NULL)
                        {
                              perror("malloc");
		                  return -1;
                        }
                        /*
                              The getcwd() function copies an absolute pathname of the current working 
                              directory to the array pointed to by buf, which is of length size.
                        */
                        getcwd(buf, MAX_LEN);
                        if(strncmp(file->d_name, file_name, strlen(file_name)) == 0)
                              printf("%s/%s %s", buf, file->d_name, ctime(&statbuf.st_mtime));
                        free(buf);
                  }
            }
      }
      
      if(errno != 0)
      {
            perror("readdir");
            return -1;
      }

      if(closedir(dir) == -1)
      {     // possiamo anche non gestire l'errore dato che non è una syscall
            fprintf(stderr, "ERRORE: chiudendo la directory %s\n", dir_name);
            return -1;
      }

      return 1;
}

int main(int argc, char *argv[])
{
      if(argc != 3)
      {
            fprintf(stderr, "Usa: %s dir nome-file\n", argv[0]);
            return -1;
      }
      if(strcmp(argv[1], ".") == 0 || strcmp(argv[1], "..") == 0)
      {
            fprintf(stderr, "Usa: %s dir nome-file\ndir deve essere diverso da \".\" e \"..\"\n", argv[0]);
            return -1;
      }
      /*
            struct stat 
            {
                  dev_t     st_dev;         * ID of device containing file *
                  ino_t     st_ino;         * Inode number *
                  mode_t    st_mode;        * File type and mode *
                  nlink_t   st_nlink;       * Number of hard links *
                  uid_t     st_uid;         * User ID of owner *
                  gid_t     st_gid;         * Group ID of owner *
                  dev_t     st_rdev;        * Device ID (if special file) *
                  off_t     st_size;        * Total size, in bytes *
                  blksize_t st_blksize;     * Block size for filesystem I/O *
                  blkcnt_t  st_blocks;      * Number of 512B blocks allocated *

                  struct timespec st_atim;  * Time of last access *
                  struct timespec st_mtim;  * Time of last modification *
                  struct timespec st_ctim;  * Time of last status change *
                  ...
            };
      */
      char *dir_name = argv[1], *file_name = argv[2];
      struct stat statbuf;
      int r;
      SYSCALL(stat, r, stat(dir_name, &statbuf), "chiamando stat su %s, errno = %d\n", dir_name, errno);

      if(!S_ISDIR(statbuf.st_mode))
      {
            fprintf(stderr, "%s non è una directory\n", dir_name);
            return -1;
      }

      find(dir_name, file_name); // ricorsiva
      return 0;
}