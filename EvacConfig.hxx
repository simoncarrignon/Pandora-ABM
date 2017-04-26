#ifndef __EvacConfig_hxx__
#define __EvacConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>
#include <Point2D.hxx>
#include <map>
#include <list>

namespace Evacuation
{


class EvacConfig : public Engine::Config
{

        typedef std::list<Engine::Point2D<int> > SignList;
        typedef std::list<Engine::Point2D<int> > ExitConfigList;
        typedef std::list<Engine::Point2D<int> > DoorList;


	int _numAgents;
        int childPercentage;
        int elderlyPercentage;
        int malePercentage;
        int floorNumber;
        int notMovedPanicTreshold;

        
        // exit list from config
        ExitConfigList exitconfiglist;
        	
        // Sign List
        SignList signList;

        // Door List
        DoorList doorList;


public:
	EvacConfig( const std::string & xmlFile );
	virtual ~EvacConfig();
	void loadParams();
        int returnChildPerc() const{return childPercentage;}
        int returnMalePerc() const{return malePercentage;}
        int returnElderlyPerc() const{return elderlyPercentage;}
        int returnFloorNumber() const{return floorNumber;}
        int returnPanicTresh() const{return notMovedPanicTreshold;}
        
	
	friend class EvacWorld;
	friend class EvacAgent;
};

} // namespace Evacuation

#endif 

