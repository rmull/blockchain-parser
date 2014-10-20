#ifndef PARSE_H_
#define PARSE_H_

#define PAGESZ  4096
#define MMAP

enum parse_msg_state {
    P_MSG_MAGIC,
    P_MSG_COMMAND,
    P_MSG_PAYLOAD_LEN,
    P_MSG_PAYLOAD_SUM
};

enum parse_tx_state {
    P_TX_VERSION,
    P_TX_TXIN_CNT,
    P_TX_TXIN,
    P_TX_TXOUT_CNT,
    P_TX_TXOUT,
    P_TX_LOCKTIME
};

void parse(int blkfd, off_t sz);

#endif
