#ifndef __EvacWorld_hxx
#define __EvacWorld_hxx

#include <World.hxx>
#include <GeneralState.hxx>

namespace Evacuation 
{
class EvacAgent;
class EvacConfig;

enum Rasters
{
	// obstacle, 1 if true, 0 if false
	eObstacles,

	// exits, 1 if true, 0 if false
	eExits, 

        // temporary count of agents wanting to move to this cell. 
        eTempCells,
        
        // stairs
        eStairs,
    
        //chemotaxi-trails
        eChemoTaxiTrails,

        //signs for evacuation
        eSigns,

        //doors locations, like temporary exit-goals
        eDoors,

        // room or corridor (room == 1, corridor == 0, door == 2)
        eRoomOrCoridor,
	
        // floor number for initial distribution
        eFloor,

	// number of agents in each cell
	eNumAgents

	// eDeaths, - Does not seem required now.

};

class EvacWorld: public Engine::World
{

	void createAgents();
	void createRasters();

	void fillExitList();
	void stepEnvironment();
        void ResolveCompetition();
        void ResetExits();
        void UpdateExitValues();
 

public:
	EvacWorld( EvacConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~EvacWorld();
// FROM ABOVE THESE 3 LINES HERE
	typedef std::list<Engine::Point2D<int> > ExitsList;
	ExitsList _exits;
        ExitsList returnList const();
};

} // namespace Evacuation

#endif // __EvacWorld_hxx

