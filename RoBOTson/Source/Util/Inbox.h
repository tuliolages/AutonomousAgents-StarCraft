#ifndef __INBOX_H__
#define __INBOX_H__

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "..\Util\Message.h"

using namespace BWAPI;
using namespace std;

/**
* Message box class.
*/
class Inbox : public SetContainer<Message*, std::hash<void*>>{
public:

};

#endif