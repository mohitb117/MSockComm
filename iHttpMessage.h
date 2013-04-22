#ifndef HTTPMSG_H
#define HTTPMSG_H

# include <iostream>
# include<string>


# include "Base64.h"

# include <sstream>

enum contentType {text,fileChunk};
enum messageType {async,sync};

std::string convertVectorStrings2String(std::vector<std::string> body)
{
			std::string result="";
			for(auto x:body)
			{
				result+=x;
			}
			return result;
}


std::string convertVectorChar2String(std::vector<char> body)
{
			std::string result="";
			for(auto x:body)
			{
				result+=x;
			}
			return result;
}

std::vector<char> convertString2CharVector(std::string body)
{
			std::vector<char> result;
			
			for(auto x:body)
			{
				result.push_back(x);
			}
			
			return result;
}



class messageHeader
{
	private:
	int mType,cType;
	
	std::string fileName;
	std::string fileFragement;

	std::string senderAddress;
	std::string senderIPAddress;

	std::string portNo;
	std::string contentLength;

public:
	messageHeader():mType(text),cType(sync),senderIPAddress("127.0.0.1"),portNo("4117"),contentLength("0"),fileFragement("-1"),fileName("null")
	{

	}

	messageHeader(int m,int c,std::string senderIP,std::string port,std::string len,std::string fileChunk,std::string fName):
		mType(m),cType(c),senderIPAddress(senderIP),portNo(port),
			contentLength(len),fileFragement(fileChunk),fileName(fName)
	{

	}
	//setters & getters for the message parameters

	void setMessageType(std::string x)
	{
		if(x=="text" )
			mType=text;
		if(x=="fileChunk")
			mType=fileChunk;
	}

	void setContentType(std::string x)
	{
		if(x=="sync")
			cType=sync;
		if(x=="sync")
			cType=async;
	}

	int getMessageType()
	{return mType;}

	int getContentType()
	{return cType;}

	
	void setSenderAddress(std::string addr)
	{
		senderAddress=addr;
	}

	std::string getSenderAddress()
	{
		std::ostringstream convert;
		convert<<portNo;

		return senderIPAddress+":"+convert.str();
	}

	std::string getFileName()
	{return fileName;}

	std::string getFileFragementNumber()
	{
		std::ostringstream conv;
		conv<<fileFragement;

		return conv.str();
	}

	std::string getIPAddress()
	{return senderIPAddress;}

	std::string getPortNo()
	{return portNo;}

	std::string getContentLength()
	{
		return contentLength;
	}

	void setContentLength(std::string len)
	{contentLength=len;}


	void setFileName(std::string f)
	{
		fileName=f;
	}

	void setFileFragment(std::string no)
	{
		fileFragement=no;
	}

};


class messageBody
{
	std::string messageBodyPlainTxt;//each message inherently contains a encoded message body!!
	std::string messageBodyEncoded;

	public:

		messageBody(std::string messageFragment="")
		{
			if(messageFragment.size()>0)
				setMessageBody(messageFragment);
			else
			{
				messageBodyPlainTxt=messageBodyPlainTxt=messageFragment;
			}
		}

	//message representation methods
	
	void setMessageBody(std::string messageFragment,bool alreadyEncoded=false)
	{
		std::vector<char> messageFrag;

		if(alreadyEncoded)
			messageBodyEncoded=messageFragment;

		if(alreadyEncoded==false)
		{
			for(auto charEq:messageFragment)
			{
				if(charEq!='\0')
					messageFrag.push_back(charEq);
			}
				messageBodyEncoded=Base64::encode(messageFrag);
		}

	}

	std::string getMessagePlainTxt()
	{
		std::vector<char> messageFragDecoded;

		if(messageBodyPlainTxt.size()>0)
			return messageBodyPlainTxt;
		
		else
		{
			messageFragDecoded=Base64::decode(messageBodyEncoded);
			for(auto ch:messageFragDecoded)
			{
				messageBodyPlainTxt+=ch;
			}
			return messageBodyPlainTxt;
		}
		
	}

	std::string getMessageBodyEncoded()
	{
		return messageBodyEncoded;
	}

};

class HttpMessage
{
private:
	messageHeader *header;
	messageBody *body;
	
public:

	//default constructor for the HTTP message
	HttpMessage()
	{
		header=new messageHeader();
		body=new messageBody();
	}

	//parameterized Constructor 
	HttpMessage(int m,int c,std::string senderIP,std::string port,std::string len,std::string fileChunk,std::string fName,std::string messageFragment)
	{
		header=new messageHeader(m,c,senderIP,port,len,fileChunk,fName);

		body=new messageBody(messageFragment);
		//std::cout<<"\ncustom HTTP \n";
	}


	//Parameterized Constructor
	
	HttpMessage(std::vector<std::string> lines )
	{
		header=new messageHeader();
		body=new messageBody();

			if(lines.size()>=8)
			{
				header->setContentType(lines[0]);
				header->setMessageType(lines[1]);
				header->setSenderAddress(lines[2]);
				header->setContentLength(lines[3]);
				header->setFileName(lines[4]);
				header->setFileFragment(lines[5]);
				body->setMessageBody(lines[7],true);
			}
	}

	void setHttpMessageBody(std::string input)
	{
		body->setMessageBody(input);
	}

	messageBody* getMessageBodyHandle()
	{
		return body;
	}

	messageHeader* getMessageHeaderHandle()
	{
		return header;
	}

	/*
		example format
				string/file  \n  line 1
				Async/Sync \n    line 2
				192.168.1.0:8080\n line 3
				1024\n  line 4
				mytext.cpp\n line 5
				12\n   line 6
				\n		<<== Blank Line line 7
				lksfjabskfbasdkbasjblksbflbslfvjsadvhfhvsd\0   line 8
		*/

		
	std::string getMessageString()
	{
		std::string result;
		std::string mType,cType;
	
		switch (header->getContentType())
		{
		case text:
			cType="string";
				break;

		case fileChunk:
			cType="file";
			break;
		default:
			cType="-";
		}


		switch (header->getMessageType())
		{
		case sync:
			mType="sync";
				break;

		case async:
			mType="async";
			break;

		default:
			mType="-";
		}

	 result=cType+"\n"+
			mType+"\n"+
			header->getSenderAddress()+"\n"+
			header->getContentLength()+"\n"+
			header->getFileName()+"\n"+
			header->getFileFragementNumber()+"\n"+
			body->getMessageBodyEncoded()+"\n";
	
		return result;
	}

		std::vector<std::string> splitHttpString(std::string X)
		{
			std::vector<std::string> lines;

			std::string temp="";
	
			for(auto x:X)
			{
				if(x!='\n')
				{
					temp+=x;
				}
				if(x=='\n')
				{
					lines.push_back(temp);
					temp="";
				}
			}
			return lines;
		}

	

	void HttpObjectFromString(std::vector<std::string> lines)
	{
			if(lines.size()>=8)
			{
				header->setContentType(lines[0]);
				header->setMessageType(lines[1]);
				header->setSenderAddress(lines[2]);
				header->setContentLength(lines[3]);
				header->setFileName(lines[4]);
				header->setFileFragment(lines[5]);

				body->setMessageBody(lines[7],true);//existing HTTP message is already encoded, so no need to encode it again
			}

	}
};


#endif

