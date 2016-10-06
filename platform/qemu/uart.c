#include <if/uart.h>
#include <kernel/device.h>
#include <kernel/fs/vnode.h>

struct lm3s6965_uart {
	unsigned int uartdr;
	char pad_0[0x14];
	unsigned int uartfr;
	char pad_1[0x10];
	unsigned int uartlcrh;
	unsigned int uartctl;
};

static volatile struct lm3s6965_uart *const uart0 = (struct lm3s6965_uart *const) 0x4000c000;

void __uart_enable(void)
{
	uart0->uartctl |= 1;
	uart0->uartlcrh |= (3 << 5);
}

void __uart_putchar(char c)
{
	while (uart0->uartfr & (1 << 3))
		;
	uart0->uartdr = c;
}

void __uart_putstring(const char *s)
{
	while (*s) {
		if ('\n' == *s)
			__uart_putchar('\r');
		__uart_putchar(*s++);
	}
}

void __uart_init(void)
{
	__uart_enable();
}

static void lm3s6965_init(struct lm3s6965_uart *uart)
{
	uart->uartctl |= 1;
	uart->uartlcrh |= (3 << 5);
}

static void lm3s6965_putchar(char c, struct lm3s6965_uart *uart)
{
	while (uart->uartfr & (1 << 3))
		;
	uart->uartdr = c;
}


//int qemu_uart_open(struct vnode *vp, int flags)
int qemu_uart_open(struct device *dev, int flags)
{
	(void)flags;

	lm3s6965_init(dev->drvdata);

	return 0;
}

//int qemu_uart_write(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
int qemu_uart_write(struct device *dev, void *buf, size_t count, off_t off, size_t *n)
{
	(void)off;

	size_t initial_count = count;
	const char *bufp = buf;

	for (; count; count--) {
		if ('\n' == *bufp)
			lm3s6965_putchar('\r', dev->drvdata);
		lm3s6965_putchar(*bufp++, dev->drvdata);
	}
	*n = initial_count - count;

	return 0;
}

struct cdevops qemu_uart_cdevops = {
	.cdevop_open = qemu_uart_open,
	.cdevop_write = qemu_uart_write,
};

struct cdev qemu_uart_cdev = {
	.cdev_name = "lm3s6965_uart",
	.cdev_ops = &qemu_uart_cdevops,
};

#define LM3S6965_UART0_BASE  0x4000c000

struct device qemu_uart0_dev = {
	.char_dev = &qemu_uart_cdev,
	.drvdata = (void *)LM3S6965_UART0_BASE,
};

int qemu_uart_init(void)
{
	mkdev(&qemu_uart0_dev, "ttyS0");

	return 0;
}
