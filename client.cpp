/*
This is the Bank Client Program
Author: Kaushik Velusamy
Date: 10-06-2017
To Compile and Run : Follow the Make File
		
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

char transactionfile[100];
int clientID = 0;

int main (int argc, char *argv[])
{
	int timer =0,timer_counter = 0;
	int sockfd;
	int tot_trans_count = 1;
	int a=0, b=0, c=0, d=0, e=0,f=0;

	char * timer_token;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char ch;

	char clid_char[256];
	char transaction_item[256];
	char tot_trans_count_char[1];
	char buffer1[256],buffer2[256],buffer3[256], buffer4[256];
	
	float time = atof(argv[4]);	
	strcpy(transactionfile, argv[5]);
	clientID =atoi(argv[1]);

	// Creating the client socket

	sockfd = socket(AF_INET, SOCK_STREAM,0);
	server = gethostbyname(argv[2]); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

	
	
	//Connection to the server
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
	{
		cout<<endl<<endl<<"Error1: Connection to the Server Failed"<<endl;	
		return 1;
	}
	else
		cout<<"Connected to the Server"<<endl;

	
	
	
	// sending the clientID to the server part 
	
	sprintf(clid_char,"%d",clientID);
	
	a = send(sockfd, clid_char, strlen(clid_char),0);
	if(a<0)
	{
		cout<<"Error 2: could not send the client ID to Server" <<endl<<endl;
		return 1;
	}
	else
		cout<<"Sent the Client Id from client to server : "<<clientID <<endl;
	
	usleep(2000000);
	
	
	
	// Count the total transactions

	FILE *fileptr1 = fopen(transactionfile,"r");
	while(!feof(fileptr1))
	{
		ch = fgetc(fileptr1);
		if(ch=='\n')
			tot_trans_count++;	
	}
	cout<<"Total Number of Transactions to Process : "<<tot_trans_count <<endl<<endl;
	cout<<"Waiting time for each transaction : " <<time <<endl<<endl<<endl;

	
	
	//To send number of instruction to server via send, converting to char array
	sprintf(tot_trans_count_char,"%d",tot_trans_count);

	b = write(sockfd,tot_trans_count_char,strlen(tot_trans_count_char));
	if(b<0)
	{
		cout<<"Error 3: could not send the Total  number of transaction to Server" <<endl<<endl;
		return 1;
	}
	else
		cout<<endl<<"Sent from Client to Server: Total number of client Transactions"<<endl;
	
	
	
	
	
	
	// Processing the  Transaction file	
	FILE *fileptr = fopen(transactionfile,"r");
	if(fileptr == NULL)
	{
		cout<<"Error4: Couldn not open the Transaction File"<<endl;
		return 1;
	}

	while(fgets(transaction_item,sizeof(transaction_item),fileptr)!=NULL)
	{
		strcpy(buffer1,transaction_item);
		timer_token = strtok (buffer1," ,.-");
		timer = atoi(timer_token);
		
		while(timer > timer_counter)
		{
			timer_counter++;
			usleep(1000000*time);
		}
		
		
		
		//sending each transaction line
		c = write(sockfd,transaction_item,strlen(transaction_item));
		if(c < 0)
		{
			cout<<"Error 5: Transaction line was not sent to server"<<endl;	
			return 1;
		}
		else
			//Printing transaction one by one line
			cout<<"Transaction line was sent to server from client "<<endl<<endl;
		
		
		
		
		
		
		//Recieveing acknowledgement of recieving each transaction
		d = read(sockfd,buffer2,256);
		if(d < 0)
		{
			cout<<"Error 6: Transaction line ACK was not recieved from server"<<endl;	
			return 1;
		}
		else
			//Printing transaction one by one line
			cout<<"ACK: "<<buffer2<<endl<<endl;
		
		//Printing each transaction from the client 
		printf("Printing Transaction from client : %s \n", transaction_item);
		
		
		
		
		
		//To Recieve the old Status
		e = read(sockfd,buffer3,256);
		if(e < 0)
		{
			cout<<"Error 7: Old status before transaction is not recieved to client"<<endl;
			return 1;
		}
		else
			cout<<"\t Old Status Sent from Server to Client: "<<buffer3<<endl;

		
		
		
		//To Receive the new status
		f = read(sockfd,buffer4,256);
		if(f < 0)
		{
			cout<<"Error 8: New status acknowledgement is not recieved on server"<<endl;
			return 1;
		}
		else
			cout<<"\t New Status sent from Server to Client: "<<buffer4<<endl;
	
		printf("*********************\n");	
		
	}
	cout<<endl<<endl<<"All the Transactions are completed Successfully. Closing the client connection"<<endl<<endl<<endl;
	close(sockfd);
	return 0;
}


