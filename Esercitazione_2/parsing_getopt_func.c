#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
      !!!!! DÀ QUALCHE INFORMAZIONE IN PIÙ RISPETTO A parsing_getopt.c (è possibile accorgersene provando gli input scritti in parsing_getopt.c)

      INPUT
            nome-programma -n <numero> -m <numero> -o <stringa> -h
      OUTPUT
            -n: numero
            -m: numero
            -o: "stringa"
            -h: nome-programma -n <numero> -m <numero> -o <stringa> -h
*/
long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

int n_flag = 0, m_flag = 0, o_flag = 0, h_flag = 0;

int arg_n(const char *arg)
{
      int n_val;

      if(n_flag == 1)
      {
            fprintf(stderr, "Opzione \"-n\" settata più volte.\n");
            return -1;
      }
      else if((n_val = isNumber(optarg)) == -1)
      {     // optarg non è un numero oppure è vuoto
            fprintf(stderr, "Opzione \"-n\" deve essere seguita da un numero.\n");
            return -1;
      }

      printf("-n: %d\n", n_val);
      n_flag = 1;
      return 0;
}

int arg_m(const char *arg)
{
      int m_val;

      if(m_flag == 1)
      {
            fprintf(stderr, "Opzione \"-m\" settata più volte.\n");
            return -1;
      }
      else if((m_val = isNumber(optarg)) == -1)
      {     // optarg non è un numero
            fprintf(stderr, "Opzione \"-m\" deve essere seguita da un numero.\n");
            return -1;
      }
      
      printf("-m: %d\n", m_val);
      m_flag = 1;
      return 0;
}

int arg_o(const char *arg)
{
      if(o_flag == 1)
      {
            fprintf(stderr, "Opzione \"-o\" settata più volte.\n");
            return -1;
      }
      
      printf("-o: \"%s\"\n", optarg);
      o_flag = 1;
      return 0;
}

int arg_h(const char *file_name)
{
      printf("-h: %s -n <numero> -m <numero> -o <stringa> -h\n", file_name);
      return 0;
}

int main(int argc, char* argv[]) 
{
      /* 
            Inizializzazione dell'array V con i puntatori a funzione.

            pos('h' = 104) = 104 % 4 = 0
            pos('m' = 109) = 109 % 4 = 1
            pos('n' = 110) = 110 % 4 = 2
            pos('o' = 111) = 111 % 4 = 3
      */
      int (*V[4]) (const char *) = {arg_h, arg_m, arg_n, arg_o};

      int opt;

      opterr = 0;

      while((opt = getopt(argc, argv, "n:m:o:h")) != -1) 
      {
            switch(opt) 
            {
                  case '?':
                  /*
                        Si entra qui per 2 motivi:
                              1) optarg di -n/m/o è vuoto (NULL).
                              2) opt sconosciuta.

                        If getopt() does not recognize an option character, it prints an error message 
                        to stderr, stores the character in optopt, and returns '?'. 
                        The calling program may prevent the error message by setting opterr to 0.
                  */
                        switch(optopt)
                        {
                              case 'n':
                                    if(n_flag == 1)
                                          fprintf(stderr, "Opzione \"-n\" settata più volte.\n");
                                    else
                                          fprintf(stderr, "Opzione \"-n\" deve essere seguita da un numero.\n");
                                    return -1;
                              case 'm':
                                    if(m_flag == 1)
                                          fprintf(stderr, "Opzione \"-m\" settata più volte.\n");
                                    else
                                          fprintf(stderr, "Opzione \"-m\" deve essere seguita da un numero.\n");
                                    return -1;
                              case 'o':
                                    if(o_flag == 1)
                                          fprintf(stderr, "Opzione \"-o\" settata più volte.\n");
                                    else
                                          fprintf(stderr, "Opzione \"-o\" deve essere seguita da una stringa.\n");
                                    return -1;
                              default:
                                    fprintf(stderr, "Opzione \"-%c\" sconosciuta.\n", optopt);
                        }
                  default: 
                        if(opt == 'h' || opt == 'm' || opt == 'n' || opt == 'o')
                              V[opt % 4] ((optarg == NULL ? argv[0] : optarg));
            }
      }
      return 0; 
}