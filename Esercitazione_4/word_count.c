#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#define FOPEN_ERR(file, path, mode) \
      if((file = fopen(path, mode)) == NULL)                                                    \
      {                                                                                         \
            fprintf(stderr, "Impossibile aprire il file %s in modalità \"%s\".\n", path, mode);   \
            exit(-1);                                                                           \
      }

void print_l_w(char *file_path, int no_flag, int l_flag, int w_flag)
{
      FILE *ifp;
      FOPEN_ERR(ifp, file_path, "r");

      int newlines_count = 0, words_count = 0;
      
      char c;
      int is_word = 0;
      while((c = fgetc(ifp)) != EOF)
      {
            if(c == '\n')
                  newlines_count++;
            
            if(!isspace(c) && is_word == 0)
            {
                  is_word = 1;
                  words_count++;
            }
            
            if(isspace(c) && is_word)
                  is_word = 0;
      }

      static int first_print = 0;
      if(first_print == 0)
      {
            first_print = 1;
            printf("====================================\n");
      }
      printf("%s\n", file_path);

      if(no_flag)
            l_flag = w_flag = 1;
      if(l_flag)
            printf("-l: %d\n", newlines_count);
      if(w_flag)
            printf("-w: %d\n", words_count);

      printf("====================================\n");

      fclose(ifp);
}

int main(int argc, char *argv[])
{
      int   opt,

            no_flag = 1,
            l_flag = 0, // 1 se ho trovato l/w, 0 altrimenti
            w_flag = 0;

      opterr = 0; // getopt di default stampa errore quando trova un comando non riconosciuto (cosa che non desidero quindi serve mettere opterr = 0;)

      while((opt = getopt(argc, argv, "lw")) != -1)
            switch(opt)
            {
                  case 'l':
                        no_flag = 0;
                        l_flag = 1;
                        break;
                  case 'w':
                        no_flag = 0;
                        w_flag = 1;
                        break;
                  case '?':
                        fprintf(stderr, "Opzione \"-%c\" sconosciuta.\n", optopt);
                        return -1;
            }

      // The variable optind is the index of the next element to be processed in argv.
      int n_files = argc - optind;

      if(n_files == 0)
      {
            fprintf(stderr, "Usa: %s [-l -w] file1 [file2 file3 ...]\n", argv[0]);
            return -1;
      }

      for(int i = optind; i < argc; i++)
            print_l_w(argv[i], no_flag, l_flag, w_flag);
            
      return 0;
}