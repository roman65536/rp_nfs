#define OPEN	1
#define CLOSE 	2
#define READDIR 3
#define READ    4
#define WRITE	5
#define MKDIR	6
#define RM	7
#define LSEEK   8
#define STAT	9


struct mydir {
		int d_ino;
		char d_name[256];
	     };

struct mystat {
		int dev;
		int ino;
		int mode;
		int nlink;
		int uid;
		int gid;
		int rdev;
		int size;
		int atime;
	      };


struct msg {
		int opcode;	/* opcodes above */
		int len;	/* len of requested data */
		int ret;	/* return values from server */
		int fd;		/* file descriptor */
	   union {
		struct mydir dir;
		struct mystat mstat;
		char name[256];
		char data[256];
		int pos;
	       }u;
	   };



