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

uint32_t blk_cnt = 0;
enum parse_blk_state p_blk_s = P_BLK_MAGIC;
enum parse_tx_state p_tx_s = P_TX_VERSION;
enum parse_txin_state p_txin_s = P_TXIN_PREV_HASH;
enum parse_txout_state p_txout_s = P_TXOUT_VALUE;

/*
 * Map the magic number into network enumeration
 */
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

/*
 * Process a var_int starting at p into dest
 */
uint8_t
parse_varint(uint8_t *p, uint64_t *dest)
{
    uint8_t varint = *p;
    uint8_t mv = 1;

    if (varint < VAR_INT_2BYTE) {
        *dest = (uint64_t)varint;
    } else if (varint == VAR_INT_2BYTE) {
        *dest = (uint64_t)( *(uint16_t *)(p+1) );
        mv += 2;
    } else if (varint == VAR_INT_4BYTE) {
        *dest = (uint64_t)( *(uint32_t *)(p+1) );
        mv += 4;
    } else if (varint == VAR_INT_8BYTE) {
        *dest = (uint64_t)( *(uint64_t *)(p+1) );
        mv += 8;
    }

    return mv;
}

/*
 * Print what we know about a given tx_input
 */
void
parse_txin_print(struct tx_input *i)
{
    uint8_t j;

    printf("    prev output: ");
    for (j=HASH_LEN-1; j<HASH_LEN; j--) {
        printf("%02X", i->prev_hash[j]);
    }
    printf("\n");
    printf("    index: %d\n", i->index);
    printf("    script len: %lu\n", i->script_len);
    printf("    sequence: %X\n", i->sequence);
    printf("\n");
}

/*
 * Print what we know about a given tx_output
 */
void
parse_txout_print(struct tx_output *o)
{
    printf("    value: %lu\n", o->value);
    printf("    script len: %lu\n", o->script_len);
    printf("\n");
}

/*
 * Print what we know about a given bitcoin transaction
 */
void
parse_tx_print(struct tx *t)
{
    printf("  version: %u\n", t->version);
    printf("  txin cnt: %lu\n", t->txin_cnt);
    printf("  txout cnt: %lu\n", t->txout_cnt);
    printf("  lock time: %u\n", t->lock_time);
    printf("\n");
}

/*
 * Print what we know about a block in the blockchain
 */
void
parse_block_print(struct block *b)
{
    time_t t = b->time;
    struct tm *tm = gmtime(&t);
    char timestr[32];
    uint8_t i;
    
    strftime(timestr, 32, "%Y-%m-%d %H:%M:%S", tm);

    printf("magic: 0x%X\n", b->magic);
    printf("size: %u\n", b->size);
    printf("version: %u\n", b->version);
    printf("prev block: ");
    /* Print the hashes in the correct endianness */
    for (i=HASH_LEN-1; i<HASH_LEN; i--) {
        printf("%02X", b->prev_block[i]);
    }
    printf("\n");
    printf("merkle root: ");
    for (i=HASH_LEN-1; i<HASH_LEN; i--) {
        printf("%02X", b->merkle_root[i]);
    }
    printf("\n");
    printf("time: %s\n", timestr);
    printf("bits: %u\n", b->bits);
    printf("nonce: %u\n", b->nonce);
    printf("tx count: %lu\n", b->tx_cnt);
    printf("\n");
}

/*
 * Parse count tx_inputs from the stream starting at p
 * Return the number of bytes processed
 */
uint64_t
parse_txin(uint8_t *src, uint64_t count)
{
    uint8_t *p = src;
    struct tx_input i;
    uint64_t skip = 0;
    uint64_t done = 0;

    p_txin_s = P_TXIN_PREV_HASH;

    while (count > 0) {

        p += skip;

        switch (p_txin_s) {

        case P_TXIN_PREV_HASH:
            memcpy((void *)&i.prev_hash, p, HASH_LEN);
            skip = HASH_LEN;
            p_txin_s = P_TXIN_INDEX;
            break;

        case P_TXIN_INDEX:
            i.index = *(uint32_t *)p;
            skip = INDEX_LEN;
            p_txin_s = P_TXIN_SCRIPT_LEN;
            break;

        case P_TXIN_SCRIPT_LEN:
            skip = (uint64_t)parse_varint(p, &(i.script_len));
            p_txin_s = P_TXIN_SCRIPT;
            break;

        case P_TXIN_SCRIPT:
            i.script = p;
            skip = i.script_len;
            p_txin_s = P_TXIN_SEQUENCE;
            break;

        case P_TXIN_SEQUENCE:
            i.sequence = *(uint32_t *)p;
            skip = SEQUENCE_LEN;
            parse_txin_print(&i);
            count--;
            p_txin_s = P_TXIN_PREV_HASH;
            break;

        default:
            break;
        }

        done += skip;
    }

    return done;
}

/*
 * Parse count tx_outputs from the stream starting at p
 * Return the number of bytes processed
 */
