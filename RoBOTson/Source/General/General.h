#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <windows.h>
#include <BWAPI.h>
#include "..\Util\GlobalBlackBoard.h"
#include "..\Agent\Agent.h"
#include "..\Agent\AgentSet.h"
#include <queue>

/**
* General class
*/
class General : public Agent {

private:
	bool started;
protected:
	static General* instance;
	bool hasBarracks;
	bool needsBarracks;
	queue <BWAPI::UnitType> buildPlan;
	bool buildMessageSent;

public:
	AgentSet* agentSet;
	int x;

	General();

	General(Unit unit, HANDLE mutex);

	virtual ~General();

	static General* getInstance(Unit unit, HANDLE handler);

	static General* getInstance();

	AgentSet* getAgents();

	static DWORD WINAPI run(LPVOID param);

	void startMyThread();
};

#endif