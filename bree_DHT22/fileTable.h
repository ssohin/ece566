struct row{

	int name; //1 for temperature, 
	int (*read)();
	int (*write)();
	
};

struct row devTable[6];