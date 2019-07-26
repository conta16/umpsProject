#include <umps/libumps.h>
#include "handlers.h"
#include "types_rikaya.h"
#include "const_rikaya.h"
#include "utils.h"
#include "syscall.h"

extern short int spec_assigned[3];

extern state_t *sysbk_old, *tlb_old, *pgmtrap_old;
extern state_t *sysbk_new, *tlb_new, *pgmtrap_new;

void tlb_handler(){
  if (!spec_assigned[1])
    SYSCALL(TERMINATEPROCESS, 0, 0, 0);
  else{
    copyState((state_t*)TLB_OLDAREA, tlb_old);
    LDST(sysbk_new);
  }
}

void pgmtrap_handler(){
  if (!spec_assigned[2])
    SYSCALL(TERMINATEPROCESS, 0, 0, 0);
  else{
    copyState((state_t*)PGMTRAP_OLDAREA, pgmtrap_old);
    LDST(pgmtrap_new);
  }
}

void sysbk_handler(){
  if (!(((state_t*)SYSBK_OLDAREA)->reg_a0<11 && ((state_t*)SYSBK_OLDAREA)->reg_a0>0)){
    if (!spec_assigned[0])
      SYSCALL(TERMINATEPROCESS, 0, 0, 0);
    else{
      copyState((state_t*)SYSBK_OLDAREA, sysbk_old);
      LDST(sysbk_new);
    }
  }
  else
    syscall_handler();
}
