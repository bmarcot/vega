#include "unistd.h"
#include "systick.h"

long sysconf_1(int name)
{
	switch (name) {
	case PAGESIZE:
		return 2048;
	case _SC_CLK_TCK:
		return 1000 / SYSTICK_PERIOD_IN_MSECS;
	}

	return -1;
}
