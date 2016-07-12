#ifndef __SCV_H__
#define __SCH_H__

#include <windows.h>
#include <BWAPI.h>
#include "..\Util\GlobalBlackBoard.h"
#include "..\Agent\Agent.h"
#include "..\General\General.h"

using namespace BWAPI;

/**
 * SCV class
 */
class SCV : public Agent {

private:
	int state;

public:
	/** States */

	/** SCV is gathering minerals. */
	static const int GATHER_MINERALS = 0;
	/** SCV is gathering gas. */
	static const int GATHER_GAS = 1;
	/** SCV is trying to find a buildspot for a requested building. */
	static const int FIND_BUILDSPOT = 2;
	/** SCV is moving to a found buildspot. */
	static const int MOVE_TO_SPOT = 3;
	/** SCV is constructing a building. */
	static const int CONSTRUCT = 4;
	/** SCV is repairing a building. */
	static const int REPAIRING = 5;
	/** SCV is needed to attack an enemy nearby. */
	static const int ATTACKING = 6;

	SCV(Unit unit, HANDLE ghMutex);
	
	virtual ~SCV();

	static DWORD WINAPI run(LPVOID param);

	void startMyThread();
};

#endif