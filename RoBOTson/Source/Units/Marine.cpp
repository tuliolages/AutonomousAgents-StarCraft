#include "Marine.h"

Marine::Marine(Unit mUnit, HANDLE ghMutex, int id)
{
	Broodwar->sendText("A Marine came to existence!");
	unitID = mUnit->getID();
	type = mUnit->getType();
	state = ATTACKING;
	unit = mUnit;
	unithMutex = ghMutex;
	this->startMyThread();
	this->id = id;

}

Marine::~Marine()
{

}

void Marine::checkInbox() {
	Marine* marine = (Marine*)this;

	for (auto &m : marine->inbox)
	{
		// Read
		
		// Delete message
		marine->inbox.erase(m);
		marine->checkInbox();
		return;
		
	}
}

DWORD WINAPI Marine::run(void* param)
{
	Marine* marine = (Marine*)param;
	BWAPI::Unit unit = marine->unit;
	HANDLE unithMutex = marine->unithMutex;

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

			marine->checkInbox();

			if (marine->state == ATTACKING)
			{
				int bestTargetScore = -99999;
				Unit target = NULL;
				for (auto &u : Broodwar->enemy()->getUnits())
				{
					UnitType t = u->getType();

					bool canAttack = false;
					if (u->isBurrowed() && !u->isDetected())
					{
						canAttack = false;
					}

					int maxRange = 600;
					if (marine->unit->isSieged() || marine->unit->isBurrowed() || marine->unit->isLoaded()) maxRange = marine->unit->getType().groundWeapon().maxRange();

					if (canAttack && marine->unit->getPosition().getDistance(u->getPosition()) <= maxRange)
					{
						double mod = 1;
						int cScore = (int)((double)t.destroyScore() * mod);
						if (u->getHitPoints() < u->getInitialHitPoints())
						{
							//Prioritize damaged targets
							cScore++;
						}

						if (cScore > bestTargetScore)
						{
							bestTargetScore = cScore;
							target = u;
						}
					}
				}
				marine->unit->attack(target, true);
			}
			
			if (marine->state == MOVE_TO_SPOT)
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

void Marine::startMyThread()
{
	//DWORD ThreadID;
	//CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadID);
	CreateThread(NULL, 0, run, (void*)this, 0, NULL);
}