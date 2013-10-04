
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */

#define	SA	struct sockaddr
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */


//ssize_t	
void writen(int fd, const void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nwritten;
	const char *ptr;
        
	ptr = vptr;
	nleft = n;
	while (nleft > 0) 
	{
           //ptr[i]=vptr[i];    
	   if ( (nwritten = write(fd, ptr, 1)) <= 0) 
		{
		 if (errno == EINTR)
			nwritten = 0;		// and call write again 
		 else
			return ;			// error
		}

		nleft --;
		ptr   += nwritten;
               // i++;
	}
	//return(n);
}


ssize_t readline(int fd,void *vptr,size_t maxlen)
{
   ssize_t n,rc;
   char c,*ptr;

   ptr=vptr;
   for(n=1;n<maxlen;n++)
   {
ag:
   if( (rc=read(fd,&c,1))==1)
      {
      *ptr++=c;
       if(c=='\n')
          break;        //end of line
      }
   else 
      if (rc==0)
      {
         if(n==1)
            return(0);   //eof data no reading
         else 
            break;      //data reading
     }
      else
         {
          if(errno==EINTR)
             goto ag;
          return(-1);
          }
   }
   *ptr=0;
   return (n);
}


ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		printf("\nreadline error");
	return(n);
}


void fun_echo(int sockfd)
{
	ssize_t	n;
	char line[MAXLINE];
	char cmp[MAXLINE]="exit";
	char rec[100]="==DISCONNECTED==\n";

	for ( ; ; ) 
	{
		if ((n = Readline(sockfd, line, MAXLINE)) == 0)
			return;		
		printf("%s",line);
		if (line[0] == cmp[0]&&line[1] == cmp[1]&&line[2] == cmp[2]&&line[3] == cmp[3])//budlokod
		{
			printf("\nclient disconnected\n");
			writen(sockfd, rec, strlen(rec));
			exit(1);
		}
		writen(sockfd, line, n);
	}
}




int main(int argc, char **argv)
{
	int listensoc, connsoc;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
    //    char port[8], ipaddr[15];
	char *port;
	char *ipaddr;
	//printf("Enter IP: ");
	//scanf("%s",ipaddr);
	ipaddr = argv[1];
	
	//printf("Enter port: ");
	//scanf("%s",port);
	port = argv[2];
	printf("Created IP: %s\nPort:%s\n",ipaddr,port);
	
	listensoc = socket(AF_INET, SOCK_STREAM, 0);  ////get socet
	
	bzero(&servaddr, sizeof(servaddr)); ////obnulenie struct
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipaddr);/////////////****   htonl(INADDR_ANY);//
	servaddr.sin_port = htons(atoi(port));          //servaddr.sin_port = htons(10000);

	bind(listensoc, (SA *) &servaddr, sizeof(servaddr)); //bind

	listen(listensoc, LISTENQ); // listen
	//exit proc
		
	for ( ; ; ) 
	   {
		clilen = sizeof(cliaddr);
		connsoc = accept(listensoc, (SA *) &cliaddr, &clilen);   // get client socet
		
				
		if ((childpid = fork()) == 0) 
		{	
			close(listensoc);	
			fun_echo(connsoc);	
			exit(0);
		}
		close(connsoc);	
	}

}
