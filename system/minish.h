/*
 * minish.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _MINISH_H
#define _MINISH_H

#define MINISH_LINE_MAX 128

enum ascii_control_char {
	ASCII_NULL            =  000,
	ASCII_BACKSPACE       =  010,
	ASCII_CARRIAGE_RETURN =  015,
	ASCII_ESCAPE          =  033,
	ASCII_DELETE          = 0177,
};

#endif /* !_MINISH_H */
