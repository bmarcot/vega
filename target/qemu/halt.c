void v7m_semih_exit(int status);

void __platform_halt(void)
{
	v7m_semih_exit(0);
}
