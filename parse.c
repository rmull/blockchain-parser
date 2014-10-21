#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "format.h"
#include "parse.h"

enum parse_blk_state p_blk_s = P_BLK_MAGIC;
enum parse_tx_state p_tx_s = P_TX_VERSION;
uint32_t blk_cnt = 0;

enum magic_net
parse_is_magic(uint32_t m)
{
    enum magic_net mn = MAGIC_NET_NONE;

    if (m == MAGIC_MAIN) {
        mn = MAGIC_NET_MAIN;
    } else if (m == MAGIC_TESTNET) {
        mn = MAGIC_NET_TESTNET;
    } else if (m == MAGIC_TESTNET3) {
        mn = MAGIC_NET_TESTNET3;
    } else if (m == MAGIC_NAMECOIN) {
        mn = MAGIC_NET_NAMECOIN;
    }

    return mn;
}

void
parse_block_print(struct block *b)
{
    time_t t = b->time;
    struct tm *tm = localtime(&t);
    char timestr[32];
    uint8_t i;
    
    strftime(timestr, 32, "%Y-%m-%d %H:%M:%S", tm);

    printf("magic: 0x%X\n", b->magic);
    printf("size: %d\n", b->size);
    printf("version: %d\n", b->version);
    printf("prev block: ");
    for (i=0; i<HASH_LEN; i++) {
        printf("%02X", b->prev_block[i]);
    }
    printf("\n");
    printf("merkle root: ");
    for (i=0; i<HASH_LEN; i++) {
        printf("%02X", b->merkle_root[i]);
    }
    printf("\n");
    printf("time: %s\n", timestr);
    printf("difficulty: %d\n", b->difficulty);
    printf("nonce: %ud\n", b->nonce);
}

void
parse(int blkfd, off_t sz)
{
    struct block b;
    uint8_t *blk;
    uint8_t *p;
    uint32_t i = 0;

    /* Map the input file */
    blk = (uint8_t *)mmap(NULL, sz, PROT_READ, MAP_PRIVATE, blkfd, 0);

    /* Initialize our seek pointer */
    p = blk;
    
    /* Loop through the bytes in the input file */
    while (p < (blk+sz)) {

        /* Look for different patterns depending on our state */
        switch (p_blk_s) {

        /* Look for the magic number */
        case P_BLK_MAGIC:

            /* Check for magic number */
            b.magic = *(uint32_t *)p;

            /* If blk[i] starts the magic bytes, we can skip ahead */
            if (parse_is_magic(b.magic) != MAGIC_NET_NONE) {
                p += MAGIC_LEN;
                p_blk_s = P_BLK_SZ;

            /* No magic number at this byte */
            } else {
                p++;
            }

            break;

        case P_BLK_SZ:

            b.size = *(uint32_t *)p;
            p += BLKSZ_LEN;
            p_blk_s = P_BLK_VERSION;

            break;

        case P_BLK_VERSION:
            b.version = *(uint32_t *)p;
            p += VERSION_LEN;
            p_blk_s = P_BLK_PREV;
            break;

        case P_BLK_PREV:
            memcpy(&b.prev_block, p, HASH_LEN);
            p += HASH_LEN;
            p_blk_s = P_BLK_MERKLE;
            break;

        case P_BLK_MERKLE:
            memcpy(&b.merkle_root, p, HASH_LEN);
            p += HASH_LEN;
            p_blk_s = P_BLK_TIME;
            break;

        case P_BLK_TIME:
            b.time = *(uint32_t *)p;
            p += TIME_LEN;
            p_blk_s = P_BLK_DIFFICULTY;
            break;

        case P_BLK_DIFFICULTY:
            b.difficulty = *(uint32_t *)p;
            p += DIFFICULTY_LEN;
            p_blk_s = P_BLK_NONCE;
            break;

        case P_BLK_NONCE:
            b.nonce = *(uint32_t *)p;
            p += NONCE_LEN;
            p_blk_s = P_BLK_TXCNT;
            break;

        case P_BLK_TXCNT:
            /* TODO: Handle the VAR_INT here */
            printf("txcnt: %d\n", *p);

            p_blk_s = P_BLK_TX;
            break;

        case P_BLK_TX:
            /* TODO: Parse the tx here */

            blk_cnt++;
            printf("block: %d\n", blk_cnt);
            parse_block_print(&b);
            printf("\n");

            p_blk_s = P_BLK_MAGIC;
            break;
            
        default:
            break;
        }

    }

    munmap(blk, sz);
}
