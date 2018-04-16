#include <libsemi/v7m_semi.h>

void __platform_halt(void)
{
	v7m_semi_exit(0);
}
