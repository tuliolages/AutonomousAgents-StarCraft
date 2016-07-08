#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <windows.h>
#include <BWAPI.h>
#include "..\Util\GlobalBlackBoard.h"
#include "..\Agent\Agent.h"

/**
* General class
*/
class General : public Agent {

private:

protected:
	//Unit Agent Set

public:
	General(Unit unit, HANDLE mutex);

	virtual ~General();

	static DWORD WINAPI run(LPVOID param);

	void startMyThread();
};

#endif