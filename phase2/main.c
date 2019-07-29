/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/


#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "init.h"
#include "scheduler.h"
#include "utils.h"

pcb_t ready_queue;
pcb_t *test_pcb;

int main(void)
{
	init(&ready_queue);
	scheduler(&(ready_queue.p_next));
	HALT();
}
