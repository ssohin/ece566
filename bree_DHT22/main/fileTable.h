#ifndef FILETABLE_SEEN
#define FILETABLE_SEEN

struct row{

	int name;
	int (*read)();
	int (*write)();
	
};
int rowsInUse = 0;
struct row devTable[6];

#endif /* FILETABLE_SEEN */
