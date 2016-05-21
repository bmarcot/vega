#ifndef UNISTD_H
#define UNISTD_H

enum sc_varname {
	PAGESIZE,
	_SC_CLK_TCK
};

long sysconf(int name);
long sysconf_1(int name);

#endif /* !UNISTD_H */
