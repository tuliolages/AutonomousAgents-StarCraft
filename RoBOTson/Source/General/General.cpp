#include "General.h"

using namespace BWAPI;
using namespace Filter;

General* General::instance = NULL;

General::General()
{
	this->x = x;
	this->agentSet = new AgentSet();
	this->started = false;
}

General::General(Unit mUnit, HANDLE ghMutex, int id)
{
	Broodwar->sendText("A general came to existence!");
	this->unitID = mUnit->getID();
	this->type = mUnit->getType();
	this->unit = mUnit;
	this->unithMutex = ghMutex;
	this->startMyThread();
	this->agentSet = new AgentSet();
	this->x = 999;
	this->started = true;
	this->hasBarracks = false;
	this->needsBarracks = false;
	this->buildPlan.push(UnitTypes::Terran_Supply_Depot);
	this->buildMessageSent = false;
	this->id;
	this->buildOrderMessageSent = false;
	this->isTryingToBuild = false;
	this->barracks = NULL;
}

General* General::getInstance()
{
	if (instance == NULL)
	{
		instance = new General();
	}
	return instance;
}

General* General::getInstance(Unit u, HANDLE h, int id)
{
	Broodwar << "uh oh..." << std::endl;
	if (instance == NULL)
	{
		instance = new General(u, h, id);
	}
	else if (!instance->started) {
		Broodwar->sendText("A general started to run!");
		instance->unitID = u->getID();
		instance->type = u->getType();
		instance->unit = u;
		instance->unithMutex = h;
		instance->startMyThread();
		instance->started = true;
		instance->hasBarracks = false;
		instance->needsBarracks = false;
		instance->buildPlan.push(UnitTypes::Terran_Supply_Depot);
		instance->buildPlan.push(UnitTypes::Terran_Supply_Depot);
		instance->buildPlan.push(UnitTypes::Terran_Barracks);
		instance->buildMessageSent = false;
		instance->id = id;
		instance->buildOrderMessageSent = false;
		instance->isTryingToBuild = false;
		instance->barracks = NULL;
	}

	return instance;
}

General::~General()
{
	this->instance = NULL;
}

void General::checkInbox()
{
	General* unitClass = (General*)this;
	// Checks buildplan responses
	if (!unitClass->buildPlan.empty() && unitClass->buildMessageSent)
	{
		for (auto &m : unitClass->inbox)
		{
			if (m->receiverType == 0 && m->code == 1 && m->type == 2)
			{
				// Read and delete.
				int dist = std::get<0>(m->messageContentD);
				unitClass->idShortestDistance = dist < unitClass->shortestDistance ? m->senderID : unitClass->idShortestDistance;
				unitClass->shortestDistance = dist < unitClass->shortestDistance ? dist : unitClass->shortestDistance;
				Broodwar << "Somebody wants to build stuff!!! " << unitClass->shortestDistance << std::endl;
			}
			else if (m->receiverType == 0 && m->code == 2)
			{
				unitClass->buildOrderMessageSent = true;
				
			}
			unitClass->inbox.erase(m);
			unitClass->checkInbox();
			return;
		}
	}
}

