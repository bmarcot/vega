#include "timer.h"

void msleep(unsigned int msecs)
{
	timer_create(msecs);
}
