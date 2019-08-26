#include <umps/libumps.h>
#include "handlers.h"
#include "types_rikaya.h"
#include "const_rikaya.h"
#include "utils.h"
#include "syscall.h"
#include "scheduler.h"

extern short int spec_assigned[3];
extern pcb_t* current;
extern pcb_t ready_queue;

void tlb_handler(){
  if (!(current->spec_assigned[1]))
    terminate_process(0);
  else{
    copyState((state_t*)TLB_OLDAREA, current->tlb_old);
    LDST(current->tlb_new);
  }
  scheduler(&(ready_queue.p_next));
}

void pgmtrap_handler(){
  if (!(current->spec_assigned[2]))
    terminate_process(0);
  else{
    copyState((state_t*)PGMTRAP_OLDAREA, current->pgmtrap_old);
    LDST(current->pgmtrap_new);
  }
  scheduler(&(ready_queue.p_next));
}

void sysbk_handler(){
  if (!(((state_t*)SYSBK_OLDAREA)->reg_a0<11 && ((state_t*)SYSBK_OLDAREA)->reg_a0>0)){
    if (!(current->spec_assigned[0]))
      terminate_process(0);
    else{
      oldarea_pc_increment();
      copyState((state_t*)SYSBK_OLDAREA, current->sysbk_old);
      LDST(current->sysbk_new);
    }
    scheduler(&(ready_queue.p_next));
  }
  else
    syscall_handler();
}
