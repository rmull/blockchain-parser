#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "format.h"
#include "parse.h"

uint8_t buf[PAGESZ];

void
parse(int blkfd, off_t sz)
{
    ssize_t nbytes;
    off_t i;
    off_t z = 0;
    volatile uint8_t x;

#ifdef MMAP
    uint8_t *blk;

    /* First parse the message */
    blk = (uint8_t *)mmap(NULL, sz, PROT_READ, MAP_PRIVATE, blkfd, MAP_POPULATE);

    for (i=0; i<sz; i++) {
        //printf("%02X ", blk[i]);
        x = blk[i];
        /*
        if (i > 0) {
            if ((i+1) % 16 == 0)
                printf("\n");
        }
        */
    }
    printf("\n");
    munmap(blk, sz);
#else

    do {
        nbytes = read(blkfd, &buf[0], sizeof(buf));

        for (i=0; i<nbytes; i++) {
            x = buf[i];
            //printf("%02X ", buf[i]);
            //z++;
            //if (z % 16 == 0)
            //    printf("\n");
        }
    } while (nbytes > 0);

    printf("\n");


#endif
}
