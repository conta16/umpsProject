#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "init.h"
#include "scheduler.h"
#include "utils.h"

pcb_t ready_queue;
pcb_t *tests[TEST_PROCS];

int main(void)
{
	init(&ready_queue, tests);
	scheduler(&(ready_queue.p_next));
	HALT();
}