DWORD WINAPI General::run(void* param)
{
	Broodwar << "Gen is running now!" << std::endl;
	General* unitClass = (General*)param;
	BWAPI::Unit hq = unitClass->unit;
	HANDLE unithMutex = unitClass->unithMutex;
	
	DWORD dwWaitResult;

	/*
	AgentSet* agents = unitClass->getAgents();
	for  (auto &a : *agents)
	{
		Message* msg = new Message(0, 0, 0, 0, 0);
		a->inbox.inbox.push_back(msg);
	}	
	*/

	int count = 0;
	int lele = 0;
	while (true){

		dwWaitResult = WaitForSingleObject(
			unithMutex,    // handle to mutex
			100);  // time-out interval

		if (GlobalBlackBoard::GameOver || hq == NULL || !hq->exists()) {
			ReleaseMutex(unithMutex);
			return 0; // end thread
		}
		// Some things are commom between units, so you can apply a little of OO here.

		if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_ABANDONED)
		{

			int noMessages = 0;
			static int lastCheckedBuildPlanMsgs = 0;
			static int waitForWorkers = 0;

			lastCheckedBuildPlanMsgs += (lastCheckedBuildPlanMsgs > 400) ? 0 : Broodwar->getFrameCount() - lastCheckedBuildPlanMsgs;
			waitForWorkers = Broodwar->getFrameCount();

			int workers = 0;
			int marines = 0;
			AgentSet* agents = unitClass->getAgents();
			for (auto &a : *agents)
			{
				if (a->type.isWorker()) {
					workers++;
				}
				else if (a->type == UnitTypes::Terran_Marine)
				{
					marines++;
				}
			}

			// Checks buildplan
			if (!unitClass->buildPlan.empty() && !unitClass->buildMessageSent && waitForWorkers > 100 && !unitClass->isTryingToBuild && workers >= (10 + lele))
			{
				unitClass->shortestDistance = 999999;

				// Asks for units to build it
				Broodwar << "Sending message" << std::endl;
				AgentSet* agents = unitClass->getAgents();
				for (auto &a : *agents)
				{
					if (a->type.isWorker()) {
						Message* msg = new Message(4, 1, unitClass->id, 0, -1, 1);
						a->inbox.insert(msg);
						noMessages++;
					}
				}
				if (noMessages >= 0)
				{
					unitClass->buildMessageSent = true;
					unitClass->buildOrderMessageSent = false;
					Broodwar << "sent to " << noMessages << std::endl;
				}
				lastCheckedBuildPlanMsgs = 0;
			}
			
			if (lastCheckedBuildPlanMsgs > 400 && !unitClass->buildOrderMessageSent && unitClass->buildMessageSent) {
				unitClass->checkInbox();

				if (unitClass->shortestDistance < 999999)
				{
					AgentSet* agents = unitClass->getAgents();
					for (auto &a : *agents)
					{
						if (a->type.isWorker() && a->id == unitClass->idShortestDistance) {
							int building_code = 1;
							if (unitClass->buildPlan.front() == UnitTypes::Terran_Barracks) {
								building_code = 2;
							}
							Broodwar << "please build this" << unitClass->buildPlan.front() << std::endl;
							Message* msg = new Message(1, 2, unitClass->id, 0, a->id, 1, building_code);
							a->inbox.insert(msg);
						}
					}
				}
				lastCheckedBuildPlanMsgs = 0;
				unitClass->buildOrderMessageSent = true;
			}

			if (lastCheckedBuildPlanMsgs > 100 && unitClass->buildOrderMessageSent && unitClass->buildMessageSent) {
				unitClass->checkInbox();
				Broodwar << "I guess its ok now" << std::endl;
				lele += 2;
				unitClass->buildPlan.pop();
				unitClass->buildOrderMessageSent = false;
				unitClass->buildMessageSent = false;
			}

			if (workers < 15 && !unitClass->isTryingToBuild) {
				if (hq->isIdle() && !hq->train(hq->getType().getRace().getWorker()))
				{

				} // closure: failed to train idle unit
			}
			else if (marines < 10 && !unitClass->isTryingToBuild) {
				if (unitClass->barracks != NULL) {
					if (hq->isIdle() && !unitClass->barracks->train(UnitTypes::Terran_Marine))
					{

					} // closure: failed to train idle unit
				}
			}
			// Release ownership of the mutex object
			if (!ReleaseMutex(unithMutex))
			{
				// Handle error.
			}
		}
		Sleep(20);
	}
}

AgentSet* General::getAgents()
{
	return this->agentSet;
}

void General::startMyThread()
{
	DWORD ThreadID;
	//CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadID);
	CreateThread(NULL, 0, run, (void*)this, 0, NULL);
}