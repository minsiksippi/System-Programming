#define TRUE 1
#define FALSE 0
#define ERROR -1
#include "csapp.h"

typedef struct item
{ 
	int id, left_stock, price;		
	struct item *leftitem, *rightitem;
} StrItem;
StrItem *StrStock;

typedef struct
{
	int *buf;
	int n, front, rear;
	sem_t mutex, slots, items;
} sbuf_t;
sbuf_t structBuf;

sem_t mutex, mutex2;
int count1, count2, globalcnt;

char *FuncTran(int p)
{
	int Trancnt = 0, sizeofchar;
	static char opp[10];
	sizeofchar = sizeof(opp);
	memset(opp, 0, sizeofchar);
	Trancnt = Trancnt + 1;
	sprintf(opp, "%d", p);

	return opp;
}

void FuncShow(StrItem *curItem, char *resItem)
{
	int countshow = 0, countswitch = FALSE;
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


void FuncThread(int connfd) 
{
	rio_t Threadtemp;
    int threadbyte, ThreadId = 0, StockCount = 0, commandcnt = 0;;
	int commandswitch = FALSE, mutexswitch = FALSE, tempn = 0;
    char ThreadBuffer[8192], ThreadResult[8192], command[10];
	char *Threadarr;
    Rio_readinitb(&Threadtemp, connfd);

    while((threadbyte = Rio_readlineb(&Threadtemp, ThreadBuffer, 8192)) != FALSE)
	{
		StrItem *Itemsell = StrStock;
		StrItem	*Itembuy = StrStock;

		printf("server received %d bytes\n", threadbyte); 
		memset(ThreadResult, 0, sizeof(ThreadResult));
		commandcnt = commandcnt + 1;
		Threadarr = strtok(ThreadBuffer, " \n");
		strcpy(command, Threadarr);
		if(commandswitch==TRUE)
		{
			commandswitch = TRUE;
			mutexswitch = TRUE;
        }
		if((Threadarr = strtok(NULL, " \n")) != NULL) ThreadId = atoi(Threadarr);
		if((Threadarr = strtok(NULL, " \n")) != NULL) StockCount = atoi(Threadarr);
		
		if(strcmp(command, "show") == FALSE)
		{
			P(&mutex);
			commandswitch = TRUE;
			count2 = count2 + 1;
			if(count2 == TRUE)
			{
				mutexswitch = TRUE;
				P(&mutex2);
            }
			V(&mutex);

			FuncShow(StrStock, ThreadResult);
			P(&mutex);
			commandswitch = TRUE;
			count2 = count2 - 1;
			if(count2 == FALSE)
			{
				mutexswitch = TRUE;
				V(&mutex2);
            }
			V(&mutex);
		}
		else if(strcmp(command, "exit") == FALSE)
		{
			strcpy(ThreadResult, "exit\n");
			commandcnt = commandcnt + 1;
			return;
		}
		else if(strcmp(command, "sell") == FALSE)
		{
			commandcnt = commandcnt + 1;
			while(Itemsell != NULL)
			{
				tempn = tempn + 1;
				if(ThreadId < Itemsell->id) Itemsell = Itemsell->leftitem;
				else if(ThreadId > Itemsell->id) Itemsell = Itemsell->rightitem;
				else break;
			}
			tempn = commandcnt;
			P(&mutex2);
			commandswitch = TRUE;
			Itemsell->left_stock = Itemsell->left_stock + StockCount; 
			V(&mutex2);	

			strcpy(ThreadResult, "[sell] success\n");
		}
		else if(strcmp(command, "buy") == FALSE)
		{
			int resBuy = FALSE;
			int switchbuy = FALSE;

			if (Itembuy->left_stock - StockCount < 0) switchbuy = FALSE;
			else switchbuy = TRUE;

			while (Itembuy != NULL)
			{
				if (ThreadId < Itembuy->id) Itembuy = Itembuy->leftitem;
				else if (ThreadId > Itembuy->id) Itembuy = Itembuy->rightitem;
				else break;
				switchbuy = TRUE;
			}

			if (Itembuy->left_stock - StockCount < 0) resBuy = ERROR;
			if (resBuy == ERROR) strcpy(ThreadResult, "Not enough left stocks\n");
			else
			{
				switchbuy = TRUE;
				P(&mutex2);
				commandswitch = TRUE;	
				Itembuy->left_stock = Itembuy->left_stock - StockCount; 
				V(&mutex2);
				switchbuy = switchbuy + 1;
				strcpy(ThreadResult, "[buy] success\n");
			}
		}
		else
		{
			strcpy(ThreadResult, "invalid\n");
			return;
		}
		if(commandswitch == TRUE && mutexswitch == TRUE)
		{
			globalcnt = StockCount;
		}
		Rio_writen(connfd, ThreadResult, 8192);
    }
}

void sbuf_init(sbuf_t *sp, int n)
{
	int count1 = 0;
	sp->buf = Calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	count1 = count1 + 1;
	Sem_init(&sp->mutex, 0, 1);
	Sem_init(&sp->slots, 0, n);
	Sem_init(&sp->items, 0, 0);	
}

void sbuf_insert(sbuf_t *sp, int item)
{
	int count2 = 0;
	P(&sp->slots);
	P(&sp->mutex);
	count2 = count2 + 1;
	sp->buf[(++sp->rear)%(sp->n)] = item;
	V(&sp->mutex);
	V(&sp->items);
	count2 = count2 + 1;
}

int sbuf_remove(sbuf_t *sp)
{
	int count3 = 0;
	int item;
	P(&sp->items);
	P(&sp->mutex);
	count3 = count3 + 1;
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->mutex);
	V(&sp->slots);
	return item;
}

