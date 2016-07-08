#include "General.h"

using namespace BWAPI;
using namespace Filter;

General::General(Unit mUnit, HANDLE ghMutex)
{
	Broodwar->sendText("A general came to existence!");
	this->unitID = mUnit->getID();
	this->type = mUnit->getType();
	this->unit = mUnit;
	this->unithMutex = ghMutex;
	this->startMyThread();

}

General::~General()
{

}

DWORD WINAPI General::run(void* param)
{
	General* unitClass = (General*)param;
	BWAPI::Unit hq = unitClass->unit;
	HANDLE unithMutex = unitClass->unithMutex;
	
	DWORD dwWaitResult;

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

void General::startMyThread()
{
	DWORD ThreadID;
	//CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadID);
	CreateThread(NULL, 0, run, (void*)this, 0, NULL);
}