uint64_t
parse_txout(uint8_t *src, uint64_t count)
{
    uint8_t *p = src;
    struct tx_output o;
    uint64_t skip = 0;
    uint64_t done = 0;

    p_txout_s = P_TXOUT_VALUE;

    while (count > 0) {

        p += skip;

        switch (p_txout_s) {

        case P_TXOUT_VALUE:
            o.value = *(uint64_t *)p;
            skip = VALUE_LEN;
            p_txout_s = P_TXOUT_SCRIPT_LEN;
            break;

        case P_TXOUT_SCRIPT_LEN:
            skip = (uint64_t)parse_varint(p, &(o.script_len));
            p_txout_s = P_TXOUT_SCRIPT;
            break;

        case P_TXOUT_SCRIPT:
            o.script = p;
            skip = o.script_len;
            parse_txout_print(&o);
            count--;
            p_txout_s = P_TXOUT_VALUE;
            break;

        default:
            break;
        }

        done += skip;
    }

    return done;
}

/*
 * Parse count transactions from the stream starting at p
 * Return number of bytes processed
 */
uint64_t
parse_tx(uint8_t *src, uint64_t count)
{
    uint8_t *p = src;
    struct tx t;
    uint64_t skip = 0;
    uint64_t done = 0;

    p_tx_s = P_TX_VERSION;

    while (count > 0) {

        p += skip;

        switch (p_tx_s) {

        case P_TX_VERSION:
            t.version = *(uint32_t *)p;
            skip = VERSION_LEN;
            p_tx_s = P_TX_TXIN_CNT;
            break;

        case P_TX_TXIN_CNT:
            skip = (uint64_t)parse_varint(p, &(t.txin_cnt));
            p_tx_s = P_TX_TXIN;
            break;

        case P_TX_TXIN:
            /* Process each input in this transaction */
            skip = parse_txin(p, t.txin_cnt);
            p_tx_s = P_TX_TXOUT_CNT;
            break;

        case P_TX_TXOUT_CNT:
            skip = (uint64_t)parse_varint(p, &(t.txout_cnt));
            p_tx_s = P_TX_TXOUT;
            break;

        case P_TX_TXOUT:
            /* Process each output in this transaction */
            skip = parse_txout(p, t.txout_cnt);
            p_tx_s = P_TX_LOCKTIME;
            break;

        case P_TX_LOCKTIME:
            t.lock_time = *(uint32_t *)p;
            skip = LOCKTIME_LEN;
            parse_tx_print(&t);
            count--;
            p_tx_s = P_TX_VERSION;
            break;

        default:
            break;
        }

        done += skip;
    }

    return done;
}

/*
 * Parse a series of blockchain blocks between p and end
 * Return the number of bytes processed
 */
uint64_t
parse_block(uint8_t *src, uint64_t sz)
{
    uint8_t *p = src;
    struct block b;
    uint64_t skip = 0;
    uint64_t done = 0;

    /* Look for different patterns depending on our state */
    while (sz > skip) {

        p += skip;
        sz -= skip;

        switch (p_blk_s) {

        /* Look for the magic number */
        case P_BLK_MAGIC:

            /* Check for magic number */
            b.magic = *(uint32_t *)p;

            /* If blk[i] starts the magic bytes, we can skip ahead */
            if (parse_is_magic(b.magic) != MAGIC_NET_NONE) {
                skip = MAGIC_LEN;
                p_blk_s = P_BLK_SZ;

            /* No magic number at this byte, check the next one */
            } else {
                skip = 1;
            }
            break;

        case P_BLK_SZ:
            b.size = *(uint32_t *)p;
            skip = BLKSZ_LEN;
            p_blk_s = P_BLK_VERSION;
            break;

        case P_BLK_VERSION:
            b.version = *(uint32_t *)p;
            skip = VERSION_LEN;
            p_blk_s = P_BLK_PREV;
            break;

        case P_BLK_PREV:
            memcpy((void *)&b.prev_block, p, HASH_LEN);
            skip = HASH_LEN;
            p_blk_s = P_BLK_MERKLE;
            break;

        case P_BLK_MERKLE:
            memcpy((void *)&b.merkle_root, p, HASH_LEN);
            skip = HASH_LEN;
            p_blk_s = P_BLK_TIME;
            break;

        case P_BLK_TIME:
            b.time = *(uint32_t *)p;
            skip = TIME_LEN;
            p_blk_s = P_BLK_BITS;
            break;

        case P_BLK_BITS:
            b.bits = *(uint32_t *)p;
            skip = DIFFICULTY_LEN;
            p_blk_s = P_BLK_NONCE;
            break;

        case P_BLK_NONCE:
            b.nonce = *(uint32_t *)p;
            skip = NONCE_LEN;
            p_blk_s = P_BLK_TXCNT;
            break;

        case P_BLK_TXCNT:
            skip = (uint64_t)parse_varint(p, &(b.tx_cnt));
            p_blk_s = P_BLK_TX;
            break;

        case P_BLK_TX:
            /* Process each transaction in this block */
            skip = parse_tx(p, b.tx_cnt);

            printf("block: %d\n", blk_cnt++);
            parse_block_print(&b);
            p_blk_s = P_BLK_MAGIC;

            break;
            
        default:
            break;
        }

        done += skip;
    }

    return done;
}

uint64_t
parse(int blkfd, uint64_t sz)
{
    uint8_t *blk;
    uint64_t done;

    /* Map the input file */
    blk = (uint8_t *)mmap(NULL, sz, PROT_READ, MAP_PRIVATE, blkfd, 0);

    /* Process each block in this file */
    done = parse_block(blk, sz);

    /* Drop the mapping */
    munmap(blk, sz);

    return done;
}
