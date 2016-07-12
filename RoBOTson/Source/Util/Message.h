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
	int type;
	int code;
	int sender;
	int receiver;
	std::tuple <BWAPI::TilePosition, int> messageContentP;
	std::tuple <double, int> messageContentD;

	/** Constructor - not used */
	Message();
	/** Constructor */
	Message(int type, int code, int sender, int receiver, std::tuple<BWAPI::TilePosition, int> messageContent);
	Message(int type, int code, int sender, int receiver, std::tuple<double, int> messageContent);
	Message(int type, int code, int sender, int receiver);
};

#endif