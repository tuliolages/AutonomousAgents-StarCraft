#include "SCV.h"

int frames = 0;
TilePosition target;
BWAPI::UnitType bui_type;
SCV::SCV(Unit mUnit, HANDLE ghMutex, int id)
{
	Broodwar->sendText("A SCV came to existence!");
	unitID = mUnit->getID();
	type = mUnit->getType();
	state = GATHER_MINERALS;
	unit = mUnit;
	unithMutex = ghMutex;
	this->startMyThread();
	this->id = id;

}

SCV::~SCV()
{

}

void SCV::checkInbox() {
	SCV* worker = (SCV*)this;
	
	for (auto &m : worker->inbox)
	{
		//if (m->type == 4) // Request
		//{
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
				General::getInstance()->inbox.insert(new Message(2, 1, worker->id, 1, -1, 0, std::make_tuple(distance, isFree)));
				
				Broodwar << "General sent: " << m->type << " " << m->code << std::endl;
				//}
			}
			else if (m->code == 2 && m->senderType == 0)
			{
				Broodwar << "General order" << std::endl;
				General::getInstance()->inbox.insert(new Message(2, 1, worker->id, 1, -1, 0, true));
				TilePosition targetBuildLocation;
				if (m->building == 1)
				{
					bui_type = UnitTypes::Terran_Supply_Depot;
					targetBuildLocation = Broodwar->getBuildLocation(UnitTypes::Terran_Supply_Depot, worker->unit->getTilePosition());
				}
				else if (m->building == 2) {
					bui_type = UnitTypes::Terran_Barracks;
					targetBuildLocation = Broodwar->getBuildLocation(UnitTypes::Terran_Barracks, worker->unit->getTilePosition());
				}
				
				if (targetBuildLocation)
				{
					target = targetBuildLocation;
					// Order the builder to construct the supply structure
					//Position toMove = Position(target.x * 32 + 16, target.y * 32 + 16);
					//worker->unit->rightClick(toMove);
					worker->state = MOVE_TO_SPOT;
				}
			}
			worker->inbox.erase(m);
			worker->checkInbox();
			return;
		//}
	}
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
			
			worker->checkInbox();

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
				General::getInstance()->isTryingToBuild = false;
				if (worker->isBuilt()) {
					worker->state = GATHER_MINERALS;
					//Broodwar << "Going to build depot!! -n" << std::endl;
				}

			}
			if (worker->state == MOVE_TO_SPOT)
			{
				General::getInstance()->isTryingToBuild = true;
				if (!worker->buildSpotExplored())
				{
					Position toMove = Position(target.x * 32 + 16, target.y * 32 + 16);
					worker->unit->rightClick(toMove);
				}

				if (worker->buildSpotExplored() && !worker->unit->isConstructing())
				{
					//UnitType supplyProviderType = General::getInstance()->unit->getType().getRace().getSupplyProvider();
					bool ok = unit->build(bui_type, target); 
					//Broodwar << "Ill build? " << (ok ? "yes" : "no") << std::endl;
					if (!ok)
					{
						
					}
				}

				if (unit->isConstructing())
				{
					worker->state = CONSTRUCT;
				}
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

bool SCV::buildSpotExplored()
{
	int sightDist = 64;

	double dist = unit->getPosition().getDistance(Position(target));
	if (dist > sightDist)
	{
		return false;
	}
	return true;
}

bool SCV::isBuilt()
{
	if (unit->isConstructing()) return false;

	Unit b = unit->getTarget();
	if (b != NULL) if (b->getRemainingBuildTime() > 0) return false;

	return true;
}

void SCV::startMyThread()
{
	//DWORD ThreadID;
	//CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadID);
	CreateThread(NULL, 0, run, (void*)this, 0, NULL);
}