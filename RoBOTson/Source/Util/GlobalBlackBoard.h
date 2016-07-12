#ifndef __GLOBAL_BLABKBOARD__
#define __GLOBAL_BLABKBOARD__

#include "Inbox.h"

/**
 * General notice board
 */
class GlobalBlackBoard {
public:
	Inbox inbox;
	static bool GlobalBlackBoard::GameOver;
};

#endif