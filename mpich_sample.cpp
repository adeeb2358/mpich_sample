#include "bigHeader.h"
#include "mpich_sample.h"


void send_custom_object(int *argc , char ***argv){
	

	MPI_Init(argc,argv);

	int size = 0;
	int rank = 0;

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if(rank == 0){
		/*
		data for passing to all of the process
		*/
		nlohmann::json Person;
		Person["name"]   = "adeeb";
		Person["age"]    = 24;
		Person["status"] = true;
		auto person_string = Person.dump();
		/*
			End of data
		*/

		for(size_t i = 1; i < size; i++){
			MPI_Send(person_string.c_str(),person_string.size()+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
		}
	}else{
		
		MPI_Status status;
		MPI_Probe(0,0,MPI_COMM_WORLD,&status);
		
		int count;
		MPI_Get_count(&status,MPI_CHAR,&count);
		std::cout << "we have" << count << std::endl;
		
		
		char  *buffer = new char[count];
		MPI_Recv(buffer,count,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
		
		std::cout << "we got" << buffer<< std::endl;
		std::string person_string(buffer);
		
		nlohmann::json Person = nlohmann::json::parse(buffer);
		std::cout << "Hello," << Person["name"] << " you are " << Person["age"] <<" years old" <<"\n";
		delete[] buffer;
	}

	MPI_Finalize();

}


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