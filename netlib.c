#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFFER 32768

my_conn(char *addr,int port)
{
 int a,d,e,f;
 struct sockaddr_in name;
 struct sockaddr_in sin;
 struct sockaddr_in sin1;
 struct sockaddr_in sin2;
 struct servent *pse;
 struct servent *pse1;
 struct hostent *phe;
 struct hostent *phe1;
 FILE *file;

 fd_set arfds,awfds,rfds,wfds;
 int nfds;
  char buf[BUFFER];
  char buf1[BUFFER];
  char host[80];
  char line[80];
  char afile[15];
  int s,cc;
  int ss,ss1,alen;
  int cou;
  int opt=1;
  int buf_size = BUFFER;


  bzero((char *) &sin,sizeof(sin));
  bzero((char *) &sin1,sizeof(sin));
  sin.sin_port=htons((u_short)port);


if(phe=gethostbyname(addr))
      bcopy((char *)phe->h_addr,(char *) &sin.sin_addr,(int) phe->h_length);
  else if((sin.sin_addr.s_addr=inet_addr(addr)) == INADDR_NONE)

 { 
	fprintf(stderr,"\nNETLIB: Can't resolve %s\n",addr);
                   return(-1);
                   }

sin.sin_family=AF_INET;
  s=socket(AF_INET,SOCK_STREAM,0);
if(connect(s,(struct sockaddr *)&sin,sizeof(sin))
    < 0) 
	 {
		fprintf(stderr,"\nNETLIB: Can't connect %s ",addr);
		close(s);
		return(-1);
	}
return(s);

}



int my_acc(char *addr,int port)
{
 struct sockaddr_in sin;
 struct sockaddr_in sin1;
 struct hostent *phe;
 int opt,alen=sizeof(sin1);
 int ss,s;


 bzero((char *) &sin,sizeof(sin));
 bzero((char *) &sin1,sizeof(sin));
 sin.sin_port=htons(port);
if(addr[0] != '*')
{
  if(phe=gethostbyname(addr))
      bcopy((char *)phe->h_addr,(char *) &sin.sin_addr,(int) phe->h_length);
  else if((sin.sin_addr.s_addr=inet_addr(addr)) == INADDR_NONE)
                { fprintf(stderr,"\nNETLIB: No host address !!");
                   exit(1);
                   }
  }
  else sin.sin_addr.s_addr=INADDR_ANY;


  sin.sin_family=AF_INET;
  ss=socket(AF_INET,SOCK_STREAM,0);
  setsockopt(ss,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
/*
  setsockopt(ss,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
*/
  bind(ss,(struct sockaddr *) &sin,sizeof(sin));
  listen(ss,64);
  return (ss);
}

