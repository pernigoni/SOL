#include "../Utils/utils.h"
#include "../Conn/conn.h"
#define BUF_SIZE 256

typedef struct msg // messaggio
{
	int len;
	char *str;
} msg_t;

int main(int argc, char *argv[]) 
{
	// creo il socket
	int sock_fd;
	SYSCALL_EXIT(socket, sock_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");
	
	// setto l'indirizzo
	struct sockaddr_un serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;    
	strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

	// mi connetto al socket del server
	int not_used;
	SYSCALL_EXIT(connect, not_used, connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "connect", "");

	msg_t msg;
	msg.str = safe_malloc(BUF_SIZE * sizeof(char));
	
	while(1) 
	{
		char buf[BUF_SIZE]; 
		memset(msg.str, '\0', BUF_SIZE);

		if(fgets(msg.str, BUF_SIZE - 1, stdin) == NULL) 
			break;
		if(strncmp(msg.str, "quit", 4) == 0) 
			break;
		msg.len = strlen(msg.str);
		
		// invio il messaggio al server
		SYSCALL_EXIT(writen, not_used, writen(sock_fd, &msg.len, sizeof(int)), "writen.1", "");
		SYSCALL_EXIT(writen, not_used, writen(sock_fd, msg.str, msg.len), "writen.2", ""); 
		
		int n; 
		// il client si mette in attesa di ricevere la risposta dal server
		SYSCALL_EXIT(readn, not_used, readn(sock_fd, &n, sizeof(int)), "readn.1", "");
		SYSCALL_EXIT(readn, not_used, readn(sock_fd, buf, n), "readn.2", "");  
		
		buf[n] = '\0';
		printf("Risultato: %s\n", buf);
	}

	if(msg.str != NULL) 
		free(msg.str);
	int r;
	SYSCALL_EXIT(close, r, close(sock_fd), "close", "");
	return 0;
}