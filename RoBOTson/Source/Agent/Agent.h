#ifndef __AGENT_H__
#define __AGENT_H__

#include <windows.h>
#include <BWAPI.h>
#include <vector>
#include <string>

#include "..\Util\Inbox.h"

using namespace BWAPI;
using namespace std;

/** forward declaration */
//class Inbox;

/**
* Basis for agent implementation.
* Each agent has a message inbox and is a thread.
*/
class Agent {

protected:
	HANDLE unithMutex;//currently using the global handle mutex
	vector<int> readMessasIDs;

public:
	Unit unit;
	UnitType type;
	TilePosition goal;
	int unitID; //type id
	int id;

	Inbox inbox;
	/** Default constructor. Not used. */
	Agent();
	/** Constructor. */
	Agent(Unit mUnit, HANDLE mutex, int id);
	/** Destructor. */
	virtual ~Agent();
	/** Thread initialization */
	void startThread();
	/** Thread execution */
	static DWORD WINAPI run(LPVOID param);
	void sendMessage(int type, int code, Agent receiver, string message);
};

#endif