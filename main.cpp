#include "bigHeader.h"
#include "mpich_sample.h"

int main(int argc , char **argv){
	//mpich_check(&argc,&argv);
	//send_custom_object(&argc,&argv);
	boost_serialize_check(&argc,&argv);
	return 0;
}