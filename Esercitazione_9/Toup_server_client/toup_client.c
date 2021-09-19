#include <assert.h>
#include "../Utils/utils.h"
#include "../Conn/conn.h"

int main(int argc, char *argv[]) 
{
      if(argc < 2) 
      {
            fprintf(stderr, "Usa: %s stringa [stringa]\n", argv[0]);
            exit(EXIT_FAILURE);
      }

      int sock_fd;
      SYSCALL_EXIT(socket, sock_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");
      
      struct sockaddr_un serv_addr;      
      memset(&serv_addr, '0', sizeof(serv_addr));
      serv_addr.sun_family = AF_UNIX;    
      strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME) + 1);

      int not_used;
      SYSCALL_EXIT(connect, not_used, connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "connect", "");
      
      for(int i = 1; i < argc; i++) 
      {
            int n = strlen(argv[i]) + 1;
            // invio il messaggio al server
            SYSCALL_EXIT(writen, not_used, writen(sock_fd, &n, sizeof(int)), "writen.1", "");
            SYSCALL_EXIT(writen, not_used, writen(sock_fd, argv[i], n * sizeof(char)), "writen.2", "");

            char *buf = safe_calloc(n + 1, sizeof(char));
            // mi metto in attesa di ricevere la risposta dal server
            SYSCALL_EXIT(readn, not_used, readn(sock_fd, &n, sizeof(int)), "readn.1", "");
            SYSCALL_EXIT(readn, not_used, readn(sock_fd, buf, n * sizeof(char)), "readn.2", "");
            
            buf[n] = '\0';
            printf("Risultato: %s\n", buf);
            free(buf);
      }

      int r;
      SYSCALL_EXIT(close, r, close(sock_fd), "close", "");
      return 0;
}