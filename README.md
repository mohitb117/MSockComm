MSockComm
=========
This application is a usage of the Existing API's from 
Syracuse Univesity's Graduate Level Object Oriented Design Coursework CSE 687.



Project #3 – Socket-based Communication  	      
	
Purpose:
This project requires you to build a socket-based communication system 
for sending files and messages between network machines.  
You will demonstrate this capability with a peer-to-peer system that can send, 
possibly binary, files and text messages from either end of the communication channel.	

Requirements:
Your PeerToPeer communication system:	

1.	shall use standard C++, the standard library, compile and link from the command line, using  Visual Studio 2012, as provided in the ECS clusters and operate in the environment provided there .	
	
2.	shall use services of the C++ std::iostream library and Win32 sockets for all input and output in the communication channel.  Peers shall use the C++ operator new and delete for all dynamic memory management.	

3.	shall support posting text messages to a send queue and getting message from the corresponding receive queue.	

4.	shall support sending, possibly binary, files across the communication channel in chunks.  It is suggested that you send a start text message indicating the beginning of a file transfer, then send blocks with text headers  and binary bodies.  After the last file block is sent you send an end text message indicating end of file transfer.	
	
5.	shall demonstrate the communication system by sending both text and file messages from multiple concurrent clients to a single receiver.  The receiver shall send back an acknowledgement message .	

6.	shall support asynchronous communication.  That means that a client sends a message and breaks the connection.  If the receiver wishes to send a reply it later connects to the client to send the reply .	
 
7.	shall demonstrate asynchronous processing by starting at least two clients that each send to all of at least two running receivers.  The clients each request receivers to process a computational task and break the connection.  Receivers must reply to the requestor with the computational results. 	



