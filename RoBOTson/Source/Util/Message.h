#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "BWAPI.h"
#include <string>
#include <tuple>

using namespace BWAPI;

/** Forward declaration */
class Agent;

/**
* Message class.
*/

class Message {

public:
	/* Message id */
	int type;
	int code;

	/* Sender */
	int senderID;
	int senderType;

	/* Receiver */
	int receiverID;
	int receiverType;

	/* Content */
	std::tuple <BWAPI::TilePosition, int> messageContentP;
	std::tuple <double, int> messageContentD;
	bool res;
	int building;

	/** Constructor - not used */
	Message();
	/** Constructor */
	Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, std::tuple<BWAPI::TilePosition, int> messageContent);
	Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, std::tuple<double, int> messageContent);
	Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, bool res);
	Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType, int building);
	Message(int type, int code, int senderID, int senderType, int receiverID, int receiverType);
};

#endif