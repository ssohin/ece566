#include "DHT_main.c"
#include "fileTable.h"
#include "light_main.c"
#include "qExample.c"

void app_main(){
	temp_open();
	humid_open(); //the example code in class defined the table as a set of constants. We implemented it this way to make it easier to change sensors, but same idea.
	light_open();
	q_app_main();
	printf("Test");
	//to test that the devTable's function pointers work properly
	devTable[0].write();
	devTable[1].write();
	devTable[2].write();
}
