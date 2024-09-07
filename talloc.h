#include "types.h"

void tinit();
void *talloc(void);
void *talloc_with_size(int size);
void tmemset(void *dest, uint8 val, uint32 len);
