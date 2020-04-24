#include <mpi.h>
#include <iostream>
#include <vector>
#include <iterator>
using namespace std;


void taskFirst(int rank, int size, MPI_Group world_group);
void taskSecond(int rank, int size, MPI_Group world_group, MPI_Comm comm_world);

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Comm comm_world;
	MPI_Group world_group;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_dup(MPI_COMM_WORLD, &comm_world);
	MPI_Comm_group(comm_world, &world_group);

	taskFirst(rank, size, world_group);
	taskSecond(rank, size, world_group, comm_world);


	MPI_Group_free(&world_group);
	MPI_Comm_free(&comm_world);
	MPI_Finalize();
	return 0;
}

void taskFirst(int rank, int size, MPI_Group world_group)
{
	if (size >= 4)
	{
		MPI_Group groupOne, groupTwo, groupDifference;
		vector <int> ranksFirst = {0, 1, 3 };
		vector <int> ranksSecond = { 1, 2, 3 }; 
		int groupRank;
		

		MPI_Group_incl(world_group, ranksFirst.size(), ranksFirst.data(), &groupOne);//group1
		MPI_Group_incl(world_group, ranksSecond.size(), ranksSecond.data(), &groupTwo);//group2
		MPI_Group_difference(groupOne, groupTwo, &groupDifference);//difference
		MPI_Group_rank(groupDifference, &groupRank);
		if (rank == 0) {
			cout << "TASK1: Difference:" << endl;
			cout << "RanksOne:" << endl;
			copy(ranksFirst.begin(), ranksFirst.end(), ostream_iterator<int>(cout, " "));
			cout << endl;
			cout << "RanksTwo:" << endl;
			copy(ranksSecond.begin(), ranksSecond.end(), ostream_iterator<int>(cout, " "));
		}
		if (groupRank >= 0) {
			cout << endl;
			cout << "Rank =  " << rank << " include in groupDifference " << groupRank << endl;
		}
		MPI_Group_free(&groupOne);
		MPI_Group_free(&groupTwo);
		MPI_Group_free(&groupDifference);
	}
}

void taskSecond(int rank, int size, MPI_Group world_group, MPI_Comm comm_world) {

	if (size >= 12) {

		MPI_Group group; 
		MPI_Comm comm;
		vector<int> groupsRanks = { 0, 1, 2, 4, 6, 7, 8, 10, 11 };
		int group_size, group_rank,message;

		MPI_Group_incl(world_group, groupsRanks.size(), groupsRanks.data(),&group);
		MPI_Comm_create(comm_world, group, &comm);

		
		MPI_Group_size(group, &group_size);
		MPI_Group_rank(group, &group_rank);
		if (group_rank >= 0)
		{
			cout << "Rank = " << rank << " group Rank = " << group_rank << endl;
			
			if (group_rank == group_size - 1)
			{
				message = group_size;
				cout << endl;
				cout << "Rank  = " << rank << " group Rank = " << group_rank << " send message = " << message << endl;
			}
			MPI_Bcast(&message, sizeof(message), MPI_INT, group_size - 1, comm);
			MPI_Group_rank(group, &group_rank);
			if (group_rank % 4 == 0)
			{
				cout << endl;
				cout << "Rank = " << rank << " group Rank = " << group_rank << " received message = " << message;
				
			}
		}
		MPI_Group_free(&group);
	}
}
