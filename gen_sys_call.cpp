#include <iostream>
#include <sstream>
#include <seccomp.h>
#include <unistd.h>

void make_syscall_list() {
  std::stringstream ans;
  int nxt = 0;
#define chk ans.seekg(0, std::ios::end); if (ans.tellg() >= nxt) ans << "\n    ", nxt += 80; ans.seekg(0, std::ios::beg)
#define out(str) ans << "{\""#str"\"," << __NR_##str << "},"; chk;
#ifdef __NR_accept
  out(accept)
#endif
#ifdef __NR_accept4
  out(accept4)
#endif
#ifdef __NR_access
  out(access)
#endif
#ifdef __NR_acct
  out(acct)
#endif
#ifdef __NR_add_key
  out(add_key)
#endif
#ifdef __NR_adjtimex
  out(adjtimex)
#endif
#ifdef __NR_afs_syscall
  out(afs_syscall)
#endif
#ifdef __NR_alarm
  out(alarm)
#endif
#ifdef __NR_ARCH_mmap
  out(ARCH_mmap)
#endif
#ifdef __NR_arch_prctl
  out(arch_prctl)
#endif
#ifdef __NR_arch_specific_syscall
  out(arch_specific_syscall)
#endif
#ifdef __NR_arm_fadvise64_64
  out(arm_fadvise64_64)
#endif
#ifdef __NR_arm_sync_file_range
  out(arm_sync_file_range)
#endif
#ifdef __NR_atomic_barrier
  out(atomic_barrier)
#endif
#ifdef __NR_atomic_cmpxchg_32
  out(atomic_cmpxchg_32)
#endif
#ifdef __NR_bdflush
  out(bdflush)
#endif
#ifdef __NR_bfin_spinlock
  out(bfin_spinlock)
#endif
#ifdef __NR_bind
  out(bind)
#endif
#ifdef __NR_bpf
  out(bpf)
#endif
#ifdef __NR_break
  out(break)
#endif
#ifdef __NR_brk
  out(brk)
#endif
#ifdef __NR_cachectl
  out(cachectl)
#endif
#ifdef __NR_cacheflush
  out(cacheflush)
#endif
#ifdef __NR_capget
  out(capget)
#endif
#ifdef __NR_capset
  out(capset)
#endif
#ifdef __NR_chdir
  out(chdir)
#endif
#ifdef __NR_chmod
  out(chmod)
#endif
#ifdef __NR_chown
  out(chown)
#endif
#ifdef __NR_chown16
  out(chown16)
#endif
#ifdef __NR_chown32
  out(chown32)
#endif
#ifdef __NR_chroot
  out(chroot)
#endif
#ifdef __NR_clock_adjtime
  out(clock_adjtime)
#endif
#ifdef __NR_clock_getres
  out(clock_getres)
#endif
#ifdef __NR_clock_gettime
  out(clock_gettime)
#endif
#ifdef __NR_clock_nanosleep
  out(clock_nanosleep)
#endif
#ifdef __NR_clock_settime
  out(clock_settime)
#endif
#ifdef __NR_clone
  out(clone)
#endif
#ifdef __NR_clone2
  out(clone2)
#endif
#ifdef __NR_close
  out(close)
#endif
#ifdef __NR_compat_exit
  out(compat_exit)
#endif
#ifdef __NR_compat_read
  out(compat_read)
#endif
#ifdef __NR_compat_restart_syscall
  out(compat_restart_syscall)
#endif
#ifdef __NR_compat_rt_sigreturn
  out(compat_rt_sigreturn)
#endif
#ifdef __NR_compat_sigreturn
  out(compat_sigreturn)
#endif
#ifdef __NR_compat_syscalls
  out(compat_syscalls)
#endif
#ifdef __NR_compat_write
  out(compat_write)
#endif
#ifdef __NR_connect
  out(connect)
#endif
#ifdef __NR_copy_file_range
  out(copy_file_range)
#endif
#ifdef __NR_creat
  out(creat)
#endif
#ifdef __NR_create_module
  out(create_module)
#endif
#ifdef __NR_delete_module
  out(delete_module)
#endif
#ifdef __NR_dipc
  out(dipc)
#endif
#ifdef __NR_dma_memcpy
  out(dma_memcpy)
#endif
#ifdef __NR_dup
  out(dup)
#endif
#ifdef __NR_dup2
  out(dup2)
#endif
#ifdef __NR_dup3
  out(dup3)
#endif
#ifdef __NR_epoll_create
  out(epoll_create)
#endif
#ifdef __NR_epoll_create1
  out(epoll_create1)
#endif
#ifdef __NR_epoll_ctl
  out(epoll_ctl)
#endif
#ifdef __NR_epoll_ctl_old
  out(epoll_ctl_old)
#endif
#ifdef __NR_epoll_pwait
  out(epoll_pwait)
#endif
#ifdef __NR_epoll_wait
  out(epoll_wait)
#endif
#ifdef __NR_epoll_wait_old
  out(epoll_wait_old)
#endif
#ifdef __NR_eventfd
  out(eventfd)
#endif
#ifdef __NR_eventfd2
  out(eventfd2)
#endif
#ifdef __NR_execv
  out(execv)
#endif
#ifdef __NR_execve
  out(execve)
#endif
#ifdef __NR_execveat
  out(execveat)
#endif
#ifdef __NR_exec_with_loader
  out(exec_with_loader)
#endif
#ifdef __NR_exit
  out(exit)
#endif
#ifdef __NR_exit_group
  out(exit_group)
#endif
#ifdef __NR_faccessat
  out(faccessat)
#endif
#ifdef __NR_fadvise64
  out(fadvise64)
#endif
#ifdef __NR_fadvise64_64
  out(fadvise64_64)
#endif
#ifdef __NR_fallocate
  out(fallocate)
#endif
#ifdef __NR_fanotify_init
  out(fanotify_init)
#endif
#ifdef __NR_fanotify_mark
  out(fanotify_mark)
#endif
#ifdef __NR_fchdir
  out(fchdir)
#endif
#ifdef __NR_fchmod
  out(fchmod)
#endif
#ifdef __NR_fchmodat
  out(fchmodat)
#endif
#ifdef __NR_fchown
  out(fchown)
#endif
#ifdef __NR_fchown16
  out(fchown16)
#endif
#ifdef __NR_fchown32
  out(fchown32)
#endif
#ifdef __NR_fchownat
  out(fchownat)
#endif
#ifdef __NR_fcntl
  out(fcntl)
#endif
#ifdef __NR_fcntl64
  out(fcntl64)
#endif
#ifdef __NR_fdatasync
  out(fdatasync)
#endif
#ifdef __NR_fgetxattr
  out(fgetxattr)
#endif
#ifdef __NR_finit_module
  out(finit_module)
#endif
#ifdef __NR_flistxattr
  out(flistxattr)
#endif
#ifdef __NR_flock
  out(flock)
#endif
#ifdef __NR_fork
  out(fork)
#endif
#ifdef __NR_fremovexattr
  out(fremovexattr)
#endif
#ifdef __NR_fsetxattr
  out(fsetxattr)
#endif
#ifdef __NR_fstat
  out(fstat)
#endif
#ifdef __NR_fstat64
  out(fstat64)
#endif
#ifdef __NR_fstatat64
  out(fstatat64)
#endif
#ifdef __NR_fstatfs
  out(fstatfs)
#endif
#ifdef __NR_fstatfs64
  out(fstatfs64)
#endif
#ifdef __NR_fsync
  out(fsync)
#endif
#ifdef __NR_ftime
  out(ftime)
#endif
#ifdef __NR_ftruncate
  out(ftruncate)
#endif
#ifdef __NR_ftruncate64
  out(ftruncate64)
#endif
#ifdef __NR_futex
  out(futex)
#endif
#ifdef __NR_futimesat
  out(futimesat)
#endif
#ifdef __NR_getcount
  out(getcount)
#endif
#ifdef __NR_getcpu
  out(getcpu)
#endif
#ifdef __NR_getcwd
  out(getcwd)
#endif
#ifdef __NR_getdents
  out(getdents)
#endif
#ifdef __NR_getdents64
  out(getdents64)
#endif
#ifdef __NR_getdomainname
  out(getdomainname)
#endif
#ifdef __NR_getdtablesize
  out(getdtablesize)
#endif
#ifdef __NR_getegid
  out(getegid)
#endif
#ifdef __NR_getegid16
  out(getegid16)
#endif
#ifdef __NR_getegid32
  out(getegid32)
#endif
#ifdef __NR_geteuid
  out(geteuid)
#endif
#ifdef __NR_geteuid16
  out(geteuid16)
#endif
#ifdef __NR_geteuid32
  out(geteuid32)
#endif
#ifdef __NR_getgid
  out(getgid)
#endif
#ifdef __NR_getgid16
  out(getgid16)
#endif
#ifdef __NR_getgid32
  out(getgid32)
#endif
#ifdef __NR_getgroups
  out(getgroups)
#endif
#ifdef __NR_getgroups16
  out(getgroups16)
#endif
#ifdef __NR_getgroups32
  out(getgroups32)
#endif
#ifdef __NR_gethostname
  out(gethostname)
#endif
#ifdef __NR_getitimer
  out(getitimer)
#endif
#ifdef __NR_get_kernel_syms
  out(get_kernel_syms)
#endif
#ifdef __NR_get_mempolicy
  out(get_mempolicy)
#endif
#ifdef __NR_getpagesize
  out(getpagesize)
#endif
#ifdef __NR_getpeername
  out(getpeername)
#endif
#ifdef __NR_getpgid
  out(getpgid)
#endif
#ifdef __NR_getpgrp
  out(getpgrp)
#endif
#ifdef __NR_getpid
  out(getpid)
#endif
#ifdef __NR_getpmsg
  out(getpmsg)
#endif
#ifdef __NR_getppid
  out(getppid)
#endif
#ifdef __NR_getpriority
  out(getpriority)
#endif
#ifdef __NR_getprocinfo
  out(getprocinfo)
#endif
#ifdef __NR_getrandom
  out(getrandom)
#endif
#ifdef __NR_getresgid
  out(getresgid)
#endif
#ifdef __NR_getresgid16
  out(getresgid16)
#endif
#ifdef __NR_getresgid32
  out(getresgid32)
#endif
#ifdef __NR_getresuid
  out(getresuid)
#endif
#ifdef __NR_getresuid16
  out(getresuid16)
#endif
#ifdef __NR_getresuid32
  out(getresuid32)
#endif
#ifdef __NR_getrlimit
  out(getrlimit)
#endif
#ifdef __NR_get_robust_list
  out(get_robust_list)
#endif
#ifdef __NR_getrusage
  out(getrusage)
#endif
#ifdef __NR_getsid
  out(getsid)
#endif
#ifdef __NR_getsockname
  out(getsockname)
#endif
#ifdef __NR_getsockopt
  out(getsockopt)
#endif
#ifdef __NR_get_thread_area
  out(get_thread_area)
#endif
#ifdef __NR_gettid
  out(gettid)
#endif
#ifdef __NR_gettimeofday
  out(gettimeofday)
#endif
#ifdef __NR_getuid
  out(getuid)
#endif
#ifdef __NR_getuid16
  out(getuid16)
#endif
#ifdef __NR_getuid32
  out(getuid32)
#endif
#ifdef __NR_getunwind
  out(getunwind)
#endif
#ifdef __NR_getuserweight
  out(getuserweight)
#endif
#ifdef __NR_getxattr
  out(getxattr)
#endif
#ifdef __NR_getxgid
  out(getxgid)
#endif
#ifdef __NR_getxpid
  out(getxpid)
#endif
#ifdef __NR_getxuid
  out(getxuid)
#endif
#ifdef __NR_gtty
  out(gtty)
#endif
#ifdef __NR_idle
  out(idle)
#endif
#ifdef __NR_init_module
  out(init_module)
#endif
#ifdef __NR_inotify_add_watch
  out(inotify_add_watch)
#endif
#ifdef __NR_inotify_init
  out(inotify_init)
#endif
#ifdef __NR_inotify_init1
  out(inotify_init1)
#endif
#ifdef __NR_inotify_rm_watch
  out(inotify_rm_watch)
#endif
#ifdef __NR_io_cancel
  out(io_cancel)
#endif
#ifdef __NR_ioctl
  out(ioctl)
#endif
#ifdef __NR_io_destroy
  out(io_destroy)
#endif
#ifdef __NR_io_getevents
  out(io_getevents)
#endif
#ifdef __NR_ioperm
  out(ioperm)
#endif
#ifdef __NR_iopl
  out(iopl)
#endif
#ifdef __NR_ioprio_get
  out(ioprio_get)
#endif
#ifdef __NR_ioprio_set
  out(ioprio_set)
#endif
#ifdef __NR_io_setup
  out(io_setup)
#endif
#ifdef __NR_io_submit
  out(io_submit)
#endif
#ifdef __NR_ipc
  out(ipc)
#endif
#ifdef __NR_kcmp
  out(kcmp)
#endif
#ifdef __NR_kern_features
  out(kern_features)
#endif
#ifdef __NR_kexec_file_load
  out(kexec_file_load)
#endif
#ifdef __NR_kexec_load
  out(kexec_load)
#endif
#ifdef __NR_keyctl
  out(keyctl)
#endif
#ifdef __NR_kill
  out(kill)
#endif
#ifdef __NR_lchown
  out(lchown)
#endif
#ifdef __NR_lchown32
  out(lchown32)
#endif
#ifdef __NR_lgetxattr
  out(lgetxattr)
#endif
#ifdef __NR_link
  out(link)
#endif
#ifdef __NR_linkat
  out(linkat)
#endif
#ifdef __NR_listen
  out(listen)
#endif
#ifdef __NR_listxattr
  out(listxattr)
#endif
#ifdef __NR_llistxattr
  out(llistxattr)
#endif
#ifdef __NR_llseek
  out(llseek)
#endif
#ifdef __NR__llseek
  out(_llseek)
#endif
#ifdef __NR_lock
  out(lock)
#endif
#ifdef __NR_lookup_dcookie
  out(lookup_dcookie)
#endif
#ifdef __NR_lremovexattr
  out(lremovexattr)
#endif
#ifdef __NR_lseek
  out(lseek)
#endif
#ifdef __NR_lsetxattr
  out(lsetxattr)
#endif
#ifdef __NR_lstat
  out(lstat)
#endif
#ifdef __NR_lstat64
  out(lstat64)
#endif
#ifdef __NR_madvise
  out(madvise)
#endif
#ifdef __NR_madvise1
  out(madvise1)
#endif
#ifdef __NR_max
  out(max)
#endif
#ifdef __NR_mbind
  out(mbind)
#endif
#ifdef __NR_membarrier
  out(membarrier)
#endif
#ifdef __NR_memfd_create
  out(memfd_create)
#endif
#ifdef __NR_memory_ordering
  out(memory_ordering)
#endif
#ifdef __NR_migrate_pages
  out(migrate_pages)
#endif
#ifdef __NR_mincore
  out(mincore)
#endif
#ifdef __NR_mkdir
  out(mkdir)
#endif
#ifdef __NR_mkdirat
  out(mkdirat)
#endif
#ifdef __NR_mknod
  out(mknod)
#endif
#ifdef __NR_mknodat
  out(mknodat)
#endif
#ifdef __NR_mlock
  out(mlock)
#endif
#ifdef __NR_mlock2
  out(mlock2)
#endif
#ifdef __NR_mlockall
  out(mlockall)
#endif
#ifdef __NR_mmap
  out(mmap)
#endif
#ifdef __NR_mmap2
  out(mmap2)
#endif
#ifdef __NR_mmap_4koff
  out(mmap_4koff)
#endif
#ifdef __NR_mmap_pgoff
  out(mmap_pgoff)
#endif
#ifdef __NR_modify_ldt
  out(modify_ldt)
#endif
#ifdef __NR_mount
  out(mount)
#endif
#ifdef __NR_move_pages
  out(move_pages)
#endif
#ifdef __NR_mprotect
  out(mprotect)
#endif
#ifdef __NR_mpx
  out(mpx)
#endif
#ifdef __NR_mq_getsetattr
  out(mq_getsetattr)
#endif
#ifdef __NR_mq_notify
  out(mq_notify)
#endif
#ifdef __NR_mq_open
  out(mq_open)
#endif
#ifdef __NR_mq_timedreceive
  out(mq_timedreceive)
#endif
#ifdef __NR_mq_timedsend
  out(mq_timedsend)
#endif
#ifdef __NR_mq_unlink
  out(mq_unlink)
#endif
#ifdef __NR_mremap
  out(mremap)
#endif
#ifdef __NR_msgctl
  out(msgctl)
#endif
#ifdef __NR_msgget
  out(msgget)
#endif
#ifdef __NR_msgrcv
  out(msgrcv)
#endif
#ifdef __NR_msgsnd
  out(msgsnd)
#endif
#ifdef __NR_msync
  out(msync)
#endif
#ifdef __NR_multiplexer
  out(multiplexer)
#endif
#ifdef __NR_munlock
  out(munlock)
#endif
#ifdef __NR_munlockall
  out(munlockall)
#endif
#ifdef __NR_munmap
  out(munmap)
#endif
#ifdef __NR_myinit
  out(myinit)
#endif
#ifdef __NR_myservice
  out(myservice)
#endif
#ifdef __NR_name_to_handle_at
  out(name_to_handle_at)
#endif
#ifdef __NR_nanosleep
  out(nanosleep)
#endif
#ifdef __NR_newfstatat
  out(newfstatat)
#endif
#ifdef __NR__newselect
  out(_newselect)
#endif
#ifdef __NR_nfsservctl
  out(nfsservctl)
#endif
#ifdef __NR_nice
  out(nice)
#endif
#ifdef __NR_ni_syscall
  out(ni_syscall)
#endif
#ifdef __NR_old_adjtimex
  out(old_adjtimex)
#endif
#ifdef __NR_oldfstat
  out(oldfstat)
#endif
#ifdef __NR_oldlstat
  out(oldlstat)
#endif
#ifdef __NR_old_mmap
  out(old_mmap)
#endif
#ifdef __NR_old_mmap_pgoff
  out(old_mmap_pgoff)
#endif
#ifdef __NR_oldolduname
  out(oldolduname)
#endif
#ifdef __NR_oldselect
  out(oldselect)
#endif
#ifdef __NR_oldstat
  out(oldstat)
#endif
#ifdef __NR_oldumount
  out(oldumount)
#endif
#ifdef __NR_olduname
  out(olduname)
#endif
#ifdef __NR_oldwait4
  out(oldwait4)
#endif
#ifdef __NR_open
  out(open)
#endif
#ifdef __NR_openat
  out(openat)
#endif
#ifdef __NR_open_by_handle_at
  out(open_by_handle_at)
#endif
#ifdef __NR_or1k_atomic
  out(or1k_atomic)
#endif
#ifdef __NR_pause
  out(pause)
#endif
#ifdef __NR_pciconfig_iobase
  out(pciconfig_iobase)
#endif
#ifdef __NR_pciconfig_read
  out(pciconfig_read)
#endif
#ifdef __NR_pciconfig_write
  out(pciconfig_write)
#endif
#ifdef __NR_perfctr
  out(perfctr)
#endif
#ifdef __NR_perf_event_open
  out(perf_event_open)
#endif
#ifdef __NR_perfmonctl
  out(perfmonctl)
#endif
#ifdef __NR_personality
  out(personality)
#endif
#ifdef __NR_pipe
  out(pipe)
#endif
#ifdef __NR_pipe2
  out(pipe2)
#endif
#ifdef __NR_pivotroot
  out(pivotroot)
#endif
#ifdef __NR_pivot_root
  out(pivot_root)
#endif
#ifdef __NR_pkey_alloc
  out(pkey_alloc)
#endif
#ifdef __NR_pkey_free
  out(pkey_free)
#endif
#ifdef __NR_pkey_mprotect
  out(pkey_mprotect)
#endif
#ifdef __NR_poll
  out(poll)
#endif
#ifdef __NR_ppoll
  out(ppoll)
#endif
#ifdef __NR_prctl
  out(prctl)
#endif
#ifdef __NR_pread
  out(pread)
#endif
#ifdef __NR_pread64
  out(pread64)
#endif
#ifdef __NR_preadv
  out(preadv)
#endif
#ifdef __NR_preadv2
  out(preadv2)
#endif
#ifdef __NR_printargs
  out(printargs)
#endif
#ifdef __NR_prlimit64
  out(prlimit64)
#endif
#ifdef __NR_process_vm_readv
  out(process_vm_readv)
#endif
#ifdef __NR_process_vm_writev
  out(process_vm_writev)
#endif
#ifdef __NR_prof
  out(prof)
#endif
#ifdef __NR_profil
  out(profil)
#endif
#ifdef __NR_pselect6
  out(pselect6)
#endif
#ifdef __NR_ptrace
  out(ptrace)
#endif
#ifdef __NR_putpmsg
  out(putpmsg)
#endif
#ifdef __NR_pwrite
  out(pwrite)
#endif
#ifdef __NR_pwrite64
  out(pwrite64)
#endif
#ifdef __NR_pwritev
  out(pwritev)
#endif
#ifdef __NR_pwritev2
  out(pwritev2)
#endif
#ifdef __NR_query_module
  out(query_module)
#endif
#ifdef __NR_quotactl
  out(quotactl)
#endif
#ifdef __NR_read
  out(read)
#endif
#ifdef __NR_readahead
  out(readahead)
#endif
#ifdef __NR_readdir
  out(readdir)
#endif
#ifdef __NR_readlink
  out(readlink)
#endif
#ifdef __NR_readlinkat
  out(readlinkat)
#endif
#ifdef __NR_readv
  out(readv)
#endif
#ifdef __NR_reboot
  out(reboot)
#endif
#ifdef __NR_recv
  out(recv)
#endif
#ifdef __NR_recvfrom
  out(recvfrom)
#endif
#ifdef __NR_recvmmsg
  out(recvmmsg)
#endif
#ifdef __NR_recvmsg
  out(recvmsg)
#endif
#ifdef __NR_remap_file_pages
  out(remap_file_pages)
#endif
#ifdef __NR_removexattr
  out(removexattr)
#endif
#ifdef __NR_rename
  out(rename)
#endif
#ifdef __NR_renameat
  out(renameat)
#endif
#ifdef __NR_renameat2
  out(renameat2)
#endif
#ifdef __NR_request_key
  out(request_key)
#endif
#ifdef __NR_reserved152
  out(reserved152)
#endif
#ifdef __NR_reserved153
  out(reserved153)
#endif
#ifdef __NR_reserved244
  out(reserved244)
#endif
#ifdef __NR_reserved253
  out(reserved253)
#endif
#ifdef __NR_reserved286
  out(reserved286)
#endif
#ifdef __NR_restart_syscall
  out(restart_syscall)
#endif
#ifdef __NR_rmdir
  out(rmdir)
#endif
#ifdef __NR_rtas
  out(rtas)
#endif
#ifdef __NR_rt_sigaction
  out(rt_sigaction)
#endif
#ifdef __NR_rt_sigpending
  out(rt_sigpending)
#endif
#ifdef __NR_rt_sigprocmask
  out(rt_sigprocmask)
#endif
#ifdef __NR_rt_sigqueueinfo
  out(rt_sigqueueinfo)
#endif
#ifdef __NR_rt_sigreturn
  out(rt_sigreturn)
#endif
#ifdef __NR_rt_sigsuspend
  out(rt_sigsuspend)
#endif
#ifdef __NR_rt_sigtimedwait
  out(rt_sigtimedwait)
#endif
#ifdef __NR_rt_tgsigqueueinfo
  out(rt_tgsigqueueinfo)
#endif
#ifdef __NR_s390_pci_mmio_read
  out(s390_pci_mmio_read)
#endif
#ifdef __NR_s390_pci_mmio_write
  out(s390_pci_mmio_write)
#endif
#ifdef __NR_s390_runtime_instr
  out(s390_runtime_instr)
#endif
#ifdef __NR_sched_getaffinity
  out(sched_getaffinity)
#endif
#ifdef __NR_sched_get_affinity
  out(sched_get_affinity)
#endif
#ifdef __NR_sched_getattr
  out(sched_getattr)
#endif
#ifdef __NR_sched_getparam
  out(sched_getparam)
#endif
#ifdef __NR_sched_get_priority_max
  out(sched_get_priority_max)
#endif
#ifdef __NR_sched_get_priority_min
  out(sched_get_priority_min)
#endif
#ifdef __NR_sched_getscheduler
  out(sched_getscheduler)
#endif
#ifdef __NR_sched_rr_get_interval
  out(sched_rr_get_interval)
#endif
#ifdef __NR_sched_setaffinity
  out(sched_setaffinity)
#endif
#ifdef __NR_sched_set_affinity
  out(sched_set_affinity)
#endif
#ifdef __NR_sched_setattr
  out(sched_setattr)
#endif
#ifdef __NR_sched_setparam
  out(sched_setparam)
#endif
#ifdef __NR_sched_setscheduler
  out(sched_setscheduler)
#endif
#ifdef __NR_sched_simple_rr_getquantum
  out(sched_simple_rr_getquantum)
#endif
#ifdef __NR_sched_simple_rr_setquantum
  out(sched_simple_rr_setquantum)
#endif
#ifdef __NR_sched_yield
  out(sched_yield)
#endif
#ifdef __NR_seccomp
  out(seccomp)
#endif
#ifdef __NR_seccomp_exit_32
  out(seccomp_exit_32)
#endif
#ifdef __NR_seccomp_read_32
  out(seccomp_read_32)
#endif
#ifdef __NR_seccomp_sigreturn_32
  out(seccomp_sigreturn_32)
#endif
#ifdef __NR_seccomp_write_32
  out(seccomp_write_32)
#endif
#ifdef __NR_security
  out(security)
#endif
#ifdef __NR_select
  out(select)
#endif
#ifdef __NR_semctl
  out(semctl)
#endif
#ifdef __NR_semget
  out(semget)
#endif
#ifdef __NR_semop
  out(semop)
#endif
#ifdef __NR_semtimedop
  out(semtimedop)
#endif
#ifdef __NR_send
  out(send)
#endif
#ifdef __NR_sendfile
  out(sendfile)
#endif
#ifdef __NR_sendfile64
  out(sendfile64)
#endif
#ifdef __NR_sendmmsg
  out(sendmmsg)
#endif
#ifdef __NR_sendmsg
  out(sendmsg)
#endif
#ifdef __NR_sendto
  out(sendto)
#endif
#ifdef __NR_setdomainname
  out(setdomainname)
#endif
#ifdef __NR_setfsgid
  out(setfsgid)
#endif
#ifdef __NR_setfsgid16
  out(setfsgid16)
#endif
#ifdef __NR_setfsgid32
  out(setfsgid32)
#endif
#ifdef __NR_setfsuid
  out(setfsuid)
#endif
#ifdef __NR_setfsuid16
  out(setfsuid16)
#endif
#ifdef __NR_setfsuid32
  out(setfsuid32)
#endif
#ifdef __NR_setgid
  out(setgid)
#endif
#ifdef __NR_setgid16
  out(setgid16)
#endif
#ifdef __NR_setgid32
  out(setgid32)
#endif
#ifdef __NR_setgroups
  out(setgroups)
#endif
#ifdef __NR_setgroups16
  out(setgroups16)
#endif
#ifdef __NR_setgroups32
  out(setgroups32)
#endif
#ifdef __NR_sethae
  out(sethae)
#endif
#ifdef __NR_sethostname
  out(sethostname)
#endif
#ifdef __NR_setitimer
  out(setitimer)
#endif
#ifdef __NR_set_mempolicy
  out(set_mempolicy)
#endif
#ifdef __NR_setns
  out(setns)
#endif
#ifdef __NR_setpgid
  out(setpgid)
#endif
#ifdef __NR_setpgrp
  out(setpgrp)
#endif
#ifdef __NR_setpriority
  out(setpriority)
#endif
#ifdef __NR_setregid
  out(setregid)
#endif
#ifdef __NR_setregid16
  out(setregid16)
#endif
#ifdef __NR_setregid32
  out(setregid32)
#endif
#ifdef __NR_setresgid
  out(setresgid)
#endif
#ifdef __NR_setresgid16
  out(setresgid16)
#endif
#ifdef __NR_setresgid32
  out(setresgid32)
#endif
#ifdef __NR_setresuid
  out(setresuid)
#endif
#ifdef __NR_setresuid16
  out(setresuid16)
#endif
#ifdef __NR_setresuid32
  out(setresuid32)
#endif
#ifdef __NR_setreuid
  out(setreuid)
#endif
#ifdef __NR_setreuid16
  out(setreuid16)
#endif
#ifdef __NR_setreuid32
  out(setreuid32)
#endif
#ifdef __NR_setrlimit
  out(setrlimit)
#endif
#ifdef __NR_set_robust_list
  out(set_robust_list)
#endif
#ifdef __NR_setsid
  out(setsid)
#endif
#ifdef __NR_setsockopt
  out(setsockopt)
#endif
#ifdef __NR_set_thread_area
  out(set_thread_area)
#endif
#ifdef __NR_set_tid_address
  out(set_tid_address)
#endif
#ifdef __NR_settimeofday
  out(settimeofday)
#endif
#ifdef __NR_setuid
  out(setuid)
#endif
#ifdef __NR_setuid16
  out(setuid16)
#endif
#ifdef __NR_setuid32
  out(setuid32)
#endif
#ifdef __NR_setup
  out(setup)
#endif
#ifdef __NR_setuserweight
  out(setuserweight)
#endif
#ifdef __NR_setxattr
  out(setxattr)
#endif
#ifdef __NR_set_zone_reclaim
  out(set_zone_reclaim)
#endif
#ifdef __NR_sgetmask
  out(sgetmask)
#endif
#ifdef __NR_shmat
  out(shmat)
#endif
#ifdef __NR_shmatcall
  out(shmatcall)
#endif
#ifdef __NR_shmctl
  out(shmctl)
#endif
#ifdef __NR_shmdt
  out(shmdt)
#endif
#ifdef __NR_shmget
  out(shmget)
#endif
#ifdef __NR_show
  out(show)
#endif
#ifdef __NR_shutdown
  out(shutdown)
#endif
#ifdef __NR_sigaction
  out(sigaction)
#endif
#ifdef __NR_sigaltstack
  out(sigaltstack)
#endif
#ifdef __NR_signal
  out(signal)
#endif
#ifdef __NR_signalfd
  out(signalfd)
#endif
#ifdef __NR_signalfd4
  out(signalfd4)
#endif
#ifdef __NR_sigpending
  out(sigpending)
#endif
#ifdef __NR_sigprocmask
  out(sigprocmask)
#endif
#ifdef __NR_sigreturn
  out(sigreturn)
#endif
#ifdef __NR_sigsuspend
  out(sigsuspend)
#endif
#ifdef __NR_socket
  out(socket)
#endif
#ifdef __NR_socketcall
  out(socketcall)
#endif
#ifdef __NR_socketpair
  out(socketpair)
#endif
#ifdef __NR_spill
  out(spill)
#endif
#ifdef __NR_splice
  out(splice)
#endif
#ifdef __NR_spu_create
  out(spu_create)
#endif
#ifdef __NR_spu_run
  out(spu_run)
#endif
#ifdef __NR_sram_alloc
  out(sram_alloc)
#endif
#ifdef __NR_sram_free
  out(sram_free)
#endif
#ifdef __NR_ssetmask
  out(ssetmask)
#endif
#ifdef __NR_stat
  out(stat)
#endif
#ifdef __NR_stat64
  out(stat64)
#endif
#ifdef __NR_statfs
  out(statfs)
#endif
#ifdef __NR_statfs64
  out(statfs64)
#endif
#ifdef __NR_statx
  out(statx)
#endif
#ifdef __NR_stime
  out(stime)
#endif
#ifdef __NR_streams1
  out(streams1)
#endif
#ifdef __NR_streams2
  out(streams2)
#endif
#ifdef __NR_stty
  out(stty)
#endif
#ifdef __NR_subpage_prot
  out(subpage_prot)
#endif
#ifdef __NR_subtract
  out(subtract)
#endif
#ifdef __NR_swapcontext
  out(swapcontext)
#endif
#ifdef __NR_swapoff
  out(swapoff)
#endif
#ifdef __NR_swapon
  out(swapon)
#endif
#ifdef __NR_switch_endian
  out(switch_endian)
#endif
#ifdef __NR_symlink
  out(symlink)
#endif
#ifdef __NR_symlinkat
  out(symlinkat)
#endif
#ifdef __NR_sync
  out(sync)
#endif
#ifdef __NR_sync_file_range
  out(sync_file_range)
#endif
#ifdef __NR_sync_file_range2
  out(sync_file_range2)
#endif
#ifdef __NR_syncfs
  out(syncfs)
#endif
#ifdef __NR_syscall
  out(syscall)
#endif
#ifdef __NR_SYSCALL_BASE
  out(SYSCALL_BASE)
#endif
#ifdef __NR_syscall_compat_max
  out(syscall_compat_max)
#endif
#ifdef __NR_syscall_count
  out(syscall_count)
#endif
#ifdef __NR_syscall_max
  out(syscall_max)
#endif
#ifdef __NR_syscalls
  out(syscalls)
#endif
#ifdef __NR_sysctl
  out(sysctl)
#endif
#ifdef __NR__sysctl
  out(_sysctl)
#endif
#ifdef __NR_sys_debug_setcontext
  out(sys_debug_setcontext)
#endif
#ifdef __NR_sysfs
  out(sysfs)
#endif
#ifdef __NR_sysinfo
  out(sysinfo)
#endif
#ifdef __NR_sys_kexec_load
  out(sys_kexec_load)
#endif
#ifdef __NR_syslog
  out(syslog)
#endif
#ifdef __NR_sysmips
  out(sysmips)
#endif
#ifdef __NR_sys_setaltroot
  out(sys_setaltroot)
#endif
#ifdef __NR_tas
  out(tas)
#endif
#ifdef __NR_tee
  out(tee)
#endif
#ifdef __NR_tgkill
  out(tgkill)
#endif
#ifdef __NR_time
  out(time)
#endif
#ifdef __NR_timer_create
  out(timer_create)
#endif
#ifdef __NR_timer_delete
  out(timer_delete)
#endif
#ifdef __NR_timerfd
  out(timerfd)
#endif
#ifdef __NR_timerfd_create
  out(timerfd_create)
#endif
#ifdef __NR_timerfd_gettime
  out(timerfd_gettime)
#endif
#ifdef __NR_timerfd_settime
  out(timerfd_settime)
#endif
#ifdef __NR_timer_getoverrun
  out(timer_getoverrun)
#endif
#ifdef __NR_timer_gettime
  out(timer_gettime)
#endif
#ifdef __NR_timer_settime
  out(timer_settime)
#endif
#ifdef __NR_times
  out(times)
#endif
#ifdef __NR_tkill
  out(tkill)
#endif
#ifdef __NR_truncate
  out(truncate)
#endif
#ifdef __NR_truncate64
  out(truncate64)
#endif
#ifdef __NR_tuxcall
  out(tuxcall)
#endif
#ifdef __NR_ugetrlimit
  out(ugetrlimit)
#endif
#ifdef __NR_ulimit
  out(ulimit)
#endif
#ifdef __NR_umask
  out(umask)
#endif
#ifdef __NR_umount
  out(umount)
#endif
#ifdef __NR_umount2
  out(umount2)
#endif
#ifdef __NR_uname
  out(uname)
#endif
#ifdef __NR_unlink
  out(unlink)
#endif
#ifdef __NR_unlinkat
  out(unlinkat)
#endif
#ifdef __NR_unshare
  out(unshare)
#endif
#ifdef __NR_uselib
  out(uselib)
#endif
#ifdef __NR_userfaultfd
  out(userfaultfd)
#endif
#ifdef __NR_ustat
  out(ustat)
#endif
#ifdef __NR_utime
  out(utime)
#endif
#ifdef __NR_utimensat
  out(utimensat)
#endif
#ifdef __NR_utimes
  out(utimes)
#endif
#ifdef __NR_utrap_install
  out(utrap_install)
#endif
#ifdef __NR_vfork
  out(vfork)
#endif
#ifdef __NR_vhangup
  out(vhangup)
#endif
#ifdef __NR_vm86
  out(vm86)
#endif
#ifdef __NR_vm86old
  out(vm86old)
#endif
#ifdef __NR_vmsplice
  out(vmsplice)
#endif
#ifdef __NR_vserver
  out(vserver)
#endif
#ifdef __NR_wait4
  out(wait4)
#endif
#ifdef __NR_waitid
  out(waitid)
#endif
#ifdef __NR_waitpid
  out(waitpid)
#endif
#ifdef __NR_write
  out(write)
#endif
#ifdef __NR_writev
  out(writev)
#endif
#ifdef __NR_xtensa
  out(xtensa)
#endif
  std::string str = ans.str();
  str.pop_back();
  std::cout << "#include \"syscall_list.h\"\n\nconst std::unordered_map<std::string, int> syscall_num = {" << str << "};\n";
}

int main()
{
  make_syscall_list();
}
