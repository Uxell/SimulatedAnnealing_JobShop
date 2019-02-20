#include "jobProcessor.h"

#define WARMING 1
#define COOLING 0
#define MAX_ATTEMPTS 1000000

JobProcessor::JobProcessor()
{
    this->machine_count = 0;
    this->job_count = 0;
    this->operations_number = 0;
    this->vertex_weights.push_back(0);
}

JobProcessor::~JobProcessor()
{
    this->jobs.clear();
}

void JobProcessor::setMachineCount(int machineCount)
{
    this->machine_count = machineCount;
}

void JobProcessor::setJobCount(int jobCount)
{
    this->job_count = jobCount;
}

int JobProcessor::getMachineCount()
{
    return this->machine_count;
}

int JobProcessor::getJobCount()
{
    return this->job_count;
}

void JobProcessor::addJob(Job *job)
{
    this->jobs.push_back(job);
}

Job *JobProcessor::getJob(int jobId)
{
    return this->jobs[jobId];
}

bool JobProcessor::importOrlib(std::string filename, int selected_jobs)
{
    std::ifstream input;                             //create an input file stream
    input.open(filename.c_str(), std::ifstream::in); //open file with input stream

    if (input.is_open())
    {
        int temp = 0, i = 0, read_jobs = 0;

        std::string current_line;
        std::stringstream ss;

        std::getline(input, current_line); //reads first line with job and machine number

        ss << current_line;

        ss >> temp; //reads job number
        if (selected_jobs == (-1))
        {
            this->setJobCount(temp);
        }
        else
        {
            this->setJobCount(selected_jobs);
        }

        ss >> temp; //reads machine number
        this->setMachineCount(temp);
        this->machines.resize(temp);

        ss.clear(); //cleans string stream

        while (std::getline(input, current_line) && read_jobs < this->getJobCount()) //while there is a next line
        {
            Job *j = new Job(); //create a new job (new line = new job)
            Task *t;            //create task placeholder
            temp = 0;           //clears temp variable

            ss << current_line; //reads a line

            while (ss >> temp) //while line is not empty, read a token
            {
                if (i % 2 == 0) //if even element then it is a machine number of the task
                {
                    t = new Task(); //create task and set currently known data
                    t->setMachineId(temp);
                    operations_number++;
                    this->machines[temp].push_back(operations_number);
                }
                else //if odd element then it is a processing duration of the task (we already created the task, since machine numbers are always first)
                {
                    t->setDuration(temp);
                    j->addTask(t);
                    vertex_weights.push_back(temp);
                }
                i++; //increment i, to distinguish between machine number and duration
            }
            ss.clear();      //cleans string stream
            this->addJob(j); //adds read job to the job vector
            read_jobs++;
        }
        input.close(); //closing our input file stream
        return true;
    }
    else
    {
        std::cout << "Error occurred while opening the file" << std::endl;
        return false;
    }
}

bool JobProcessor::importTaillard(std::string filename, int selected_jobs)
{
    std::ifstream input;                             //create an input file stream
    input.open(filename.c_str(), std::ifstream::in); //open file with input stream
    if (input.is_open())
    {
        int job_num = 0, task_id = 0, temp = 0;
        Task *t; //create a task placeholder
        Job *j;  //create a job placeholder

        std::string current_line;
        std::stringstream ss;

        std::getline(input, current_line); //reads first line with job and machine number and other not important(to us) data

        ss << current_line;

        ss >> temp; //reads job number
        if (selected_jobs == (-1))
        {
            this->setJobCount(temp);
        }
        else
        {
            this->setJobCount(selected_jobs);
        }

        ss >> temp; //reads machine number
        this->setMachineCount(temp);
        this->machines.resize(temp);

        while (ss >> temp) //goes to the end of line
            ;

        ss.clear();

        bool times = true; //variable which allows us to distinguish between machine number and processing duration lines

        while (std::getline(input, current_line)) //while there is a next line
        {

            if (current_line.find("Times") != std::string::npos) //if we encounter a "Times" line, we set times to true, so we only create new jobs and tasks and fill tasks with processing durations
            {
                times = true;
                continue;
            }

            if (current_line.find("Machines") != std::string::npos) //if we encounter a "Machines" line, we iterate over previously created jobs and fill the task's machine number
            {
                times = false;
                job_num = 0; //we clear the job_num so we can iterate over previously created jobs
                continue;
            }

            if (times && job_num < this->getJobCount()) //if times is true, then we have to create a new Job
            {
                j = new Job(); //create a new job (new line = new job)
            }

            ss << current_line;
            if (job_num < this->getJobCount())
            {
                while (ss >> temp) //while line is not empty, read a token
                {
                    if (times) //if times is true, create new tasks and fill in currently known data
                    {
                        t = new Task();
                        t->setDuration(temp);
                        j->addTask(t);
                        vertex_weights.push_back(temp);
                    }
                    else //if times is not true, just fill in the task's machine id
                    {
                        this->getJob(job_num)->getTask(task_id)->setMachineId(temp - 1);
                        operations_number++;
                        this->machines[temp - 1].push_back(operations_number);
                    }
                    task_id++;
                }
                if (times) //if times is true, then we have to add our job
                {
                    this->addJob(j);
                }
            }
            while (ss >> temp) //goes to the end of line
                ;
            ss.clear();
            job_num++;
            task_id = 0;
        }

        return true;
    }
    else
        return false;
}

