#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

/*
#define MAX_TASKS 1000
#define MAX_WORKERS 10
#define TERMINATE_TASK 0
*/


//Task message struct
typedef struct {
    int task_id;
    int effort; //sleep time - task effort
} TaskMessage;

//worker completion struct
typedef struct {
    int worker_id;
    pid_t pid;
    int tasks_processed;
    int total_time;
} WorkerCompletionMessage;

mqd_t mq_task, mq_result;  //message que to distribute tasks / get results
int num_workers, num_tasks, queue_size;  //config from command line args


//function is used for the current timestamp in hour/min/second format, easier to adjust than using just time() if different format is needed
void get_timestamp(char *buffer) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 9, "%H:%M:%S", timeinfo);
}

//function definitions
void create_worker(int worker_id);
void process_task(int worker_id);
void send_results(int worker_id, int tasks_processed, int total_time);
void initialize_message_queues();
void clean_up();

//function so that the worker receives tasks over MQ and work is simulated through this
void worker_process(int worker_id) {
    process_task(worker_id);
    send_results(worker_id, 0, 0);
}

//function for the initialization of the message queue for the tasks and results
void initialize_message_queues() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = queue_size; //defines the maximum messages in the queue alloewd
    attr.mq_msgsize = sizeof(TaskMessage); //size of the message is the size of taskmessage
    attr.mq_curmsgs = 0;

    //taskqueue sends tasks to the workers
    mq_task = mq_open("/task_queue", O_CREAT | O_RDWR, 0666, &attr);
    if (mq_task == (mqd_t) -1) {
        perror("Task Queue creation failed");
        exit(1);
    }

    //result queue so worker results can be received
    attr.mq_msgsize = sizeof(WorkerCompletionMessage);    //message size is size of the workerCompletionMessage
    mq_result = mq_open("/result_queue", O_CREAT | O_RDWR, 0666, &attr);
    if (mq_result == (mqd_t) -1) {
        perror("Result Queue creation failed");
        exit(1);
    }
}

//after program is done cleanup is being started: Close task queue, result queue, remove task queue, remove result queue
void clean_up() {
    mq_close(mq_task);
    mq_close(mq_result);
    mq_unlink("/task_queue");
    mq_unlink("/result_queue");
}

//function that simulates the workers working on the tasks
void process_task(int worker_id) {
    TaskMessage task;
    int tasks_processed = 0;
    int total_time = 0;
    char timestamp[9];

    //tasks are being processes continuously until a termination signal is being received
    while (1) {
        //receive tasks
        if (mq_receive(mq_task, (char *) &task, sizeof(TaskMessage), NULL) == -1) {
            if (errno == EINTR) continue;  //signal interruption handling
            perror("Receiving task not possible, ERROR");
            exit(1);
        }

        //termination signal
        if (task.effort == 0) {
            break;
        }

        //simulates work by sleeping for the task effort time span
        sleep(task.effort);
        tasks_processed++; //increments task count
        total_time += task.effort; //saves time spent on task

        //print out worker status after receiving task
        get_timestamp(timestamp);
        printf("%s | Worker #%02d | Received task with effort %d\n", timestamp, worker_id, task.effort);
    }

    //send work completion message
    send_results(worker_id, tasks_processed, total_time);

    //print out status after terminating task
    get_timestamp(timestamp);
    printf("%s | Worker #%02d | Received termination task\n", timestamp, worker_id);
    exit(0);  //exit worker process
}

//function sends worker results back to the ventilator
void send_results(int worker_id, int tasks_processed, int total_time) {
    WorkerCompletionMessage result;
    result.worker_id = worker_id;
    result.pid = getpid();
    result.tasks_processed = tasks_processed;
    result.total_time = total_time;

    //sends result to the message queue
    if (mq_send(mq_result, (const char *) &result, sizeof(WorkerCompletionMessage), 0) == -1) {
        perror("Sending results not possible, ERROR");
        exit(1);
    }
}

