struct row{

	char* name[16];
	int (*read)();
	int (*write)();
	
};

struct row devTable[6];