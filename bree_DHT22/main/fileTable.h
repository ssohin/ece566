#ifndef FILETABLE_SEEN
#define FILETABLE_SEEN

struct row{

	int name;
	int (*read)();
	int (*write)();
	void (*close)();
	void (*open)();
};
int rowsInUse = 0;
struct row devTable[6];
int toComm = -1234; //the data being sent to comm

#endif /* FILETABLE_SEEN */
