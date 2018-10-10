#ifdef NCURSES
#include <ncurses/curses.h>
#else
#include <curses.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "nc.h"
#include "nfs.h"

struct dir_e *showit(struct dir_e *,struct dir_e *,int ,int );
int hascolor;


readit( char *name)
{
int fd;
int fd2;
struct mydir dire;
struct stat stat1;
struct dir_e *dir_ep;
char name1[160];
int a=0;


fd=my_open(name,0);
while((my_readdir(fd,&dire)))
{
/*
if(strcmp(dire.d_name,".") == 0) 
	goto next;
*/
dir_ep = (struct dir_e *) malloc(sizeof(dir_e));

strcpy(name1,name);
strcat(name1,"/");
strcat(name1,dire.d_name);

fd2=my_open(name1,0);
my_stat(fd2,&stat1);
my_close(fd2);
dir_ep->size=stat1.st_size;
strcpy(dir_ep->name,&dire.d_name[0]);
/*
dir_ep->name[dire->d_reclen]=0;
*/
fprintf(stderr,"\n%s %x",dir_ep->name,stat1.st_mode);
switch ((stat1.st_mode & S_IFMT))
{
case S_IFREG:	
case S_IFCHR:
case S_IFBLK:
		dir_ep->mode=FILE;
		insfile(dir_ep);
		break;
case S_IFDIR:	dir_ep->mode=DIR;
		insdir(dir_ep);
		break;
}
next:
a++;

}
my_close(fd);
/*
sortit(DIR);
sortit(FILE); 
*/
return a;
}


sortit(int mode)
{
struct dir_e *a1;
struct dir_e *a2;
struct dir_e *ffirst;
char tmpc[160];
long size;
int flag;
int tmp=0;

ffirst=first;
a2=last;
for(a1=first;a1->next !=0;a1=a1->next)
 if(a1->mode == mode)  {ffirst=a1;
			break; }

for( a1=ffirst;((a1->next != a2) && (a1->next != 0)); a1= a1->next)
{
 if(a1->mode != mode ) return;
 for(a2=last ;((a2 !=a1 ) && (a2 !=0)); a2=a2->prev)
 {  
if((strcmp(a1->name,a2->name) > 0) && (a1->mode == mode) && (a2->mode ==mode))
      {
	strcpy(tmpc,a1->name);
	strcpy(a1->name,a2->name);
	strcpy(a2->name,tmpc);
	size=a1->size;
	a1->size=a2->size;
	a2->size=size;
#if DEBUG
	showit(first,first,20,2);
#endif
      }
   }
 } 
}
 


insfile(struct dir_e *dir)
{
 if(first == 0) {
		first=last=dir;
		dir->prev=0;
		dir->next=0;
		return;
		}
last->next=dir;
dir->prev=last;
dir->next=0;
last=dir;
}

insdir(struct dir_e *dir)
{
 if(first == 0) {
               first=last=dir;
	       dir->prev=0;
		dir->next=0;
		return;
		}
dir->next=first;
first->prev=dir;
dir->prev=0;
first=dir;
}

freeit(struct dir_e *dir)
{
 struct dir_e *cur;
 cur=dir;
 while(cur != 0)
 {
  cur=dir->next;
  free(dir);
  dir=cur;
 }
first=0;
last=0;
}

