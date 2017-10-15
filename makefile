ServerProgram	= server.cpp
ClientProgram	= client.cpp
ServerObject    = Server
ClientObject    = Client
CPP     		= g++
FLAGS  			= -std=c++11 -pthread
PORTNUM			= 8005
IP				= 127.0.0.1
TIMESTEP		= 0.1
RECORDFILE		= Records.txt
TRANSACTIONFILE	= Transactions.txt
CLIENTID		= 1

all: compilingserver compilingclient startingserver	

compilingserver:
		$(CPP) -o $(ServerObject) $(ServerProgram) $(FLAGS)
compilingclient:
		$(CPP) -o $(ClientObject) $(ClientProgram)
startingserver:
		./$(ServerObject) $(PORTNUM) $(RECORDFILE)

#To start the client follow the below command in a separate terminal
#startingclient:
#		./Client 1 127.0.0.1 8005 0.1
#		./$(ClientObject) $(CLIENTID) $(IP) $(PORTNUM) $(TIMESTEP) $(TRANSACTIONFILE)

clean:
		rm $(ServerObject) $(ClientObject)
