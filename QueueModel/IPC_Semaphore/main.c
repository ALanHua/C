//
//  main.c
//  IPC_Semaphore
//
//  Created by yhp on 2017/10/26.
//  Copyright Â© 2017å¹? YouHuaPei. All rights reserved.
//

#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/stat.h>
#include "curr_time.h"
#include "get_num.h"

int main(int argc, const char * argv[]) {
    int semid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0) {
        perror("error");
    }

    if (argc == 2) {
        union semun arg;
        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (-1 == semid) {
            exit(EXIT_FAILURE);
        }
        arg.val = getInt(argv[1], 0, "init-value");
        if (semctl(semid, 0, SETVAL,arg) == -1) {
            exit(EXIT_FAILURE);
        }
        printf("Semaphore ID = %d\n",semid);
    }else{
        struct sembuf sop;
        semid = getInt(argv[1], 0, "semid");
        sop.sem_num  = 0;
        sop.sem_flg  = 0;
        sop.sem_op   = getInt(argv[2], 0, "operation");
        if (semop(semid, &sop, 1) == -1) {
            exit(EXIT_FAILURE);
        }
        printf("%ld: semop completed at %s\n", (long) getpid(), currTime("%T"));
    }
    

    return 0;
}