void JobProcessor::printData()
{
    /*
        Prints data imported with either of methods
    */
    std::cout << "Imported data with " << this->getJobCount() << " jobs and " << this->getMachineCount() << " machines" << std::endl;
    for (int i = 0; i < job_count; i++)
    {
        for (int j = 0; j < (this->getJob(0)->getTaskCount()); j++)
        {
            Task *task = this->getJob(i)->getTask(j);
            std::cout << task->getMachineId() << " " << task->getDuration() << " ";
        }
        std::cout << std::endl;
    }
}

void JobProcessor::createGraph(void)
{
    /*
        Creates disjunctive graph using imported data
	*/
    Graph g(operations_number + 2); // number of operations + source + sink

    int source = 0;
    int sink = operations_number + 1; // last vertex

    int current_operation_vertex = 1; // first vertex = first task

    int jobs_number = this->getJobCount(); // Number of jobs
    for (int i = 0; i < jobs_number; i++)  // For every job
    {
        int job_operations = this->getJob(i)->getTaskCount(); // Calculate number of operations/tasks
        g.addArc(0, current_operation_vertex, 0);            // add arc from source to first task of the job (distance == 0)
        for (int j = 0; j < job_operations - 1; j++)          // For every task (of job i) except the last one
        {
            // Add arc from j task to j+1 task with distance equal to time of the task
            g.addArc(current_operation_vertex, current_operation_vertex + 1, this->getJob(i)->getTask(j)->getDuration());
            current_operation_vertex++;
        }
        // Add arc from last task to sink with distance equal to the time of the last task
        g.addArc(current_operation_vertex, sink, this->getJob(i)->getTask(job_operations - 1)->getDuration());

        current_operation_vertex++;
    }

    for (int i = 0; i < machines.size(); i++) // For every machine create a clique
    {
        Machine m = machines[i]; // m is a vector of vertices of the operations which are done on machine i

        int no_ops_machine = m.size(); // Number of operations on machine i

        vector<int> l; // Vector containing lengths of the operations on machine i

        for (int j = 0; j < no_ops_machine; j++) // for every operation
        {
            l.push_back(vertex_weights[m[j]]); // get length of exiting arcs
        }

        g.createAcyclicClique(m, l); // create clique (it has to be cycle-free!)
    }
    this->graph = g;
}

int JobProcessor::getCmax(void)
{
    /*
        Returns total duration, which simply means path from source to sink of the maximum length
	*/
    return graph.maxDistances().back();
}

vector<int> JobProcessor::selectArc(deque<int> criticalPath)
{
    /*
        Randomly selects arc connecting two vertices from critical path
    */
    int random_vector = (rand() % (criticalPath.size() - 2)) + 1;
    vector<int> selected_arc;
    selected_arc.resize(2);
    selected_arc[0] = criticalPath[random_vector];
    selected_arc[1] = criticalPath[random_vector + 1];
    return selected_arc;
}

