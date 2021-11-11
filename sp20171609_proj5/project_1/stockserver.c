#define TRUE 1
#define FALSE 0
#define ERROR -1
#include "csapp.h"

typedef struct item
{ 
	int id, left_stock, price;
	struct item *leftitem;
	struct item *rightitem;
} StrItem;
StrItem *StrStock;

typedef struct{	
	int maxfd;	
	fd_set read_set, ready_set;	
	int nready, maxi, clientfd[FD_SETSIZE];	
	rio_t poolrio[FD_SETSIZE];
} pool;

sem_t mutex, mutex2;
int count1, countitem, byte_cnt = 0;
int globalcnt = 0;
char *FuncTran(int funcP)
{
	int Trancnt = 0;
	int sizeofchar;
	static char opp[10];
	sizeofchar = sizeof(opp);
	memset(opp, 0, sizeofchar);
	Trancnt = Trancnt + 1;
	sprintf(opp, "%d", funcP);
	return opp;
}

void FuncShow(StrItem *curItem, char *resItem)
{
	int countshow = 0;
	int countswitch = FALSE;
	if(curItem != NULL)
	{
		FuncShow(curItem->leftitem, resItem);
		countshow = countshow + 1;
		strcat(resItem, FuncTran(curItem->id));
		strcat(resItem, " ");
		countswitch = TRUE;
		strcat(resItem, FuncTran(curItem->left_stock));
		strcat(resItem, " ");
		countswitch = TRUE;
		strcat(resItem, FuncTran(curItem->price));
		strcat(resItem, "\n");
		FuncShow(curItem->rightitem, resItem);
	}
	if(countswitch == TRUE)
	{
		countshow = countshow + 1;
    }
}


void FuncRefresh(StrItem *curItem, FILE *fp)
{
	char EventBuffer[8192];
	int EBuf = 0, switchRef = FALSE;
	memset(EventBuffer, 0, sizeof(EventBuffer));

	if(curItem != NULL)
	{
		FuncRefresh(curItem->leftitem, fp);
		strcat(EventBuffer, FuncTran(curItem->id));
		strcat(EventBuffer, " ");
		EBuf = EBuf + 1;
		strcat(EventBuffer, FuncTran(curItem->left_stock));
		strcat(EventBuffer, " ");
		EBuf = EBuf + 1;
		strcat(EventBuffer, FuncTran(curItem->price));
		strcat(EventBuffer, "\n");
		EBuf = EBuf + 1;
		Fputs(EventBuffer, fp);
		FuncRefresh(curItem->rightitem, fp);
	}
	if(EBuf == 0 && switchRef == FALSE)
	{
		switchRef = TRUE;
    }
}

