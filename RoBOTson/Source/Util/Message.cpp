#include "Message.h"

using namespace std;

Message::Message(int type, int code, int sender, int receiver, tuple<BWAPI::TilePosition, int> messageContent)
{
	this->type = type;
	this->code = code;
	this->sender = sender;
	this->receiver = receiver;
	this->messageContentP = messageContent;
}

Message::Message(int type, int code, int sender, int receiver, tuple<double, int> messageContent)
{
	this->type = type;
	this->code = code;
	this->sender = sender;
	this->receiver = receiver;
	this->messageContentD = messageContent;
}

Message::Message(int type, int code, int sender, int receiver)
{
	this->type = type;
	this->code = code;
	this->sender = sender;
	this->receiver = receiver;
}

Message::Message()
{
	
}