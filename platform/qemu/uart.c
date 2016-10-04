#include <if/uart.h>

struct lm3s6965_uart {
	unsigned int uartdr;
	char pad[0x28];
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
	while ((*(volatile int *) 0x4000c018) & (1 << 3))
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

/* ---- */

#include <kernel/fs/vnode.h>
#include <kernel/fs/fs.h>

int dev_uart_open(struct vnode *vp, int flags);
int dev_uart_write(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n);

extern struct vnode vn_dev;

static const struct vnodeops dev_uart_vops = {
	.vop_open = dev_uart_open,
	.vop_write = dev_uart_write,
};

struct vnode vn_dev_uart = {
	.v_path = "ttyS0",
	.v_type = VCHR,
	.v_head = LIST_HEAD_INIT(vn_dev_uart.v_head),
	.v_ops = &dev_uart_vops
};

void build_uart_vn(void)
{
	vn_insert(&vn_dev_uart, &vn_dev);
}

int dev_uart_open(struct vnode *vp, int flags)
{
	(void)vp;
	(void)flags;

	__uart_init();

	return 0;
}

int dev_uart_write(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	(void)vp;
	(void)off;

	size_t initial_count = count;
	const char *bufp = buf;

	for (; count; count--) {
		if ('\n' == *bufp)
			__uart_putchar('\r');
		__uart_putchar(*bufp++);
	}
	*n = initial_count - count;

	return 0;
}
