#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include "ipc_comm.h"
#include "ipc_message.h"

// define macro function to short sentence
#define PRINT_ADD(msg)  std::cout << msg << std::flush
#define PRINT_END(msg)  std::cout << msg << " <-- " << getpid() << std::endl
#define PRINT(msg)      std::cout << msg << " <-- " << getpid() << std::endl


int ipc_message_demo_server()
{
    /* initialize the queue attributes */
    struct mq_attr attr;
    attr.mq_flags = 0;
    // must less than msg_max: /proc/sys/fs/mqueue/msg_max, or be fail.
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = IPC_MESSAGE_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mode_t old_mask;
    old_mask = umask(0);
    PRINT_ADD("Create message queue: ") << IPC_MESSAGE_QUEUE_SERVER << " ...";
    mqd_t mq_server = mq_open(IPC_MESSAGE_QUEUE_SERVER, O_CREAT | O_WRONLY | O_NONBLOCK, IPC_FILE_PERMISSION, &attr);
    if (-1 == mq_server)
    {
        PRINT_END("[failed]: ");
        PRINT_ADD(strerror(errno)) << std::endl;
        umask(old_mask);
        return  -1;
    }
    PRINT_END("[done]");

    PRINT_ADD("Create message queue: ") << IPC_MESSAGE_QUEUE_CLIENT << " ...";
    mqd_t mq_client = mq_open(IPC_MESSAGE_QUEUE_CLIENT, O_CREAT | O_RDONLY | O_NONBLOCK, IPC_FILE_PERMISSION, &attr);
    if (-1 == mq_client)
    {
        PRINT_END("[failed]: ");
        PRINT_ADD(strerror(errno)) << std::endl;
        umask(old_mask);
        return  -2;
    }
    umask(old_mask);
    PRINT_END("[done]");



    // read and write message by message queue
    ssize_t bytes_read = 0;
    ssize_t bytes_send = 0;
    char msg_buffer[IPC_MESSAGE_SIZE + 1] = {0};
    while  (true) {
        PRINT_ADD("Send message into queue: ") << IPC_MESSAGE_QUEUE_SERVER << std::flush;
        bytes_send = mq_send(mq_server, SHM_SVR_TEXT, sizeof(SHM_SVR_TEXT), 0);
        if (0 <= bytes_send) {
            PRINT_END("[done]. ");
        }
        else if ((0 > bytes_send) && (EAGAIN == errno))
        {
            PRINT_END("[full, try again]");
            usleep(1000);
        }
        else
        {
            PRINT_END("[failed]: ");
            PRINT_ADD(strerror(errno)) << std::endl;
            break;
        }


        memset(msg_buffer, 0, sizeof(msg_buffer));
        bytes_read = mq_receive(mq_client, msg_buffer, sizeof(msg_buffer), NULL);
        if (0 < bytes_read)
        {
            PRINT("Read message from queue: ");
            PRINT_ADD(msg_buffer) << std::endl << std::endl;
        }
        else if ((0 > bytes_read) && (EAGAIN == errno))
        {
            PRINT("Read message from queue...[empty, try again].");
        }
        else
        {
            PRINT("Read message from queue failed: ");
            PRINT_ADD(strerror(errno)) << std::endl;
            break;
        }

        // Choose path by user input char
        std::cout << "Wait user input to choose path: " << getpid() << std::endl;
        switch(getchar())
        {
            case 'q':
            case 'Q':
                goto TARGET_RELEASE;
                break;

            default:
                break;
        }

    }


    TARGET_RELEASE:

    // close message queue
    PRINT_ADD("Close message queue : ") << IPC_MESSAGE_QUEUE_SERVER << " ...";
    mq_close(mq_server);
    mq_unlink(IPC_MESSAGE_QUEUE_SERVER);
    PRINT_END("[done]");

    PRINT_ADD("Close message queue : ") << IPC_MESSAGE_QUEUE_CLIENT << " ...";
    mq_close(mq_client);
    mq_unlink(IPC_MESSAGE_QUEUE_CLIENT);
    PRINT_END("[done]");

    return 0;
}

int ipc_message_demo_client()
{
    /* initialize the queue attributes */
    struct mq_attr attr;
    attr.mq_flags = 0;
    // must less than msg_max: /proc/sys/fs/mqueue/msg_max, or be fail.
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = IPC_MESSAGE_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    PRINT_ADD("Create message queue: ") << IPC_MESSAGE_QUEUE_SERVER << " ...";
    mqd_t mq_server = mq_open(IPC_MESSAGE_QUEUE_SERVER, O_RDONLY | O_NONBLOCK, IPC_FILE_PERMISSION, &attr);
    if (-1 == mq_server)
    {
        PRINT_END("[failed]: ");
        PRINT_ADD(strerror(errno)) << std::endl;
        return  -1;
    }
    PRINT_END("[done]");

    PRINT_ADD("Create message queue: ") << IPC_MESSAGE_QUEUE_CLIENT << " ...";
    mqd_t mq_client = mq_open(IPC_MESSAGE_QUEUE_CLIENT, O_WRONLY | O_NONBLOCK, IPC_FILE_PERMISSION, &attr);
    if (-1 == mq_client)
    {
        PRINT_END("[failed]: ");
        PRINT_ADD(strerror(errno)) << std::endl;
        return  -2;
    }
    PRINT_END("[done]");

    // read and write message by message queue
    ssize_t bytes_read = 0;
    ssize_t bytes_send = 0;
    char msg_buffer[IPC_MESSAGE_SIZE + 1] = {0};
    while  (true) {
        memset(msg_buffer, 0, sizeof(msg_buffer));
        bytes_read = mq_receive(mq_server, msg_buffer, sizeof(msg_buffer), NULL);
        if (0 < bytes_read)
        {
            PRINT("Read message from queue: ");
            PRINT_ADD(msg_buffer) << std::endl << std::endl;
        }
        else if ((0 > bytes_read) && (EAGAIN != errno))
        {
            PRINT("Read message from queue failed: ");
            PRINT_ADD(strerror(errno)) << std::endl;
            break;
        }

        PRINT_ADD("Send message into queue: ") << IPC_MESSAGE_QUEUE_CLIENT << std::flush;
        bytes_send = mq_send(mq_client, SHM_CLT_TEXT, sizeof(SHM_CLT_TEXT), 0);
        if (0 <= bytes_send) {
            PRINT_END("[done]");
        }
        else if ((0 > bytes_send) && (EAGAIN != errno))
        {
            PRINT_END("[failed]: ");
            PRINT_ADD(strerror(errno)) << std::endl;
            break;
        } else {
            PRINT_END("[full, try again]");
        }

        sleep(5);
    }


    TARGET_RELEASE:

    // close message queue
    PRINT_ADD("Close message queue : ") << IPC_MESSAGE_QUEUE_SERVER << " ...";
    mq_close(mq_server);
    mq_unlink(IPC_MESSAGE_QUEUE_SERVER);
    PRINT_END("[done]");

    PRINT_ADD("Close message queue : ") << IPC_MESSAGE_QUEUE_CLIENT << " ...";
    mq_close(mq_client);
    mq_unlink(IPC_MESSAGE_QUEUE_CLIENT);
    PRINT_END("[done]");

    return  0;
}
