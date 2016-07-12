#ifndef __MARINE_H__
#define __MARINE_H__

#include <windows.h>
#include <BWAPI.h>
#include "..\Util\GlobalBlackBoard.h"
#include "..\Agent\Agent.h"
#include "..\General\General.h"

using namespace BWAPI;

/**
* Marine class
*/
class Marine : public Agent {

private:
	int state;

public:
	/** States */

	/** Marine is moving to a found buildspot. */
	static const int MOVE_TO_SPOT = 3;
	/** Marine is needed to attack an enemy nearby. */
	static const int ATTACKING = 6;

	Marine(Unit unit, HANDLE ghMutex, int id);

	virtual ~Marine();

	static DWORD WINAPI run(LPVOID param);

	void startMyThread();

	void checkInbox();
};

#endif