/*
 * system/sh.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _SH_H
#define _SH_H

#define BUF_LINE_LEN 128

enum ascii_control_char {
	ASCII_NULL            =  000,
	ASCII_BACKSPACE       =  010,
	ASCII_CARRIAGE_RETURN =  015,
	ASCII_ESCAPE          =  033,
	ASCII_DELETE          = 0177,
};

#endif /* !_SH_H */