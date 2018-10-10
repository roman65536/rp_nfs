#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "nfs.h"


static char *server;
static int port;


my_setnet(char *name,int lport)
{
 server=name;
 port=lport;
}

my_open(char *name,int mode)
{
 int fd;
 struct msg mymsg ;

 fd=my_conn(server,port);
 
 strcpy(mymsg.u.name,name);
 mymsg.opcode=OPEN;
 mymsg.len=mode;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
 close(fd);
 return(mymsg.ret);
}

my_close(int hd)
{
 int fd;
 struct msg mymsg ;

 fd=my_conn(server,port);
 mymsg.opcode=CLOSE;
 mymsg.fd=hd;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
 close(fd);
 return(mymsg.ret);
}
  
my_stat(int hd, struct stat *buf)
{
 int fd;
 struct msg mymsg ;

 fd=my_conn(server,port);
 mymsg.opcode=STAT;
 mymsg.fd=hd;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
 close(fd);
 buf->st_dev=mymsg.u.mstat.dev;
 buf->st_ino=mymsg.u.mstat.ino;
 buf->st_mode=mymsg.u.mstat.mode;
 buf->st_nlink=mymsg.u.mstat.nlink;
 buf->st_uid=mymsg.u.mstat.uid;
 buf->st_gid=mymsg.u.mstat.gid;
 buf->st_size=mymsg.u.mstat.size;
 buf->st_atime=mymsg.u.mstat.atime;

 return(mymsg.ret);
}

my_readdir(int hd,struct mydir *dir)
{
 int fd;
 struct msg mymsg ;

 fd=my_conn(server,port);
 
 mymsg.opcode=READDIR;
 mymsg.fd=hd;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
 strcpy(dir->d_name,mymsg.u.dir.d_name);
 dir->d_ino=mymsg.u.dir.d_ino; 
 close(fd);
 return(mymsg.ret);
}

my_read(int hd,char *buffer,int  len )
{
 int fd;
 struct msg mymsg ;
 char *tmp=buffer;
 int rlen;
 int toread;
 rlen=0;
 toread=len;

do
 {
 fd=my_conn(server,port);
 mymsg.opcode=READ;
 mymsg.fd=hd;
 if(toread > 256) mymsg.len=256;
 else 
  mymsg.len=toread;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
  if(mymsg.ret <= 0 ) 
 		{
		     close(fd);
		     return (rlen);
		}
 memcpy(tmp,mymsg.u.data,mymsg.ret);
 close(fd);
 rlen += mymsg.ret;
 toread-=mymsg.ret;
 tmp+=mymsg.len;
 }while (toread);

 return (rlen);
}

my_write(int hd,char *buffer,int  len )
{
 int fd;
 struct msg mymsg ;
 char *tmp=buffer;
 int rlen;
 int toread;
 rlen=0;
 toread=len;

do
 {
 fd=my_conn(server,port);
 mymsg.opcode=WRITE;
 mymsg.fd=hd;
 if(toread > 256) mymsg.len=256;
 else 
  mymsg.len=toread;
 memcpy(mymsg.u.data,tmp,mymsg.ret);
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
  if(mymsg.ret <= 0 ) 
 		{
		     close(fd);
		     return (rlen);
		}
 close(fd);
 rlen += mymsg.ret;
 toread-=mymsg.ret;
 tmp+=mymsg.len;
 }while (toread);

 return (rlen);
}



my_lseek(int hd, int pos)
{

  int fd;
  struct msg mymsg ;
  
  fd=my_conn(server,port);
 
 mymsg.opcode=LSEEK;
 mymsg.fd=hd;
 mymsg.u.pos=pos;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));

 return (mymsg.ret);
}

/*
main(int argn , char *argc[] )
{

 struct mydir dir;
 int fd;
 char buffer[4096];
 int len;
 
 server=argc[1];
 port=atoi(argc[2]);

fd= my_open(".",0);

 while(my_readdir(fd,&dir) > 0)
  printf("%d %s\n",dir.d_ino,dir.d_name);
my_close(fd);
 
 fd=my_open("tru",0);
 while((len=my_read(fd,buffer,4096)) > 0)
  write(1,buffer,len);
 my_close(fd);

}
*/

test(int argn ,char *argc[] )
{
 int fd;
 int sock;
 struct mydir buf;
 char buffer[80];
 int tmp;

 struct msg mymsg;
 
 fd=my_conn(argc[1],atoi(argc[2]));

 
/* 
 read(sock,&mymsg,sizeof(mymsg));
 switch(mymsg.opcode)
 {

 case OPEN:
		mymsg.ret=my_open(mymsg.u.name,mymsg.len);
		break;
 case CLOSE:
		mymsg.ret=my_close(mymsg.fd);
		break;
 case READDIR:
		mymsg.ret=my_readdir(mymsg.fd,&mymsg.u.dir);
		break;
 case READ:
		mymsg.ret=my_read(mymsg.fd,&mymsg.u.data[0],mymsg.len);
		break;
 case LSEEK:
		mymsg.ret=my_lseek(mymsg.fd,mymsg.u.pos);
		break;
 default:	
		mymsg.ret=-1;
		break;
 }
 write(sock,&mymsg,sizeof(mymsg));
 close(sock);
 } 
 */

 sprintf(mymsg.u.name,".");
 mymsg.opcode=OPEN;
 mymsg.len=0;
 write(fd,&mymsg,sizeof(mymsg));
 read(fd,&mymsg,sizeof(mymsg));
 printf("\n Return: %d ",mymsg.ret);
 close(fd);
}
  
