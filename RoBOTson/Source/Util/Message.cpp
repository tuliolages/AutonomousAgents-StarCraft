#include "Message.h"

using namespace std;

Message::Message(int type, int code/*, Agent sender, Agent receiver*/, string message)
{
	this->type = type;
	this->code = code;
	//this->sender = sender;
	//this->receiver = receiver;
	this->message = message;
}

Message::Message()
{
	
}