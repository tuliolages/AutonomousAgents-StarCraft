#ifndef __INBOX_H__
#define __INBOX_H__

#include <vector>
#include "..\Util\Message.h"

using namespace std;

/** Forward declaration */
class Message;

/**
* Message box class.
*/
class Inbox {

public:
	vector <Message*> inbox;

	/** Constructor */
	Inbox();
};

#endif