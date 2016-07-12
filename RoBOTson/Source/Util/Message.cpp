#include "Message.h"

using namespace std;

Message::Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, std::tuple<BWAPI::TilePosition, int> messageContent)
{
	this->type = type;
	this->code = code;
	this->senderID = senderID;
	this->senderType = senderType;
	this->receiverID = receiverID;
	this->receiverType = receiverType;
	this->messageContentP = messageContent;
}

Message::Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, tuple<double, int> messageContent)
{
	this->type = type;
	this->code = code;
	this->senderID = senderID;
	this->senderType = senderType;
	this->receiverID = receiverID;
	this->receiverType = receiverType;
	this->messageContentD = messageContent;
}

Message::Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, bool res)
{
	this->type = type;
	this->code = code;
	this->senderID = senderID;
	this->senderType = senderType;
	this->receiverID = receiverID;
	this->receiverType = receiverType;
	this->res = res;
}

Message::Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, int building)
{
	this->type = type;
	this->code = code;
	this->senderID = senderID;
	this->senderType = senderType;
	this->receiverID = receiverID;
	this->receiverType = receiverType;
	this->building = building;
}

Message::Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType)
{
	this->type = type;
	this->code = code;
	this->senderID = senderID;
	this->senderType = senderType;
	this->receiverID = receiverID;
	this->receiverType = receiverType;
}

Message::Message()
{
	
}