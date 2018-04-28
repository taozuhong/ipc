#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include "ipc_comm.h"
#include "ipc_message.h"


int ipc_message_demo_server()
{
    std::cout << "Not implement" << " <-- " << getpid() << std::endl;

    return 0;
}

int ipc_message_demo_client()
{
    std::cout << "Not implement" << " <-- " << getpid() << std::endl;

    return  0;
}
