#include "bigHeader.h"
#include "mpich_sample.h"

void mpich_check(int *argc , char ***argv){
	MPI_Init(argc,argv);

	/*
		the program comes here
	*/

	MPI_Finalize();
	return;
}