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

General::General(Unit mUnit, HANDLE ghMutex)
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
}

General* General::getInstance()
{
	if (instance == NULL)
	{
		instance = new General();
	}
	return instance;
}

General* General::getInstance(Unit u, HANDLE h)
{
	Broodwar << "uh oh..." << std::endl;
	if (instance == NULL)
	{
		instance = new General(u, h);
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
		instance->buildMessageSent = false;
	}

	return instance;
}

General::~General()
{
	this->instance = NULL;
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

			if (count == 0)
				Broodwar << "kkk" << unitClass->buildPlan.empty() << std::endl;
			lastCheckedBuildPlanMsgs += (lastCheckedBuildPlanMsgs > 1000) ? 0 : Broodwar->getFrameCount() - lastCheckedBuildPlanMsgs;
			if (count++ == 0)
				Broodwar << "jjj" << (unitClass->buildMessageSent ? "yes" : "no") << std::endl;
			// Checks buildplan
			if (!unitClass->buildPlan.empty() && !unitClass->buildMessageSent)
			{
				// Asks for units to build it
				Broodwar << "Sending message" << std::endl;
				AgentSet* agents = unitClass->getAgents();
				for (auto &a : *agents)
				{
					if (a->type.isWorker()) {
						Message* msg = new Message(4, 1, 0, 1);
						a->inbox.inbox.push_back(msg);
						noMessages++;
					}
				}
				if (noMessages >= 0)
				{
					unitClass->buildMessageSent = true;
					Broodwar << "sent to " << noMessages << std::endl;
				}
				lastCheckedBuildPlanMsgs = 0;
			}

			// Checks buildplan responses
			if (!unitClass->buildPlan.empty() && unitClass->buildMessageSent && lastCheckedBuildPlanMsgs > 1000)
			{
				for (auto &m : unitClass->inbox.inbox)
				{
					if (m->receiver == 0)
					{
						// Read and delete.
						Broodwar << "Somebody wants to build stuff!!!" << std::endl;
						unitClass->inbox.inbox.erase(std::remove(unitClass->inbox.inbox.begin(), unitClass->inbox.inbox.end(), m), unitClass->inbox.inbox.end());
					}
				}
			}

			if (hq->isIdle() && !hq->train(hq->getType().getRace().getWorker()))
			{
				// If that fails, get error
				Error lastErr = Broodwar->getLastError();

				// Retrieve the supply provider type in the case that we have run out of supplies
				UnitType supplyProviderType = hq->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (lastErr == Errors::Insufficient_Supply &&
					lastChecked + 400 < Broodwar->getFrameCount() &&
					Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
				{
					lastChecked = Broodwar->getFrameCount();
					lastCheckedBuildPlanMsgs = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed
					Unit supplyBuilder = hq->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
						(IsIdle || IsGatheringMinerals) &&
						IsOwned);
					// If a unit was found
					if (supplyBuilder)
					{
						if (supplyProviderType.isBuilding())
						{
							TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
							if (targetBuildLocation)
							{
								// Order the builder to construct the supply structure
								supplyBuilder->build(supplyProviderType, targetBuildLocation);
							}
						}
						else
						{
							// Train the supply provider (Overlord) if the provider is not a structure
							supplyBuilder->train(supplyProviderType);
						}
					} // closure: supplyBuilder is valid
				} // closure: insufficient supply
			} // closure: failed to train idle unit
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