void sbuf_deinit(sbuf_t *sp)
{
	Free(sp->buf);
}

void FuncRefresh(StrItem *curItem, FILE *rffp)
{
	char EventBuffer[8192];
	int EBuf = 0, switchRef = FALSE;
	memset(EventBuffer, 0, sizeof(EventBuffer));

	if(curItem != NULL)
	{
		FuncRefresh(curItem->leftitem, rffp);
		strcat(EventBuffer, FuncTran(curItem->id));
		strcat(EventBuffer, " ");
		EBuf = EBuf + 1;
		strcat(EventBuffer, FuncTran(curItem->left_stock));
		strcat(EventBuffer, " ");
		EBuf = EBuf + 1;
		strcat(EventBuffer, FuncTran(curItem->price));
		strcat(EventBuffer, "\n");
		EBuf = EBuf + 1;
		Fputs(EventBuffer, rffp);
		FuncRefresh(curItem->rightitem, rffp);
	}
	if(EBuf == 0 && switchRef == FALSE)
	{
		switchRef = TRUE;
    }
}


void *FuncTh(void *aa)
{
	Pthread_detach(pthread_self());
	while(1)
	{
		int connfd = sbuf_remove(&structBuf);
		FuncThread(connfd);	
		Close(connfd);

		P(&mutex2);	
		FILE *fp = fopen("stock.txt", "w");
		if(fp == NULL)
		{
			fprintf(stderr, "ERROR\n");
			exit(0);
		}

		FuncRefresh(StrStock, fp);
		fclose(fp);
		V(&mutex2);	
	}
}


int main(int argc, char **argv) 
{
	socklen_t opera;
	pthread_t thread_id;
	FILE *fp = fopen("stock.txt", "r");

	struct sockaddr_storage Cadd;  
    int listenfd, connfd, i, tempcount = 0, ccount = 0;
	int Cid, j, opper = 0, money = 0, moneyarr[10], Carr[1024];
	int switch1 = FALSE, switch2 = FALSE;
	char Cname[8192], Cport[8192], Cline[8192];
	char *linearr;

	count1 = 0;
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }

	if(fp == NULL)
	{
		fprintf(stderr, "ERROR\n");
		exit(0);
	}

	while(fgets(Cline, 8192, fp) != NULL)
	{
		StrItem *newItem, *curItem = StrStock;
		newItem = malloc(sizeof(struct item));
		tempcount = tempcount + 1;
		if(tempcount == ERROR)
		{
			switch1 = TRUE;
        }
		linearr = strtok(Cline, " \n");
		Cid = atoi(linearr);
		switch2 = TRUE;
		if((linearr = strtok(NULL, " \n")) != NULL) opper = atoi(linearr);
		if((linearr = strtok(NULL, " \n")) != NULL) money = atoi(linearr);
		ccount = tempcount;

		newItem->leftitem = NULL;
		newItem->rightitem = NULL;
		newItem->id = Cid;
		newItem->left_stock = opper;
		newItem->price = money;
		for(j = 0; j < 10; j++)
		{
			moneyarr[j] = j;
        }
		if(count1 == FALSE) StrStock = newItem;
		else
		{
			while(1)
			{
				moneyarr[0] = moneyarr[0] + moneyarr[1];
				if(newItem->id < curItem->id)
				{
					switch2 = TRUE;
					if(curItem->leftitem == NULL)
					{
						switch1 = TRUE;
						curItem->leftitem = newItem;
						break;
					}
					curItem = curItem->leftitem;
				}
				else
				{
					switch2 = TRUE;
					if(curItem->rightitem == NULL)
					{
						switch1 = TRUE;
						curItem->rightitem = newItem;
						break;
					}
					curItem = curItem->rightitem;
				}
			}
		}
		if(switch1 == TRUE && switch2 == TRUE)
		{
			ccount = ccount + tempcount;
        }
		count1 = count1 + 1;
	}
	if(tempcount == ERROR)
	{
			switch1 = TRUE;
    }
	fclose(fp);

	Sem_init(&mutex2, 0, 1);
	Sem_init(&mutex, 0, 1);
	
	count2 = 0;		
	for(j = 0; j < 10; j++)
	{
		moneyarr[j] = 0;
    }
    listenfd = Open_listenfd(argv[1]); 
	sbuf_init(&structBuf, 8192);		
	for(i = 0; i < 1024; i = i + 1) Pthread_create(&thread_id, NULL, FuncTh, NULL);
	for(j = 0; j < 1024; j = j + 1)
	{
		Carr[j] = 0;
    }
	Carr[0] = Carr[0] + 1;
    while (1)
	{
		switch1 = FALSE;
		switch2 = FALSE;
		opera = sizeof(struct sockaddr_storage); 
		connfd = Accept(listenfd, (SA *)&Cadd, &opera); 
		Getnameinfo((SA *) &Cadd, opera, Cname, 8192, Cport, 8192, 0); 
        printf("Connected to (%s, %s)\n", Cname, Cport); 
		sbuf_insert(&structBuf, connfd); 
    }

	sbuf_deinit(&structBuf);
    exit(0);
}





