
#define FILE 1
#define DIR 2

struct dir_e {
	char name[80];
	long size;
	int mode;
	struct dir_e *next;
	struct dir_e *prev;
	} dir_e;

struct dir_e *first;
struct dir_e *last;
 /*struct dir_e *select; */

