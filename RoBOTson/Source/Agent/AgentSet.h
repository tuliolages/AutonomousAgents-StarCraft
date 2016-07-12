#ifndef __AGENTSET_H__
#define __AGENTSET_H__

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "Agent.h"

using namespace BWAPI;
using namespace std;

class AgentSet : public SetContainer<Agent*, std::hash<void*>> {

};

#endif