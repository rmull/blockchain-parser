#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "format.h"
#include "parse.h"

void
parse(int blkfd, off_t sz)
{
    off_t i;
    uint8_t *blk;

    /* First parse the message */
    blk = (uint8_t *)mmap(NULL, sz, PROT_READ, MAP_PRIVATE, blkfd, 0);

    for (i=0; i<sz; i++) {
        printf("%02X ", blk[i]);
        asm("nop");
        if (i > 0) {
            if ((i+1) % 16 == 0)
                printf("\n");
        }
    }
    printf("\n");


    munmap(blk, sz);
}
