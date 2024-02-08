#ifdef __aarch64__

#include "scalls.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>

#include <elf.h> // See /usr/include/elf.h
#include <sys/uio.h>

/*****************
 * NAME HANDLING *
 *****************/

const char *get_syscall_name(int id)
{
    switch(id)
    {
        #ifdef  __NR_io_setup
         case __NR_io_setup : return "io_setup";
        #endif
        #ifdef  __NR_io_destroy
         case __NR_io_destroy : return "io_destroy";
        #endif
        #ifdef  __NR_io_submit
         case __NR_io_submit : return "io_submit";
        #endif
        #ifdef  __NR_io_cancel
         case __NR_io_cancel : return "io_cancel";
        #endif
        #ifdef  __NR_io_getevents
         case __NR_io_getevents : return "io_getevents";
        #endif
        #ifdef  __NR_setxattr
         case __NR_setxattr : return "setxattr";
        #endif
        #ifdef  __NR_lsetxattr
         case __NR_lsetxattr : return "lsetxattr";
        #endif
        #ifdef  __NR_fsetxattr
         case __NR_fsetxattr : return "fsetxattr";
        #endif
        #ifdef  __NR_getxattr
         case __NR_getxattr : return "getxattr";
        #endif
        #ifdef  __NR_lgetxattr
         case __NR_lgetxattr : return "lgetxattr";
        #endif
        #ifdef  __NR_fgetxattr
         case __NR_fgetxattr : return "fgetxattr";
        #endif
        #ifdef  __NR_listxattr
         case __NR_listxattr : return "listxattr";
        #endif
        #ifdef  __NR_llistxattr
         case __NR_llistxattr : return "llistxattr";
        #endif
        #ifdef  __NR_flistxattr
         case __NR_flistxattr : return "flistxattr";
        #endif
        #ifdef  __NR_removexattr
         case __NR_removexattr : return "removexattr";
        #endif
        #ifdef  __NR_lremovexattr
         case __NR_lremovexattr : return "lremovexattr";
        #endif
        #ifdef  __NR_fremovexattr
         case __NR_fremovexattr : return "fremovexattr";
        #endif
        #ifdef  __NR_getcwd
         case __NR_getcwd : return "getcwd";
        #endif
        #ifdef  __NR_lookup_dcookie
         case __NR_lookup_dcookie : return "lookup_dcookie";
        #endif
        #ifdef  __NR_eventfd2
         case __NR_eventfd2 : return "eventfd2";
        #endif
        #ifdef  __NR_epoll_create1
         case __NR_epoll_create1 : return "epoll_create1";
        #endif
        #ifdef  __NR_epoll_ctl
         case __NR_epoll_ctl : return "epoll_ctl";
        #endif
        #ifdef  __NR_epoll_pwait
         case __NR_epoll_pwait : return "epoll_pwait";
        #endif
        #ifdef  __NR_dup
         case __NR_dup : return "dup";
        #endif
        #ifdef  __NR_dup3
         case __NR_dup3 : return "dup3";
        #endif
        #ifdef  __NR3264_fcntl
         case __NR3264_fcntl : return "fcntl";
        #endif
        #ifdef  __NR_inotify_init1
         case __NR_inotify_init1 : return "inotify_init1";
        #endif
        #ifdef  __NR_inotify_add_watch
         case __NR_inotify_add_watch : return "inotify_add_watch";
        #endif
        #ifdef  __NR_inotify_rm_watch
         case __NR_inotify_rm_watch : return "inotify_rm_watch";
        #endif
        #ifdef  __NR_ioctl
         case __NR_ioctl : return "ioctl";
        #endif
        #ifdef  __NR_ioprio_set
         case __NR_ioprio_set : return "ioprio_set";
        #endif
        #ifdef  __NR_ioprio_get
         case __NR_ioprio_get : return "ioprio_get";
        #endif
        #ifdef  __NR_flock
         case __NR_flock : return "flock";
        #endif
        #ifdef  __NR_mknodat
         case __NR_mknodat : return "mknodat";
        #endif
        #ifdef  __NR_mkdirat
         case __NR_mkdirat : return "mkdirat";
        #endif
        #ifdef  __NR_unlinkat
         case __NR_unlinkat : return "unlinkat";
        #endif
        #ifdef  __NR_symlinkat
         case __NR_symlinkat : return "symlinkat";
        #endif
        #ifdef  __NR_linkat
         case __NR_linkat : return "linkat";
        #endif
        #ifdef  __NR_renameat
         case __NR_renameat : return "renameat";
        #endif
        #ifdef  __NR_umount2
         case __NR_umount2 : return "umount2";
        #endif
        #ifdef  __NR_mount
         case __NR_mount : return "mount";
        #endif
        #ifdef  __NR_pivot_root
         case __NR_pivot_root : return "pivot_root";
        #endif
        #ifdef  __NR_nfsservctl
         case __NR_nfsservctl : return "nfsservctl";
        #endif
        #ifdef  __NR3264_statfs
         case __NR3264_statfs : return "statfs";
        #endif
        #ifdef  __NR3264_fstatfs
         case __NR3264_fstatfs : return "fstatfs";
        #endif
        #ifdef  __NR3264_truncate
         case __NR3264_truncate : return "truncate";
        #endif
        #ifdef  __NR3264_ftruncate
         case __NR3264_ftruncate : return "ftruncate";
        #endif
        #ifdef  __NR_fallocate
         case __NR_fallocate : return "fallocate";
        #endif
        #ifdef  __NR_faccessat
         case __NR_faccessat : return "faccessat";
        #endif
        #ifdef  __NR_chdir
         case __NR_chdir : return "chdir";
        #endif
        #ifdef  __NR_fchdir
         case __NR_fchdir : return "fchdir";
        #endif
        #ifdef  __NR_chroot
         case __NR_chroot : return "chroot";
        #endif
        #ifdef  __NR_fchmod
         case __NR_fchmod : return "fchmod";
        #endif
        #ifdef  __NR_fchmodat
         case __NR_fchmodat : return "fchmodat";
        #endif
        #ifdef  __NR_fchownat
         case __NR_fchownat : return "fchownat";
        #endif
        #ifdef  __NR_fchown
         case __NR_fchown : return "fchown";
        #endif
        #ifdef  __NR_openat
         case __NR_openat : return "openat";
        #endif
        #ifdef  __NR_close
         case __NR_close : return "close";
        #endif
        #ifdef  __NR_vhangup
         case __NR_vhangup : return "vhangup";
        #endif
        #ifdef  __NR_pipe2
         case __NR_pipe2 : return "pipe2";
        #endif
        #ifdef  __NR_quotactl
         case __NR_quotactl : return "quotactl";
        #endif
        #ifdef  __NR_getdents64
         case __NR_getdents64 : return "getdents64";
        #endif
        #ifdef  __NR3264_lseek
         case __NR3264_lseek : return "__NR3264_lseek";
        #endif
        #ifdef  __NR_read
         case __NR_read : return "read";
        #endif
        #ifdef  __NR_write
         case __NR_write : return "write";
        #endif
        #ifdef  __NR_readv
         case __NR_readv : return "readv";
        #endif
        #ifdef  __NR_writev
         case __NR_writev : return "writev";
        #endif
        #ifdef  __NR_pread64
         case __NR_pread64 : return "pread64";
        #endif
        #ifdef  __NR_pwrite64
         case __NR_pwrite64 : return "pwrite64";
        #endif
        #ifdef  __NR_preadv
         case __NR_preadv : return "preadv";
        #endif
        #ifdef  __NR_pwritev
         case __NR_pwritev : return "pwritev";
        #endif
        #ifdef  __NR3264_sendfile
         case __NR3264_sendfile : return "__NR3264_sendfile";
        #endif
        #ifdef  __NR_pselect6
         case __NR_pselect6 : return "pselect6";
        #endif
        #ifdef  __NR_ppoll
         case __NR_ppoll : return "ppoll";
        #endif
        #ifdef  __NR_signalfd4
         case __NR_signalfd4 : return "signalfd4";
        #endif
        #ifdef  __NR_vmsplice
         case __NR_vmsplice : return "vmsplice";
        #endif
        #ifdef  __NR_splice
         case __NR_splice : return "splice";
        #endif
        #ifdef  __NR_tee
         case __NR_tee : return "tee";
        #endif
        #ifdef  __NR_readlinkat
         case __NR_readlinkat : return "readlinkat";
        #endif
        #ifdef  __NR3264_fstatat
         case __NR3264_fstatat : return "__NR3264_fstatat";
        #endif
        #ifdef  __NR3264_fstat
         case __NR3264_fstat : return "__NR3264_fstat";
        #endif
        #ifdef  __NR_sync
         case __NR_sync : return "sync";
        #endif
        #ifdef  __NR_fsync
         case __NR_fsync : return "fsync";
        #endif
        #ifdef  __NR_fdatasync
         case __NR_fdatasync : return "fdatasync";
        #endif
        #ifdef  __NR_sync_file_range2
         case __NR_sync_file_range2 : return "sync_file_range2";
        #endif
        #ifdef  __NR_sync_file_range
         case __NR_sync_file_range : return "sync_file_range";
        #endif
        #ifdef  __NR_timerfd_create
         case __NR_timerfd_create : return "timerfd_create";
        #endif
        #ifdef  __NR_timerfd_settime
         case __NR_timerfd_settime : return "timerfd_settime";
        #endif
        #ifdef  __NR_timerfd_gettime
         case __NR_timerfd_gettime : return "timerfd_gettime";
        #endif
        #ifdef  __NR_utimensat
         case __NR_utimensat : return "utimensat";
        #endif
        #ifdef  __NR_acct
         case __NR_acct : return "acct";
        #endif
        #ifdef  __NR_capget
         case __NR_capget : return "capget";
        #endif
        #ifdef  __NR_capset
         case __NR_capset : return "capset";
        #endif
        #ifdef  __NR_personality
         case __NR_personality : return "personality";
        #endif
        #ifdef  __NR_exit
         case __NR_exit : return "exit";
        #endif
        #ifdef  __NR_exit_group
         case __NR_exit_group : return "exit_group";
        #endif
        #ifdef  __NR_waitid
         case __NR_waitid : return "waitid";
        #endif
        #ifdef  __NR_set_tid_address
         case __NR_set_tid_address : return "set_tid_address";
        #endif
        #ifdef  __NR_unshare
         case __NR_unshare : return "unshare";
        #endif
        #ifdef  __NR_futex
         case __NR_futex : return "futex";
        #endif
        #ifdef  __NR_set_robust_list
         case __NR_set_robust_list : return "set_robust_list";
        #endif
        #ifdef  __NR_get_robust_list
         case __NR_get_robust_list : return "get_robust_list";
        #endif
        #ifdef  __NR_nanosleep
         case __NR_nanosleep : return "nanosleep";
        #endif
        #ifdef  __NR_getitimer
         case __NR_getitimer : return "getitimer";
        #endif
        #ifdef  __NR_setitimer
         case __NR_setitimer : return "setitimer";
        #endif
        #ifdef  __NR_kexec_load
         case __NR_kexec_load : return "kexec_load";
        #endif
        #ifdef  __NR_init_module
         case __NR_init_module : return "init_module";
        #endif
        #ifdef  __NR_delete_module
         case __NR_delete_module : return "delete_module";
        #endif
        #ifdef  __NR_timer_create
         case __NR_timer_create : return "timer_create";
        #endif
        #ifdef  __NR_timer_gettime
         case __NR_timer_gettime : return "timer_gettime";
        #endif
        #ifdef  __NR_timer_getoverrun
         case __NR_timer_getoverrun : return "timer_getoverrun";
        #endif
        #ifdef  __NR_timer_settime
         case __NR_timer_settime : return "timer_settime";
        #endif
        #ifdef  __NR_timer_delete
         case __NR_timer_delete : return "timer_delete";
        #endif
        #ifdef  __NR_clock_settime
         case __NR_clock_settime : return "clock_settime";
        #endif
        #ifdef  __NR_clock_gettime
         case __NR_clock_gettime : return "clock_gettime";
        #endif
        #ifdef  __NR_clock_getres
         case __NR_clock_getres : return "clock_getres";
        #endif
        #ifdef  __NR_clock_nanosleep
         case __NR_clock_nanosleep : return "clock_nanosleep";
        #endif
        #ifdef  __NR_syslog
         case __NR_syslog : return "syslog";
        #endif
        #ifdef  __NR_ptrace
         case __NR_ptrace : return "ptrace";
        #endif
        #ifdef  __NR_sched_setparam
         case __NR_sched_setparam : return "sched_setparam";
        #endif
        #ifdef  __NR_sched_setscheduler
         case __NR_sched_setscheduler : return "sched_setscheduler";
        #endif
        #ifdef  __NR_sched_getscheduler
         case __NR_sched_getscheduler : return "sched_getscheduler";
        #endif
        #ifdef  __NR_sched_getparam
         case __NR_sched_getparam : return "sched_getparam";
        #endif
        #ifdef  __NR_sched_setaffinity
         case __NR_sched_setaffinity : return "sched_setaffinity";
        #endif
        #ifdef  __NR_sched_getaffinity
         case __NR_sched_getaffinity : return "sched_getaffinity";
        #endif
        #ifdef  __NR_sched_yield
         case __NR_sched_yield : return "sched_yield";
        #endif
        #ifdef  __NR_sched_get_priority_max
         case __NR_sched_get_priority_max : return "sched_get_priority_max";
        #endif
        #ifdef  __NR_sched_get_priority_min
         case __NR_sched_get_priority_min : return "sched_get_priority_min";
        #endif
        #ifdef  __NR_sched_rr_get_interval
         case __NR_sched_rr_get_interval : return "sched_rr_get_interval";
        #endif
        #ifdef  __NR_restart_syscall
         case __NR_restart_syscall : return "restart_syscall";
        #endif
        #ifdef  __NR_kill
         case __NR_kill : return "kill";
        #endif
        #ifdef  __NR_tkill
         case __NR_tkill : return "tkill";
        #endif
        #ifdef  __NR_tgkill
         case __NR_tgkill : return "tgkill";
        #endif
        #ifdef  __NR_sigaltstack
         case __NR_sigaltstack : return "sigaltstack";
        #endif
        #ifdef  __NR_rt_sigsuspend
         case __NR_rt_sigsuspend : return "rt_sigsuspend";
        #endif
        #ifdef  __NR_rt_sigaction
         case __NR_rt_sigaction : return "rt_sigaction";
        #endif
        #ifdef  __NR_rt_sigprocmask
         case __NR_rt_sigprocmask : return "rt_sigprocmask";
        #endif
        #ifdef  __NR_rt_sigpending
         case __NR_rt_sigpending : return "rt_sigpending";
        #endif
        #ifdef  __NR_rt_sigtimedwait
         case __NR_rt_sigtimedwait : return "rt_sigtimedwait";
        #endif
        #ifdef  __NR_rt_sigqueueinfo
         case __NR_rt_sigqueueinfo : return "rt_sigqueueinfo";
        #endif
        #ifdef  __NR_rt_sigreturn
         case __NR_rt_sigreturn : return "rt_sigreturn";
        #endif
        #ifdef  __NR_setpriority
         case __NR_setpriority : return "setpriority";
        #endif
        #ifdef  __NR_getpriority
         case __NR_getpriority : return "getpriority";
        #endif
        #ifdef  __NR_reboot
         case __NR_reboot : return "reboot";
        #endif
        #ifdef  __NR_setregid
         case __NR_setregid : return "setregid";
        #endif
        #ifdef  __NR_setgid
         case __NR_setgid : return "setgid";
        #endif
        #ifdef  __NR_setreuid
         case __NR_setreuid : return "setreuid";
        #endif
        #ifdef  __NR_setuid
         case __NR_setuid : return "setuid";
        #endif
        #ifdef  __NR_setresuid
         case __NR_setresuid : return "setresuid";
        #endif
        #ifdef  __NR_getresuid
         case __NR_getresuid : return "getresuid";
        #endif
        #ifdef  __NR_setresgid
         case __NR_setresgid : return "setresgid";
        #endif
        #ifdef  __NR_getresgid
         case __NR_getresgid : return "getresgid";
        #endif
        #ifdef  __NR_setfsuid
         case __NR_setfsuid : return "setfsuid";
        #endif
        #ifdef  __NR_setfsgid
         case __NR_setfsgid : return "setfsgid";
        #endif
        #ifdef  __NR_times
         case __NR_times : return "times";
        #endif
        #ifdef  __NR_setpgid
         case __NR_setpgid : return "setpgid";
        #endif
        #ifdef  __NR_getpgid
         case __NR_getpgid : return "getpgid";
        #endif
        #ifdef  __NR_getsid
         case __NR_getsid : return "getsid";
        #endif
        #ifdef  __NR_setsid
         case __NR_setsid : return "setsid";
        #endif
        #ifdef  __NR_getgroups
         case __NR_getgroups : return "getgroups";
        #endif
        #ifdef  __NR_setgroups
         case __NR_setgroups : return "setgroups";
        #endif
        #ifdef  __NR_uname
         case __NR_uname : return "uname";
        #endif
        #ifdef  __NR_sethostname
         case __NR_sethostname : return "sethostname";
        #endif
        #ifdef  __NR_setdomainname
         case __NR_setdomainname : return "setdomainname";
        #endif
        #ifdef  __NR_getrlimit
         case __NR_getrlimit : return "getrlimit";
        #endif
        #ifdef  __NR_setrlimit
         case __NR_setrlimit : return "setrlimit";
        #endif
        #ifdef  __NR_getrusage
         case __NR_getrusage : return "getrusage";
        #endif
        #ifdef  __NR_umask
         case __NR_umask : return "umask";
        #endif
        #ifdef  __NR_prctl
         case __NR_prctl : return "prctl";
        #endif
        #ifdef  __NR_getcpu
         case __NR_getcpu : return "getcpu";
        #endif
        #ifdef  __NR_gettimeofday
         case __NR_gettimeofday : return "gettimeofday";
        #endif
        #ifdef  __NR_settimeofday
         case __NR_settimeofday : return "settimeofday";
        #endif
        #ifdef  __NR_adjtimex
         case __NR_adjtimex : return "adjtimex";
        #endif
        #ifdef  __NR_getpid
         case __NR_getpid : return "getpid";
        #endif
        #ifdef  __NR_getppid
         case __NR_getppid : return "getppid";
        #endif
        #ifdef  __NR_getuid
         case __NR_getuid : return "getuid";
        #endif
        #ifdef  __NR_geteuid
         case __NR_geteuid : return "geteuid";
        #endif
        #ifdef  __NR_getgid
         case __NR_getgid : return "getgid";
        #endif
        #ifdef  __NR_getegid
         case __NR_getegid : return "getegid";
        #endif
        #ifdef  __NR_gettid
         case __NR_gettid : return "gettid";
        #endif
        #ifdef  __NR_sysinfo
         case __NR_sysinfo : return "sysinfo";
        #endif
        #ifdef  __NR_mq_open
         case __NR_mq_open : return "mq_open";
        #endif
        #ifdef  __NR_mq_unlink
         case __NR_mq_unlink : return "mq_unlink";
        #endif
        #ifdef  __NR_mq_timedsend
         case __NR_mq_timedsend : return "mq_timedsend";
        #endif
        #ifdef  __NR_mq_timedreceive
         case __NR_mq_timedreceive : return "mq_timedreceive";
        #endif
        #ifdef  __NR_mq_notify
         case __NR_mq_notify : return "mq_notify";
        #endif
        #ifdef  __NR_mq_getsetattr
         case __NR_mq_getsetattr : return "mq_getsetattr";
        #endif
        #ifdef  __NR_msgget
         case __NR_msgget : return "msgget";
        #endif
        #ifdef  __NR_msgctl
         case __NR_msgctl : return "msgctl";
        #endif
        #ifdef  __NR_msgrcv
         case __NR_msgrcv : return "msgrcv";
        #endif
        #ifdef  __NR_msgsnd
         case __NR_msgsnd : return "msgsnd";
        #endif
        #ifdef  __NR_semget
         case __NR_semget : return "semget";
        #endif
        #ifdef  __NR_semctl
         case __NR_semctl : return "semctl";
        #endif
        #ifdef  __NR_semtimedop
         case __NR_semtimedop : return "semtimedop";
        #endif
        #ifdef  __NR_semop
         case __NR_semop : return "semop";
        #endif
        #ifdef  __NR_shmget
         case __NR_shmget : return "shmget";
        #endif
        #ifdef  __NR_shmctl
         case __NR_shmctl : return "shmctl";
        #endif
        #ifdef  __NR_shmat
         case __NR_shmat : return "shmat";
        #endif
        #ifdef  __NR_shmdt
         case __NR_shmdt : return "shmdt";
        #endif
        #ifdef  __NR_socket
         case __NR_socket : return "socket";
        #endif
        #ifdef  __NR_socketpair
         case __NR_socketpair : return "socketpair";
        #endif
        #ifdef  __NR_bind
         case __NR_bind : return "bind";
        #endif
        #ifdef  __NR_listen
         case __NR_listen : return "listen";
        #endif
        #ifdef  __NR_accept
         case __NR_accept : return "accept";
        #endif
        #ifdef  __NR_connect
         case __NR_connect : return "connect";
        #endif
        #ifdef  __NR_getsockname
         case __NR_getsockname : return "getsockname";
        #endif
        #ifdef  __NR_getpeername
         case __NR_getpeername : return "getpeername";
        #endif
        #ifdef  __NR_sendto
         case __NR_sendto : return "sendto";
        #endif
        #ifdef  __NR_recvfrom
         case __NR_recvfrom : return "recvfrom";
        #endif
        #ifdef  __NR_setsockopt
         case __NR_setsockopt : return "setsockopt";
        #endif
        #ifdef  __NR_getsockopt
         case __NR_getsockopt : return "getsockopt";
        #endif
        #ifdef  __NR_shutdown
         case __NR_shutdown : return "shutdown";
        #endif
        #ifdef  __NR_sendmsg
         case __NR_sendmsg : return "sendmsg";
        #endif
        #ifdef  __NR_recvmsg
         case __NR_recvmsg : return "recvmsg";
        #endif
        #ifdef  __NR_readahead
         case __NR_readahead : return "readahead";
        #endif
        #ifdef  __NR_brk
         case __NR_brk : return "brk";
        #endif
        #ifdef  __NR_munmap
         case __NR_munmap : return "munmap";
        #endif
        #ifdef  __NR_mremap
         case __NR_mremap : return "mremap";
        #endif
        #ifdef  __NR_add_key
         case __NR_add_key : return "add_key";
        #endif
        #ifdef  __NR_request_key
         case __NR_request_key : return "request_key";
        #endif
        #ifdef  __NR_keyctl
         case __NR_keyctl : return "keyctl";
        #endif
        #ifdef  __NR_clone
         case __NR_clone : return "clone";
        #endif
        #ifdef  __NR_execve
         case __NR_execve : return "execve";
        #endif
        #ifdef  __NR3264_mmap
         case __NR3264_mmap : return "__NR3264_mmap";
        #endif
        #ifdef  __NR3264_fadvise64
         case __NR3264_fadvise64 : return "__NR3264_fadvise64";
        #endif
        #ifdef  __NR_swapon
         case __NR_swapon : return "swapon";
        #endif
        #ifdef  __NR_swapoff
         case __NR_swapoff : return "swapoff";
        #endif
        #ifdef  __NR_mprotect
         case __NR_mprotect : return "mprotect";
        #endif
        #ifdef  __NR_msync
         case __NR_msync : return "msync";
        #endif
        #ifdef  __NR_mlock
         case __NR_mlock : return "mlock";
        #endif
        #ifdef  __NR_munlock
         case __NR_munlock : return "munlock";
        #endif
        #ifdef  __NR_mlockall
         case __NR_mlockall : return "mlockall";
        #endif
        #ifdef  __NR_munlockall
         case __NR_munlockall : return "munlockall";
        #endif
        #ifdef  __NR_mincore
         case __NR_mincore : return "mincore";
        #endif
        #ifdef  __NR_madvise
         case __NR_madvise : return "madvise";
        #endif
        #ifdef  __NR_remap_file_pages
         case __NR_remap_file_pages : return "remap_file_pages";
        #endif
        #ifdef  __NR_mbind
         case __NR_mbind : return "mbind";
        #endif
        #ifdef  __NR_get_mempolicy
         case __NR_get_mempolicy : return "get_mempolicy";
        #endif
        #ifdef  __NR_set_mempolicy
         case __NR_set_mempolicy : return "set_mempolicy";
        #endif
        #ifdef  __NR_migrate_pages
         case __NR_migrate_pages : return "migrate_pages";
        #endif
        #ifdef  __NR_move_pages
         case __NR_move_pages : return "move_pages";
        #endif
        #ifdef  __NR_rt_tgsigqueueinfo
         case __NR_rt_tgsigqueueinfo : return "rt_tgsigqueueinfo";
        #endif
        #ifdef  __NR_perf_event_open
         case __NR_perf_event_open : return "perf_event_open";
        #endif
        #ifdef  __NR_accept4
         case __NR_accept4 : return "accept4";
        #endif
        #ifdef  __NR_recvmmsg
         case __NR_recvmmsg : return "recvmmsg";
        #endif
        #ifdef  __NR_arch_specific_syscall
         case __NR_arch_specific_syscall : return "arch_specific_syscall";
        #endif
        #ifdef  __NR_wait4
         case __NR_wait4 : return "wait4";
        #endif
        #ifdef  __NR_prlimit64
         case __NR_prlimit64 : return "prlimit64";
        #endif
        #ifdef  __NR_fanotify_init
         case __NR_fanotify_init : return "fanotify_init";
        #endif
        #ifdef  __NR_fanotify_mark
         case __NR_fanotify_mark : return "fanotify_mark";
        #endif
        #ifdef  __NR_name_to_handle_at
         case __NR_name_to_handle_at : return "name_to_handle_at";
        #endif
        #ifdef  __NR_open_by_handle_at
         case __NR_open_by_handle_at : return "open_by_handle_at";
        #endif
        #ifdef  __NR_clock_adjtime
         case __NR_clock_adjtime : return "clock_adjtime";
        #endif
        #ifdef  __NR_syncfs
         case __NR_syncfs : return "syncfs";
        #endif
        #ifdef  __NR_setns
         case __NR_setns : return "setns";
        #endif
        #ifdef  __NR_sendmmsg
         case __NR_sendmmsg : return "sendmmsg";
        #endif
        #ifdef  __NR_process_vm_readv
         case __NR_process_vm_readv : return "process_vm_readv";
        #endif
        #ifdef  __NR_process_vm_writev
         case __NR_process_vm_writev : return "process_vm_writev";
        #endif
        #ifdef  __NR_kcmp
         case __NR_kcmp : return "kcmp";
        #endif
        #ifdef  __NR_finit_module
         case __NR_finit_module : return "finit_module";
        #endif
        #ifdef  __NR_sched_setattr
         case __NR_sched_setattr : return "sched_setattr";
        #endif
        #ifdef  __NR_sched_getattr
         case __NR_sched_getattr : return "sched_getattr";
        #endif
        #ifdef  __NR_renameat2
         case __NR_renameat2 : return "renameat2";
        #endif
        #ifdef  __NR_seccomp
         case __NR_seccomp : return "seccomp";
        #endif
        #ifdef  __NR_getrandom
         case __NR_getrandom : return "getrandom";
        #endif
        #ifdef  __NR_memfd_create
         case __NR_memfd_create : return "memfd_create";
        #endif
        #ifdef  __NR_bpf
         case __NR_bpf : return "bpf";
        #endif
        #ifdef  __NR_execveat
         case __NR_execveat : return "execveat";
        #endif
        #ifdef  __NR_userfaultfd
         case __NR_userfaultfd : return "userfaultfd";
        #endif
        #ifdef  __NR_membarrier
         case __NR_membarrier : return "membarrier";
        #endif
        #ifdef  __NR_mlock2
         case __NR_mlock2 : return "mlock2";
        #endif
        #ifdef  __NR_copy_file_range
         case __NR_copy_file_range : return "copy_file_range";
        #endif
        #ifdef  __NR_preadv2
         case __NR_preadv2 : return "preadv2";
        #endif
        #ifdef  __NR_pwritev2
         case __NR_pwritev2 : return "pwritev2";
        #endif
        #ifdef  __NR_pkey_mprotect
         case __NR_pkey_mprotect : return "pkey_mprotect";
        #endif
        #ifdef  __NR_pkey_alloc
         case __NR_pkey_alloc : return "pkey_alloc";
        #endif
        #ifdef  __NR_pkey_free
         case __NR_pkey_free : return "pkey_free";
        #endif
        #ifdef  __NR_statx
         case __NR_statx : return "statx";
        #endif
        #ifdef  __NR_io_pgetevents
         case __NR_io_pgetevents : return "io_pgetevents";
        #endif
        #ifdef  __NR_rseq
         case __NR_rseq : return "rseq";
        #endif
        #ifdef  __NR_kexec_file_load
         case __NR_kexec_file_load : return "kexec_file_load";
        #endif
        #ifdef  __NR_clock_gettime64
         case __NR_clock_gettime64 : return "clock_gettime64";
        #endif
        #ifdef  __NR_clock_settime64
         case __NR_clock_settime64 : return "clock_settime64";
        #endif
        #ifdef  __NR_clock_adjtime64
         case __NR_clock_adjtime64 : return "clock_adjtime64";
        #endif
        #ifdef  __NR_clock_getres_time64
         case __NR_clock_getres_time64 : return "clock_getres_time64";
        #endif
        #ifdef  __NR_clock_nanosleep_time64
         case __NR_clock_nanosleep_time64 : return "clock_nanosleep_time64";
        #endif
        #ifdef  __NR_timer_gettime64
         case __NR_timer_gettime64 : return "timer_gettime64";
        #endif
        #ifdef  __NR_timer_settime64
         case __NR_timer_settime64 : return "timer_settime64";
        #endif
        #ifdef  __NR_timerfd_gettime64
         case __NR_timerfd_gettime64 : return "timerfd_gettime64";
        #endif
        #ifdef  __NR_timerfd_settime64
         case __NR_timerfd_settime64 : return "timerfd_settime64";
        #endif
        #ifdef  __NR_utimensat_time64
         case __NR_utimensat_time64 : return "utimensat_time64";
        #endif
        #ifdef  __NR_pselect6_time64
         case __NR_pselect6_time64 : return "pselect6_time64";
        #endif
        #ifdef  __NR_ppoll_time64
         case __NR_ppoll_time64 : return "ppoll_time64";
        #endif
        #ifdef  __NR_io_pgetevents_time64
         case __NR_io_pgetevents_time64 : return "io_pgetevents_time64";
        #endif
        #ifdef  __NR_recvmmsg_time64
         case __NR_recvmmsg_time64 : return "recvmmsg_time64";
        #endif
        #ifdef  __NR_mq_timedsend_time64
         case __NR_mq_timedsend_time64 : return "mq_timedsend_time64";
        #endif
        #ifdef  __NR_mq_timedreceive_time64
         case __NR_mq_timedreceive_time64 : return "mq_timedreceive_time64";
        #endif
        #ifdef  __NR_semtimedop_time64
         case __NR_semtimedop_time64 : return "semtimedop_time64";
        #endif
        #ifdef  __NR_rt_sigtimedwait_time64
         case __NR_rt_sigtimedwait_time64 : return "rt_sigtimedwait_time64";
        #endif
        #ifdef  __NR_futex_time64
         case __NR_futex_time64 : return "futex_time64";
        #endif
        #ifdef  __NR_sched_rr_get_interval_time64
         case __NR_sched_rr_get_interval_time64 : return "sched_rr_get_interval_time64";
        #endif
        #ifdef  __NR_pidfd_send_signal
         case __NR_pidfd_send_signal : return "pidfd_send_signal";
        #endif
        #ifdef  __NR_io_uring_setup
         case __NR_io_uring_setup : return "io_uring_setup";
        #endif
        #ifdef  __NR_io_uring_enter
         case __NR_io_uring_enter : return "io_uring_enter";
        #endif
        #ifdef  __NR_io_uring_register
         case __NR_io_uring_register : return "io_uring_register";
        #endif
        #ifdef  __NR_open_tree
         case __NR_open_tree : return "open_tree";
        #endif
        #ifdef  __NR_move_mount
         case __NR_move_mount : return "move_mount";
        #endif
        #ifdef  __NR_fsopen
         case __NR_fsopen : return "fsopen";
        #endif
        #ifdef  __NR_fsconfig
         case __NR_fsconfig : return "fsconfig";
        #endif
        #ifdef  __NR_fsmount
         case __NR_fsmount : return "fsmount";
        #endif
        #ifdef  __NR_fspick
         case __NR_fspick : return "fspick";
        #endif
        #ifdef  __NR_pidfd_open
         case __NR_pidfd_open : return "pidfd_open";
        #endif
        #ifdef  __NR_clone3
         case __NR_clone3 : return "clone3";
        #endif
        #ifdef  __NR_syscalls
         case __NR_syscalls : return "syscalls";
        #endif
         default:
            return "unknown_syscall";

    }
}

/*
struct user_regs_struct
{
  unsigned long long regs[31];
  unsigned long long sp;
  unsigned long long pc;
  unsigned long long pstate;
};

*/

/*
       struct iovec {
           void   *iov_base;  // Starting address
           size_t  iov_len;   // Size of the memory pointed to by iov_base.
       };
*/

static void printf_regs(struct user_regs_struct regs)
{
    for (int i = 0; i < 32; i++)
    {
        printf("regs[%d] : %lld\n", i, regs.regs[i]);
    }

    printf("sp : %lld\n", regs.sp);
    printf("pc : %lld\n", regs.pc);
    printf("pstate : %lld\n", regs.pstate);
}

int get_syscall_id(pid_t pid)
{
    // struct user_regs_struct regs;
    int i = -1;
    struct iovec iovector;
    iovector.iov_base = &i;
    iovector.iov_len = sizeof(int);

    // Macro found in elf.h : "ARM system call number"
    if (ptrace(PTRACE_GETREGSET, pid, NT_ARM_SYSTEM_CALL, &iovector) < 0)
    {
        perror("ptrace (getregset)");
        return -1;
    }

    return i;
}

#endif
