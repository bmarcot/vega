#include <stdarg.h>
#include <ucontext.h>

void return_from_makecontext();

void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)
{
	va_list ap;

	/* pass arguments to the context entry function */
	if (argc) {
		va_start(ap, argc);

		/* the 4 first arguments go into registers r0-r3 */
		for (int i = 0; (i < argc) && (i < 4); i++)
			ucp->uc_mcontext.gprs[i] = va_arg(ap, int);

		/* extra arguments go into the stack */

		va_end(ap);
	}

	/* top of the stack has a back-link pointer to the context's struct */
	ucp->uc_stack.ss_sp = (void *)((unsigned long)ucp->uc_stack.ss_sp - sizeof(unsigned long));
	*(unsigned long *)ucp->uc_stack.ss_sp = (unsigned long) ucp;

	/* initialize the machine context */
	//FIXME: ss_sp and mcontext_sp are mutually redundant
	ucp->uc_mcontext.sp = (unsigned long) ucp->uc_stack.ss_sp;
	ucp->uc_mcontext.lr = (unsigned long) return_from_makecontext;
	ucp->uc_mcontext.pc = (unsigned long) func | 1;    /* force Thumb_Mode */
}