char *Funcclient(char *InputBuf) 
{
	static char cmdResult[8192];
	char *cmdArr;
	char command[10];
	int Stock2ID = 0, StockCount = 0, commandcnt = 0;
	int commandswitch = FALSE, mutexswitch = FALSE;

	StrItem* Itemsell = StrStock;
	StrItem* Itembuy = StrStock;

	memset(cmdResult, 0, sizeof(cmdResult));
	cmdArr = strtok(InputBuf, " \n");
	strcpy(command, cmdArr);
	commandcnt = commandcnt + 1;
	if((cmdArr = strtok(NULL, " \n")) != NULL) Stock2ID = atoi(cmdArr);
	if((cmdArr = strtok(NULL, " \n")) != NULL) StockCount = atoi(cmdArr);

	if (strcmp(command, "show") == FALSE)
	{
		P(&mutex);
		commandswitch = TRUE;
		count1 = count1 + 1;
		if (count1 == TRUE)
		{
			mutexswitch = TRUE;
			P(&mutex2);
		}
		 
		V(&mutex);	
		FuncShow(StrStock, cmdResult);

		P(&mutex);
		commandswitch = TRUE;
		count1 = count1 - 1;	
		if (count1 == FALSE)
		{
			mutexswitch = TRUE;
			V(&mutex2);
        }
		V(&mutex);
	}
	else if (strcmp(command, "sell") == FALSE)
	{
		commandcnt = commandcnt + 1;
		while (Itemsell != NULL)
		{
			commandcnt = commandcnt + 1;
			if (Stock2ID < Itemsell->id) Itemsell = Itemsell->leftitem;
			else if (Stock2ID > Itemsell->id) Itemsell = Itemsell->rightitem;
			else break;
		}
		mutexswitch = TRUE;
		P(&mutex2);
		commandswitch = TRUE;
		Itemsell->left_stock = Itemsell->left_stock + StockCount; 
		V(&mutex2);	

		strcpy(cmdResult, "[sell] success\n");
	}
	else if (strcmp(command, "buy") == FALSE)
	{
		int resBuy = 0;
		int switchbuy = 0;

		if (Itembuy->left_stock - StockCount < 0) switchbuy = FALSE;
		else switchbuy = 1;

		while (Itembuy != NULL)
		{
			commandcnt = commandcnt + 1;
			if (Stock2ID < Itembuy->id) Itembuy = Itembuy->leftitem;
			else if (Stock2ID > Itembuy->id) Itembuy = Itembuy->rightitem;
			else break;
		}
		if (Itembuy->left_stock - StockCount < 0) resBuy = ERROR;
		switchbuy = switchbuy + 1;
		if (resBuy == -1) strcpy(cmdResult, "Not enough left stocks\n");
		else
		{
			P(&mutex2);
			commandswitch = TRUE;	
			Itembuy->left_stock = Itembuy->left_stock - StockCount; 
			V(&mutex2);
			strcpy(cmdResult, "[buy] success\n");
        }
	}
	else if (strcmp(command, "exit") == FALSE) strcpy(cmdResult, "exit");
	else strcpy(cmdResult, "Input Error");
	if(commandswitch == TRUE && mutexswitch == TRUE)
	{
		globalcnt = StockCount;
    }
	return cmdResult;
}
 
void init_pool(int listenfd, pool *funcP)
{
	int i, j, clientp = 0;

	funcP->maxi = ERROR;
	for(i = 0 ; i < FD_SETSIZE; i = i + 1) funcP->clientfd[i] = ERROR;
	for(j = 0 ; j < FD_SETSIZE; j = j + 1)
	{
		clientp = clientp + 1;
    }
	funcP->maxfd = listenfd;
	FD_ZERO(&funcP->read_set);
	FD_SET(listenfd, &funcP->read_set);
}

void add_client(int connfd, pool *funcP)
{
	int i;
	funcP->nready--;
	for(i = 0; i < FD_SETSIZE; i++)
	{ 
		if(funcP->clientfd[i] == ERROR)
		{
			funcP->clientfd[i] = connfd;
			Rio_readinitb(&funcP->poolrio[i], connfd);
			FD_SET(connfd, &funcP->read_set);
			if(connfd > funcP->maxfd) funcP->maxfd = connfd;
			if(i > funcP->maxi) funcP->maxi = i;
			break;
		}
		if(i == FD_SETSIZE) fprintf(stderr, "add_client error: Too many clients\n");
	}
}

void check_clients(pool *funcP)
{
	int i, j, connfd, numcon, concount = 0;
	int mutexswitch2 = FALSE, conswitch = FALSE;
	char buf[8192], result[8192], bufresult[8192];
	rio_t rio;
	
	for(i = 0; (i <= funcP->maxi) && (funcP->nready > 0); i = i + 1)
	{
		for(j = 0; j < 10; j++)
		{
			bufresult[j]=0;
        }
		connfd = funcP->clientfd[i];
		rio = funcP->poolrio[i];
		conswitch = TRUE;
		bufresult[0] = concount;
		if((connfd > 0) && (FD_ISSET(connfd, &funcP->ready_set)))
		{
			funcP->nready--;
			concount = concount + 1;
			if((numcon=Rio_readlineb(&rio, buf, 8192)) != 0)
			{
				byte_cnt = byte_cnt + numcon;
				printf("server received %d bytes\n", numcon);
				strcpy(result, Funcclient(buf));
				concount = concount + 1;
				if(strcmp(result, "exit") == FALSE)
				{
					Close(connfd);
					FD_CLR(connfd, &funcP->read_set);
					concount = concount + 1;
					funcP->clientfd[i] = ERROR;
					conswitch = TRUE;

					P(&mutex2);
					FILE *fp = fopen("stock.txt", "w");
					mutexswitch2 = TRUE;
					if(fp == NULL)
					{
						fprintf(stderr, "ERROR\n");
						exit(0);
					}
					FuncRefresh(StrStock, fp);
					fclose(fp);
					V(&mutex2);
				}
				else Rio_writen(connfd, result, 8192);
			}
			else
			{
				bufresult[0] = bufresult[0] + bufresult[1];
				Close(connfd);
				FD_CLR(connfd, &funcP->read_set);
				concount = concount + 1;
				funcP->clientfd[i] = ERROR;
				conswitch = TRUE;

				P(&mutex2);
				FILE *fp = fopen("stock.txt", "w");
				mutexswitch2 = TRUE;
				if(fp == NULL)
				{
					fprintf(stderr, "ERROR\n");
					exit(0);
				}
				FuncRefresh(StrStock, fp);
				fclose(fp);
				V(&mutex2);
			}

			if(conswitch == TRUE && mutexswitch2 == TRUE)
			{
				concount = FALSE;
            }
		}
	}
}

