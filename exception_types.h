/* 
 * by WN @ Nov. 16, 2009
 */

def_exp_type(EXP_WRONG_CKPT, "checkpoint format error")
def_exp_type(EXP_CKPT_REGION_NOT_FOUND, "unable to find a region in checkpoint")
def_exp_type(EXP_FILE_NOT_FOUND, "file not found")
def_exp_type(EXP_FILE_READ_ERROR, "file read error")
def_exp_type(EXP_FILE_WRITE_ERROR, "file write error")
def_exp_type(EXP_FILE_END, "file is end")
def_exp_type(EXP_FILE_CORRUPTED, "file corrupted")
def_exp_type(EXP_LZO_INFLATE, "lzo inflate failed")
def_exp_type(EXP_OUT_OF_MEMORY, "out of memory")
def_exp_type(EXP_PROC_MAPS, "error when processing /proc/xxx/maps")
def_exp_type(EXP_PROC_MAPS_REGION_NOT_FOUND, "unable to find a region in /proc/xxx/maps")
def_exp_type(EXP_PTRACE, "ptrace error")
def_exp_type(EXP_ELF, "elf processing error")
def_exp_type(EXP_ELF_SYMBOL_NOT_FOUND, "unable to find a symbol in a elf file")

def_exp_type(EXP_SOCKPAIR_ERROR, "socketpair in gdbserver error")
def_exp_type(EXP_SOCKPAIR_UNRECOVERABLE, "socketpair unrecoverable")
def_exp_type(EXP_SOCKPAIR_UNSYNC, "sockpair communication protocol broken")
def_exp_type(EXP_SOCKPAIR_TIMEOUT, "sockpair communication timeout")

def_exp_type(EXP_RESTORE_CHILD, "child mem operation failed")

def_exp_type(EXP_GDBSERVER_EXIT, "gdbserver calls exit() or gdbserver_main return")
def_exp_type(EXP_GDBSERVER__EXIT, "gdbserver calls _exit()")
def_exp_type(EXP_GDBSERVER_TARGET_SIGNALED, "target process receives a signal")
def_exp_type(EXP_GDBSERVER_ERROR, "gdbserver encounters an error")
def_exp_type(EXP_GDBSERVER_DETACH, "gdbserver detach child")

def_exp_type(EXP_LOG_CORRUPTED, "log corrupted")
def_exp_type(EXP_LOG_END, "log end")

def_exp_type(EXP_SYSCALL_REPLAYER, "exception in replay system call")

def_exp_type(EXP_UNIMPLEMENTED, "unimplented feature")

def_exp_type(EXP_IO, "io exception")
def_exp_type(EXP_CHDIR, "chdir exception")
def_exp_type(EXP_FORK, "fork failed")
def_exp_type(EXP_UNMATCH_CKPT, "unable to use this checkpoint")
def_exp_type(EXP_WRONG_INST, "try to execute wrong instruction")
def_exp_type(EXP_CHILD_EXIT, "child exit")

// vim:ts=4:sw=4:cino=l1,\:0

