#include <sys/mman.h>
#include <sys/stat.h>
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

int named_pipe_demo_server()
{
    // create duplex named pipe
    if((mkfifo(IPC_PIPE_SVR_WRITE, IPC_PIPE_PERMISSION ) < 0) && ( errno != EEXIST)) {
        std::cerr << "Can't create named pipe(fifo): " << IPC_PIPE_SVR_WRITE << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -1;
    }
    std::cout << "Create named pipe(fifo) success: " << IPC_PIPE_SVR_WRITE << " <-- " << getpid() << std::endl;

    if((mkfifo(IPC_PIPE_SVR_READ, IPC_PIPE_PERMISSION ) < 0) && ( errno != EEXIST)) {
        std::cerr << "Can't create named pipe(fifo): " << IPC_PIPE_SVR_READ << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -2;
    }
    std::cout << "Create named pipe(fifo) success: " << IPC_PIPE_SVR_READ << " <-- " << getpid() << std::endl;


    // open duplex named pipe
    int pipe_write_fd = open(IPC_PIPE_SVR_WRITE, O_WRONLY /*| O_NONBLOCK*/, 0);
    if ((-1 == pipe_write_fd) && (ENXIO == errno))
    {
        std::cerr << "Open named pipe(fifo) fail: " << IPC_PIPE_SVR_WRITE << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -3;
    }
    std::cout << "Open named pipe(fifo) success:" << IPC_PIPE_SVR_WRITE << " <-- " << getpid() << std::endl;

    int pipe_read_fd = open(IPC_PIPE_SVR_READ, O_RDONLY /*| O_NONBLOCK*/, 0);
    if ((-1 == pipe_write_fd) && (ENXIO == errno))
    {
        std::cerr << "Open named pipe(fifo) fail: " << IPC_PIPE_SVR_READ << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -4;
    }
    std::cout << "Open named pipe(fifo) success:" << IPC_PIPE_SVR_READ << " <-- " << getpid() << std::endl;


    ssize_t send_size = 0;
    ssize_t read_size = 0;
    char pipe_buff[8192] = {0};
    while (true)
    {
        // send data to client by fifo
        send_size = write(pipe_write_fd, SHM_SVR_TEXT, sizeof(SHM_SVR_TEXT));
        if (-1 == send_size)
        {
            std::cerr << "Send data to client by fifo fail: " <<  strerror(errno) << " <-- " << getpid() << std::endl;
            break;
        }
        std::cout << "Send data to client: " << send_size << " <-- " << getpid() << std::endl;


        //read data from client by fifo
        memset(pipe_buff, 0, sizeof(pipe_buff));
        while ((-1 == (read_size = read(pipe_read_fd, pipe_buff, 8192))) && (errno==EAGAIN));
        if( (-1 == read_size) && (EAGAIN != errno)) {
            std::cerr << "Read data from client by fifo fail: " <<  strerror(errno) << " <-- " << getpid() << std::endl;
            break;

        }

        if (0 < read_size) {
            std::cout << "Read data from client: " << read_size << " <-- " << getpid() << std::endl;
            std::cout << pipe_buff << std::endl;
        }


        // Hold lock until user input char
        std::cout << "Wait user input to continue..." << " <-- " << getpid() << std::endl;
        switch(getchar())
        {
            case 'q':
            case 'Q':
                goto TARGET_EXIT;
                break;

            default:
                break;
        }
        std::cout << "Wait user input to continue...[done]" << " <-- " << getpid() << std::endl;
    }

    TARGET_EXIT:

    close(pipe_write_fd);
    close(pipe_read_fd);
    unlink(IPC_PIPE_SVR_WRITE);
    unlink(IPC_PIPE_SVR_READ);
    std::cout << "Close pipe handlee and unlink name. " << " <-- " << getpid() << std::endl;

    return  0;
}

int named_pipe_demo_client()
{
    int pipe_read_fd = open(IPC_PIPE_SVR_WRITE, O_RDONLY /*| O_NONBLOCK*/, 0);
    if ((-1 == pipe_read_fd) && (ENXIO == errno))
    {
        std::cerr << "Open named pipe(read fifo) fail: " << IPC_PIPE_SVR_WRITE << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -2;
    }
    std::cout << "Open named pipe(fifo) success:" << IPC_PIPE_SVR_WRITE << " <-- " << getpid() << std::endl;


    int pipe_write_fd = open(IPC_PIPE_SVR_READ, O_WRONLY /*| O_NONBLOCK*/, 0);
    if ((-1 == pipe_write_fd) && (ENXIO == errno))
    {
        std::cerr << "Open named pipe(fifo) fail: " << IPC_PIPE_SVR_READ << ", error: " << strerror(errno) << " <-- " << getpid() << std::endl;
        return  -2;
    }
    std::cout << "Open named pipe(fifo) success:" << IPC_PIPE_SVR_READ << " <-- " << getpid() << std::endl;

    ssize_t send_size = 0;
    ssize_t read_size = 0;
    char pipe_buff[8192] = {0};

    // getchar();
    while (true)
    {
        // send data to client by fifo
        send_size = write(pipe_write_fd, SHM_CLT_TEXT, sizeof(SHM_CLT_TEXT));
        if (-1 == send_size)
        {
            std::cerr << "Send data to server by fifo fail: " <<  strerror(errno) << " <-- " << getpid() << std::endl;
            break;
        }
        std::cout << "Send data to server: " << send_size << " <-- " << getpid() << std::endl;


        //read data from client by fifo
        WAIT_TO_READ:
        memset(pipe_buff, 0, sizeof(pipe_buff));
        while ((-1 == (read_size = read(pipe_read_fd, pipe_buff, 8192))) && (errno==EAGAIN));
        if( (-1 == read_size) && (EAGAIN != errno)) {
            std::cerr << "Read data from server by fifo fail: " <<  strerror(errno) << " <-- " << getpid() << std::endl;
            break;
        }

        if ((0 == read_size) && (0 == errno)) {
            goto WAIT_TO_READ;
        }
        else {
            std::cout << "Read data from server: " << read_size << " <-- " << getpid() << std::endl;
            std::cout << pipe_buff << std::endl;
        }


        // Hold lock until user input char
        std::cout << "Wait user input to continue... " << " <-- " << getpid() << std::endl;
        switch(getchar())
        {
            case 'q':
            case 'Q':
                goto TARGET_EXIT;
                break;

            default:
                break;
        }
        std::cout << "Wait user input to continue...[done] " << " <-- " << getpid() << std::endl;
    }

    TARGET_EXIT:

    close(pipe_read_fd);
    close(pipe_write_fd);
    unlink(IPC_PIPE_SVR_WRITE);
    unlink(IPC_PIPE_SVR_READ);
    std::cout << "Close pipe handlee and unlink name. " << " <-- " << getpid() << std::endl;

    return  0;
}