bool JobProcessor::successChance(int cmax, int new_cmax, double temperature)
{
    double chance = ((double)rand() / (RAND_MAX));
    double result = exp((cmax - new_cmax) / (temperature));
    if (chance <= result){
        return true;
    }
    else
        return false;
}

vector<int> JobProcessor::getStartTimes(void)
{
    /*
        Returns start times of all operations, which means calculating max distances from source to every vertex. 
        We exclude first distance which is always 0.
	*/
    vector<int> v = graph.maxDistances();
    v.erase(v.begin(), v.begin() + 1);
    v.pop_back();
    return v;
}

vector<int> JobProcessor::getJobLenghts(void)
{
    /*
        Returns vector containing duration of every job.
    */
    vector<int> v;
    for (int i = 0; i < this->jobs.size(); i++)
    {
        v.push_back(this->getJob(i)->getTaskCount());
    }
    return v;
}

void JobProcessor::printStartTimes(void)
{
    /*
        Prints start times of every task, where every row is a job
    */
    vector<int> times = getStartTimes();
    vector<int> job_lenghts = getJobLenghts();
    int current_job_pos = 0;
    int current_job = 0;
    for (int i = 0; i < times.size(); i++)
    {
        if (current_job_pos == job_lenghts[current_job])
        {
            current_job++;
            current_job_pos = 0;
            printf("\n");
        }
        printf("%d ", times[i]);
        current_job_pos++;
    }
}

