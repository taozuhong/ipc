#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include "ipc_comm.h"

#define STRIP_FLAG_HELP 1
#include "gflags/gflags.h"

int share_memory_demo();
int pipe_demo();

DEFINE_bool(h, false, "Show help");
DEFINE_bool(pipe, false, "Enter pipe mode");
DEFINE_bool(shm, false, "Enter share memory mode");

const char *    App_Usage =
        "./ipcv {OPTIONS} \n\
\n\
utils to demo IPC methods\n\
\n\
OPTIONS: \n\
-h                Show help \n\
-pipe            Enter pipe mode\n\
-shm             Enter pipe mode\n\
\n\
Welcome to contribute and share with your friends.";


int main(int argc, char * argv[]) {
    gflags::SetVersionString("0.1");
    gflags::SetUsageMessage(App_Usage);
    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_h) {
        gflags::ShowUsageWithFlagsRestrict(argv[0], App_Usage);
        return 0;
    }

    if (FLAGS_shm) {
        share_memory_demo();
    }

    if (FLAGS_pipe) {
        pipe_demo();
    }
}

int share_memory_demo()
{
    // create share memory object
    int ipc_shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, S_IRWXU);
    if (0 > ipc_shm_fd)
    {
        std::cerr << "Create share memory failed: " << strerror(errno) << std::endl;
        return -1;
    }
    std::cout << "Create share memory " << getpid() << std::endl;

    // alloc memory for share memory
    ftruncate64(ipc_shm_fd, SHM_SIZE);

    // mount the share memory into process
    char * shm_in_proc = (char *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ipc_shm_fd, 0);
    if (MAP_FAILED == shm_in_proc)
    {
        std::cerr << "Map share memory into process failed: " << strerror(errno) << std::endl;
        return  -2;
    }
    std::cout << "Map share memory into proc " << getpid() << std::endl;


    uint64_t shm_data_size = 0;
    char *  shm_data_add = shm_in_proc + SHM_HEAD_SIZE + SHM_DATA_SIZE;

    // set the init state
    uint64_t shm_sync_flag = SHM_FLAG_LOCK;
    atomic_write<uint64_t>(shm_in_proc, SHM_FLAG_LOCK);
    atomic_write<uint64_t>(shm_in_proc+SHM_HEAD_SIZE, 0ull);

    while (true)
    {
        std::cout << "Handle share memory...... " << getpid() << std::endl;

        // wait to idle
        while (SHM_FLAG_LOCK == atomic_read<uint64_t>(shm_in_proc))
        {
            usleep(100);
        }

        if (SHM_FLAG_RELEASE == atomic_read<uint64_t>(shm_in_proc))
        {
            // Read data from share memory
            shm_data_size = atomic_read<uint64_t>(shm_in_proc + SHM_HEAD_SIZE);
            if (0 < shm_data_size)
            {
                std::cout << "Read data from share memory " << shm_data_size  << " <-- " << getpid() << std::endl;
                std::cout << shm_data_add << std::endl;
            }

            break;
        }

        std::cout << "the Lock is idle for writing data: " << getpid() << std::endl;

        // LOCK: make flag before write data
        atomic_write<uint64_t>(shm_in_proc, SHM_FLAG_LOCK);
        std::cout << "Get Lock to write data: " << getpid() << std::endl;

        // Read data from share memory
        shm_data_size = atomic_read<uint64_t>(shm_in_proc + SHM_HEAD_SIZE);
        if (0 < shm_data_size)
        {
            std::cout << "Read data from share memory " << shm_data_size  << " <-- " << getpid() << std::endl;
            std::cout << shm_data_add << std::endl;
        }

        // write data to share memory
        shm_data_size = sizeof(SHM_SVR_TEXT);
        atomic_write<uint64_t>(shm_in_proc + SHM_HEAD_SIZE, shm_data_size);
        strncpy(shm_data_add, SHM_SVR_TEXT, shm_data_size);
        std::cout << "Write data to share memory: " << shm_data_size  << " <-- " << getpid() << std::endl;


        // Hold lock until user input char
        std::cout << "Wait user input to release lock: " << getpid() << std::endl;
        switch(getchar())
        {
            case 'q':
            case 'Q':
                atomic_write<uint64_t>(shm_in_proc, SHM_FLAG_RELEASE);
                std::cout << "Release lock before exit: " << getpid() << std::endl;
                goto TARGET_RELEASE;
                break;

            default:
                break;
        }


        // UNLOCK: release the flag
        atomic_write<uint64_t>(shm_in_proc, SHM_FLAG_UNLOCK);
        std::cout << "Release Lock to idle: " << getpid() << std::endl;

        sleep(5);
    }


    TARGET_RELEASE:

    munmap(shm_in_proc, SHM_SIZE);
    shm_unlink(SHM_NAME);

    std::cout << "Unmap share memory and release it " << getpid() << std::endl;

    return  0;
}

int pipe_demo()
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