//function that creates the worker processes
void create_worker(int worker_id) {
    pid_t pid = fork();    //new worker process
    if (pid == 0) {
        worker_process(worker_id);  //in the child process simulate worker activity
    } else if (pid > 0) {
        char timestamp[9];
        get_timestamp(timestamp);
        printf("%s | Worker #%02d | Started worker PID %d\n", timestamp, worker_id, pid);
    } else {
        perror("Forking error");
        exit(1);
    }
}

//function that is distributing tasks and waiting for worker results
void ventilator_process() {
    TaskMessage task;
    WorkerCompletionMessage result;
    char timestamp[9];

    //print out starting worker message after input from command line has been given (prints out how many workers, tasks, queue size)
    get_timestamp(timestamp);
    printf("%s | Ventilator | Starting %d workers for %d tasks and a queue size of %d\n",
           timestamp, num_workers, num_tasks, queue_size);

    //print out task distribution from ventilator
    get_timestamp(timestamp);
    printf("%s | Ventilator | Distributing tasks\n", timestamp);

    // Start by creating workers dynamically based on the number of workers specified
    for (int i = 1; i <= num_workers; i++) {
        create_worker(i);
    }

    //distribute / generate tasks
    for (int i = 0; i < num_tasks; i++) {
        task.task_id = i + 1;
        task.effort = rand() % 10 + 1;  //random effort between 1 and 10 by using rand

        //send task to queue for the workers
        if (mq_send(mq_task, (const char *) &task, sizeof(TaskMessage), 0) == -1) {
            perror("Error sending task");
            exit(1);
        }
        get_timestamp(timestamp);
        printf("%s | Ventilator | Queuing task #%d with effort %d\n", timestamp, task.task_id, task.effort);
    }

    //prints sending termination tasks to workers
    get_timestamp(timestamp);
    printf("%s | Ventilator | Sending termination tasks\n", timestamp);

    for (int i = 0; i < num_workers; i++) {
        task.task_id = 0;
        task.effort = 0; //effort must be 0 so task gets terminated
        if (mq_send(mq_task, (const char *) &task, sizeof(TaskMessage), 0) == -1) {
            perror("Error sending termination task");
            exit(1);
        }
    }

    //waiting for workers before results are being collected by ventilator
    get_timestamp(timestamp);
    printf("%s | Ventilator | Waiting for workers to terminate\n", timestamp);

    //collecting worker results
    for (int i = 0; i < num_workers; i++) {
        if (mq_receive(mq_result, (char *) &result, sizeof(WorkerCompletionMessage), NULL) == -1) {
            perror("Error receiving result");
            exit(1);
        }

        //prints worker completion with what worker processed how many tasks in x seconds and their exit + pid
        get_timestamp(timestamp);
        printf("%s | Ventilator | Worker %d processed %d tasks in %d seconds\n",
               timestamp, result.worker_id, result.tasks_processed, result.total_time);
        get_timestamp(timestamp);
        printf("%s | Ventilator | Worker %d with PID %d exited with status 0\n",
               timestamp, result.worker_id, result.pid);
    }


    for (int i = 0; i < num_workers; i++) {
        wait(NULL); //waits for all workers to finish
    }


    clean_up();


    get_timestamp(timestamp);
    printf("%s | Ventilator | Exiting\n", timestamp);
}

int main(int argc, char *argv[]) {

    if (argc != 7) {
        fprintf(stderr, "Input Standard: %s -w <workers> -t <tasks> -s <queue_size>\n", argv[0]);
        exit(1);
    }

    num_workers = atoi(argv[2]);
    num_tasks = atoi(argv[4]);
    queue_size = atoi(argv[6]);

    if (num_workers <= 0 || num_tasks <= 0 || queue_size <= 0) {
        fprintf(stderr, "Invalid Input\n");
        exit(1);
    }

    //message queue initialization
    initialize_message_queues();

    //start distributing tasks
    ventilator_process();

    return 0;
}
