/**
 * @file producer.cpp
 * @author Elliott Hager
 * @brief The producer in the producer and consumer problem
 * @date 2023-11-05
 * 
 */

#include "../include/producer.hpp"
#include "../include/membuff.hpp"


/**
 * @brief The main function for the producer
 * 
 * @param argc The number of arguments fed into the program
 * @param argv Array to hold the command line fed arguments, used for the shared memory file name
 * @return int Exit status of the program
 */

int main(int argc, char *argv[]) {

    //Vars
    int fd;
    char * shmpath;
    struct sharedMem *prodMem;


    //Grab the shared mem path from command
    shmpath = argv[1];


    //open mem
    fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0600);


    //Truncate it to the size of the struct, ensures extra is not used
    ftruncate(fd, sizeof(*prodMem));

    prodMem = static_cast<sharedMem*>(mmap(NULL, sizeof(*prodMem), PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0));


    //Init the structs
    sem_init(&(prodMem->mutex), 1,1);
    
    sem_init(&(prodMem->full), 1,0);
    
    sem_init(&(prodMem->empty), 1, TABLE_SIZE);

    prodMem->in, prodMem->out = 0;

    for(int i = 4; i >= 0; --i) {
        //Check if buffer is empty and if the mutex is NOT in use
        //If either one is NOT ready, then wait
        sem_wait(&prodMem->empty);
        sem_wait(&prodMem->mutex);

        int prod_val = rand();

        std::cout << "Produced: " << prod_val << std::endl;

        prodMem->table[prodMem->in] = prod_val;


        //Need to mod to ensure only 0 to TABLE_SIZE are used again and again
        prodMem->in = (prodMem->in + 1) % TABLE_SIZE;


        //Release the mutex and mark the buffer as full
        sem_post(&prodMem->mutex);
        sem_post(&prodMem->full);

        sleep(rand()%10);
    }

    shm_unlink(shmpath);


    return 0;
}



