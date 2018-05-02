#include "ipc_shm.h"
#include "ipc_pipe.h"
#include "ipc_signal.h"
#include "ipc_message.h"
#include "ipc_comm.h"

#define STRIP_FLAG_HELP 1
#include "gflags/gflags.h"


DEFINE_bool(h, false, "Show help");
DEFINE_bool(shm, false, "Enter share memory mode");
DEFINE_bool(pipe, false, "Enter pipe mode(anony pipe)");
DEFINE_bool(fifo, false, "Enter fifo mode(named pipe)");
DEFINE_int32(signal, 1024, "Enter signal mode");
DEFINE_bool(msg, false, "Enter message queue mode");


int main(int argc, char * argv[]) {
    gflags::SetVersionString("0.1");
    gflags::SetUsageMessage(App_Usage);
    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_h) {
        gflags::ShowUsageWithFlagsRestrict(argv[0], App_Usage);
        return 0;
    }

    int ret_val = 0;
    if (FLAGS_shm) {
        ret_val = share_memory_demo_server();
    }

    if (FLAGS_pipe) {
        ret_val = anonymous_pipe_demo();
    }

    if (FLAGS_fifo)
    {
        ret_val = named_pipe_demo_server();
    }

    if ((0 < FLAGS_signal) && (1000 > FLAGS_signal))
    {
        ret_val = ipc_signal_demo(FLAGS_signal);
    }

    if (FLAGS_msg)
    {
        ret_val = ipc_message_demo_server();
    }

    return  ret_val;
}



