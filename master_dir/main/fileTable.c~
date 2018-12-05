//This is a shared data structure to be used by whatever overarching file we 
//have for calling suspend() and resume() and doing other important things

struct row{ //each entry in the fileTable will be a row

char name[16];
int valid = 0; //set to 1 when a sensor put onto fileTable, set to 0 when removed

int (*read)(); //each entry in the file table needs their information stored
	
}

struct fileTable{ //this is just an idea of how it would be implemented
	
	int rowsInUse = 0;
	struct row Table[6]; //fileTable can hold up to 6 entries. Idk how many sensors we have
} 

//Need to implement a straightforward "add something to Table[]" function