#include <signal.h>
#include <sys/types.h>
#include "../utils/utils.h"

static volatile sig_atomic_t sigint_count = -1, sigstp_count = -1, sigstp_flag = 0;

static void sig_handler(int sig) 
{
	switch(sig)
	{
		case SIGINT:
			if(sigint_count == -1)
				sigint_count++;
			sigint_count++;
			break;
		case SIGTSTP:
			sigstp_flag = 1;
			if(sigstp_count == -1)
				sigstp_count++;
			sigstp_count++;
			break;
		case SIGALRM:
			return ;
		default:
			abort();
	}
}

int main() 
{
	sigset_t mask, old_mask;
	sigemptyset(&mask); // resetto tutti i bit
	sigaddset(&mask, SIGINT); // aggiunto SIGINT (Ctrl + C) alla maschera
	sigaddset(&mask, SIGTSTP); // aggiunto SIGTSTP (Ctrl + Z) alla maschera

	// blocco i segnali SIGINT e SIGTSTP finchè non ho finito l'installazione degli handler, mi conservo la vecchia maschera
	if(sigprocmask(SIG_BLOCK, &mask, &old_mask) == -1) // qui usiamo le funzioni dei processi ma poi useremo sempre quelle dei thread 
	{
		perror("sigprocmask");
		return EXIT_FAILURE;
	}
	
	// installo un unico signal handler per tutti i segnali che mi interessano
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));   
	sa.sa_handler = sig_handler;
	sigset_t handler_mask;
	sigemptyset(&handler_mask);
	sigaddset(&handler_mask, SIGINT);
	sigaddset(&handler_mask, SIGTSTP);
	sigaddset(&handler_mask, SIGALRM);
	sa.sa_mask = handler_mask; // l'handler eseguirà con SIGINT e SIGTSTP mascherati

	int r;
	SYSCALL_RETURN(sigaction, r, sigaction(SIGINT, &sa, NULL), "sigaction SIGINT", "");
	SYSCALL_RETURN(sigaction, r, sigaction(SIGTSTP, &sa, NULL), "sigaction SIGTSTP", "");
	SYSCALL_RETURN(sigaction, r, sigaction(SIGALRM, &sa, NULL), "sigaction SIGALRM", "");

	while(1) 
	{
		// atomicamente setta la maschera e si sospende (come alternativa avrei potuto usare sigwait senza installare il signal handler)
		if(sigsuspend(&old_mask) == -1 && errno != EINTR) // se errno == EINTR vuol dire che la chiamata è stata interrotta da un segnale 
		{
			perror("sigsuspend");
			return EXIT_FAILURE;
		}
		// qui i segnali sono nuovamente bloccati

		if(sigstp_flag == 1)
		{
			printf("Ricevuti %d SIGINT\n", sigint_count < 0 ? 0 : sigint_count);
			sigstp_flag = 0;
			sigint_count = 0;
		}

		if(sigstp_count == 3)
		{
			sigstp_count = 0;
			printf("Il programma terminerà tra 10 secondi, vuoi continuare? (y/n):\n");
			fflush(stdout);
			alarm(10);
			char y = 'n';
			scanf(" %c", &y);
			if(y == 'y')
				alarm(0); // resetto l'allarme... potrei non farcela
			else 
				break;
		}
	}
	return 0;
}