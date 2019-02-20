#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "jobProcessor.h"

int main(int argc, char *argv[])
{
    double initial_temperature = 50; //Range: 50 100
    double alpha_warming[5] = {0.9, 1.0, 1.5, 2.0}; //Range: 0.9, 1.0, 1.5, 2.0
    double j;
    double alpha_cooling = 0.85;      // Range: 0.85 0.9
    int cooling_length = 300;        //Range: 300 600 900
    double warming_threshold = 0.85; //Range: 0.85 0.9 0.95
    int max_moves_without_improvement = 100000;

    double best_initial_temperature;
    double best_alpha_warming;
    double best_alpha_cooling;
    int best_cooling_length;
    double best_warming_threshold;
    int best_max_moves_without_improvement;

    int cmax;
    int best_cmax = 120000;

    JobProcessor *jobProcessor = new JobProcessor();

    jobProcessor->importTaillard("../instances/tailard/tai20.txt", -1); //tuning on instance tai20
    
    for (double i = initial_temperature; i < 101; i = i + 50)
    {
        for (int q = 0; q < 4; q++)
        {
            j = alpha_warming[q];
            for (double k = alpha_cooling; k < 0.91; k = k + 0.05)
            {
                for (int l = cooling_length; l < 901; l = l + 300)
                {
                    for (double m = warming_threshold; m < 0.96; m = m + 0.05)
                    {
                        jobProcessor->solveUsingSA(i, j, k, l, m, max_moves_without_improvement);
                        int cmax = jobProcessor->getCmax();
                        printf("%d, %f, %f, %f, %d, %f, %d\n", cmax, i, j, k, l, m, max_moves_without_improvement);
                        if (cmax < best_cmax)
                        {
                            best_cmax = cmax;
                            best_initial_temperature = i;
                            best_alpha_warming = j;
                            best_alpha_cooling = k;
                            best_cooling_length = l;
                            best_warming_threshold = m;
                            best_max_moves_without_improvement = max_moves_without_improvement;
                        }
                    }
                }
            }
        }
    }

    printf("Best cmax: %d, best parameters: %f %f %f %d %f %d \n", best_cmax, best_initial_temperature, best_alpha_warming, 
    best_alpha_cooling, best_cooling_length, best_warming_threshold, best_max_moves_without_improvement);

    delete jobProcessor;

    return 0;
}