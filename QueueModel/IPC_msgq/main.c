//
//  main.c
//  IPC_msgq
//
//  Created by yhp on 2017/10/26.
//  Copyright © 2017年 YouHuaPei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/msg.h>

#define BUF_SIZE 1024

struct my_msg_st{
    long int message_type;
    char some_text[BUF_SIZE];
};

int main(int argc, const char * argv[]) {
    struct my_msg_st some_date;
    int runing = 1;
    long int msg_to_recevie = 0;
    int msgid;
    ssize_t msgRcvRet;
    
//   创建一个消息队列
    msgid = msgget((key_t)1234, IPC_CREAT | 0666);
    if (-1 == msgid) {
        fprintf(stderr, "msgget failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }

    while (runing) {
        msgRcvRet = msgrcv(msgid, (void *)&some_date, BUF_SIZE, msg_to_recevie, 0);
        if (-1 == msgRcvRet) {
            fprintf(stderr, "msgrcv failed with error: %d\n",errno);
            exit(EXIT_FAILURE);
        }
        printf("You write, %zd\n",some_date.message_type);
        printf("You write, %s",some_date.some_text);
        if (0 == strncmp(some_date.some_text, "end",3)) {
            runing = 0;
        }
    }
    
//    删除消息队列
    msgRcvRet = msgctl(msgid, IPC_RMID, 0);
    if (-1 == msgRcvRet) {
        fprintf(stderr, "msgctl failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
    
    return 0;
}
