#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include "ipc_comm.h"

int anonymous_pipe_demo()
{
    /*
     * core design: create a pipe to communicate between parent and child
     * NOTES: fd[0] is set up for reading, fd[1] is set up for writing
     */

    int pipe_write_to_child[2];
    int pipe_write_to_parent[2];
    if (0 != pipe(pipe_write_to_child))
    {
        std::cerr << "Create pipe to child failed: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -1;
    }

    if (0 != pipe(pipe_write_to_parent))
    {
        std::cerr << "Create pipe to parent failed: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -1;
    }


    if (fork())   // parent process
    {
        close(pipe_write_to_child[PIPE_STDIN]);
        close(pipe_write_to_parent[PIPE_STDOUT]);

        // write data to child
        write(pipe_write_to_child[1], SHM_SVR_TEXT, sizeof(SHM_SVR_TEXT));


        char pipe_buff[4096] = {0};
        while (0 == read(pipe_write_to_parent[0],pipe_buff, 4096));

        std::cout << "I'm parent, read data from child: " << getpid() << std::endl;
        std::cout << pipe_buff << std::endl << std::endl;

        getchar();

        std::cout << "I'm parent, exit now... " << getpid() << std::endl;
        close(pipe_write_to_child[PIPE_STDOUT]);
        close(pipe_write_to_parent[PIPE_STDIN]);
    }
    else        // child process
    {
        close(pipe_write_to_child[PIPE_STDOUT]);
        close(pipe_write_to_parent[PIPE_STDIN]);

        // read data from parent
        char pipe_buff[4096] = {0};
        while (0 == read(pipe_write_to_child[0],pipe_buff, 4096));

        std::cout << "I'm child, read data from parent: " << getpid() << std::endl;
        std::cout << pipe_buff << std::endl << std::endl;

        // write data to parent
        write(pipe_write_to_parent[1], SHM_CLT_TEXT, sizeof(SHM_CLT_TEXT));

        getchar();
        std::cout << "I'm child, exit now... " << getpid() << std::endl;


        close(pipe_write_to_child[PIPE_STDIN]);
        close(pipe_write_to_parent[PIPE_STDOUT]);
    }

    return  0;
}