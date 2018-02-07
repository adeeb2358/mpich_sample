#include "bigHeader.h"
#include "mpich_sample.h"

void mpich_check(int *argc , char ***argv){
	MPI_Init(argc,argv);

	/*
		the program comes here
	*/
	int size = 0;
	int rank = 0;

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if(rank == 0){
		int n;
		std::cout <<"Enter a number\n";
		std::cin >> n;

		for(size_t i = 1; i < size; i++){
			MPI_Send(&n,1,MPI_INT,i,0,MPI_COMM_WORLD);
		}

	}else{
		int n;
		MPI_Recv(&n,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		std::cout << n <<"^" << rank << "=" << pow(n,rank) << "\n";
	}

	MPI_Finalize();
	return;
}