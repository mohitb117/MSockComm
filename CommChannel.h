//////////////////////////////////////////////////////////////////////////////////
// CommChannel.h - Represents Functionality for File Chnuking and Reconstruction//
//				   between peers(in binary as well as text)			            //
// ver 1.0												                    //
//   It contains Listner Socket and a Sender Socket with						//
//               (Dynamically Allocated Receiver Sockets)						//
//																				//
//The Peer Threads (i.e. Sender, reciver threads								//
//      use the Communication channel BQ in order to Send/Receive messages)     //	
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////



# include "Sockets.h"
# include "BlockingQueue.h"//include sockets before blocking queue

# include "Sender.h"
# include "Reciever.h"

class CommChannel
{

public:
	CommChannel(int listnerPort,bool is_file,std::string fileNameOrTextMessage="",bool isSync=true)
	{
		sndr=new Sender(listnerPort,is_file,OPBQ,fileNameOrTextMessage,isSync);
		rcvr=new Receiver(IPBQ);
	}

	BlockingQueue<std::string> getIPBQ()
	{return IPBQ;}

	BlockingQueue<std::string> getOPBQ()
	{return OPBQ;}

	Socket* getSenderSocket()
	{return sender;}

	SocketListener* getListnerSocket()
	{return sockListner;}

	Sender* getSender()
	{return sndr;}

	Receiver* getReciever()
	{return rcvr;}

	SendThread* getSenderThread()
	{return sThread;}

	ListenThread* getListnerThread()
	{return lThread;}

private:
	BlockingQueue<std::string> IPBQ;
	
	BlockingQueue<std::string> OPBQ;
	
	Socket* sender;
	
	SocketListener *sockListner;
	Sender *sndr;

	Receiver *rcvr;
	
	SendThread *sThread;
	ListenThread *lThread;
};
