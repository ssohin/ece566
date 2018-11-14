#include "Relevant header file" 
//I think we should organize these header files at some point

struct Sensor { 

	char name[16]; //used for differentiating one sensor from another. 
					//Begins with "/" for future compatibility with the MQTT writing
	
	int (*open)(); //a function pointer points to the start of executable code. 
					//In this case, a generic open, which returns an integer
					//For the sensor objects being implemented, (*open)() will put the relevant task onto a shared file table
					//I think using the file table will be helpful in the future, and will write about that on "filetable.txt"
					//This doesn't need any input parameters, but we can add some if we want
	
	int (*read)(); //This function pointer points to the object's reading function, which
					//for DHT could be either readDHT() or DHT_task depending on how I'm interpreting that.
					//I think readDHT() would be an easier solution, at least as a proof-of-concept
					//Once again, I don't think this needs any input parameters but can add some if you want.
					
	
	//write(), ioctl(), and close() all have similar declarations, 
	//and they need implementations at some point for each sensor
} devices[] = {
	{"/temp", &dht_open, &dht_read}; 
	//this creates an array of the type Sensor (currently of size 1) with predefined name and functions	
	//the (*open)() is now pointing to the address of dht_open, same with read()
}