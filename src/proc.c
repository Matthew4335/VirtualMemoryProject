#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "va_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 3 --------------------------------------
 * Checkout PDF section 4 for this problem
 * 
 * This function gets called every time a new process is created.
 * You will need to allocate a frame for the process's page table using the
 * free_frame function. Then, you will need update both the frame table and
 * the process's PCB. 
 * 
 * @param proc pointer to process that is being initialized 
 * 
 * HINTS:
 *      - pcb_t: struct defined in pagesim.h that is a process's PCB.
 *      - You are not guaranteed that the memory returned by the free frame allocator
 *      is empty - an existing frame could have been evicted for our new page table.
 * ----------------------------------------------------------------------------------
 */
void proc_init(pcb_t *proc) {
    pfn_t pfn = free_frame();
    memset(mem + pfn * PAGE_SIZE, 0, PAGE_SIZE);
    (*proc).saved_ptbr = pfn;
    frame_table[pfn].protected = 1;
    frame_table[pfn].mapped = 1;
    frame_table[pfn].process = proc;
}

/**
 * --------------------------------- PROBLEM 4 --------------------------------------
 * Checkout PDF section 5 for this problem
 * 
 * Switches the currently running process to the process referenced by the proc 
 * argument.
 * 
 * Every process has its own page table, as you allocated in proc_init. You will
 * need to tell the processor to use the new process's page table.
 * 
 * @param proc pointer to process to become the currently running process.
 * 
 * HINTS:
 *      - Look at the global variables defined in pagesim.h. You may be interested in
 *      the definition of pcb_t as well.
 * ----------------------------------------------------------------------------------
 */
void context_switch(pcb_t *proc) {
    PTBR = (*proc).saved_ptbr;
    current_process = proc;
}

/**
 * --------------------------------- PROBLEM 8 --------------------------------------
 * Checkout PDF section 8 for this problem
 * 
 * When a process exits, you need to free any pages previously occupied by the
 * process.
 * 
 * HINTS:
 *      - If the process has swapped any pages to disk, you must call
 *      swap_free() using the page table entry pointer as a parameter.
 *      - If you free any protected pages, you must also clear their "protected" bits.
 * ----------------------------------------------------------------------------------
 */
void proc_cleanup(pcb_t *proc) {
    pfn_t ptbr = (*proc).saved_ptbr;
        
    pte_t *pt = (pte_t*)(mem + (ptbr * PAGE_SIZE));

    // TODO: Iterate the proc's page table and clean up each valid page
    for (size_t i = 0; i < NUM_PAGES; i++) {
        if (pt[i].valid == 1) {
            pt[i].referenced = 0;
            frame_table[pt[i].pfn].protected = 0;
            frame_table[pt[i].pfn].mapped = 0;
            frame_table[pt[i].pfn].vpn = 0;
            //frame_table[pt[i].pfn].process = NULL;
        }
        if (swap_exists(&pt[i])) {
            swap_free(&pt[i]);
        }
    }
    //find out how to release page table
    frame_table[ptbr].protected = 0;
    frame_table[ptbr].mapped = 0;
    //frame_table[ptbr].process = NULL;
}

#pragma GCC diagnostic pop
