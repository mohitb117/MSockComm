///////////////////////////////////////////////////////////////////////////////////////
// Receiver.h - Contains Listner Thread,ClientHandler Threads,Acknowledgement Threads// 
//					as well as Receiver and Acknowledgement Classes 				 //
// ver 1.1.0												                         //
//																				     //
// Language:    Visual C++,														     //
// Platfrom:    Lenovo U410 Ultrabook Win7x64									     //
// Application: CSE687 - Object Oriented Design, Spring 2013                         //
// Author:		Mohit Bhalla Syracuse University						            //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University					        //
//              (315) 443-3948, jfawcett@twcny.rr.com							    //
//////////////////////////////////////////////////////////////////////////////////////




#include "Sockets.h"
#include "Threads.h"
#include "Locks.h"
#include "BlockingQueue.h"
#include <string>
# include "iHttpMessage.h"
# include "HiResTimer.h"

using namespace HRTimer;

///////////////////
//ACK Thread

class AckThread : public threadBase
{
public:
  AckThread(Socket s) : s_(s)
  {
  }

  std::string& status() { return status_; }

private:

  void run()
  {

    ip=s1.getIpFromName(s1.getHostName());
    status_ = "good";
    
	doLog("send thread running");
    
	std::string m1("ACK");
	
	std::string msg("Acknowledge Message Received by Peer "+s1.getHostName()+" at  "+ip);
    
	ip=s1.getIpFromName(s1.getHostName());
 			
	if(!s_.writeLine(m1) || !s_.writeLine(msg) )
    {
        sout << "\n  bad status in sending thread";
        status_ = "bad";
    }

	s_.disconnect();
  }

  std::string status_;
  Socket s_;
  SocketSystem s1;
  std::string ip; 	
  int sourcePort;
};

///////////////////////////////////////////////////

class sendAckMsg  //Class responsible for sending Acknowldegment message  
{
public:
	sendAckMsg()
	{	}
 
  void sendAck(std::string destIP,int destPort)
  {
	//	sout << locker << "\n ACK Sender  started" << unlocker;
 	ack = new AckThread(s_);
	if(!s_.connect(destIP, destPort))
    {
	  sout << locker << "\n  couldn't connect to Peer for ACK Message " << destIP << "-> " << destPort << "\n\n" << unlocker;
      delete ack;
      return;
    }
	ack->start();

    ack->join();
    
	delete ack;
  }
private:
  Socket s_;
  AckThread * ack;
};
std::string getIP(std::string msg)
{
	std::string temp;
		for(size_t i=msg.find("(")+1;i<msg.find(":");i++)
		{
		temp+=msg[i];
		}
		return temp;
	//if(msg.find("(") <=INT_MAX && msg.find(":")<=INT_MAX)
	//	return msg.substr(msg.find("(")+1,msg.find_last_of("."));
	//else return "NONE";
}

int getPort(std::string msg)
{

		if(msg.find(":") <=INT_MAX && msg.find(")")<=INT_MAX)
		{	
			std::string str=msg.substr(msg.find(":")+1,msg.find(")"));
			return atoi(str.c_str());
		}

		else return 1;
}

std::string checkCustomResponse(std::string msg,Socket s,char* compare)
{
		 if(std::strcmp(msg.c_str(),compare)==0)
		 {
			 std::string msg1=s.readLine();
			 return msg1;
		 }

		 if(std::strcmp(compare,"ACK")==0)		 return "ack";
		 if(std::strcmp(compare,"ASP")==0)       return "asp";
		 
		 else
			 return "xx";
}

///////////////////////////////////////////////////
// ClientHandlerThread thread

