extern unsigned int tick_count;

void msleep(unsigned int msecs)
{
	unsigned int t = tick_count;

	while (tick_count < t + msecs)
		;
}
