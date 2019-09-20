/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/



#include <umps/libumps.h>
#include "handlers.h"
#include "types_rikaya.h"
#include "const_rikaya.h"
#include "utils.h"
#include "syscall.h"

extern short int spec_assigned[3];

extern pcb_t* current;

void tlb_handler(){
  if (!(current->spec_assigned[1]))
    SYSCALL(TERMINATEPROCESS, 0, 0, 0);
  else{
    copyState((state_t*)TLB_OLDAREA, current->tlb_old);
    LDST(current->tlb_new);
  }
}

void pgmtrap_handler(){
  if (!(current->spec_assigned[2]))
    SYSCALL(TERMINATEPROCESS, 0, 0, 0);
  else{
    copyState((state_t*)PGMTRAP_OLDAREA, current->pgmtrap_old);
    LDST(current->pgmtrap_new);
  }
}

void sysbk_handler(){
  if (!(((state_t*)SYSBK_OLDAREA)->reg_a0<11 && ((state_t*)SYSBK_OLDAREA)->reg_a0>0)){
    if (!(current->spec_assigned[0]))
      SYSCALL(TERMINATEPROCESS, 0, 0, 0);
    else{
      oldarea_pc_increment();
      copyState((state_t*)SYSBK_OLDAREA, current->sysbk_old);
      LDST(current->sysbk_new);
    }
  }
  else
    syscall_handler();
}
