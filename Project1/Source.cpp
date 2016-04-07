#include <stdio.h>          // printf
#include <mpi.h>
#include <stdlib.h>         
#include <time.h>   
#include <math.h>

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	

	int rank, size;
	double starttime=0, endtime=0;
	float time_status[4] ;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	FILE *basefile;

	printf("-------------- proces %d z %d -------------- \n", rank, size);

	/*wczytanie pliku*/
	starttime = MPI_Wtime();
		if (basefile = fopen("wektory.txt", "r") == NULL) {
			printf("Nie mogê otworzyæ pliku test.txt do zapisu!\n");
			exit(1);
		}
		else{
			basefile = fopen("wektory.txt", "r");
		}

		float x, y, z;
		/*zliczenie wierszy*/
		int vector_count = 0;
		while (fscanf(basefile, "%f %f %f", &x, &x, &z) > 0) {
			vector_count++;
		}
		rewind(basefile);


		/*allokacja tablicy na wektory*/
		float **vectors_array;
		vectors_array = (float**)malloc(vector_count*sizeof(float *));
		for (int i = 0; i < vector_count; i++){
			vectors_array[i] = (float*)malloc(3 * sizeof(float));
		}


		/*wczytanie wektorów do tablic*/
		printf("wczytane wektory : \n");
		for (int i = 0; i < vector_count; i++){
			fscanf(basefile, "%f %f %f", &vectors_array[i][0], &vectors_array[i][1], &vectors_array[i][2]);

			printf("%f %f %f  \n", vectors_array[i][0], vectors_array[i][1], vectors_array[i][2]);
		}
	endtime = MPI_Wtime();
	time_status[0] = endtime - starttime;


	printf("liczba wektorow ogolnie - %d \n\n", vector_count);


	/*dekompozycja*/
	rewind(basefile);

	int from = 0;
	int to = 0;
	
	int my_from = 0;
	int my_to = 0;

	int ilosc = vector_count / size;
	int rest = vector_count%size;


	if (rank == 0) {
		if (rank < rest){
			ilosc++;
		}
		my_to = from + ilosc;
		to = my_to;
		
		
		for (int i = 1; i < size; i++) {

			ilosc = vector_count / size;
			from = to;
			if (i < rest){
				ilosc++;
			}

			to = from + ilosc;
			MPI_Send(&from, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(&to, 1, MPI_INT, i, i, MPI_COMM_WORLD);
		}
	}
	else {

		MPI_Recv(&my_from, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&my_to, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	int vectors_per_process = my_to - my_from;
	printf("liczba wektorów w procesie %d. Wektory od %d do %d\n", vectors_per_process, my_from, my_to - 1);



	float sr_l = 0;
	float sr_r[3] = { 0, 0, 0 };

	starttime = MPI_Wtime();
	if (rank<vector_count){
		int dirty_from = my_from;
		printf("Wektory w procesie\n");
		for (int i = 0; i < vectors_per_process; i++){
			printf("%f %f %f\n", vectors_array[dirty_from][0], vectors_array[dirty_from][1], vectors_array[dirty_from][2]);

			sr_l = sr_l+ sqrt(pow(vectors_array[dirty_from][0], 2) + pow(vectors_array[dirty_from][1], 2) + pow(vectors_array[dirty_from][2], 2));
			for (int j = 0; j < 3; j++){
				sr_r[j] = sr_r[j] + vectors_array[dirty_from][j];
			}
			dirty_from++;
		}

		sr_l = sr_l / vectors_per_process;
		for (int j = 0; j < 3; j++){
			sr_r[j] = sr_r[j] / vectors_per_process;
		}
		endtime = MPI_Wtime();
		time_status[1] = endtime - starttime;

		printf("sr dlugosc %f \n", sr_l);
		printf("sr wektor  %f %f %f\n", sr_r[0], sr_r[1], sr_r[2]);
	}

		/*mpi reduce*/
		float all_l = 0;
		float all_r[3] = { 0, 0, 0 };
		starttime = MPI_Wtime();
		MPI_Reduce(&sr_l, &all_l, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		all_l = all_l / size;
		if (size>vector_count){
			all_l = all_l  / vector_count;
		}
		else{
			all_l = all_l / size;
		}
	

		MPI_Reduce(&sr_r, &all_r, 3, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

		endtime = MPI_Wtime();
		time_status[2] = endtime - starttime;

		for (int j = 0; j < 3; j++){
			if (size>vector_count){
				all_r[j] = all_r[j] / vector_count;
			}
			else{
				all_r[j] = all_r[j] / size;
			}
		}
		
		
		
		FILE *time;

		time = fopen("time.txt", "a");
		fprintf(time, "Timings(proc %d)\n", rank);
		fprintf(time, "readData   %f\n",  time_status[0]);
		fprintf(time, "ProcessData  %f\n", time_status[1]);
		fprintf(time, "ReduceResults  %f\n",  time_status[2]);
		time_status[3] = time_status[0] + time_status[1] + time_status[2];
		fprintf(time, "total  %f\n\n\n",  time_status[3]);

		float time_all[4];
		MPI_Reduce(&time_status, &time_all, 4, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);


		if (rank == 0){
			printf("sredni dlugosc wektoru calosci %f\n", all_l);
			printf("sredni wektor %f %f %f \n", all_r[0], all_r[1], all_r[2]);

			fprintf(time, "total timing  \n");
			fprintf(time, "readData   %f\n", time_all[0]);
			fprintf(time, "ProcessData  %f\n", time_all[1]);
			fprintf(time, "ReduceResults  %f\n", time_all[2]);
			time_all[3] = time_all[0] + time_all[1] + time_all[2];
			fprintf(time, "total  %f\n\n\n", time_all[3]);
		}

		fclose(time);


		
			

			
		

		
	


	printf("---------------------------- \n");

	fclose(basefile);
	MPI_Finalize();
	return 0;
}