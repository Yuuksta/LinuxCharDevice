#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFF_LEN 100

int main(){
    int fd, rt;
    char str[BUFF_LEN];
    char command[15];
    int offset, whence, readlen;
    if((fd = open("/dev/MyDriver", O_RDWR, S_IRUSR | S_IWUSR)) < 0){
        printf("Fail to open device.\n");
        exit(0);
    }
    printf("choose function and parameter below:\n1. write\n2. read\n3. lseek\n4. exit\n");
    printf("please input command name:\n");
    while(1){
        //printf("input command:\n");
        scanf("%[^\n]%*c",command);
        if(!strcmp(command, "write")){
            printf("input a string:\n");
            scanf("%[^\n]%*c",str);
            if((rt = write(fd, str, strlen(str))) < 0){
                printf("Fail to write device.\n");
                exit(0);
            }
        }
        else if(!strncmp(command,"read",4)){
            char buffer[BUFF_LEN];
            printf("input read length: ");
            scanf("%d", &readlen);
            getchar();
            if((rt = read(fd, buffer, readlen)) < 0){
                printf("Fail to read from device.\n");
                exit(0);
            }
            printf("%s\n",buffer);
            for(int i = 0;i < BUFF_LEN; i++){
                buffer[i] = '\0';
            }
        }
        else if(!strncmp(command,"lseek",5)){
                printf("input offset and whence: ");
                scanf("%d %d",&offset, &whence);
                getchar();
                if((rt = lseek(fd,offset,whence)) < 0){
                    printf("Fail to lseek.\n");
                    exit(0);
                }
        }
        if(!strcmp(command, "exit")){
            break;
        }
    }
    return 0;
}