#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
      INPUT
            nome-programma -n <numero> -m <numero> -o <stringa> -h
      OUTPUT
            -n: numero
            -m: numero
            -o: "stringa"
            -h: nome-programma -n <numero> -m <numero> -o <stringa> -h

      In fondo ho messo degli esempi di input e output testati.
*/
long isNumber(const char *s) 
{
      char *e = NULL;
      long val = strtol(s, &e, 0);
      if(e != NULL && *e == (char)0) 
            return val; 
      return -1;
}

int main(int argc, char *argv[])
{
      int   opt,

            n_flag = 0, // 1 se ho trovato n/m/o/h, 0 altrimenti
            m_flag = 0,
            o_flag = 0,
            h_flag = 0,

            n_val = -1,
            m_val = -1;

      opterr = 0; // getopt di default stampa errore quando trova un comando non riconosciuto (cosa che non desidero quindi serve mettere opterr = 0;)

      for(int i = 1; i < argc; i++)
      {
            opt = getopt(argc, argv, "n:m:o:h");
            switch(opt)
            {     // in optarg c'è l'argomento di -n/m/o
                  case 'n':
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
                        else
                        {
                              printf("-n: %d\n", n_val);
                              n_flag = 1;
                        }
                        break;
                  case 'm':
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
                        else
                        {
                              printf("-m: %d\n", m_val);
                              m_flag = 1;
                        }
                        break;
                  case 'o':
                        if(o_flag == 1)
                        {
                              fprintf(stderr, "Opzione \"-o\" settata più volte.\n");
                              return -1;
                        }
                        else
                        {
                              printf("-o: \"%s\"\n", optarg);
                              o_flag = 1;
                        }           
                        break;
                  case 'h':
                        if(h_flag == 0)
                        {
                              h_flag = 1;
                              printf("-h: %s -n <numero> -m <numero> -o <stringa> -h\n", argv[0]);
                        }
                        break;
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
            }
      }
      return 0;
}
/*
      INPUT_0
            ./a.out -n 12 -g -v -m 44
      OUTPUT_0
            -n: 12
            Opzione "-g" sconosciuta.
            Opzione "-v" sconosciuta.
            -m: 44
            
      INPUT_1
            ./a.out -n 12 ciao -o casa -h -o hotel -m cc -o
      OUTPUT_1
            -n: 12
            -o: "casa"
            -h: ./a.out -n <numero> -m <numero> -o <stringa> -h
            Opzione "-o" settata più volte.

      INPUT_2
            ./a.out -n 12 ciao -o casa -h -m cc -o
      OUTPUT_2
            -n: 12
            -o: "casa"
            -h: ./a.out -n <numero> -m <numero> -o <stringa> -h
            Opzione "-m" deve essere seguita da un numero.

      INPUT_3
            ./a.out -n 12 ciao -o casa -h -o
      OUTPUT_3
            -n: 12
            -o: "casa"
            -h: ./a.out -n <numero> -m <numero> -o <stringa> -h
            Opzione "-o" settata più volte.

      INPUT_4
            ./a.out -n 12 ciao -o casa -h -m cc
      OUTPUT_4
            -n: 12
            -o: "casa"
            -h: ./a.out -n <numero> -m <numero> -o <stringa> -h
            Opzione "-m" deve essere seguita da un numero.
*/