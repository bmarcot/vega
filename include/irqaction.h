#ifndef IRQACTION_H
#define IRQACTION_H

struct irqaction {
	void (*ia_irqaction)(void *);
};

void stage_irqaction(struct irqaction *irqaction, void *arg);

#endif /* !IRQACTION_H */
