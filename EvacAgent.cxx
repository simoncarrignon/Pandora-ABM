
#include "EvacAgent.hxx"

#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <cmath>

#include "EvacWorld.hxx"
#include "EvacConfig.hxx"

namespace Evacuation
{



//EvacAgent::EvacAgent( const std::string & id) : Agent(id), _direction(0), _exited(false), _panicked(false), _rest(0.0f, 0.0f), _consecutive(0)
EvacAgent::EvacAgent( const std::string & id) : Agent(id), speed(0.0f), floor(0), isOnStairs(false), exited(false), gender(0), age(0), panicLevel(0), currGoal(0, 0), vision(0), evacDist(0), evacTime(0)
{
        const EvacConfig & evacConfig = (const EvacConfig &)getConfig();
	floor = Engine::GeneralState::statistics().getUniformDistValue(0,floorNumber-1);
        gender = Engine::GeneralState::statistics().getUniformDistValue(0,100) > malePercentage ? 1:0;
	b = Engine::GeneralState::statistics().getUniformDistValue(0,100)
        if ((childPercentage + elderlyPercentage) > 100) {
             exit(8);
        }
        else {
            if (b <= childPercentage){ age = 0;}
            else if ((b>childPercentage) && (b<=(childPercentage+elderlyPercentage))) { age = 2;}
            else {age = 1;}
        }
        if (age == 1) {speed = 3; vision = ;}
        else if (age == 2) {speed = 1; vision = ;}
        else if (age == 0) {speed = 2; vision = ;}
}

EvacAgent::~EvacAgent()
{
}

// SET TEMP NEXT POSITION
void EvacAgent::SetTempNextPosition()
{
    const EvacConfig & evacConfig = (const EvacConfig &)getConfig();
    

        
    if (getValue(eRoomOrCoridor, agent->getPosition())==1) // IN A ROOM
        {
        
        // WE NEED TO DEFINE HOW TO DEAL WITH EXITED PEOPLE ALREADY AND GO FROM LOCATED TO EXIT TO REMOVED OR SMT - Seems done?>!?!!?!?
        
        // WE NEED TO ADD eTempCells to the movement when it has been decided!!!!!!!! - OK ?!?!
        
        // WE MAYBE NEED TO ADD RANDOM MOVEMENTS IF AGENT PANICKED 
        // ALSO WE MAYBE NEED TO CHANGE PANIC TO SOME LEVEL IF FOR A NUMBER OF STEPS THE AGENT HAS KNOWLEDGE 0 
        
        //WE NEED TO DECIDE WHAT PANIC LEVEL DOES TO A PERSON IN A SITUATION. HOW DOES IT AFFECT HIS ACTIONS????? How do we model trampling?
        //we need to add attributes to count metrics = time of evacuation, total distance travelled. - ADD PARAMETERS FOR EVERY AGENT IN AGENT.CXX
        //let it be agent.evacTime and agent.evacDist - done ?!?!?!?!
        
        //WHAT HAPPENS IF 0 CELLS AROUND ARE AVAILABLE ?!?!?! Model staying in same place, and maybe agressive behaviour. - Staying in same place seems OK - tempNext is position, cell occupied. Aggressive behavopir - NEED TO COME UP WITH IDEA
        
        if (agent.knowledge == 0)
            {
            int seesigns = 0;
            int seedoors = 0;
            Engine::Point2D<int> index, currentPos;
            currentPos = agent->getPosition();
            // HOW TO DEAL WITH CORNERED VIEW BLOCKED ?!?!?!?!? IT IS STILL IN RADIUS BUT IN REALITY NOT SEEN
            for (index._x = currentPos._x - agent.vision; index._x <= currentPos._x + agent.vision; index._x++) 
                {
                for (index._y = currentPos._y - agent.vision; index._y <= currentPos._y + agent.vision; index._y++)
                    {
                    if ((getValue(eSigns, index) == 1) && (getValue(eRoomOrCoridor, index) == 1)) {seesigns += 1;}
                    else if (getValue(eDoors, index) == 1) {seedoors +=1;}
                    }
                }
            if ((seedoors > 0) || (seesigns > 0)) 
                {
                 int doorfound = 0; // when door is located, it becomes one and is recorded as the temp goal of the agent
                 int i = 1;
                 Engine::Point2D<int> radius, step;
                 while ((doorfound = 0) && (knowledge == 0))
                     {
                     int count = 0;
                     for (radius._x = currentPos._x - i; radius._x <= currentPos._x + i; radius._x++)
                        {
                        for (radius._y = currentPos._y - i; radius._y <= currentPos._y + i; radius._y++)
                            {
                            if (getValue(eDoors, radius) == 1)
                                {
                                agent.currGoal = radius;
                                doorfound += 1;
                                agent.knowledge = 1;
                                break;
                                }
                            i++;
                            }
                        }
                     }
                  }
            else // RANDOM WALK IF DOESNT SEE SIGN OR DOOR
                {
                agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                 // HOW TO FORBID THEM TO GO THROUGH WALLS?
                while ((agent.tempNextPosition._x < 0) || (agent.tempNextPosition._x > getBoundaries()._size._width-1) || (agent.tempNextPosition._y < 0) || (agent.tempNextPosition._y > getBoundaries()._size._height-1) || (getValue(eObstacles, agent.tempNextPosition) == 1) || (getValue(eRoomOrCoridor, agent.tempNextPosition) != 1))
                    {
                    agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                    agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                    }
                setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
                }      
            }
        if (agent.knowledge == 1)
            {
            Engine::Point2D<int> step;
            currentPos = agent->getPosition();
            agent.tempNextPosition = currentPos;
            int i;
            for (step._x = currentPos._x - agent.speed; step._x <= currentPos._x + agent.speed; step._x++)
                {
                for (step._y = currentPos._y - i; step._y <= currentPos._y + agent.speed; step._y++)
                    {
                    if (sqrt(pow((agent.currGoal._x - step._x),2) + pow((agent.currGoal._y - step._y),2)) <= sqrt(pow((agent.currGoal._x - agent.tempNextPosition._x),2) + pow((agent.currGoal._y - agent.tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getBoundaries()._size._height-1) || (getValue(eObstacles, step) == 1) || (getValue(eRoomOrCoridor, step) != 1) ) {continue;}          
                        // WE ALSO NEED TO DEFINE SOMEHOW THAT THE AGENTS DO NOT GO THROUGH CORNERS OR TO OTHER ROOMS
                        agent.tempNextPosition = step;
                        }
                    }
                }
            setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
            }
        }
    else if getValue(eRoomOrCoridor, agent->getPosition())==0) // IN THE CORRIDOR
        {
        int seesigns = 0;
        Engine::Point2D<int> index, currentPos;
        currentPos = agent->getPosition();
        // WE NEED TO MAKE SURE THE SIGNS IN THE CORRIDOR ARE DISTINGUISHED FROM THE ONES IN ROOMS AND NOT TAKE INTO ACCOUNT SIGNS IN ROOMS FOR THIS
        for (index._x = currentPos._x - agent.vision; index._x <= currentPos._x + agent.vision; index._x++) 
            {
            for (index._y = currentPos._y - agent.vision; index._y <= currentPos._y + agent.vision; index._y++)
                {
                if ((getValue(eSigns, index) == 1) && (getValue(eRoomOrCoridor, index)==0 )) {seesigns += 1;}
                }
            }        
            
        if ((seesigns == 0) && (agent.knowledge == 0)) //RANDOM WALK IF DOESNT SEE SIGNS OR DOESNT KNOW WHERE TO GO
             {
             agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
             agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
             // HOW TO FORBID THEM TO GO THROUGH WALLS?
             while ((agent.tempNextPosition._x < 0) || (agent.tempNextPosition._x > getBoundaries()._size._width-1) || (agent.tempNextPosition._y < 0) || (agent.tempNextPosition._y > getBoundaries()._size._height-1) || (getValue(eObstacles, agent.tempNextPosition) == 1) || (getValue(eRoomOrCoridor, agent.tempNextPosition) != 0) )
                 {
                 agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                 agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
                 }
             setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
             }
        }
        else if (agent.knowledge == 1) // HE KNOWS WHERE TO GO 
            {
            Engine::Point2D<int> step;
            currentPos = agent->getPosition();
            agent.tempNextPosition = currentPos;
            int i = 1;
            for (step._x = currentPos._x - agent.speed; step._x <= currentPos._x + agent.speed; step._x++)
                {
                for (step._y = currentPos._y - i; step._y <= currentPos._y + agent.speed; step._y++)
                    {
                    if (sqrt(pow((agent.currGoal._x - step._x),2) + pow((agent.currGoal._y - step._y),2)) <= sqrt(pow((agent.currGoal._x - agent.tempNextPosition._x),2) + pow((agent.currGoal._y - agent.tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getBoundaries()._size._height-1) || (getValue(eObstacles, step) == 1) || (getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        agent.tempNextPosition = step;
                        }
                    }
                }
            setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
            }
        else if ((agent.knowledge == 0) && (seesigns > 0))
            {
            agent.currGoal = _exits[0];
            int i;
            for (i= 0; i< _exits.size(); i++)
                {
                if (sqrt(pow((agent.currGoal._x - agent.getPosition()._x),2) + pow((agent.currGoal._y - agent.getPosition()._y),2)) >= sqrt(pow((_exits[i]._x - agent.getPosition()._x),2) + pow((_exits[i]._y - agent.getPosition()._y),2))) {agent.currGoal = _exits[i];}
                }
            agent.knowledge = 1;
            // NOW SELECT STEP LIKE PREVIOUS - DONE !!!!
            Engine::Point2D<int> step, currenPos;
            currentPos = agent->getPosition();
            agent.tempNextPosition = currentPos;
            int i = 1;
            for (step._x = currentPos._x - agent.speed; step._x <= currentPos._x + agent.speed; step._x++)
                {
                for (step._y = currentPos._y - i; step._y <= currentPos._y + agent.speed; step._y++)
                    {
                    if (sqrt(pow((agent.currGoal._x - step._x),2) + pow((agent.currGoal._y - step._y),2)) <= sqrt(pow((agent.currGoal._x - agent.tempNextPosition._x),2) + pow((agent.currGoal._y - agent.tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getBoundaries()._size._height-1) || (getValue(eObstacles, step) == 1) || (getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        agent.tempNextPosition = step;
                        }
                    }
                }
            setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
            }
    else if getValue(eRoomOrCoridor, agent->getPosition())==2) //WHEN YOU ARE AT THE DOOR
        {
        // _exits - list with exit signs
        agent.knowledge = 0;
        int seesigns = 0;
        int seeexits = 0;
        Engine::Point2D<int> index, currentPos;
        currentPos = agent->getPosition();
        for (index._x = currentPos._x - agent.vision; index._x <= currentPos._x + agent.vision; index._x++) 
            {
            for (index._y = currentPos._y - agent.vision; index._y <= currentPos._y + agent.vision; index._y++)
                {
                if ((getValue(eSigns, index) == 1) && ((getValue(eRoomOrCoridor, index) == 0)) ) {seesigns += 1;}
                else if (getValue(eExits, index) == 1 ) {seeexits +=1;}
                }
            }
        if ((seesigns == 0) && (seeexits == 0)) // RANDOM IF DOESNT KNOW ANYTHING
        {
        agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
        agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
        // HOW TO FORBID THEM TO GO THROUGH WALLS?
        while ((agent.tempNextPosition._x < 0) || (agent.tempNextPosition._x > getBoundaries()._size._width-1) || (agent.tempNextPosition._y < 0) || (agent.tempNextPosition._y > getBoundaries()._size._height-1) || (getValue(eObstacles, agent.tempNextPosition) == 1) || (getValue(eRoomOrCoridor, agent.tempNextPosition) != 0) )
            {
            agent.tempNextPosition._x = agent.getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
            agent.tempNextPosition._y = agent.getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * agent.speed), agent.speed);
            }
        setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
        }
        else if ((seesigns > 0) || (seeexits > 0)) //sees exits or signs 
            {
            agent.currGoal = _exits[0];
            int i; 
            for (i= 0; i< _exits.size(); i++)
                {
                if (sqrt(pow((agent.currGoal._x - agent.getPosition()._x),2) + pow((agent.currGoal._y - agent.getPosition()._y),2)) >= sqrt(pow((_exits[i]._x - agent.getPosition()._x),2) + pow((_exits[i]._y - agent.getPosition()._y),2))) {agent.currGoal = _exits[i];}
                }
            agent.knowledge = 1;                
            Engine::Point2D<int> step, currentPos;
            currentPos = agent->getPosition();            
            agent.tempNextPosition = currentPos;
            int i = 1;
            for (step._x = currentPos._x - agent.speed; step._x <= currentPos._x + agent.speed; step._x++)
                {
                for (step._y = currentPos._y - i; step._y <= currentPos._y + agent.speed; step._y++)
                    {
                    if (sqrt(pow((agent.currGoal._x - step._x),2) + pow((agent.currGoal._y - step._y),2)) <= sqrt(pow((agent.currGoal._x - agent.tempNextPosition._x),2) + pow((agent.currGoal._y - agent.tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getBoundaries()._size._height-1) || (getValue(eObstacles, step) == 1) || (getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        agent.tempNextPosition = step;
                        }
                    }
                }
            setValue(eTempCells, agent.tempNextPosition, getValue(eTempCells, (agent.tempNextPosition)+1));
            }        
        }
}




void EvacAgent::NextPosition() // I BELIEVE THIS SHOULD BE IN WORLD CXX
{
    const EvacConfig & evacConfig = (const EvacConfig &)getConfig();
//    for (int i=0; i<evacConfig._numAgents; i++){    
        if (getDynamicRaster(eTempCells).getValue(agent.position) == 1)
        {
            agent.evacDist = agent.evacDist + sqrt(pow((agent.tempNextPosition._x - agent.position._x),2) + pow((agent.tempNextPosition._y - agent.position._y),2));
            agent->setPosition(agent.tempNextPosition);
            agent.evacTime++;
            setValue(eChemoTaxiTrails, agent.tempNextPosition, getValue(eChemoTaxiTrails, agent.tempNextPosition)+1));
        }
        else {continue;}
}


void EvacAgent::updateState()
{

       // IS THIS THE CORRECT WAY TO REMOVE EXITED AGENTS ?!?!??!?!!?!?
       if (agent.exited == true)
        {
        return;
        }
    else if (getValue(eExits, agent->getPosition()) == 1)
        {
        agent.exited = true;
        removeAgent(agent);
        }
    
    else if(agent.panicked >= 1) // panic increases with more not moving
        {
        if (agent.notMoved > notMovedPanicTreshold) 
            {
            agent.panicked +=1;
            agent.notMoved = 0;
            }
        }
        
    else if((agent.notMoved > notMovedPanicTreshold) && (agent.panicked == 0))
        {
        agent.panicked = 1;
        agent.knowledge = 0;
        agent.notMoved = 0;
        }

    
        SetTempNextPosition(); // NOT SURE IF THIS HAS TO BE HERE -
        NextPosition();

// IDE on windows for methods ( or devian c++)


	/*if(_exited)
	{
		return;
	}

	if(_world->getValue(eExits, _position)==1)
	{
		_exited = true;	
		_world->removeAgent(this);
	}

    const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
	if(_world->getValue(eCompression, _position)>scenarioConfig._compressionThreshold)
	{
		_world->setValue(eDeaths, _position, _world->getValue(eDeaths, _position)+1);		
		_world->setValue(eNumAgents, _position, _world->getValue(eNumAgents, _position)-1);		
		_world->setValue(eCompression, _position, std::max(0, _world->getValue(eCompression, _position)-scenarioConfig._compressionThreshold));
		_world->removeAgent(this);
	}

	if(!_panicked && _world->getValue(ePanic, _position)==1)
	{
		_panicked = true;
	}*/

}

void EvacAgent::registerAttributes()
{
        registerIntAttribue("panicLevel");
        registerIntAttribue("speed");
	registerIntAttribute("notMoved");
        registerFloatAttribue("evacDist"); 
	registerIntAttribute("evacTime");               
}

void EvacAgent::serialize()
{
        serializeAttribute("panicLevel", panicLevel);
        serializeAttribute("speed", speed);
        serializeAttribute("notMoved", notMoved);
        serializeAttribute("evacTime", evacTime);
        serializeAttribute("evacDist", evacDist);

}
	
void EvacAgent::setExit( const Engine::Point2D<int> & exit )
{
	_exit = exit;
}

} // namespace Evacuation


