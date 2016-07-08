#include "SCV.h"

SCV::SCV(Unit mUnit, HANDLE ghMutex)
{
	Broodwar->sendText("A SCV came to existence!");
	unitID = mUnit->getID();
	type = mUnit->getType();
	state = GATHER_MINERALS;
	unit = mUnit;
	unithMutex = ghMutex;
	this->startMyThread();

}

SCV::~SCV()
{

}

DWORD WINAPI SCV::run(void* param)
{
	SCV* unitClass = (SCV*)param;
	BWAPI::Unit unit = unitClass->unit;
	HANDLE unithMutex = unitClass->unithMutex;

	DWORD dwWaitResult;

	while (true){

		dwWaitResult = WaitForSingleObject(
			unithMutex,    // handle to mutex
			100);  // time-out interval

		//// If end game, or if it exists (remember to always check)
		if (GlobalBlackBoard::GameOver || unit == NULL || !unit->exists())  {
			ReleaseMutex(unithMutex);
			return 0; // end thread
		} // end thread
		// You can check tons of others things like isStuck, isLockedDown, constructing
		if (!unit->isCompleted() || !unit->isCompleted()){ // You can create it on the onUnitComplete too!
			ReleaseMutex(unithMutex);
			Sleep(500);
			continue;
		}

		if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_ABANDONED) //RAII
		{
			// if our worker is idle
			if (unit->isIdle())
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (unit->isCarryingGas() || unit->isCarryingMinerals())
				{
					unit->returnCargo();
				}
				else if (!unit->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
					BWAPI::Unit tempu = unit->getClosestUnit(
						BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery);
					if (tempu != NULL && !unit->gather(tempu))
					{
						// If the call fails, then print the last error message
						// Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle

			if (!ReleaseMutex(unithMutex))
			{
				// Handle error.
			}

			Sleep(10); // Some agents can sleep more than others. 
		}
	}
}

void SCV::startMyThread()
{
	//DWORD ThreadID;
	//CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadID);
	CreateThread(NULL, 0, run, (void*)this, 0, NULL);
}