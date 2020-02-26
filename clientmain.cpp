#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>

#include <calcLib.h>

int main(int argc, char* argv[]) {


    if (argc < 2)
    {
        printf("Not enough");
        exit(0);
    }
    int sockInt;
    struct addrinfo hints, * servinfo, * p;
    int rV;
    memset(&hints, 0, sizeof(hints));
    uint16_t nu;
    double fval[2];
    char result[100];
    char serverMsg[200];

    int serverMsg_len = sizeof(serverMsg);

    char oper[24];

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char protocol[13] = "TEXT TCP 1.0";

    if ((rV = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "address info: %s\n", gai_strerror(rV));
        exit(0);
    }

    p = servinfo;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockInt = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client");
            continue;
        }
        if(connect(sockInt, p->ai_addr, p->ai_addrlen) == -1)
        {
            printf("connection error\n");
            close(sockInt);
            exit(0);
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client wasnt able to make socket\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    recv(sockInt, serverMsg, serverMsg_len, 0);
    printf("%s\n", serverMsg);

    if (strcmp(serverMsg, protocol) == 0)
    {
        memset(serverMsg, 0, serverMsg_len);
        char temp[40] = "OK";
        send(sockInt, temp, sizeof(temp), 0);
        memset(temp, 0, sizeof(temp));
        recv(sockInt, &serverMsg, serverMsg_len, 0);
        uint16_t currentMsg;
        uint16_t currentTemp = 0;

        for(currentMsg = 0; currentMsg <serverMsg_len; currentMsg++, currentTemp++)
        {
            if (serverMsg[currentMsg] != '\n')
            {
                temp[currentTemp] = serverMsg[currentMsg];
            }
            else
            {
                currentMsg++;
                currentTemp++;
                break;
            }
        }

        strcpy(oper, temp);
        if (strchr(temp, 'f') != NULL)
        {
            for (int i = 0; i < 2; i++)
            {

                memset(temp, 0, sizeof(temp));
                currentTemp = 0;

                for (int j = currentMsg; j < serverMsg_len; j++)
                {

                    if (serverMsg[j] != '\n')
                    {
                        temp[currentTemp++] = serverMsg[j];
                        currentMsg++;
                    }
                    else
                    {
                        currentMsg++;
                        break;
                    }
                }

                sscanf(temp, "%lf", &fval[i]);
                printf("[x]val %d: %8.8g\n", i + 1, fval[i]);
            }

            if (strcmp(oper, "fadd") == 0)
            {
                printf("Operation: Addition\n");
                sprintf(result, "%8.8g", (fval[0] + fval[1]));
            }
            else if (strcmp(oper, "fsub") == 0)
            {
                printf("Operation: Subtraction\n");
                sprintf(result, "%8.8g", (fval[0] - fval[1]));
            }
            else if (strcmp(oper, "fmul") == 0)
            {
                printf("Operation: Multi\n");
                sprintf(result, "%8.8g", (fval[0] * fval[1]));
            }
            else if (strcmp(oper, "fdiv") == 0)
            {
                printf("Operation: Division\n");
                sprintf(result, "%8.8g", (fval[0] / fval[1]));
            }

            sprintf(serverMsg, "%s", result);
            printf("Result: %s\n", serverMsg);
            serverMsg_len = sizeof(serverMsg);
            send(sockInt, serverMsg, serverMsg_len, 0);

        }

        else
        {
            for (int i = 0; i < 2; i++)
            {

                memset(temp, 0, sizeof(temp));
                currentTemp = 0;
                
                for (int j = currentMsg; j < serverMsg_len; j++)
                {
                    
                    if (serverMsg[j] != '\n')
                    {
                        temp[currentTemp++] = serverMsg[j];
                        currentMsg++;
                    }
                    else
                    {
                        currentMsg++;
                        break;
                    }
                }

                sscanf(temp, "%d", &nu);
                fval[i] = ntohs(nu);
                printf("[x]Val %d: %d\n", i + 1, (int)fval[i]);

            }

            if (strcmp(oper, "add") == 0)
            {
                printf("Operation: Addition\n");
                sprintf(result, "%d", (int)(fval[0] + fval[1]));
            }
            else if (strcmp(oper, "sub") == 0)
            {
                printf("Operation: Subtraction\n");
                sprintf(result, "%d", (int)(fval[0] - fval[1]));
            }
            else if (strcmp(oper, "mul") == 0)
            {
                printf("Operation: Multiplication\n");
                sprintf(result, "%d", (int)(fval[0] * fval[1]));
            }
            else if (strcmp(oper, "div") == 0)
            {
                printf("Operation: Division\n");
                sprintf(result, "%d", (int)(fval[0] / fval[1]));
            }
            sprintf(serverMsg, "%s", result);
            printf("Result: %s\n", serverMsg);
            serverMsg_len = sizeof(serverMsg);
            send(sockInt, serverMsg, serverMsg_len, 0);

        }

        memset(serverMsg, 0, serverMsg_len);
        recv(sockInt, serverMsg, serverMsg_len, 0);
        printf("%s\n", serverMsg);
    }

    close(sockInt);
    return 1;


}
