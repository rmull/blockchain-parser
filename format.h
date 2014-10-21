#ifndef FORMAT_H_
#define FORMAT_H_

#include <stdint.h>

#include "var_int.h"

/* 
 * All information was found at
 * https://en.bitcoin.it/wiki/Protocol_specification#block
 * https://2.bp.blogspot.com/-DaJcdsyqQSs/UsiTXNHP-0I/AAAAAAAATC0/kiFRowh-J18/s1600/blockchain.png
 */

#define MAGIC_MAIN      0xD9B4BEF9
#define MAGIC_TESTNET   0xDAB5BFFA
#define MAGIC_TESTNET3  0x0709110B
#define MAGIC_NAMECOIN  0xFEB4BEF9

/* Helper enum for keeping track of which network a msg came from */
enum magic_net {
    MAGIC_NET_NONE,
    MAGIC_NET_MAIN,
    MAGIC_NET_TESTNET,
    MAGIC_NET_TESTNET3,
    MAGIC_NET_NAMECOIN
};

#define CMD_LEN         12
#define MAGIC_LEN       4
#define BLKSZ_LEN       4
#define VERSION_LEN     4
#define HASH_LEN        32
#define TIME_LEN        4
#define DIFFICULTY_LEN  4
#define NONCE_LEN       4

/* Describes an outpoint from a bitcoin transaction */
struct outpoint {

    /* The hash of the referenced transaction */
    uint8_t hash[HASH_LEN];

    /* The index of the specific output in the transaction. First is 0, etc */
    uint32_t index;
};

/* Describes an input to a bitcoin transaction */
struct tx_input {

    /* The previous outpoint transaction reference as an outpoint structure */
    struct outpoint previous_outpoint;

    /* The length of the signature script */
    uint8_t script_len[VAR_INT];

    /* Computational script for confirming transaction authorization */
    uint8_t *script;

    /* 
     * Transaction version as defined by the sender. Intended for replacement
     * of transactions when information is updated before inclusion into a
     * block.
     */
    uint32_t sequence;

    struct tx_input *next;
};

/* Describes an output from a bitcoin transaction */
struct tx_output {

    /* Transaction value */
    int64_t value;

    /* Length of the pk_script */
    uint8_t script_len[VAR_INT];

    /*
     * Usually contains the public key as a bitcoin script setting up
     * conditions to claim this output.
     */
    uint8_t *script;

    struct tx_output *next;
};


/* Describes a bitcoin transaction */
struct tx {

    /* Transaction data format version */
    uint32_t version;

    /* Number of transaction inputs */
    uint8_t tx_in_cnt[VAR_INT];

    /* A list of 1 or more transaction inputs or sources for coins */
    struct tx_input tx_in;

    /* Number of transaction outputs */
    uint8_t tx_out_count[VAR_INT];

    /* A list of 1 or more transaction outputs or destinations for coins */
    struct tx_output tx_out;

    /* Block number or timestamp at which this transaction is locked */
    uint32_t lock_time;

};

/* Format of a bitcoin network message */
struct msg {
    
    /*
     * Magic value indicating message origin network and used to seek to next
     * message when stream state is unknown
     */
    uint32_t magic;

    /*
     * ASCII string identifying the packet content, NULL padded (non-NULL
     * padding results in packet rejected)
     */
    char command[CMD_LEN];

    /* Length of payload in bytes */
    uint32_t length;

    /* First 4 bytes of sha256(sha256(payload)) */
    uint32_t checksum;

    /* Message payload data */
    uint8_t *payload;
};

/* Format for a block in a bitcoin blockchain */
struct block {

    /* Magic number */
    uint32_t magic;

    /* Length of the entire block */
    uint32_t size;

    /* Block version info based on software version that created this block */
    uint32_t version;

    /* Hash value of the previous block this block references */
    uint8_t prev_block[HASH_LEN];

    /* 
     * Reference to a Merkle tree collection (hash of all transactions related
     * to this block) 
     */
    uint8_t merkle_root[HASH_LEN];

    /* Unix timestamp (32-bit) for when this block was created */
    uint32_t time;

    /* Calculated difficulty target being used for this block */
    uint32_t bits;

    /* 
     * Nonce used to generate this block to allow variations of the header and
     * compute different hashes
     */
    uint32_t nonce;

    /* 
     * Number of transaction entries
     */
    uint64_t tx_cnt;

    /* Transactions */
    struct tx *tx_list;

};
    

#endif
