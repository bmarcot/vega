#!/usr/bin/env python

xs = [
    # <pthread.h>
    'pthread_exit',
    'pthread_self',
    'pthread_yield',
    'pthread_create',
    'pthread_join',
    'pthread_mutex_lock',
    'pthread_mutex_unlock',
    'pthread_cond_signal',
    'pthread_cond_wait',

    # <time.h>
    'timer_create',

    # <unistd.h>
    'msleep',
    'sysconf',

    # <signal.h>
    'sigaction',
    'raise',
    'sigqueue',

    # <fcntl.h>, <unistd.h>, <sys/mount.h>...
    'open',
    'read',
    'write',
    'seek',
    'stat',
    'mount',
]

import datetime
print('//XXX: GENERATED TABLE, DO NOT EDIT FROM HERE!')
print('//XXX: Change definitions in scripts/sysc.py')
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
