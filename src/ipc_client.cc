#include <iostream>
#include "ipc_pipe.h"
#include "ipc_shm.h"
#include "ipc_comm.h"

#define STRIP_FLAG_HELP 1
#include "gflags/gflags.h"


DEFINE_bool(h, false, "Show help");
DEFINE_bool(shm, false, "Enter share memory mode");
DEFINE_bool(pipe, false, "Enter pipe mode(anony pipe)");
DEFINE_bool(fifo, false, "Enter fifo mode(named pipe)");

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
        ret_val = share_memory_demo_client();
    }

    if (FLAGS_fifo) {
        ret_val = named_pipe_demo_client();
    }

    return ret_val;
}


