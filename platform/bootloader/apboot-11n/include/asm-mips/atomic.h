#ifndef _ATOMIC_H
#define _ATOMIC_H

typedef struct {
	volatile int counter;
} atomic_t;

#define atomic_read(v)	((v)->counter)
#define atomic_set(v,i)	((v)->counter = (i))

static __inline__ void atomic_add(int i, atomic_t * v)
{
	unsigned long temp;

	__asm__ __volatile__("1:   ll      %0, %1      # atomic_add\n"
			     "     addu    %0, %2                  \n"
			     "     sc      %0, %1                  \n"
			     "     beqz    %0, 1b                  \n":"=&r"
			     (temp), "=m"(v->counter)
			     :"Ir"(i), "m"(v->counter));
}

static __inline__ void atomic_sub(int i, atomic_t * v)
{
	unsigned long temp;

	__asm__ __volatile__("1:   ll      %0, %1      # atomic_sub\n"
			     "     subu    %0, %2                  \n"
			     "     sc      %0, %1                  \n"
			     "     beqz    %0, 1b                  \n":"=&r"
			     (temp), "=m"(v->counter)
			     :"Ir"(i), "m"(v->counter));
}

#endif
