#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "nfs.h"


#define FREE 0
#define ALLOC 1
#define MAXHL 256


struct handle {
	       int alloc; 	/* is this handle in use */
	       int mode;	/* what we have file,dir */	
	       int pos;		/* position in file      */
	       int size;	/* size of the file      */	
	       union {
		int fd;
		char *dir;
		}u;
		};

struct handle rhandle[MAXHL];	/* max 256 handles */


init_handle()
{
 int a;
 
 for(a=0;a<MAXHL;a++)
  {
	rhandle[a].alloc=FREE;
	rhandle[a].mode=0;
  }
}


alloc_handle()
{
int a;
 for(a=0;a<MAXHL;a++)
  if(rhandle[a].alloc == FREE)
	{
	rhandle[a].alloc = ALLOC;
	return a;
	}

  return (-1);
}


my_open(char *name,int mode)
{
 struct stat ffstat;
 int tmp;

 tmp=alloc_handle();

 stat(name,&ffstat);
 if((ffstat.st_mode & S_IFMT) == S_IFDIR )
  
	rhandle[tmp].u.dir=opendir(name);
   else
	rhandle[tmp].u.fd=open(name,0);

	rhandle[tmp].mode=ffstat.st_mode;	
	rhandle[tmp].size=ffstat.st_size;	
	rhandle[tmp].pos=0;	

 return tmp;
}	

my_close(int tmp)
{
 
  if((rhandle[tmp].mode & S_IFMT) == S_IFDIR)
	closedir(rhandle[tmp].u.dir);
   else
	close(rhandle[tmp].u.fd);
  rhandle[tmp].alloc = FREE;
 }

my_readdir(int tmp, struct mydir *buf)
{
 struct dirent *dir;

 if((rhandle[tmp].mode & S_IFMT) != S_IFDIR) 
		return -1;
 dir=readdir(rhandle[tmp].u.dir);
 if(dir == 0) return;

 strcpy(buf->d_name,dir->d_name);
 buf->d_ino=dir->d_ino;
 return 1;
}


my_read(int tmp, char * buf, int len)
{
int rlen; 
 if((rhandle[tmp].mode & S_IFMT) == S_IFDIR) 
		return -1;

 rlen=read(rhandle[tmp].u.fd,buf,len);
 rhandle[tmp].pos+=rlen;
 return rlen;
}

my_write(int tmp, char * buf, int len)
{
int rlen; 
 if((rhandle[tmp].mode & S_IFMT) == S_IFDIR) 
		return -1;

 rlen=write(rhandle[tmp].u.fd,buf,len);
 rhandle[tmp].pos+=rlen;
 return rlen;
}

my_lseek(int tmp, int pos)
{
 int ret; 
 ret=lseek(rhandle[tmp].u.fd,pos,SEEK_SET);
 rhandle[tmp].pos=pos;
 return ret;
}

my_stat(int tmp, struct mystat *stat)
{
int ret; 
 struct stat ffstat;

  if((rhandle[tmp].mode & S_IFMT) == S_IFDIR)
	{
	  stat->mode=rhandle[tmp].mode;
	  stat->size=rhandle[tmp].size;
	  ret=0;
	}
else		 
 {
 ret=fstat(rhandle[tmp].u.fd,&ffstat);
 stat->dev=ffstat.st_dev;
 stat->ino=ffstat.st_ino;
 stat->mode=ffstat.st_mode;
 stat->nlink=ffstat.st_nlink;
 stat->uid=ffstat.st_uid;
 stat->gid=ffstat.st_gid;
 stat->size=ffstat.st_size;
 stat->atime=ffstat.st_atime;
 }

 return ret;
}
 
 
main(int argn ,char *argc[] )
{
 int fd;
 int sock;
 struct mydir buf;
 char buffer[80];
 int tmp;

 struct msg mymsg;
 
 init_handle(); 
 fd=my_acc("*",atoi(argc[1]));

 
 while( (sock=accept(fd,0,0)) )
{
 
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
 case WRITE:
		mymsg.ret=my_write(mymsg.fd,&mymsg.u.data[0],mymsg.len);
		break;
 case LSEEK:
		mymsg.ret=my_lseek(mymsg.fd,mymsg.u.pos);
		break;
 case STAT:
		mymsg.ret=my_stat(mymsg.fd,&mymsg.u.mstat);
		break;
 default:	
		mymsg.ret=-1;
		break;
 }
 write(sock,&mymsg,sizeof(mymsg));
 close(sock);
 } 

 close(fd);
 fprintf(stderr,"\n Accept Error: exiting "); 
}
  
