#ifndef FILETABLE_SEEN
#define FILETABLE_SEEN

struct row{

	int name;
	int (*read)();
	int (*write)();
	
};

struct row devTable[6];

#endif /* FILETABLE_SEEN */
