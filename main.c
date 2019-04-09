#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "p1.5test_rikaya_v0.c"
#include "init.h"

pcb_t tests[3];
pcb_t ready_queue;

int main(void)
{
	init(&ready_queue, tests);
	/*there should be HALT, function declared in libumps. Since it is implemented elsewhere and I haven't found it yet, it is still missing*/
}
