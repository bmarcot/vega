#include <string.h> //XXX: for strcpy

char *strrev(char *s)
{
	char *t = s;
	char *u = s;

	if ((s == NULL) || (*s == '\0'))
		return s;

	/* u points to the last char in the string */
	while (*(u + 1) != '\0')
		u++;

	/* t moves forward, u moves backward */
	while (t < u) {
		char tmp = *t;
		*t++ = *u;
		*u-- = tmp;
	}

	return s;
}

void strpad(char *buf, char pad_val, int count)
{
	buf[count--] = '\0';
	for (; count >= 0; count--)
		buf[count] = pad_val;
}

//XXX: [Newlib's itoa] error: conflicting types for 'itoa'
char *_itoa(unsigned int u, char *buf, const char *base)
{
	size_t base_len = strlen(base);
	unsigned int i = 0;

	if ((buf == NULL) || (base == NULL) || !base_len)
		return NULL;

	if (!u) {
		strcpy(buf, "0");
		return buf;
	}

	for (; u > 0; i++) {
		buf[i] = base[u % base_len];
		u /= base_len;
	}
	buf[i] = '\0';
	strrev(buf);

	return buf;
}
