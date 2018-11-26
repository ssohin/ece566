struct row{

	int name;
	int (*read)();
	int (*write)();
	
};

struct row devTable[6];
