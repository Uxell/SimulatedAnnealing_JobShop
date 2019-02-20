# SimulatedAnnealing_Jobshop

SimulatedAnnealing_Jobshop is a Simulated Annealing heuristic implementation for JobShop-Solving problem.  
Complete description of the problem and the algorithm:  
https://medium.com/@gilewski.slawomir/using-simulated-annealing-in-job-shop-problem-solving-5d72232a2abe

## Installation

Download cpp_code from the repo and use command line to compile the program:
```
g++ job.cpp task.cpp graph.cpp jobProcessor.cpp main.cpp -o job-shop
```

## Usage

The program starts from the console using the command:
```
./job-shop <-t/-b> <path to the file containing the data> [number of the processed jobs]
```
Where -t or -b is the data input format:  
-t for Tailard data format  
-b for Orlib data format

The program prints the results in the following form:  length_of_the_scheduling\n                   
starting_times_of_the_next_task1_operations \n
starting_times_of_the_next_task2_operations \n
starting_times_of_the_next_task3_operations \n
…
starting_times_of_the_next_last_tast_operations \n

In other words, the number with the coordinates row "i", the column "j" indicates the moment of beginning the task "j" of the job "i".


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)
