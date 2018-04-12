#pragma once

#define __noreturn __attribute__ ((__noreturn__))

__noreturn void v7m_semi_exit(int status);
void v7m_semi_write0(const char *s);
