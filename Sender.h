///////////////////////////////////////////////////////////////////////////////////////
// Receiver.h - Contains SendThread,Asynchronous Message Processing Thread           //
//	and corresponding Sender and SendAsyncMsg Classes								//			
// ver 1.1.0												                         //
//																				     //
// Language:    Visual C++,														     //
// Platfrom:    Lenovo U410 Ultrabook Win7x64									     //
// Application: CSE687 - Object Oriented Design, Spring 2013                         //
// Author:		Mohit Bhalla Syracuse University						             //
// Instructor:	Jim Fawcett, CST 2-187, Syracuse University					         //
//              (315) 443-3948, jfawcett@twcny.rr.com							     //
//////////////////////////////////////////////////////////////////////////////////////



#include "Sockets.h"
#include "Threads.h"
#include "Locks.h"
#include "BlockingQueue.h"
#include <string>
#include <iostream>
# include <conio.h>
#include <sstream>
#include "FileChunker.h"
# include "iHttpMessage.h"
# include "HiResTimer.h"

using namespace HRTimer;

std::string ToString(int i)
{
  std::ostringstream conv;
  conv << i;
  return conv.str();
}

/////////////////////////////////////////////////////
//// AsyncThread thread
// i need to create two message sending threads which will send the message and die 
//

class AsyncThread : public threadBase
{
public:
  AsyncThread(Socket s,std::string str,int port) : s_(s),message(str),sourcePort(port)
  {
  }

  std::string& status() { return status_; }

private:

  void run()
  {
    ip=s1.getIpFromName(s1.getHostName());
    status_ = "good";
	doLog("send thread running");

	ip=s1.getIpFromName(s1.getHostName());

	std::string msg("\n  Async Processing Message Sent to Peer at IP ("+ip+":"+ToString(sourcePort)+")");
	       			
	if(!s_.writeLine("ASP")||!s_.writeLine(msg) || !s_.writeLine(message))
    {
        sout << "\n  bad status in sending thread";
        status_ = "bad";
    }

	s_.disconnect();
  }
  std::string message;
  std::string status_;
  Socket s_;
  SocketSystem s1;
  std::string ip; 	
  int sourcePort;
};

///////////////////////////////////////////////////

class sendAsyncMsg  //Class responsible for sending Acknowldegment message  
{
public:
	sendAsyncMsg()
	{	}

  void sendMsg(std::string destIP,int destPort,int sourcePort,std::string message="")
  {
	//	sout << locker << "\n ACK Sender  started" << unlocker;
 	async = new AsyncThread(s_,message,sourcePort);

	if(!s_.connect(destIP, destPort))
    {
	  sout << locker << "\n  couldn't connect to Peer for Async Communication Message " << destIP << ":" << destPort << "\n\n" << unlocker;
      delete async;
      return;
    }

	async->start();

    async->join();

	async->selfTerminate();

	delete async;
 
	sout << locker << "\n  Async Processing Message Sent to Peer at "+destIP+":"+ToString(destPort)<<" from Port#"<<sourcePort<< unlocker;
     
 }

private:
  Socket s_;
  AsyncThread * async;
};

////////////////////////////////////////////////////////////////
class SendThread : public threadBase
{
public:
  SendThread(Socket s, BlockingQueue<std::string>& q) : s_(s), q_(q) 
  {

  }

std::string & status() { return status_; }
private:
 
void run()
  {
    status_ = "good";
    doLog("send thread running");
    std::string msg;
	do
    {
      doLog("send thread enqing msg");
     
	  msg = q_.deQ();
		
      if(!s_.writeLine(msg))
      {
        sout << "\n  bad status in sending thread";
        status_ = "bad";
        break;
      }

    }
	while(msg != "stop");    
	s_.disconnect();
  }

  std::string status_;
  Socket s_;
  BlockingQueue<std::string>& q_;
};



class Sender  //class responsible for sending text message or FIle chunks
{
private:
	HiResTimer timer;
	int sourcePort;
	bool isFile;
	std::string textMsg;
	std::string fileName;
	bool isSync;
	int fileSize;
public:
  
	Sender(int port,bool is_file,BlockingQueue <std::string> &output,std::string fNameorMsg="",bool sync=true):
		sourcePort(port),isFile(is_file),textMsg(fNameorMsg),fileName(fNameorMsg) ,isSync(sync),q_(output)
	{
		myCount = ++count;
	}
 
