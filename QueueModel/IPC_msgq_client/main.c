//
//  main.c
//  IPC_msgq_client
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

#define MAX_TEXT 512
#define BUF_SIZE 1024

struct my_msg_st{
    long int message_type;
    char some_text[MAX_TEXT];
};

int main(int argc, const char * argv[]) {
    struct my_msg_st some_date;
    int runing = 1;
    int msgid;
    char buffer[BUF_SIZE];
    
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (-1 == msgid) {
        fprintf(stderr, "msgget failed with error: %d\n",errno);
        exit(EXIT_FAILURE);
    }
    
    while (runing) {
        printf("Enter some text: ");
        fgets(buffer,BUFSIZ,stdin);
        some_date.message_type = 1;
        strcpy(some_date.some_text, buffer);
        if (-1 == msgsnd(msgid, (void*)&some_date, MAX_TEXT, 0)) {
            fprintf(stderr, "msgsnd failed with error: %d\n",errno);
            exit(EXIT_FAILURE);
        }
        if (0 == strncmp(some_date.some_text, "end",3)) {
            runing = 0;
        }
    }
    exit(EXIT_SUCCESS);
    
    return 0;
}
