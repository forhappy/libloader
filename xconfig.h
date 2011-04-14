#define HAVE_MALLOC_H	1
#define HAVE_ALLOCA_H	1
#define HAVE_ALLOCA	1
#define HAVE_ATEXIT	1
#define HAVE_SIGACTION	1
#define HAVE_EXECINFO_H	1
#define HAVE_BACKTRACE	1
#define HAVE_MALLOC_STATS	1
#define HAVE_MALLINFO	1
#define HAVE_STDBOOL_H	1
#define HAVE_SETJMP_H	1
#define HAVE_CLOCK_GETTIME	1
#define HAVE_SIGSETJMP	1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the declaration of `memmem', and to 0 if you don't.
   */
#define HAVE_DECL_MEMMEM 1

/* Define to 1 if you have the declaration of `perror', and to 0 if you don't.
   */
#define HAVE_DECL_PERROR 1

/* Define to 1 if you have the declaration of `strerror', and to 0 if you
   don't. */
#define HAVE_DECL_STRERROR 1

/* Define if <sys/procfs.h> has elf_fpregset_t. */
#define HAVE_ELF_FPREGSET_T 1

/* Define if errno is available */
#define HAVE_ERRNO 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <linux/elf.h> header file. */
#define HAVE_LINUX_ELF_H 1

/* Define if the target supports register sets. */
#define HAVE_LINUX_REGSETS 1

/* Define if the target supports PTRACE_PEEKUSR for register access. */
#define HAVE_LINUX_USRREGS 1

/* Define if <sys/procfs.h> has lwpid_t. */
#define HAVE_LWPID_T 1

/* Define to 1 if you have the `memmem' function. */
#define HAVE_MEMMEM 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <netinet/tcp.h> header file. */
#define HAVE_NETINET_TCP_H 1

/* Define to 1 if you have the `pread' function. */
#define HAVE_PREAD 1

/* Define to 1 if you have the `pread64' function. */
#define HAVE_PREAD64 1

/* Define if <sys/procfs.h> has prgregset_t. */
#define HAVE_PRGREGSET_T 1

/* Define to 1 if you have the <proc_service.h> header file. */
/* #undef HAVE_PROC_SERVICE_H */

/* Define if <sys/procfs.h> has psaddr_t. */
#define HAVE_PSADDR_T 1

/* Define if the target supports PTRACE_GETFPXREGS for extended register
   access. */
#define HAVE_PTRACE_GETFPXREGS 1

/* Define if the target supports PTRACE_GETREGS for register access. */
#define HAVE_PTRACE_GETREGS 1

/* Define to 1 if you have the `pwrite' function. */
#define HAVE_PWRITE 1

/* Define to 1 if you have the <sgtty.h> header file. */
#define HAVE_SGTTY_H 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if the system has the type `socklen_t'. */
#define HAVE_SOCKLEN_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/procfs.h> header file. */
#define HAVE_SYS_PROCFS_H 1

/* Define to 1 if you have the <sys/reg.h> header file. */
#define HAVE_SYS_REG_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define if td_thr_tls_get_addr is available. */
#define HAVE_TD_THR_TLS_GET_ADDR 1

/* Define if TD_VERSION is available. */
#define HAVE_TD_VERSION 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define to 1 if you have the <thread_db.h> header file. */
#define HAVE_THREAD_DB_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

#define ELFCLASS32 1

/* #undef RELAX_SIGNAL */
/* #undef USE_ZLIB */
/* #undef USE_LZO */
#define COMPRESS_CKPT	1
#define INTERP_FILE	"/lib/ld-linux.so.2"
#define LOG_DIR		"/tmp/rebranch"


#define ARCH_X86_32	1
/* #undef ARCH_X86_64 */

#define MAX_OUTPUT_FN_LEN	(128)
#define BIGBUFFER_SZ	((2048) * (1024))
#define MAX_PROC_MAPS_FILE_SZ	((1024) * 1024)
#define INIT_CODECACHE_SIZE	(1 << 20)

#define GDBSERVER_PKGVERSION	"(Snitchaser internal) "
#define REPORT_BUGS_TO		"pi3orama@gmail.com"

#define __mkstr1(___a)		#___a
#define __mkstr2(___a)		__mkstr1(___a)
#define REPLAYER_ENTRY		__replayer_entry
#define REPLAYER_ENTRY_STR	__mkstr2(REPLAYER_ENTRY)
