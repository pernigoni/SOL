#include <stdio.h>
/*
      SINTASSI PARSING
            -h -> stampo solo il messaggio di help (anche se negli argomenti ci sono altri comandi validi)
            -n <numero> -> stampo -n: <numero>
            -m <numero> -> stampo -m: <numero>
            -s <stringa> -> stampo -s: "<stringa>"
      ALTRO
            Il numero di "-" davanti al carattere deve essere >= 1. 
            Se trovo lo stesso comando (con la sintassi corretta) più volte considero solo il primo che incontro.
            Se l'argomento di -n o -m non è un numero stampo "-n/m: argomento non valido".
*/
int main(int argc, char *argv[])
{
      if(argc == 1) // in argv c'è solo il nome del programma
      {
            printf("Nessun argomento passato al programma\n");
            return -1;
      }

      for(int i = 1; i < argc; i++)
      {
            int j = 0;
            while(argv[i][j] != '\0') // cerco ----h
                  if(argv[i][j] == '-')
                        j++;
                  else if(argv[i][j] == 'h' && argv[i][j + 1] == '\0')
                  {
                        printf("help: %s -n <numero> -s <stringa> -m <numero>\n", argv[0]);
                        return 0;
                  }
                  else
                        break;
      }

      int found_n = 0, found_m = 0, found_s = 0; // senza queste variabili booleane stamperebbe tutti i ----n/m/s
      for(int i = 1; i < argc; i++)
      {
            int j = 0;
            while(argv[i][j] != '\0') // cerco -----n/m/s
                  if(argv[i][j] == '-')
                        j++;
                  else if(((argv[i][j] == 'n' || argv[i][j] == 'm' || argv[i][j] == 's') && argv[i + 1] != NULL) && argv[i][j + 1] == '\0')
                  {     // ho trovato -----n/m/s
                        int invalidArg = 0;
                        switch(argv[i][j])
                        {  
                              case 'n':
                                    i++;
                                    if(!found_n)
                                    {
                                          found_n = 1;
                                          j = 0;
                                          while(argv[i][j] != '\0') // controllo che nella stringa dopo -n ci siano solo numeri
                                                if(argv[i][j] >= '0' && argv[i][j] <= '9')
                                                      j++;
                                                else
                                                {
                                                      invalidArg = 1;
                                                      printf("-n: argomento non valido\n");
                                                      break;
                                                }
                                          if(!invalidArg)
                                                printf("-n: %s\n", argv[i]);
                                    }
                                    break;
                              case 'm':
                                    i++;
                                    if(!found_m)
                                    {
                                          found_m = 1;
                                          j = 0;
                                          while(argv[i][j] != '\0') // controllo che nella stringa dopo -m ci siano solo numeri
                                                if(argv[i][j] >= '0' && argv[i][j] <= '9')
                                                      j++;
                                                else
                                                {
                                                      invalidArg = 1;
                                                      printf("-m: argomento non valido\n");
                                                      break;
                                                }
                                          if(!invalidArg)
                                                printf("-m: %s\n", argv[i]);
                                    }
                                    break;        
                              case 's':
                                    i++;
                                    if(!found_s)
                                    {
                                          found_s = 1;
                                          printf("-s: \"%s\" \n", argv[i]);
                                    }
                                    break;
                        }
                  }
                  else
                        break;
      }
      return 0;
}