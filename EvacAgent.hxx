
#ifndef __EvacAgent_hxx__
#define __EvacAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Evacuation
{
class EvacConfig;
// TEMPORARILY DISABLE ENUMS
/*enum Gender
{
    Male;
    Female;
};
enum Age
{
    Child;
    Adult;
    Elderly;
};*/

class EvacAgent : public Engine::Agent
{
        int speed;
        Engine::Point2D<int> _exit; 
        int floor; // which floor the agent is located
        int time; // time it takes to evacuate
        int panictime; // time the agent was in panic above **SELECT THRESHOLD**
        bool isOnStairs;
        bool exited;
        //Evacuation::Gender gender;
        //Evacuation::Age age;
        int age;
        int gender;

        int vision;
        int knowledge;
        int evacTime;
        float evacDist;
        float panicLevel; // what high panic leads to ??? FIGURE OUT !!!!!!!! 
        int notMoved;// how many timesteps the agent has not moved at all
        Engine::Point2D<int> currGoal; // current goal , maybe door or exit
        Engine::Point2D<int> tempNextPosition; // temporary next position, before competition for free cell has determined who moves. 
        void NextPosition();
        void SetTempNextPosition();


public:
	// todo remove environment from here
	EvacAgent( const std::string & id);
	virtual ~EvacAgent();
	void setExit( const Engine::Point2D<int> & exit ); // not sure what this is
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();
        //void SetTempNextPosition();


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	MyAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Evacuation

#endif // __EvacAgent_hxx__

 
