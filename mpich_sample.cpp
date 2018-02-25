#include "bigHeader.h"
#include "mpich_sample.h"

struct Person{
	std::string name;
	int age;

	template <typename Ar> void serialize(Ar& ar,const unsigned int version){
		ar &name;
		ar &age;
	}	
};

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

//boost mpi sample
void boost_serialize_check(int *argc , char ***argv){
	namespace mpi = boost::mpi;

	mpi::environment env;
	mpi::communicator world;

	if(world.rank() == 0){
		
		Person p;
		p.name = "Adeeb";
		p.age  = 10;

		for(size_t i = 1; i < world.size(); i++){
			world.send(i,0,p);
		}
	}
	else{
		Person p;
		world.recv(0,0,p);
		std::cout << "hello," << p.name << std::endl;
	}
}

/*
	communitcation types in MPI
	
	point to point communcation
	Prcoess A sends message to Process B

	Stantard mode
	Retuns as soon as message is sent

	Synchronouus
	Returns only when the exchange is complete handshake

	Buffered: returns immediately buffer the mesage
	
	Ready:
	Initiated only when recieving is setup
	Finished immediately

	MPI_{I<immediate non blocking>}[{R<Ready>,S<Synchronous>,B<Buffered>]Send
	MPI_{I}Recv
	========================================================
	
	Collective communication
	Process broadcasts message to anyone willing to listen 

*/

void communication_types_mpi(int *argc , char ***argv){
	
	int size = 0;
	int rank = 0;
	
	MPI_Init(argc,argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		int value = 42;
		for(size_t i = 1; i < size; i++){
			std::cout << "Ready to send" << rank << "-->" << i << std::endl;
			//MPI_Send(&value,1,MPI_INT,i,0,MPI_COMM_WORLD);
			MPI_Ssend(&value,1,MPI_INT,i,0,MPI_COMM_WORLD);
			std::cout << "Data send |" << rank << "-->" << i << std::endl;
		}
	
	}else{
		int value = -1;
		MPI_Recv(&value,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		std::cout << rank << " recieved from 0 the value " << value << std::endl;
	}

	MPI_Finalize();

	return;
}

/*
	collective communication
	broadcast procudure
*/

void collective_communication(int *argc , char ***argv){
	int size,rank;

	MPI_Init(argc,argv);

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	int value = 42;
	MPI_Bcast(&value,1,MPI_INT,0,MPI_COMM_WORLD);

	std::cout << "Rank" << rank << "recieved from 0 the value" << value << std::endl;
	MPI_Barrier(MPI_COMM_WORLD);
	std::cout << "Rank " << rank << " is done working" << std::endl;

	MPI_Finalize();


	return;
}

/*
	reduce function
*/

int reduce(int *argc , char ***argv){
	int size,rank;

	MPI_Init(argc,argv);

	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	const int itemsPerProcess = 10;
	const int count           = size * itemsPerProcess;
	int *data                 = new int[count];

	if(rank == 0){
		for(size_t i = 0; i < count ; i++){
			data[i] = rand() % 10;
		}
	}

	int *localData = new int[itemsPerProcess];
	MPI_Scatter(
		data,itemsPerProcess,MPI_INT,
		localData,itemsPerProcess,MPI_INT,0,MPI_COMM_WORLD
	);

	int localSum = 0;
	for(size_t i = 0; i < itemsPerProcess; i++){
		localSum += localData[i];
	}

	delete[] localData;

	int globalSum = 0;
	MPI_Reduce(&localSum, &globalSum, 1 ,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

	if(rank == 0){
		std::cout << "Total Sum = " << globalSum << std::endl;
		delete[] data;
	}

	MPI_Finalize();

	return 0;
}

/*
	collective communication
	reduce function

*/

void check_reduce(int *argc , char ***argv){
	std::random_device rd{};
	std::srand(rd());
	int reduce_result = reduce(argc,argv);
	return ;
}

