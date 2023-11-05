/**
 * @file consumer.cpp
 * @author Elliott Hager
 * @brief The consumer in the producer/consumer problem
 * @date 2023-11-05
 * 
 */

#include "../include/consumer.hpp"
#include "../include/membuff.hpp"



/**
 * @brief The main function for the consumer
 * 
 * @param argc The number of arguments fed into the program
 * @param argv Array to hold the command line fed arguments, used for the shared memory file name
 * @return int Exit status of the program
 */
int main(int argc, char *argv[]) {
    //Vars
    int fd;
    char * shmpath;
    struct sharedMem *consMem;



    //Grab the shared mem path from command
    shmpath = argv[1];


    //open mem
    fd = shm_open(shmpath, O_RDWR, 0);



    //Truncate it to the size of the struct, ensures extra is not used
    ftruncate(fd, sizeof(*consMem));

    consMem = static_cast<sharedMem*>(mmap(NULL, sizeof(*consMem), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    for (int i = 4; i >= 0; --i) {
        sem_wait(&(consMem->full));
        sem_wait(&(consMem->mutex));


        int con_item;
        con_item = consMem->table[consMem->out];
        std::cout << "Consumed: " <<  con_item << std::endl;
        

        consMem->out = (consMem->out + 1) % TABLE_SIZE;


        sem_post(&(consMem->mutex));
        sem_post(&(consMem->empty));

        sleep(rand()%10);
    }


    //Detatch mem
    shm_unlink(shmpath);


    return 0;

}