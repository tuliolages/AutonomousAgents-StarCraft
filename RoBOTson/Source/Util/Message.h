#ifndef __MESSAGE_H__
#define __MESSAGE_H__

//#include "..\Agent\Agent.h"
#include <string>

/** Forward declaration */
class Agent;

/**
* Message class.
*/

class Message {

public:
	int type;
	int code;
	//Agent sender;
	//Agent receiver;
	std::string message;

	/** Constructor - not used */
	Message();
	/** Constructor */
	Message(int type, int code/* Agent sender, Agent receiver, */, std::string message);
};

#endif