double JobProcessor::solveUsingSA(double initial_temperature, double alpha_warming, double alpha_cooling, int cooling_length, double warming_threshold, int max_moves_without_improvement)
{
    /*
        Solves the Job-Shop problem using Simulated Annealing meta-heuristic
    */

    srand(time(NULL));
    int mode = WARMING; //Warming / cooling
    double temperature = initial_temperature;

    list<bool> last_moves;
    const int last_moves_size = 2000;
    last_moves.resize(last_moves_size);
    std::fill(last_moves.begin(), last_moves.end(), false);

    int accepted_moves = 0;
    int accepted_moves_out_of_last_moves = 0;
    int moves_without_improvement = 0;

    int arc_select_attempts = 0;
    int no_cycle_attempts = 0;

    int task_count_in_job = ((operations_number) / (this->getJobCount()));
    bool cant_change_path = false;
    bool cycle_created = false;
    createGraph();

    int cmax;
    int new_cmax;

    deque<int> crit_path;
    vector<int> random_arc;
    random_arc.resize(2);

    bool time_exceeded = false;

    struct timespec start, stop;
    double totaltime = 0.0;
    clock_gettime(CLOCK_REALTIME, &start);
    while (moves_without_improvement < max_moves_without_improvement && !time_exceeded) // while time is not exceeded and we still do improve a bit
    {
        crit_path = graph.criticalPath(operations_number + 1); //get critical path of the created graph
        cmax = getCmax();                                      //get cmax
        if (graph.getCycleExists())                           //detect if there is no cycle at the beginning
        {
            cout << "Wrong data: cycle detected at the beginning!" << endl;
            return -1;
        }

        no_cycle_attempts = 0;
        do //do while we do not create a cycle by inverting the arc
        {
            arc_select_attempts = 0;
            cycle_created = false;
            do //select random arc that is not beginning in source and not ending in sink, and is not between two neighbouring tasks
            {
                random_arc = selectArc(crit_path);
                ++arc_select_attempts;
                if (arc_select_attempts > MAX_ATTEMPTS)
                {
                    cant_change_path = true;
                    break;
                }
            } while (random_arc[0] == 0 || random_arc[1] == operations_number + 1 || ((random_arc[1] - random_arc[0] == 1) && (random_arc[0] % task_count_in_job != 0)));
            if (cant_change_path)
            { //if there is no arc in critical path that we can alter then finish
                break;
            }

            graph.invertArc(random_arc[0], random_arc[1]); //invert the direction of the found arc
            graph.setArcLength(random_arc[1], random_arc[0], vertex_weights[random_arc[1]]);

            getCmax(); // calculate cmax, because it uses topological sort, which detects the cycle, which later can be checked with "getCycleExists()"
            ++no_cycle_attempts;

            if (graph.getCycleExists()) //if we created a cycle by inverting, revert the arc and try again
            {
                cycle_created = true;
                graph.invertArc(random_arc[1], random_arc[0]); //revert the arc back
                graph.setArcLength(random_arc[0], random_arc[1], vertex_weights[random_arc[0]]);
            }
            if (no_cycle_attempts > MAX_ATTEMPTS)
            {
                cant_change_path = true;
                break;
            }
        } while (cycle_created);
        if (cant_change_path) //if after many attempts we cant alter the critical path, then finish
        {
            break;
        }

        new_cmax = getCmax(); //get cmax after inverting the arc

        if (new_cmax < cmax) //if new cmax is better
        {
            if (mode == WARMING) //if warming, add information about last accepted moves
            {
                last_moves.push_back(true);
                if (last_moves.front() == false)
                    accepted_moves_out_of_last_moves++;
                last_moves.pop_front();
            }
            accepted_moves++;
            cmax = new_cmax; //set cmax to new one
        }
        else if (new_cmax == cmax) //if new cmax is the same, accept the move (lack of improvement)
        {
            accepted_moves++;
            moves_without_improvement++;
            if (mode == WARMING) //if warming, add information about last accepted moves
            {
                last_moves.push_back(true);
                if (last_moves.front() == false)
                    accepted_moves_out_of_last_moves++;
                last_moves.pop_front();
            }
        }
        else //if new cmax is worse
        {
            if (successChance(cmax, new_cmax, temperature) == true) //if probability that it will succeed is more then random then accept
            {
                accepted_moves++;
                cmax = new_cmax;
                moves_without_improvement++;
                if (mode == WARMING) //if warming, add information about last accepted moves
                {
                    last_moves.push_back(true);
                    if (last_moves.front() == false)
                    {
                        accepted_moves_out_of_last_moves++;
                    }
                    last_moves.pop_front();
                }
            }
            else // else dont accept the move
            {
                moves_without_improvement++;
                if (mode == WARMING) //if warming, add information about accepted moves
                {
                    if (accepted_moves > cooling_length / 10) //adjust temperature (increase), if after 10% of cooling_length, we don't accept the move
                    {
                        accepted_moves = 0;
                        temperature += alpha_warming * initial_temperature;
                    }
                    last_moves.push_back(false); // add information about last (not)accepted moves
                    if (last_moves.front() == true)
                        accepted_moves_out_of_last_moves--;
                    last_moves.pop_front();
                }
                graph.invertArc(random_arc[1], random_arc[0]); //revert the arc back
                graph.setArcLength(random_arc[0], random_arc[1], vertex_weights[random_arc[0]]);
            }
        }
        if (mode == WARMING && (double)accepted_moves_out_of_last_moves / last_moves_size >= warming_threshold) //if warming and last accepted moves/last moves is bigger than threshold
        {
            accepted_moves = 0;
            mode = COOLING; //change the mode to cooling
        }
        else if (mode == COOLING && accepted_moves >= cooling_length) //if cooling and we exceed accepted cooling length
        {
            temperature *= alpha_cooling; //decrease temperature
            accepted_moves = 0;
        }

        clock_gettime(CLOCK_REALTIME, &stop);
        totaltime += (double)(stop.tv_sec - start.tv_sec) + 1.e-9 * (stop.tv_nsec - start.tv_nsec);

        //If time exceeded 5 minutes, end
        if (totaltime > 300.0)
            time_exceeded = true;

        clock_gettime(CLOCK_REALTIME, &start);
    }
    // std::cout << "Program ended, because : " << std::endl;
    // if (!(moves_without_improvement < max_moves_without_improvement))
    //     printf("Exceeded number of moves without improvement\n");
    // else if (time_exceeded)
    //     printf("Exceeded time\n");
    // else if (cycle_created)
    //     printf("Cycle in the graph created!.\n");
    // else if (cant_change_path)
    //     printf("Cannot alter the critical path anymore\n");

    printf("\nStart times: \n");
    printStartTimes();
    printf("\n \n");
    printf("Cmax: %d, Time in sec: %f\n", getCmax(), totaltime);   // FOR CALC

    return totaltime;
}