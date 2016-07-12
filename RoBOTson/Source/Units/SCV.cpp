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

void checkInbox() {

}

DWORD WINAPI SCV::run(void* param)
{
	SCV* worker = (SCV*)param;
	BWAPI::Unit unit = worker->unit;
	HANDLE unithMutex = worker->unithMutex;

	DWORD dwWaitResult;

	int c = 0;

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
			
			for (auto &m : worker->inbox.inbox)
			{
				if (m->type == 4) // Request
				{
					if (m->code == 1) // Distance to sent unit && it's free
					{
						//TilePosition targetBuildLocation = Broodwar->getBuildLocation(UnitTypes::Terran_Supply_Depot, unit->getTilePosition());
						//if (targetBuildLocation)
						//{
							// Order the builder to construct the supply structure
							//supplyBuilder->build(supplyProviderType, targetBuildLocation);
						double distance = General::getInstance()->unit->getDistance(unit);
							// Send response
						int isFree = unit->isIdle() ? 1 : 0;
						General::getInstance()->inbox.inbox.push_back(new Message(2, 1, 1, 0, std::make_tuple(distance, isFree)));
						if (c == 0) {
							c++;
							Broodwar << "MESSAGEEEEEEEEEEEEEEE: " << m->code << std::endl;
						}

						//worker->inbox.inbox.erase(std::remove(worker->inbox.inbox.begin(), worker->inbox.inbox.end(), m), worker->inbox.inbox.end());
						//}
					}
				}
			}

			// Acts accordng to current state
			if (worker->state == GATHER_MINERALS)
			{

				if (unit->isIdle())
				{
					// Order workers carrying a resource to return them to the center,
					// otherwise find a mineral patch to harvest.
					if (unit->isCarryingMinerals())
					{
						unit->returnCargo();
					}
					else if (!unit->getPowerUp())  // The worker cannot harvest anything if it
					{                              // is carrying a powerup such as a flag
						// Harvest from the nearest mineral patch or gas refinery
						BWAPI::Unit tempu = unit->getClosestUnit(BWAPI::Filter::IsMineralField);
						if (tempu != NULL && !unit->gather(tempu))
						{
							// If the call fails, then print the last error message
							// Broodwar << Broodwar->getLastError() << std::endl;
						}

					} // closure: has no powerup
				} // closure: if idle
			}
			if (worker->state == GATHER_GAS)
			{

			}
			if (worker->state == ATTACKING)
			{

			}
			if (worker->state == CONSTRUCT)
			{

			}
			if (worker->state == MOVE_TO_SPOT)
			{

			}
			if (worker->state == REPAIRING)
			{

			}

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