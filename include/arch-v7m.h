#ifndef ARCH_V7M_H
#define ARCH_V7M_H

#define V7M_XPSR_T (1 << 24)

#define V7M_SCB_BASE 0xe000ed00
#define V7M_VTOR 8
#define	V7M_VTOR_OFFSET_SVC 44
#define V7M_AIRCR 0x0c
#define V7M_SHCSR 0x24
#define V7M_SHCSR_USGFAULTENA (1 << 18)
#define V7M_SHCSR_BUSFAULTENA (1 << 17)
#define V7M_SHCSR_MEMFAULTENA (1 << 16)

#endif /* !ARCH_V7M_H */
