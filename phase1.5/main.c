#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"
#include "p1.5test_rikaya_v0.h"

pcb_t tests[3];
pcb_t ready_queue;
unsigned int a;
int main(void)
{
	a=1;
	/*there should be HALT, function declared in libumps. Since it is implemented elsewhere and I haven't found it yet, it is still missing*/
	init(&ready_queue, tests);
	a=127;
	SYSCALL(SYS3, 0, 0, 0);
	a=255;
	addokbuf("cavallino");
	HALT();
}
