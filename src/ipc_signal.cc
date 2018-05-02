#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <bits/sigset.h>
#include <bits/siginfo.h>
#include "ipc_comm.h"
#include "ipc_signal.h"

int ipc_signal_demo_with_none();
int ipc_signal_demo_with_args();
int ipc_signal_demo_customize();

void signal_handler_with_args(int sig, siginfo_t *siginfo, void *context);
void signal_handler_with_user(int sig, siginfo_t *siginfo, void *context);
void signal_handler_with_none(int signo);

int ipc_signal_demo(int32_t signal_type) {
    int ret_val = 0;

    switch (signal_type)
    {
        case 1:
            if (0 > ipc_signal_demo_with_none()) {
                ret_val - 1;
            }
            break;

        case 2:

            if (0 > ipc_signal_demo_with_args()) {
                ret_val = -2;
            }
            break;

        case 3:
            if (0 > ipc_signal_demo_customize()) {
                ret_val = -3;
            }
            break;

        default:
            break;
    }

    return ret_val;
}

int ipc_signal_demo_with_none()
{
    std::cout << "Register signal handler by signal(): SIGTERM ..." << std::flush;
    if (SIG_ERR == signal(SIGTERM, signal_handler_with_none))
    {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return  -1;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    std::cout << "Register signal handler by signal(): SIGINT ..." << std::flush;
    if (SIG_ERR == signal(SIGINT, signal_handler_with_none))
    {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return  -2;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;


    std::cout << "Register signal handler by signal(): SIGABRT ..." << std::flush;
    if (SIG_ERR == signal(SIGABRT, signal_handler_with_none))
    {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return  -2;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    while (true) {
        usleep(50);
    }

    return  0;
}

int ipc_signal_demo_with_args()
{
    struct sigaction act = {0};

    /* Use the sa_sigaction field because the handles has two additional parameters */
    act.sa_sigaction = &signal_handler_with_args;

    /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
    act.sa_flags = SA_SIGINFO;

    std::cout << "Register signal handler by sigaction(): SIGTERM ..." << std::flush;
    if (sigaction(SIGTERM, &act, NULL) < 0) {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return -1;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;


    std::cout << "Register signal handler by sigaction(): SIGINT ..." << std::flush;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return -2;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    std::cout << "Register signal handler by sigaction(): SIGABRT ..." << std::flush;
    if (sigaction(SIGABRT, &act, NULL) < 0) {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return -2;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    while (true)
    {
        usleep(50);
    }

    return  0;
}

int ipc_signal_demo_customize()
{
    struct sigaction act = {0};
    act.sa_sigaction = &signal_handler_with_user;
    act.sa_flags = SA_SIGINFO;

    std::cout << "Register signal handler by sigaction(): SIGUSR1 ..." << std::flush;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return -1;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    std::cout << "Register signal handler by sigaction(): SIGUSR2 ..." << std::flush;
    if (sigaction(SIGUSR2, &act, NULL) < 0) {
        std::cout << " [failed] <-- " << getpid() << std::endl;
        return -2;
    }
    std::cout << " [done] <-- " << getpid() << std::endl;

    while (true)
    {
        usleep(50);
    }
}

void signal_handler_with_none(int signo)
{
    sigset_t sigset = {0};
    if(sigprocmask(0, NULL, &sigset) != 0)
    {
        std::cout << "Call sigprocmask() fail. <-- " << getpid() << std::endl;
    }

    switch (signo) {
        case SIGINT:
            std::cout << "Caught signal: SIGINT <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGINT, SIG_DFL);
            raise(SIGINT);
            break;

        case SIGTERM:
            std::cout << "Caught signal: SIGTERM <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGTERM, SIG_DFL);
            kill(getpid(), SIGTERM);
            break;

        case SIGABRT:
            std::cout << "Caught signal: SIGABRT <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGABRT, SIG_IGN);
            sigaddset(&sigset, SIGABRT);
            sigprocmask(SIG_BLOCK, &sigset, NULL);
            //abort();
            break;

        default:
            std::cout << "Caught signal: others... <-- " << getpid() << std::endl;
            break;
    }
}

void signal_handler_with_args(int sig, siginfo_t *siginfo, void *context)
{
    sigset_t sigset = {0};
    if(sigprocmask(0, NULL, &sigset) != 0)
    {
        std::cout << "Call sigprocmask() fail. <-- " << getpid() << std::endl;
    }

    switch (sig) {
        case SIGINT:
            std::cout << "Caught signal: SIGINT <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGINT, SIG_DFL);
            raise(SIGINT);
            break;

        case SIGTERM:
            std::cout << "Caught signal: SIGTERM <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGTERM, SIG_DFL);
            raise(SIGTERM);
            break;

        case SIGABRT:
            std::cout << "Caught signal: SIGABRT <-- " << getpid() << std::endl;

            // call default handler
            signal(SIGABRT, SIG_IGN);
            sigaddset(&sigset, SIGABRT);
            sigprocmask(SIG_BLOCK, &sigset, NULL);
            //abort();
            break;

        default:
            std::cout << "Caught signal: others... <-- " << getpid() << std::endl;
            break;
    }
}

void signal_handler_with_user(int sig, siginfo_t *siginfo, void *context)
{
    sigval_t sigval = {0};

    switch (sig)
    {
        case SIGUSR1:
            std::cout << "Caught signal: SIGUSR1 <-- " << getpid() << std::endl;

            // check the user data
            if (NULL != siginfo->_sifields._rt.si_sigval.sival_ptr)
            {
                std::cout << "Get data of signal SIGUSR1 <-- " << getpid() << std::endl;

                char * user_text = (char *)siginfo->_sifields._rt.si_sigval.sival_ptr;
                std::cout << user_text << std::endl << std::endl;
            } else {
                // send message to process
                sigval.sival_ptr = (void*)SHM_SVR_TEXT;
                sigqueue(getpid(), SIGUSR2, sigval);
            }

            break;

        case SIGUSR2:
            std::cout << "Caught signal: SIGUSR2 <-- " << getpid() << std::endl;

            // check the user data
            if (NULL != siginfo->_sifields._rt.si_sigval.sival_ptr)
            {
                std::cout << "Get data of signal SIGUSR2 <-- " << getpid() << std::endl;

                char * user_text = (char *)siginfo->_sifields._rt.si_sigval.sival_ptr;
                std::cout << user_text << std::endl << std::endl;
            } else {
                // send message to process
                sigval.sival_ptr = (void*)SHM_CLT_TEXT;
                sigqueue(getpid(), SIGUSR1, sigval);
            }

            break;

        default:
            std::cout << "Caught signal: others... <-- " << getpid() << std::endl;
            break;
    }
}

