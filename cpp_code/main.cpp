#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "jobProcessor.h"

int main(int argc, char *argv[])
{
	double initial_temperature = 50;
	double alpha_warming = 1.5;
	double alpha_cooling = 0.9;
	int cooling_length = 900;
	double warming_threshold = 0.95;
	int max_moves_without_improvement = 100000;

	double totaltime;

	//std::fstream inputFile;
	if (argc < 3 || argc > 5)
	{
		std::cout << "Wrong number of arguments!" << std::endl;
		std::cout << "Use: " << argv[0] << " -t/-b <input_file_name> [number of jobs to process]" << std::endl;
		return -1;
	}

	std::string file_name(argv[2]);
	if (argv[1][1] != 't' && argv[1][1] != 'b')
	{
		std::cout << "Input file format not recognised." << std::endl;
		std::cout << "Use: -t for Taillard or -b for Orlib" << std::endl;
		return -1;
	}
	else
	{
		JobProcessor *jobProcessor = new JobProcessor();

		int selected_jobs = -1; //number of jobs to process. -1 if we want to read all jobs

		if (argc == 4) //if argument is not passed then read all jobs
		{
			selected_jobs = atoi(argv[3]);
		}

		if (argv[1][1] == 'b') //import Orlib
		{
			jobProcessor->importOrlib(file_name, selected_jobs);
		}
		else if (argv[1][1] == 't') //import Taillard
		{
			jobProcessor->importTaillard(file_name, selected_jobs);
		}
		jobProcessor->printData(); //printing our input data

		//solve the jobshop problem using simulated annealing meta-heuristics
		totaltime = jobProcessor->solveUsingSA(initial_temperature, alpha_warming, alpha_cooling,
												 cooling_length, warming_threshold, max_moves_without_improvement);

		char buffer[] = "../results.txt";	//saving the results of solving this instance in results.txt file.
		FILE *results;
		results = fopen(buffer, "a");
		fprintf(results, "Instance: %s %s; Jobs: %d; Machines: %d; Time: %f; Result: %d", argv[1], argv[2], jobProcessor->getJobCount(), jobProcessor->getMachineCount(), totaltime, jobProcessor->getCmax());
		fprintf(results, "\n");

		if (fclose(results) != 0)
		{
			printf("Error occurred while closing the file %s!\n", buffer);
		}

		delete jobProcessor;

		return 0;
	}
}