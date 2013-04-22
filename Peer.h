////////////////////////////////////////////////////////////////////////////////////
// Peer.h - Represents abstractions of a Peer Class which contains threads to start//
//				receivers and senders on seperate threads						//
// ver 1.0												                        //
//   It contains information from the console on how to configure sockets		//
//		for outbound and inbound communication from one peer to the other		//
//																				//	
//																				//
// Language:    Visual C++,														//
// Platfrom:    Lenovo U410 Ultrabook Win7x64									//
// Application: CSE687 - Object Oriented Design, Spring 2013                    //
// Author:		Mohit Bhalla Syracuse University					            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University                     //
//              (315) 443-3948, jfawcett@twcny.rr.com                           //
//////////////////////////////////////////////////////////////////////////////////

/*
Peer.h
The main idea behind a Peer:-
it contains a Communication Channel Object 
It contains:- SenderThread, Listener Thread and Dynamically Allocated(Aggregates) ReceiverThreads
Plus implements Asynchronous Processing among threads.
*/
# include "CommChannel.h"

class sThread : public threadBase
{
public:
	sThread(Sender* sndr,std::string ip="127.0.0.1",int port=8080) : sndr_(sndr),ip(ip),portNo(port) {}
private:
	
  void run()
  {
	  sndr_->start(ip,portNo);
  }
  
  Sender* sndr_;
  std::string ip;
  int portNo;
};

class rThread : public threadBase
{
public:
  
rThread(Receiver* rcvr, int port=8080) : rcvr_(rcvr),portNo(port) 
{
}

private:
  
  void run()
  {
    rcvr_->start(portNo);
  }

  Receiver* rcvr_;
  int portNo;
};


class Peer
{
	public:
	Peer()
	{
		hostIPAddress=getIPAddress();
		sendingTextFile=false;
		isSender=false;
		isSenderSync=false;
		listenPortNo=8080;
		destPortNo=8080;
		senderString="Automated Generated Message";
	}


	void startPeer(std::string fileName="", bool isPath=false)//fragment startPeer to concurrently start independently sender thread(and pass a single file path or text messafe) and single reciever thread
	{
		try
		{
			if(!isPath)
			{
					if(isSender && !sendingTextFile) 			//Sending text message and Listening at port number
						comm=new CommChannel(listenPortNo,this->isSendingTextFile(),this->getSenderTextString(),this->getSenderMode());      //portNo is the port where data is being listened

					if(isSender && sendingTextFile)//sending a file
						comm=new CommChannel(listenPortNo,this->isSendingTextFile(),this->getFileName(),this->getSenderMode());      //portNo is the port where data is being listened
			
					if(!isSender)//only listner mode
						comm=new CommChannel(listenPortNo,false);      //portNo is the port where data is being listened
			}	

			else
			{
					if(isSender && sendingTextFile)//sending a file
						comm=new CommChannel(listenPortNo,true,fileName);      //portNo is the port where data is being listened
			}
			
			//first start reciever
			//then start sender
			rThread r1(comm->getReciever(),listenPortNo);		

			sThread s1(comm->getSender(),destIPAddress,destPortNo);
		
			r1.start();
			
			if(isSender)
			{
				s1.start();
			}
		
			r1.join();		
			s1.join();//halts the execution of main thread until the thread completes execution
		}
		catch(std::exception ex)
		{
			sout<<locker<<"Exception!!\n"<<ex.what()<<unlocker;
		}
	}


	void AsyncProcessing(std::string d1IP,std::string d2IP,int portd1,int portd2,int listen)
	{
						//Sending text message and Listening at port number
			comm=new CommChannel(listen,false,"Asynchronous Message",true);      //portNo is the port where data is being listened

		   rThread r1(comm->getReciever(),listen);
		   r1.start();
		   
		    sendAsyncMsg * a1=(new sendAsyncMsg());
			a1->sendMsg(d1IP,portd1,listen,"Sending Message from Peer at "+getIPAddress());
			delete a1;
			
			sendAsyncMsg *a2=(new sendAsyncMsg());
			a2->sendMsg(d2IP,portd2,listen,"Sending Message from Peer at "+getIPAddress());
			delete a2;
	}

	void setDestinationAddress(std::string dest)
	{
		destIPAddress=dest;
	}
//////////////////////////////
	void setIsSender(bool flag)
	{
		isSender=flag;
	}

	bool IsSender(bool flag)
	{
		return isSender;
	}
//////////////////////////////

	void setSendingTextFile(bool flag)
	{
		sendingTextFile=flag;
	}
	
	bool isSendingTextFile()
	{
		return sendingTextFile;
	}
	
/////////////////////////////
	
	void setFileName(std::string fName)
	{
		fileName=fName;
	}

	std::string getFileName()
	{
		return fileName;
	}

///////////////////////////////////////
	void setSenderTextString(std::string text )
	{
		senderString=text;
	}

	std::string  getSenderTextString()
	{
		return senderString;
	}

/////////////////////////////////////////
	
	SocketSystem getMachinelayerIP()
	{
		return machineLayerIP;
	}

	std::string getIPAddress()
	{
		return machineLayerIP.getIpFromName(machineLayerIP.getHostName());
	}
//////////////////////////////////////////

	bool getSenderMode()
	{
		return isSenderSync;
	}

	void setSenderMode(bool flag=false)
	{
		isSenderSync=flag;	
	}

///////////////////////////////	
	int getListenerPort()
	{
		return listenPortNo;
	}

	void setListnerPort(int port)
	{
		listenPortNo=port;	
	}

	int getDestPort()
	{
		return destPortNo;
	}

	void setDestPort(int port)
	{
		destPortNo=port;	
	}

//////////////////////////////
	private:
	CommChannel *comm;
	
	std::string destIPAddress;
	
	std::string hostIPAddress;
	
	std::string fileName;
	
	std::string senderString;
	
	bool isSender;
	
	SocketSystem machineLayerIP;

	bool sendingTextFile;

	int listenPortNo;
	
	int destPortNo;

	bool isSenderSync;//0 for sync, 1 for async

};