class ClientHandlerThread : public tthreadBase
{
public:
ClientHandlerThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) 
{
	isFile=false;
}
  private:
	  void dumpChunks2File(int &ctr)
	  {		  //if(fileChunkerDechunker::WriteChunks2File(chunks,fileName,std::string ("../RCVDFiles")))
			  if(fileChunkerDechunker::writeBlocks2File(chunks,"."+fileName))
			  {
				 q_.enQ("File ==> "+fileName+" successfully Recieved from Client");
				 sout<<locker<<"Peer Received file of size "<<fileChunkerDechunker::filesize("."+fileName)<<" (In bytes)"<<unlocker<<"\n";
				 sout<<locker<<"Time taken to transfer "<< timer.Stop()/1000<<" miliseconds	"<<unlocker<<"\n";
				 if(isSync)	 
				 {
					 sendAckMsg *a=new sendAckMsg();
					 a->sendAck(clientIP,atoi(clientPortNo.c_str()));
				 }
				 ctr=0;				 //q_.enQ("quit");
			 }
			 else
			 {
				  q_.enQ("Unable to write received File");
				  ctr=0;			//	  q_.enQ("quit");
			 }
	  }

	  void searchFileNameFromPath(std::string msg)
	  {
		  if(fileName=="" && msg.find_last_of("\\")<=INT_MAX)
			  fileName=msg.substr(msg.find_last_of("\\")+1,msg.length());

  		  if(fileName=="" && msg.find_last_of("\\\\")<=INT_MAX)
			  fileName=msg.substr(msg.find_last_of("\\\\")+1,msg.length());

		  if(fileName=="" && msg.find_last_of("/")<=INT_MAX)
			  fileName=msg.substr(msg.find_last_of("/")+1,msg.length());	  
	  }
	 
	 
	void delegateMessageHandlingTasks(std::string msg,int &ctr)
	{
		  if(std::strcmp(msg.c_str(),"file")==0)
		 	  isFile=true;
		 
		  if(ctr==5 && isFile)
		 	  searchFileNameFromPath(msg);
		  
		  if(ctr>7 && isFile && msg!="xEOFx")		  
			  chunks.push_back(convertVectorChar2String(Base64::decode(msg)));
		  if(ctr==3)
		  {
			  clientIP=msg.substr(0,(msg.find(':')));
			  clientPortNo=msg.substr(msg.find(':')+1);
		  }
		  if(std::strcmp(msg.c_str(),"async")==0||std::strcmp(msg.c_str(),"Async")==0)
			  isSync=false;
		  if(std::strcmp(msg.c_str(),"sync")==0||std::strcmp(msg.c_str(),"Sync")==0)
					  isSync=true; 
		  if(std::strcmp(msg.c_str(),"xEOFx")==0 && isFile)
		  { 
			  dumpChunks2File(ctr);
		  }
		  if(!isFile && ctr==7)
		  {
			  q_.enQ("Text Message Received from "+clientIP+":"+clientPortNo+" ==> \n"+convertVectorChar2String(Base64::decode(msg)));
			  if(isSync)	 
			  {
					 sendAckMsg *a=new sendAckMsg();
					 a->sendAck(clientIP,atoi(clientPortNo.c_str()));
			  }
				ctr=0;//q_.enQ("quit");
		  }
	}

  void run()
  {
	int ctr=0;
	timer.Start();
		do
		{
   			  ctr++;
			  doLog("receive thread reading line");
			  
			  msg = s_.readLine();

  			  if(msg=="")
				  continue;
			  delegateMessageHandlingTasks(msg,ctr);
			  
			  std::string ack=checkCustomResponse(msg,s_,"ACK");
			  std::string asp=checkCustomResponse(msg,s_,"ASP");

			  if(!strcmp(ack.c_str(),"ack")==0)
					sout<<locker<<"\nACK received\n"<<"Message Says: "<<ack<<unlocker;
			  if(!strcmp(asp.c_str(),"asp")==0)
			  {
					sout<<locker<<"\nAsynchronous Processing Request received\n"<<"Message Says: "<<asp<<unlocker;
					
					std::string ip=getIP(asp);/*"127.0.0.1"*/;int port=getPort(asp);
					
					spawnACKThread(ip,port);
			  }
		} 
		while(msg != "quit");
	sout<<locker<<"exit"<<unlocker;	  

  }


  void spawnACKThread(std::string ip,int port)
  {
	  		 sendAckMsg *a=new sendAckMsg();
			 a->sendAck(ip,port);
  }

  private:
	HiResTimer timer;
	std::string msg;
	std::vector<std::string> chunks;    
	bool isFile;
	std::string fileName;
	std::string clientIP;
	std::string clientPortNo;
  bool isSync;
  Socket s_;
  BlockingQueue<std::string>& q_;
};

///////////////////////////////////////////////////
// listenThread thread
class ListenThread : public threadBase
{
public:
  ListenThread(int port, BlockingQueue<std::string>& q) : listener_(port), q_(q), stop_(false) 
  {
  }
  
  void stop(bool end) { stop_ = end; }

private:

  void run()
  {

    while(!stop_)
    {
      SOCKET s = listener_.waitForConnect();
      ClientHandlerThread* pCh = new ClientHandlerThread(s, q_);
      pCh->start();
    }

  }
  bool stop_;
  BlockingQueue<std::string>& q_;
  SocketListener listener_;
};


class Receiver
{
public:
	Receiver(BlockingQueue<std::string> &input):q_(input)
	{}
  void start(int port)
  {
	 sout << "\n Peer Application Reciever started at Port #"<<port<<"\n";
   
	 pLt = new ListenThread(port, q_);
    
	try
    {
      pLt->start();
      std::string msg;
      int count = 0;
      int ctr=0;
	  
	  bool flag=true;

	  do
      {
		msg = q_.deQ();

		doLog((std::string("fetching message ") + msg).c_str());
    
		sout << "\n  Recieved Message : " << msg.c_str()<<"\n";
		
		if(msg=="quit")
			flag=false;
	
		//std::cout<<" "<<ctr++<<" ";

	  } while(flag);

	
    }
    catch(std::exception& ex)
    {
		sout<<locker<<"Exception occured!!!==> "<<ex.what()<<unlocker;
      delete pLt;
    }
    catch(...)
    {
      delete pLt;
    }

  }
private:
  BlockingQueue<std::string> q_;
  ListenThread* pLt;
};




#ifdef TEST_RCVR

int main()
{

  
  try
  {
    Receiver rcvr;
    rcvr.start(8080);
  }
  
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what();
    ret = 1;
  }
  
  catch(...)
  {
    sout << "\n  something bad happened";
    ret = 1;
  }
  
  sout << "\n\n";
  return ret;

}
# endif