main(int argn , char *argc[])
{
struct dir_e *cur;
struct dir_e *now;
int line,c,y;
int nr,dis;
char tmp[160];
char tmp1[160];
char dir[160];
int tmpi;
line=20;

/*first=last=select=(struct dir_e *) 0;
*/

my_setnet(argc[1],atoi(argc[2]));

myinit(1);

strcpy(dir,".");
nr=readit(dir);
cur=first;
now=first;
dis=0;
y=2;

showit(cur,now,line,y);

while(c=getch())
{
switch(c)
{
 case 'j' :
 case KEY_DOWN:
 	     if((now->next != 0) && (dis < nr))
	      {
	       now=now->next;
	       if((dis < line-1) )
	       			 dis++;
		  else
				 cur=cur->next;
	      }
	       break;
case 'k':
case  KEY_UP:
	    if((now->prev != 0) )
	       {
	       now=now->prev;
	      if(dis > 0 ) 
	                dis--;
		else
			cur = cur->prev;
	      }
	       break;
case 'h':
case KEY_HOME:
	     cur=first;
	     now=first;
	     dis=0;
	     break;

case KEY_NPAGE:
	      
	      for(tmpi=line-1;((now->next != 0 ) && (tmpi > 0)); tmpi--,now=now->next,cur=cur->next); 
	      myinit(0);
	      break;

case KEY_PPAGE:
	     for(tmpi=line-1;((now->prev !=0 ) && (tmpi >0)); tmpi --, now=now->prev,cur = cur->prev);
	     myinit(0);
	     break; 

case KEY_F(1):
		if(now != 0)
		 {
		   endwin();
		   sprintf(&tmp[0],"view %s",now->name);
		   system(tmp);
		   myinit(1);
		}
		break;
case KEY_F(2):
		if(now !=0)
		 {
		  endwin();
		  sprintf(&tmp[0],"vi %s",now->name);
		  system(tmp);
		  myinit(1);
		}
		break;
case KEY_F(9):
		if(now !=0)
		 {
 		  move(1,60);
		  printf("Working please wait");
		  refresh();
		  endwin();
		  sprintf(&tmp[0],"tar.sh %s %d",now->name,getpid);
		  system(tmp);
		  myinit(1);
		}
		break;
case KEY_F(10):
case 'q':
		erase();
		refresh();
		endwin();
		exit(0);
		break;

case KEY_F(3):
		move(25,11);
		printw(" Input destination: .....\b\b\b\b\b");
		refresh();
		getstr(tmp1);
/*
	        endwin();
		sprintf(tmp,"cp %s %s",now->name,tmp1);
		system(tmp);
*/
		copy_file(now->name,tmp1);
		myinit(0);
		break; 	
		

case KEY_F(4):
		freeit(first);	
		nr=readit(".");
		dis=0;
		cur=first;
		now=first;
		myinit(0);
		break;
	
case KEY_ENTER:
case '\n':
		if(now !=0)
		 {
		  switch(now->mode)
		  {
		   case DIR:
		             strcat(dir,"/");		
			     strcat(dir,&now->name[0]);
			     freeit(first);
			     nr=readit(dir);
			     now=first;
			     dis=0;
			     cur=first;
			     myinit(0);
			     break;
		   case FILE:
		        erase();
			refresh();        
		  	endwin();
		  	sprintf(&tmp[0],"%s",now->name);
		  	system(tmp);
			sleep(1);	
		  	myinit(1);
			break;	     
		  }
		}
		break;
			        
}

 showit(cur,now,line,y);
 move(1,0);
 refresh();
}

}
 

struct dir_e *showit(struct dir_e *cur,struct dir_e *now,int lines,int y)
{
 struct dir_e *poi=cur;
 char pwd[80];

move(y++,0);
/*
getcwd(pwd,80);
printw(" [%s]                     ",pwd);
*/


while((poi != 0 ) && ( lines !=0))
 {
 move(y++,0);
if(poi == now)
 {
  attron(A_BOLD);
 }
if(poi->mode == DIR)
  printw("[%-20.20s]  %10d                                  ",poi->name,poi->size);
 else 
  printw("%-20.20s   %10d                                  ",poi->name,poi->size);
  attroff(A_BOLD);
  --lines;
  poi=poi->next;
 }
 refresh();
return poi;
}

myinit(int a)
{
int x,y;
if(a == 1)
 {
initscr();
/*idlok(stdscr,TRUE); */
keypad(stdscr, TRUE); 
cbreak();
/*noecho(); */
}
erase();
move(0,0);
if(has_colors())
{
start_color();
init_pair(1, COLOR_WHITE, COLOR_BLUE);
init_pair(2, COLOR_BLUE, COLOR_WHITE);
attron(COLOR_PAIR(1));
hascolor=1;
 for(x=0;x<25;x++)
  for(y=0;y<80;y++)
	addch(' ');
}
else hascolor=0;
move(0,0);

attron(A_BOLD);
printw("Pollak's Software Unix-Commander (c) 96");
attroff(A_BOLD);

attron(A_REVERSE);
move(23,0);
printw("F1-VIEW  F2-EDIT  F3-COPY                         F9-SHOW ARCHIV   F10-EXIT");
attroff(A_REVERSE);
refresh();
}




copy_file(char * name1, char * name2)
{
 int fd1,fd2,len;
 int a,b,tmpi;
 long size;
 char buffer[4096];
 struct stat stat;
 WINDOW *win;
 fd1=my_open(name1,O_RDONLY);
 fd2=open(name2,O_RDWR|O_TRUNC|O_CREAT,0666);
 my_stat(fd1,&stat);
 size=stat.st_size;
 b=0;
 tmpi=size/20;
win=newwin(3,40,10,20);
wattron(win,COLOR_PAIR(1));
werase(win);
wmove(win,0,5);
wprintw(win,"Copy %s to %s",name1,name2);
wmove(win,2,9);
wprintw(win,"[");
wmove(win,2,31);
wprintw(win,"]"); 
 while((len=my_read(fd1,buffer,4096)) > 0)
  {
    b+=len;
    for(a=0;a<(b/tmpi);a++);
      {
      wmove(win,2,10+a); 
    if(hascolor== 1) wattron(win,COLOR_PAIR(2));
    else
      wattron(win,A_REVERSE);
      wprintw(win,"X");
    if(hascolor==1) wattroff(win,COLOR_PAIR(2));
     else
      wattroff(win,A_REVERSE);
      }
      wrefresh(win);
   write(fd2,buffer,len);
 }
 my_close(fd1);
 close(fd2);
 delwin(win);
}  

 

