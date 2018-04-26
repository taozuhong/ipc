#include "ipc_shm.h"
#include "ipc_pipe.h"

#define STRIP_FLAG_HELP 1
#include "gflags/gflags.h"


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
        share_memory_demo_server();
    }

    if (FLAGS_pipe) {
        anonymous_pipe_demo();
    }
}



