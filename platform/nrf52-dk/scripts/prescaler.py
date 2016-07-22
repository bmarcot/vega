#!/usr/bin/env python

xs = []

for i in range(32, 0, -1):
    mod = (i % 32)
    if mod == 0:
        xs.append(9)
    elif mod == 16:
        xs.append(8)
    elif mod % 8 == 0:
        xs.append(7)
    elif mod % 4 == 0:
        xs.append(6)
    elif mod % 2 == 0:
        xs.append(5)
    else:
        xs.append(4)

print ', '.join(map(lambda x: str(x), xs))