  int id() { return myCount; }
  
  
  void sendTextMessage()
  {
	  	  ip=s1.getIpFromName(s1.getHostName());
		  std::string msg="Message Received from Sender "+ s1.getHostName()+" at IP address "+ip+" is "+textMsg;
				
		  HttpMessage *m1=new HttpMessage(isSync,text,ip,ToString(sourcePort),"-1","-1","-1",msg);
			
		  q_.enQ(m1->getMessageString());
			
		  ::Sleep(10 * id());  // sleep time increases with each addition Sender      

  }

  void sendChunks()
  {
			//send header
	  	  	ip=s1.getIpFromName(s1.getHostName());
			HttpMessage *m1=new HttpMessage(isSync,fileChunk,ip,ToString(sourcePort),"-1","-1",fileName,"Sending File");
			q_.enQ(m1->getMessageString());
			//send message fragments
			
			fileChunkerDechunker *f=new fileChunkerDechunker();
			/*
			Algorithm to send the file out of the socket:
						Create chunks of the file which are in binary
						convert the chunks into vector of char
						push the vector in the sender queue
			*/
			try
			{
				std::vector<File::block> Blocks2Send=fileChunkerDechunker::getBlockFromFile(fileName);
				if(Blocks2Send.size()>0)
				{
					for(auto block: Blocks2Send)
					{
						q_.enQ(Base64::encode(convertString2CharVector(fileChunkerDechunker::convertBlock2String(block))));
					}
					q_.enQ("xEOFx");
				}
				else
				{
							sout<<locker<<"Unable to Chunk, file missing, Unable to send File!!"<<unlocker;
							_getch();
				}
			}

			catch(std::exception ex)
			{
				sout<<locker<<"Exception in chunking File!"<<ex.what()<<unlocker;
			}


 }

  //}
  void start(std::string ip, int port)
  {
	sout << locker << "\n Sender #" << id() << " started" << unlocker;
	pSt = new SendThread(s_, q_);
    
	pSt->start();
	
	if(!s_.connect(ip, port))
    {
      sout << locker << "\n  couldn't connect to " << ip << ":" << port << "\n\n" << unlocker;
      delete pSt;
      return;
    }
	else
    {
      std::string logMsg = "\n  Peer Connected to " + ip + ":" + ToString(port);
      doLog(logMsg.c_str());
	  sout<<locker<<logMsg<<unlocker<<"\n";
    }
	doLog("starting Sender");
	ip=s1.getIpFromName(s1.getHostName());
	timer.Start();	
	if(pSt->status()=="good")
	{
		if(!isFile)
		{
			sendTextMessage();
		}

		if(isFile)
		{
			sendChunks();
		}
		sout<<locker<<"\n Sending Message Took "<<timer.Stop()/1000<<" miliseconds "<<unlocker<<"\n";
	}
    pSt->join();
    delete pSt;
  }

private:
  Socket s_;
  BlockingQueue<std::string> q_;
  SendThread* pSt;
  static int count;
  int myCount;
  int numMsgs_;
  SocketSystem s1;
  std::string ip; 	
};


																																								 //   for(int i=0; i<numMsgs_; ++i)
																																								 //   {
																																								 //     doLog("sending message");

																																									////  msg = "sender " + ToString(id()) + ": msg#" + ToString(i);
																																									//  sout << locker << "\n  " << msg.c_str() << unlocker;
																																								 //     
																																									//  q_.enQ(msg);
																																								 //     
																																									//  ::Sleep(10 * id());  // sleep time increases with each addition Sender
																																								 //     
																																									//  if(pSt->status() == "bad")
																																								 //       break;
																																								 //   
																																									//}

																																								 //   msg = "sender#" + ToString(id()) + ": stop";
																																								 //   
																																									//sout << "\n  " + msg;


int Sender::count = 0;

///////////////////////////////////////////////////
// DemoThread is used to get two or more senders
// running concurrently from a single process, to
// make testing easier.

class DemoThread : public threadBase
{
public:
  DemoThread(Sender sndr) : sndr_(sndr) {}
private:

  void run()
  {
    sndr_.start("127.0.0.1", 8080);
  }
  
  Sender sndr_;

};
# ifdef TEST_SNDR
int main()
{
  int ret = 0;
  try
  {
    // run two senders concurrently
    Sender sndr1(100), sndr2(100);
   
	DemoThread t1(sndr1);
    
	DemoThread t2(sndr2);
    
	t1.start();
    t2.start();
    t1.join();
    t2.join();
  }

  catch(std::exception& ex)
  {
    sout << "\n\n  " << ex.what();
    ret = 1;
  }
  catch(...)
  {
    sout << "\n\n  something bad happend to a sender";
    ret = 1;
  }
  std::cout << "\n\n  That's all Folks!\n\n";
  return ret;
}
# endif