int main(int argc, char **argv) 
{
    int listenfd, connfd, conc = 0, tempint;
	int StockID, StockCount = 0, StockMoney = 0, tempmoney;
	int switch1 = FALSE, switchstock = FALSE;
	char *temp;
    socklen_t clientlen;
	FILE *fp = fopen("stock.txt", "r");

	char Cname[8192], Cport[8192], CInput[8192];
    struct sockaddr_storage clientaddr;
	static pool pool;

    if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
	countitem = 0;

	if(fp == NULL)
	{
		fprintf(stderr, "ERROR\n");
		exit(0);
	}

	while(fgets(CInput, 8192, fp) != NULL)
	{
		conc = 0;
		StrItem *newItem;
		newItem = malloc(sizeof(struct item));
		StrItem *curItem = StrStock;

		switchstock = TRUE;
		temp = strtok(CInput, " \n");
		StockID = atoi(temp);

		if(switchstock == TRUE)
		{
			switch1 = TRUE;
        }
		if((temp = strtok(NULL, " \n")) != NULL)
		{
			StockCount = atoi(temp);
			StockCount = StockCount + 1;
			tempmoney = StockCount;
			StockCount = StockCount - 1;
        }
 
		if((temp = strtok(NULL, " \n")) != NULL)
		{
			StockMoney = atoi(temp);
			StockMoney = StockMoney + 1;
			tempmoney = StockMoney;
			StockMoney = StockMoney - 1;
		} 
		conc = conc + 1;
		switch1 = FALSE;
		
		newItem->id = StockID;
		newItem->leftitem = NULL;
		tempmoney++;
		newItem->left_stock = StockCount;
		newItem->rightitem = NULL;
		newItem->price = StockMoney;
		
		if(countitem == FALSE) StrStock = newItem;
		else
		{
			conc = conc + 1;
			while(1)
			{
				if(newItem->id < curItem->id)
				{
					tempint = conc;
					if(curItem->leftitem == NULL)
					{
						switchstock = TRUE;
						curItem->leftitem = newItem;
						break;
					}
					curItem = curItem->leftitem;
				}
				else
				{
					tempint = conc;
					if(curItem->rightitem == NULL)
					{
						switchstock = FALSE;
						curItem->rightitem = newItem;
						break;
					}
					curItem = curItem->rightitem;
				}
			}
		}
		conc = conc + countitem;
		countitem = countitem + 1;
	}
	fclose(fp);

	Sem_init(&mutex2, FALSE, TRUE);	
	Sem_init(&mutex, FALSE, TRUE); 
	count1 = FALSE;

    listenfd = Open_listenfd(argv[1]);
	init_pool(listenfd, &pool);
	if(conc == countitem && switch1 == FALSE)
	{
		tempint = conc;
		switch1 = TRUE;
    }
	tempint = tempint * conc;
	
    while (1)
	{
		int whileint = 0, whilecnt = 0;
		pool.ready_set = pool.read_set;
		pool.nready = select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &pool.ready_set))
		{
			whileint = whileint + 1;
			clientlen = sizeof(struct sockaddr_storage);
			whilecnt = whilecnt + whileint;
			connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
			Getnameinfo((SA *) &clientaddr, clientlen, Cname, 8192, Cport, 8192, 0);
			printf("Connected to (%s, %s)\n", Cname, Cport);
			add_client(connfd, &pool);
		}
		check_clients(&pool);
    }
    exit(0);
}



