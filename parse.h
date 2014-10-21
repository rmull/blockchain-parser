#ifndef PARSE_H_
#define PARSE_H_

enum parse_blk_state {
    P_BLK_MAGIC,
    P_BLK_SZ,
    P_BLK_VERSION,
    P_BLK_PREV,
    P_BLK_MERKLE,
    P_BLK_TIME,
    P_BLK_DIFFICULTY,
    P_BLK_NONCE,
    P_BLK_TXCNT,
    P_BLK_TX
};

enum parse_tx_state {
    P_TX_VERSION,
    P_TX_TXIN_CNT,
    P_TX_TXIN,
    P_TX_TXOUT_CNT,
    P_TX_TXOUT,
    P_TX_LOCKTIME
};

#define VAR_INT_2BYTE   0xFD
#define VAR_INT_4BYTE   0xFE
#define VAR_INT_8BYTE   0xFF

void parse(int blkfd, off_t sz);

#endif
