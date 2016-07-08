#ifndef __INBOX_H__
#define __INBOX_H__

#include <queue>
#include "..\Util\Message.h"

/** Forward declaration */
//class Message;

/**
* Message box class.
*/
class Inbox {

public:
	std::queue <Message*> inbox;

	/** Constructor */
	Inbox();
};

#endif