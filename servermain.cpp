#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>

#include <calcLib.h>

#define THISPORT "6234"
#define MAXBUFFERLEN 100


using namespace std;

int initCalcLib(void);
int randomInt(void);
double randomFloat(void);
char* randomType(void);


int main(int argc, char* argv[]) {

    initCalcLib();

    struct addrinfo guide, * serverInfo, * p;

    uint16_t sockInt;
    uint16_t listenTo;
    uint8_t rV;
    double fval[2];
    uint32_t un[2];

    char result[100];

    struct sockaddr_in theirAddr;
    socklen_t theirAddress_len = sizeof(theirAddr);

    char protocol[13] = "TEXT TCP 1.0";

    char calcMsgFinal[200];

    char clientMsg[300];

    int clientMsgLen = sizeof(clientMsg);

    memset(&guide, 0, sizeof(guide));

    guide.ai_family = AF_INET;
    guide.ai_socktype = SOCK_STREAM;
    guide.ai_flags = AI_PASSIVE;



    if ((rV = getaddrinfo(NULL, THISPORT, &guide, &serverInfo)) != 0)
    {
    fprintf(stderr, "Address info &s\n", gai_strerror(rV));
    exit(0);
    }

    for (p = serverInfo; p != NULL; p = p->ai_next)
    {
        
        if ((sockInt = socket(p->ai_family, p -> ai_socktype, p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockInt, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockInt);
            perror("listener: bind");
            continue;
        }

        break;

    }

    freeaddrinfo(serverInfo);

    while (1)
    {
        memset(calcMsgFinal, 0, sizeof(calcMsgFinal));
        if (listen(sockInt, 1) == -1)
        {
            printf("Nothing to listen to");
            close(sockInt);
            exit(0);
        }
        else
        {
            printf("Server::  About to listen on port %hu...\n", atoi(argv[1]));
        }

        if ((listenTo = accept(sockInt, (struct sockaddr*) & theirAddr, &theirAddress_len)) == -1)
        {
            printf("Error accepting");
            continue;
        }

        printf("--Client connected--\n [x]checking client protocol\n");

        send(listenTo, protocol, strlen(protocol), 0);

        recv(listenTo, clientMsg, clientMsgLen, 0);
        if (strstr(clientMsg, "OK") != NULL)
        {
            printf("--Client is ok--\n");
            char* oper = randomType();
            if (strchr(oper, 'f') != NULL)
            {
                char temt[30];

                for (int i = 0; i < 2; i++)
                {
                    memset(temt, 0, sizeof(temt));
                    fval[i] = randomFloat();
                    sprintf(temt, "%8.8g", fval[i]);
                    sscanf(temt, "8lf", &fval[i]);
                    printf("[x]Value %d: %8.8g\n", i + 1, fval[i]);
                }

                sprintf(calcMsgFinal, "%s %8.8g %8.8g ", oper, fval[0], fval[1]);

                if (strcmp(oper, "fadd") == 0)
                {
                    printf("[x]Operation: Addition\n");
                    sprintf(result, "%8.8g", (fval[0] + fval[1]));
                }
                else if(strcmp(oper, "fsub") == 0)
                {
                    printf("[x]Operation: Subtraction\n");
                    sprintf(result, "%8.8g", (fval[0] - fval[1]));
                }
                else if(strcmp(oper, "fmul") == 0 )
                {
                    printf("[x]Operation: Multi\n");
                    sprintf(result, "%8.8g", (fval[0] * fval[1]));
                }
                else if (strcmp(oper, "fdiv") == 0)
                {
                    printf("[x]Operation: Division");
                    sprintf(result, "%.8g", (fval[0] / fval[1]));
                }

            }

            else
            {
                for (int i = 0; i < 2; i++)
                {
                    fval[i] = randomInt();
                    un[i] = htons((int)fval[i]);
                    printf("[x]Val%d:%d\n", i + 1, (int)fval[i]);
                }
                sprintf(calcMsgFinal, "%s %d %d", oper, un[0], un[1]);

                if (strcmp(oper, "add") == 0)
                {
                    printf("[x]Operation: Addition\n");
                    sprintf(result, "%d", (int)(fval[0] + fval[1]));
                }
                else if (strcmp(oper, "sub") == 0)
                {
                    printf("[x]Operation: Subtraction");
                    sprintf(result, "%d", (int)(fval[0] - fval[1]));
                }
                else if (strcmp(oper, "mul") == 0)
                {
                    printf("[x]Operation: Multi");
                    sprintf(result, "%d", (int)(fval[0] * fval[1]));
                }
                else if (strcmp(oper, "div") == 0)
                {
                    printf("[x]Operation: Division");
                    sprintf(result, "%d", (int)(fval[0] / fval[1]));
                }
                  
            }

            send(listenTo, calcMsgFinal, strlen(calcMsgFinal), 0);
            printf("\n[x]Sent information to the client\n");
            memset(clientMsg, 0, clientMsgLen);
            recv(listenTo, clientMsg, clientMsgLen, 0);
            printf("[x]Client result: %s\n", clientMsg);
            
            if (strchr(oper, 'f') != NULL)
            {
                
                printf("[x]Server result: %s\n", result);
                

                if (atof(result) == atof(clientMsg))
                {
                    char correctCalc[3] = "OK";
                    printf("[x]Results match\n");
                    send(listenTo, correctCalc, strlen(correctCalc), 0);
                }
                else
                {
                    char correctCalc[6] = "ERROR";
                    printf("[x]Results didnt match\n");
                    send(listenTo, correctCalc, strlen(correctCalc), 0);
                }
            }
            else
            {

                printf("[x]Server result: %s\n", result);

                if (atoi(result) == atoi(clientMsg))
                {
                    char correctCalc[3] = "OK";
                    printf("[x]Results match\n");
                    send(listenTo, correctCalc, strlen(correctCalc), 0);
                }
                else
                {
                    char correctCalc[6] = "ERROR";
                    printf("[x]Results didnt match\n");
                    send(listenTo, correctCalc, strlen(correctCalc), 0);
                }
            }

            printf("[x]Notified Client\n");
            printf("[x]Disconnecting client\n");
        }
        else
        {
        close(listenTo);
        }

    }

    close(sockInt);
    return 0;
  
}
