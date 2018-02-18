#include "bigHeader.h"
#include "mpich_sample.h"

int main(int argc , char **argv){
	//mpich_check(&argc,&argv);
	send_custom_object(&argc,&argv);
	return 0;
}