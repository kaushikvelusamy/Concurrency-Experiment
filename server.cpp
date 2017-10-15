/*
This is the Bank Server Program
Author: Kaushik Velusamy
Date: 10-06-2017
To Compile and Run : Follow the Make File
		
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


using namespace std;



// Defining the Global Variables 

void *create_thread(void *);

struct records
{
	int account_number;
	char name[25];
	int balance; 
};

struct records array_record[6000];

pthread_mutex_t mutex;

char recordfile[100];






int main (int argc, char *argv[])
{
	int socket_desc, client_sock, clilen;
	int *new_sock;
	int enable = 1;
	struct sockaddr_in server, client;

	
	strcpy(recordfile, argv[2]);

	//creating the socket connection
	socket_desc = socket(AF_INET, SOCK_STREAM,0);

	if(socket_desc == -1)
		cout<<"Error 1: Socket Creation Failed"<<endl;
	else
		cout<<endl<<endl<<"Socket connection created successfully"<<endl;
 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));

	
	if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		    cout<<"Error 2: setsockopt(SO_REUSEADDR) failed"<<endl;


	//binding the socket
	if(bind(socket_desc,(struct sockaddr *) &server,sizeof(server)) <0)
	{
		cout<<"Error 3: Failure to Bind to the socket"<<endl;
		return 1;
	}
	else
		cout<<"******Server Started******"<<endl; 
	
	
	//listening to the socket 
	listen(socket_desc,100);
	cout<<endl<<endl<<"Waiting to Accept Incomming connections"<<endl;

	//Accepting the client Connections
	clilen = sizeof(struct sockaddr_in);

	while(client_sock = accept(socket_desc,(struct sockaddr *) &client,(socklen_t *)&clilen))
	{
        cout<<"Established Connection to Client"<<endl;
		new_sock = new int(1);	
		*new_sock = client_sock;
		pthread_t pid;
		if(pthread_create(&pid,NULL,create_thread,(void*)new_sock) < 0)	
		{
			cout<<"Error 4: could not create the threads"<<endl;
			return 1;
		}
		
	}
	
	if(client_sock<0)
	{
		cout<<"Error 5: Accepting connection to the client Failed Badly. You are screwed"<<endl;
		return 1;
	}
	return 0;
}





void *create_thread(void *socketdesc)
{
	int master_sock = *(int *)socketdesc;
	char buffer[256],buffer1[256], buffer2[256];
	
	memset(&buffer[0], 0, sizeof(buffer));
	memset(&buffer1[0], 0, sizeof(buffer1));
	memset(&buffer2[0], 0, sizeof(buffer2));
	
	//Opening records file
	FILE *fileptr = fopen(recordfile,"r");
	int record_size = 0;
	char records_line[256];
	char *split;


	//Filling up the structure array from records.txt
	while(fgets(records_line,sizeof(records_line),fileptr))
	{
		split = strtok (records_line," ,.-");

		while(split!= NULL)
		{
			array_record[record_size].account_number = atoi(split);
			
			split = strtok (NULL, " ,.-");
			strcpy(array_record[record_size].name,split);
			
			split = strtok (NULL, " ,.-");
			array_record[record_size].balance = atoi(split);		
			
			split = strtok (NULL, " ,.-");
		}
		record_size++;
	}

	cout<<endl<<endl<<"Total Number of lines in Records Database is "<<record_size<<endl;

	//Printing the record Database
	for (int i = 0; i < record_size; i++)
	{
		cout<<"Record "<<i<<"\t";
		cout<<array_record[i].account_number<<"\t";
		cout<<array_record[i].name<<"\t";
		cout<<array_record[i].balance<<endl;
	}
	
	//Recieve the clientID from the client
	
	memset(&buffer2[0], 0, sizeof(buffer2));
	int clid = 0;
	int h = recv(master_sock, buffer2, 256, 0);
	clid = atoi(buffer2);
	memset(&buffer2[0], 0, sizeof(buffer2));
	
	cout<<"The client ID is : " <<clid<<endl<<endl;
	
		
	memset(&buffer[0], 0, sizeof(buffer));
	memset(&buffer1[0], 0, sizeof(buffer1));
	memset(&buffer2[0], 0, sizeof(buffer2));
	

	//To Recieve the transactions from the client
	
	int size_of_transaction = recv(master_sock,buffer1,256,0);
	int instruction_size = atoi(buffer1);
	int instruction_counter = instruction_size;
	
	int trans_timestamp,trans_acc_num, trans_amount;
	char trans_type;
	
	
	int len =0;
	char *t_split;
	char status_old[256],status_new[256];
	cout<<endl<<"From Server: Reading Transactions sent by Client"<<endl<<endl;
		
	memset(&buffer[0], 0, sizeof(buffer));
	memset(&buffer1[0], 0, sizeof(buffer1));
	memset(&buffer2[0], 0, sizeof(buffer2));
	

	do{
		memset(&buffer[0], 0, sizeof(buffer));
		memset(&buffer1[0], 0, sizeof(buffer1));
		memset(&buffer2[0], 0, sizeof(buffer2));
	
		len = recv(master_sock,buffer,256,0)>0;
		len = write(master_sock," From Server: Started Processing your transaction Mentioned Below",256);
		
		cout<<"\n\nFrom ClientID :"<<clid<<"\tTransaction  "<<instruction_size - instruction_counter<<"\t\t" <<buffer<<endl<<endl;
		
		t_split = strtok (buffer," ,.-");
		trans_timestamp = atoi(t_split);
		
		t_split = strtok (NULL, " ,.-");
		trans_acc_num = atoi(t_split);
		
		t_split = strtok (NULL, " ,.-");
		trans_type = *t_split;
		
		t_split = strtok (NULL, " ,.-");
		trans_amount = atoi(t_split);
		
		t_split = strtok (NULL, " ,.-");
		
		for (int i = 0; i <record_size; i++)
		{
			if(trans_acc_num == array_record[i].account_number)
			{	
				if(trans_type == 'd')
				{
					pthread_mutex_lock(&mutex);
					sprintf(status_old,"\t Name :%s \t Old balance : %d \n",array_record[i].name,array_record[i].balance);
					printf("\t Old Status : %s \n", status_old);
					write(master_sock,status_old,256);
					array_record[i].balance = array_record[i].balance+trans_amount;
					sprintf(status_new,"New balance : %d$ \t Deposited Amount : %d$ \n",array_record[i].balance, trans_amount);
					printf("\t New Status : %s \n", status_new);
					cout<<"********"<<endl<<endl;
					write(master_sock,status_new,256);
					pthread_mutex_unlock(&mutex);
					break;
				}
				else if(trans_type == 'w')
				{
					if(trans_amount>array_record[i].balance)
					{
						pthread_mutex_lock(&mutex);
						sprintf(status_old,"\t Name: %s \t Old balance : %d \n",array_record[i].name,array_record[i].balance);
						printf("\t Old Status : %s \n", status_old);
						write(master_sock,status_old,256);
						sprintf(status_new,"Transaction Declined: Insufficient balance. \n\t\tYour balance was %d and Withdraw Request is %d",array_record[i].balance,trans_amount);
						printf("\t New Status : %s \n", status_new);
						cout<<"********"<<endl<<endl;
						write(master_sock,status_new,256);
						pthread_mutex_unlock(&mutex);
						break;
					}
					else
					{
						pthread_mutex_lock(&mutex);
						sprintf(status_old,"\t Name: %s \t Old balance : %d \n",array_record[i].name,array_record[i].balance);
						printf("\t Old Status : %s \n", status_old);
						write(master_sock,status_old,256);
						array_record[i].balance = array_record[i].balance - trans_amount;	
						sprintf(status_new,"New balance : %d$ \t Withdrawn Amount : %d$ \n",array_record[i].balance, trans_amount);
						printf("\t New Status : %s \n", status_new);
						cout<<"********"<<endl<<endl;
						write(master_sock,status_new,256);
						pthread_mutex_unlock(&mutex);
						break;

					}
					
				}
				
			}
			else if (trans_acc_num!= array_record[i].account_number && (i == (record_size-1)))
			{
				printf("\t **Wrong account number and  Transaction declined** \n");
				cout<<"********"<<endl<<endl;
				write(master_sock,"\t Old Status: \t Wrong Account Number\n",256);
				write(master_sock,"\t New Status: \t Transaction Declined\n",256);
				break;
			}
		

		}
			
	instruction_counter--;
	memset(&buffer[0], 0, sizeof(buffer));
	memset(&buffer1[0], 0, sizeof(buffer1));
	memset(&buffer2[0], 0, sizeof(buffer2));
	}while(instruction_counter>1);

close(master_sock);
}




