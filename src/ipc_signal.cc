#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include "ipc_comm.h"
#include "ipc_signal.h"

int ipc_signal_demo_with_none();
int ipc_signal_demo_with_args();
int ipc_signal_demo_customize();

void signal_handler_with_args(int sig, siginfo_t *siginfo, void *context);
void signal_handler_with_user(int sig, siginfo_t *siginfo, void *context);
void signal_handler_with_none(int signo);

int ipc_signal_demo()
{
    int ret_val = 0;
    if (0 > ipc_signal_demo_with_none())
    {
        ret_val -1;
    }

    if (0 > ipc_signal_demo_with_args())
    {
        ret_val = -2;
    }

    if (0 > ipc_signal_demo_customize())
    {
        ret_val = -3;
    }

    return  ret_val;
}

int ipc_signal_demo_with_none()
{
    return  0;
}

int ipc_signal_demo_with_args()
{
    struct sigaction act = {0};

    /* Use the sa_sigaction field because the handles has two additional parameters */
    act.sa_sigaction = &signal_handler_with_args;

    /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGTERM, &act, NULL) < 0) {
        perror ("sigaction");
        return 1;
    }
}

int ipc_signal_demo_customize()
{

}

void signal_handler_with_none(int signo)
{

}

void signal_handler_with_args(int sig, siginfo_t *siginfo, void *context)
{

}

void signal_handler_with_user(int sig, siginfo_t *siginfo, void *context)
{

}

