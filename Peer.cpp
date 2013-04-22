//////////////////////////////////////////////////////////////////////////////////
// Peer.cpp -														            //
// ver 1.0													                    //
//			The main execution Module of the MSockComm		
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

/*
Execution Starts Here, the command line parameters allow to take in :

Each peer inherently starts Receiver Mode:-

command line arguments:-  peer.exe -sendIP "<ipaddress>" -destPort port# (-fileName "<filePath>" or -textMsg "<text>") -listen "portNo" -sMode "sync/Async" -demoAsyncPrs "true/false"
									1		2			3			4				3		4									  

*/
//-send "1000.0.0.12" -text "<text>" -listen "4234" -ack "Async"


# define PEER_TEST

# ifdef PEER_TEST

# include "Peer.h"

# include <conio.h>


void printHeadline(std::string msg)
{
	sout<<"\n\n"<<msg<<"\n";
	sout<<locker<<std::string(std::strlen(msg.c_str())+2,'=')<<"\n"<<unlocker;
}

void callExit()
{				
	std::cout<<"\nIncomplete Argument List\n";
	std::cout<<"\ncommand line arguments:-  peer.exe -sendIP <ipaddress> -destPort port# (-fileName <completefilePath> or -textMsg <text>) -listen portNo -ack sync/Async\n";				
	std::cout<<"\n";
}

bool checkArgs(std::string consoleSwitch)
{
	std::vector<std::string> comparisions;
	comparisions.push_back("-sendIP");
	comparisions.push_back("-destPort");
	comparisions.push_back("-ack");
	comparisions.push_back("-file");
	comparisions.push_back("-text");
	comparisions.push_back("-listen");
	comparisions.push_back("-demoAsyncPrs");

	for(auto word:comparisions)
	{
		if(strcmp(word.c_str(),consoleSwitch.c_str())==0)
			return true;
	}	
	
	return false;
}

bool checkIncompleteArgs(int argc, char** argv)
{
		for(int i=1;i<argc;i++)//iterate through all the args and check if they are already  in the arglist
		{
				if(!checkArgs(argv[i]))
					return false;
		}

	//checked all arguments and all are they are not switches
	return true;
}

void checkSwitches(Peer *peer,int argc,char** argv,int i,bool & asyncProcessing)
{
		if(std::strcmp(argv[i],"-demoAsyncPrs")==0 && std::strcmp(argv[i+1],"true")==0)
			asyncProcessing=true;
		if(std::strcmp(argv[i],"-sendIP")==0)
		{
			peer->setIsSender(1);
			peer->setDestinationAddress(argv[i+1]);
		}
		if(std::strcmp(argv[i],"-destPort")==0&& i+1<argc)
		{		
				peer->setDestPort(atoi(argv[i+1]));
		}
		if(std::strcmp(argv[i],"-ack")==0&& i+1<argc)
		{
					if(std::strcmp(argv[i+1],"sync")==0||std::strcmp(argv[i+1],"Sync")==0)
						peer->setSenderMode(1);
					if(std::strcmp(argv[i+1],"async")==0||std::strcmp(argv[i+1],"Async")==0)
						peer->setSenderMode(0);
		}
		if(std::strcmp(argv[i],"-file")==0&& i+1<argc)
		{
				peer->setSendingTextFile(1);
				peer->setFileName(argv[i+1]);
		}

		if(std::strcmp(argv[i],"-text")==0&& i+1<argc)
		{
				peer->setSendingTextFile(0);			
				peer->setSenderTextString(argv[i+1]);
		}
		if(std::strcmp(argv[i],"-listen")==0 && i+1<argc)
		{
				peer->setListnerPort(atoi(argv[i+1]));
		}
}
void initializePeer(Peer *peer,int argc,char** argv,bool & asyncProcessing)
{	
	for(int i=1;i<argc;i++)
	{
		checkSwitches(peer,argc,argv,i,asyncProcessing);
	}
		
}
int main(int argc,char** argv)
{
	if((argc-1)%2!=0 || checkIncompleteArgs(argc,argv))//either argument list full i.e. pairs of key value pairs
	{
		callExit();
	}

	else
	{
		Peer *peer= new Peer();
		bool asyncProcessing=false;
		initializePeer(peer,argc,argv,asyncProcessing);

		std::string message="Peer Application started at Host IP Address:"+peer->getIPAddress();
		printHeadline(message);
			
		/*
		Priority 1: set up an ackwnowledge message once task is completed!  DONE!!
		Priority 2: demonstrate Asynchronous communication
		Priority 3: demonstrate Asynchronous Processing, a sender sends to peer 1, then breaks comm and then sends to peer 2.

		1.	shall support asynchronous communication.  That means that a client sends a message and breaks the connection. 
		If the receiver wishes to send a reply it later connects to the client to send the reply .	~-> DONE 
 
		2.	shall demonstrate asynchronous processing by starting at least two clients that each send to all of at least two running receivers. 
		The clients each request receivers to process a computational task and break the connection.  
		Receivers must reply to the requestor with the computational results. 	
		DONE~> need to send back Message fron receiver to Sender acknowledgement message
		*/
	
			if(!asyncProcessing)
				peer->startPeer();
	
			if(asyncProcessing)
			{
				sout<<locker<<"The Peer will Demonstrate Asynchronous Message Processing to Peers Listening \n \
					at 5000 and 6000 Ports\n"<<unlocker;

				peer->AsyncProcessing("127.0.0.1","127.0.0.1",6000,5000,peer->getListenerPort());
			}	

	}

	_getch();
	return 0;

}


# endif

