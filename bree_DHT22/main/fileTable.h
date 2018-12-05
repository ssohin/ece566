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
int signature[6] = {0,0,0,0,0,0}; //devTable[i] has its signature put into signature[i]. Values in signature[] range from -2 to +2 to represent VERY LOW, LOW, NORMAL, HIGH, and VERY HIGH, respectively.

int passFlag = 0; //check if password has been implemented. If not, don't print anything to screen

#endif //FILETABLE_SEEN
