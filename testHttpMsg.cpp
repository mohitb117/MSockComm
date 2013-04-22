////////////////////////////////////////////////////////////////////////
// testHttpMsg.h - Encapsulation Method for HTTP Message Communication//
//				   between peers									  //
// ver 2.0												              //
//                                                                    //
// Language:    Visual C++,											  //
// Platfrom:    Lenovo U410 Ultrabook Win7x64						  //
// Application: CSE687 - Object Oriented Design, Spring 2013          //
//Author:		Mohit Bhalla Syracuse University					  //
//Instructor:	Jim Fawcett, CST 2-187, Syracuse University           //
//              (315) 443-3948, jfawcett@twcny.rr.com                 //
///////////////////////////////////////////////////////////////////////


//split the http message into vector of line lines!!!!

//# define TEST_HTTP_MSG

#ifdef TEST_HTTP_MSG

# include "iHttpMessage.h"
# include <iostream>
# include <conio.h>

int main(int argc , char** argv)
{

	HttpMessage *copy=new HttpMessage();
	HttpMessage *m1=new HttpMessage(sync,fileChunk,"192.168.1.0","4123","1024","30","UNSC.cpp","Halo Is\n\n the best thing that ever happened to me!");

	//Encoding the given message
	//std::cout<<m1->getMessageString()<<"\n";

	////decoding the given string
	//std::cout<<"Plain Text="<<m1->getMessageBodyHandle()->getMessagePlainTxt();
	//	
	std::cout<<"\n-----------------------------------------------\n";

	for(auto x:m1->splitHttpString(m1->getMessageString()))
	{
		std::cout<<x<<"\n";
	}

	std::cout<<"Plain Text="<<m1->getMessageBodyHandle()->getMessagePlainTxt();

	copy=m1;
	std::cout<<"\n-------------------Demonstrating Copy Constructor----------------------------\n";
	
	std::vector<std::string> lines=m1->splitHttpString(copy->getMessageString());
	
	for(auto x:lines)
	{
		std::cout<<x<<"\n";
	}

	std::cout<<"Plain Text="<<copy->getMessageBodyHandle()->getMessagePlainTxt();
	

	std::cout<<"\n-------------------Demonstrating conversion of string to object----------------------------\n";

	
	//HttpMessage* newMsg=new HttpMessage();

	//newMsg->HttpObjectFromString(lines);
	//


	HttpMessage* newMsg=new HttpMessage(lines);

	std::cout<< newMsg->getMessageString();

	std::cout<<"Plain Text="<<newMsg->getMessageBodyHandle()->getMessagePlainTxt();
	
	_getch();
	return 0;
}

#endif

