#pragma once

#define __noreturn __attribute__ ((__noreturn__))

void v7m_semi_writec(const char *c);
void v7m_semi_write0(const char *s);
__noreturn void v7m_semi_exit(int status);
