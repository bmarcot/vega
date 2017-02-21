#!/usr/bin/env python

xs = [
    # <pthread.h>
    'pthread_exit',
    'pthread_self',
    'pthread_yield',
    'pthread_create',
    'pthread_join',
    'pthread_detach',
    'pthread_mutex_lock',
    'pthread_mutex_unlock',
    'pthread_cond_signal',
    'pthread_cond_wait',

    # <time.h>
    'timer_create',
    'timer_settime',
    'timer_gettime',

    # <unistd.h>
    'msleep',
    'sysconf',

    # <signal.h>
    'sigaction',
    'raise',
    'sigqueue',

    # <fcntl.h>, <unistd.h>, <sys/mount.h>...
    'open',
    'close',
    'read',
    'write',
    'lseek',
    'stat',
    'mount',
    'readdir_r',
]

import datetime
print('//XXX: GENERATED TABLE, DO NOT EDIT FROM HERE!')
print('//XXX: Change definitions in scripts/gen-syscalls.py')
print('//XXX: Created on ' +
      datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))

print('')

for x in list(enumerate(xs)):
    name = 'sys_' + x[1]
    print('[{}] = {},'.format(name.upper(), name))

print('')

for x in list(enumerate(xs)):
    name = 'sys_' + x[1]
    print('#define {} {}'.format(name.upper(), x[0]))

print('')

for x in list(enumerate(xs)):
    name = 'sys_' + x[1]
    print('int {}();'.format(name))
