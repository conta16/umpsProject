#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"
#include "p1.5test_rikaya_v0.h"
#include "scheduler.h"

unsigned int a;
int main(void)
{
	init(&ready_queue, tests);
	scheduler(&(ready_queue.p_next));
	HALT();
}
