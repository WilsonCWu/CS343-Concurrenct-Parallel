#define AUTOMATIC_SIGNAL uCondition cond
#define WAITUNTIL( pred, before, after ) \
	if (!pred) { \
		cond.signal(); \
		while (!pred) { \
			before; \
			cond.wait(); \
		} \
	} \
	cond.signal();

#define RETURN( expr ) cond.signal(); return expr;
