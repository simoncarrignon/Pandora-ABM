
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
        const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
	floor = Engine::GeneralState::statistics().getUniformDistValue(0,floorNumber-1);
        gender = Engine::GeneralState::statistics().getUniformDistValue(0,100) > malePercentage ? 1:0;
	int b = Engine::GeneralState::statistics().getUniformDistValue(0,100);
        if ((childPercentage + elderlyPercentage) > 100) {
             exit(8);
        }
        
        if (b <= childPercentage){ age = 0;}
        else if ((b>childPercentage) && (b<=(childPercentage+elderlyPercentage))) { age = 2;}
        else {age = 1;}
        
        if (age == 1) {speed = 3; vision = 200;}
        else if (age == 2) {speed = 1; vision = 50;}
        else if (age == 0) {speed = 2; vision = 250;}
}

EvacAgent::~EvacAgent()
{
}

// SET TEMP NEXT POSITION
void EvacAgent::SetTempNextPosition()
{
    const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
    

        
    if (getWorld()->getValue(eRoomOrCoridor, getPosition())==1) // IN A ROOM
        {
        
        // WE NEED TO DEFINE HOW TO DEAL WITH EXITED PEOPLE ALREADY AND GO FROM LOCATED TO EXIT TO REMOVED OR SMT - Seems done?>!?!!?!?
        
        // WE NEED TO ADD eTempCells to the movement when it has been decided!!!!!!!! - OK ?!?!
        
        // WE MAYBE NEED TO ADD RANDOM MOVEMENTS IF AGENT PANICKED 
        // ALSO WE MAYBE NEED TO CHANGE PANIC TO SOME LEVEL IF FOR A NUMBER OF STEPS THE AGENT HAS KNOWLEDGE 0 
        
        //WE NEED TO DECIDE WHAT PANIC LEVEL DOES TO A PERSON IN A SITUATION. HOW DOES IT AFFECT HIS ACTIONS????? How do we model trampling?
        //we need to add attributes to count metrics = time of evacuation, total distance travelled. - ADD PARAMETERS FOR EVERY AGENT IN AGENT.CXX
        //let it be agent.evacTime and agent.evacDist - done ?!?!?!?!
        
        //WHAT HAPPENS IF 0 CELLS AROUND ARE AVAILABLE ?!?!?! Model staying in same place, and maybe agressive behaviour. - Staying in same place seems OK - tempNext is position, cell occupied. Aggressive behavopir - NEED TO COME UP WITH IDEA
        
        if (knowledge == 0)
            {
            int seesigns = 0;
            int seedoors = 0;
            Engine::Point2D<int> index, currentPos;
            currentPos = getPosition();
            // HOW TO DEAL WITH CORNERED VIEW BLOCKED ?!?!?!?!? IT IS STILL IN RADIUS BUT IN REALITY NOT SEEN
            for (index._x = currentPos._x - vision; index._x <= currentPos._x + vision; index._x++) 
                {
                for (index._y = currentPos._y - vision; index._y <= currentPos._y + vision; index._y++)
                    {
                    if ((getWorld()->getValue(eSigns, index) == 1) && (getWorld()->getValue(eRoomOrCoridor, index) == 1)) {seesigns += 1;}
                    else if (getWorld()->getValue(eDoors, index) == 1) {seedoors +=1;}
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
                            if (getWorld()->getValue(eDoors, radius) == 1)
                                {
                                currGoal = radius;
                                doorfound += 1;
                                knowledge = 1;
                                break;
                                }
                            i++;
                            }
                        }
                     }
                  }
            else // RANDOM WALK IF DOESNT SEE SIGN OR DOOR
                {
                tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                 // HOW TO FORBID THEM TO GO THROUGH WALLS?
                while ((tempNextPosition._x < 0) || (tempNextPosition._x > getWorld()->getBoundaries()._size._width-1) || (tempNextPosition._y < 0) || (tempNextPosition._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, tempNextPosition) == 1) || (getWorld()->getValue(eRoomOrCoridor, tempNextPosition) != 1))
                    {
                    tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                    tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                    }
                getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
                }      
            }
        if (knowledge == 1)
            {
            Engine::Point2D<int> step, currentPos;
            currentPos = getPosition();
            tempNextPosition = currentPos;
            int i;
            for (step._x = currentPos._x - speed; step._x <= currentPos._x + speed; step._x++)
                {
                for (step._y = currentPos._y - speed; step._y <= currentPos._y + speed; step._y++)
                    {
                    if (sqrt(pow((currGoal._x - step._x),2) + pow((currGoal._y - step._y),2)) <= sqrt(pow((currGoal._x - tempNextPosition._x),2) + pow((currGoal._y - tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getWorld()->getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, step) == 1) || (getWorld()->getValue(eRoomOrCoridor, step) != 1) ) {continue;}          
                        // WE ALSO NEED TO DEFINE SOMEHOW THAT THE AGENTS DO NOT GO THROUGH CORNERS OR TO OTHER ROOMS
                        tempNextPosition = step;
                        }
                    }
                }
            getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
            }
        }
    else if (getWorld()->getValue(eRoomOrCoridor, getPosition())==0) // IN THE CORRIDOR
        {
        int seesigns = 0;
        Engine::Point2D<int> index, currentPos;
        currentPos = getPosition();
        // WE NEED TO MAKE SURE THE SIGNS IN THE CORRIDOR ARE DISTINGUISHED FROM THE ONES IN ROOMS AND NOT TAKE INTO ACCOUNT SIGNS IN ROOMS FOR THIS
        for (index._x = currentPos._x - vision; index._x <= currentPos._x + vision; index._x++) 
            {
            for (index._y = currentPos._y - vision; index._y <= currentPos._y + vision; index._y++)
                {
                if ((getWorld()->getValue(eSigns, index) == 1) && (getWorld()->getValue(eRoomOrCoridor, index)==0 )) {seesigns += 1;}
                }
            }        
            
        if ((seesigns == 0) && (knowledge == 0)) //RANDOM WALK IF DOESNT SEE SIGNS OR DOESNT KNOW WHERE TO GO
             {
             tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
             tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
             // HOW TO FORBID THEM TO GO THROUGH WALLS?
             while ((tempNextPosition._x < 0) || (tempNextPosition._x > getWorld()->getBoundaries()._size._width-1) || (tempNextPosition._y < 0) || (tempNextPosition._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, tempNextPosition) == 1) || (getWorld()->getValue(eRoomOrCoridor, tempNextPosition) != 0) )
                 {
                 tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                 tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
                 }
             getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
             }
        
        else if (knowledge == 1) // HE KNOWS WHERE TO GO 
            {
            Engine::Point2D<int> step, currentPos;
            currentPos = getPosition();
            tempNextPosition = currentPos;
            int i = 1;
            for (step._x = currentPos._x - speed; step._x <= currentPos._x + speed; step._x++)
                {
                for (step._y = currentPos._y - speed; step._y <= currentPos._y + speed; step._y++)
                    {
                    if (sqrt(pow((currGoal._x - step._x),2) + pow((currGoal._y - step._y),2)) <= sqrt(pow((currGoal._x - tempNextPosition._x),2) + pow((currGoal._y - tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getWorld()->getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, step) == 1) || (getWorld()->getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        tempNextPosition = step;
                        }
                    }
                }
            getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
            }
        else if ((knowledge == 0) && (seesigns > 0))
            {
            currGoal = _exits[0];
            int i;
            for (i= 0; i< _exits.size(); i++)
                {
                if (sqrt(pow((currGoal._x - getPosition()._x),2) + pow((currGoal._y - getPosition()._y),2)) >= sqrt(pow((_exits[i]._x - getPosition()._x),2) + pow((_exits[i]._y - getPosition()._y),2))) {currGoal = _exits[i];}
                }
            knowledge = 1;
            // NOW SELECT STEP LIKE PREVIOUS - DONE !!!!
            Engine::Point2D<int> step, currentPos;
            currentPos = getPosition();
            tempNextPosition = currentPos;
            i = 1;
            for (step._x = currentPos._x - speed; step._x <= currentPos._x + speed; step._x++)
                {
                for (step._y = currentPos._y - speed; step._y <= currentPos._y + speed; step._y++)
                    {
                    if (sqrt(pow((currGoal._x - step._x),2) + pow((currGoal._y - step._y),2)) <= sqrt(pow((currGoal._x - tempNextPosition._x),2) + pow((currGoal._y - tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getWorld()->getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, step) == 1) || (getWorld()->getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        tempNextPosition = step;
                        }
                    }
                }
            getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
            }
        }
    else if (getWorld()->getValue(eRoomOrCoridor, getPosition())==2) //WHEN YOU ARE AT THE DOOR
        {
        // _exits - list with exit signs
        knowledge = 0;
        int seesigns = 0;
        int seeexits = 0;
        Engine::Point2D<int> index, currentPos;
        currentPos = getPosition();
        for (index._x = currentPos._x - vision; index._x <= currentPos._x + vision; index._x++) 
            {
            for (index._y = currentPos._y - vision; index._y <= currentPos._y + vision; index._y++)
                {
                if ((getWorld()->getValue(eSigns, index) == 1) && ((getWorld()->getValue(eRoomOrCoridor, index) == 0)) ) {seesigns += 1;}
                else if (getWorld()->getValue(eExits, index) == 1 ) {seeexits +=1;}
                }
            }
        if ((seesigns == 0) && (seeexits == 0)) // RANDOM IF DOESNT KNOW ANYTHING
        {
        tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
        tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
        // HOW TO FORBID THEM TO GO THROUGH WALLS?
        while ((tempNextPosition._x < 0) || (tempNextPosition._x > getWorld()->getBoundaries()._size._width-1) || (tempNextPosition._y < 0) || (tempNextPosition._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, tempNextPosition) == 1) || (getWorld()->getValue(eRoomOrCoridor, tempNextPosition) != 0) )
            {
            tempNextPosition._x = getPosition()._x + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
            tempNextPosition._y = getPosition()._y + Engine::GeneralState::statistics().getUniformDistValue((-1 * speed), speed);
            }
        getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
        }
        else if ((seesigns > 0) || (seeexits > 0)) //sees exits or signs 
            {
            currGoal = _exits[0];
            int i; 
            for (i= 0; i< _exits.size(); i++)
                {
                if (sqrt(pow((currGoal._x - getPosition()._x),2) + pow((currGoal._y - getPosition()._y),2)) >= sqrt(pow((_exits[i]._x - getPosition()._x),2) + pow((_exits[i]._y - getPosition()._y),2))) {currGoal = _exits[i];}
                }
            knowledge = 1;                
            Engine::Point2D<int> step, currentPos;
            currentPos = getPosition();            
            tempNextPosition = currentPos;
            i = 1;
            for (step._x = currentPos._x - speed; step._x <= currentPos._x + speed; step._x++)
                {
                for (step._y = currentPos._y - speed; step._y <= currentPos._y + speed; step._y++)
                    {
                    if (sqrt(pow((currGoal._x - step._x),2) + pow((currGoal._y - step._y),2)) <= sqrt(pow((currGoal._x - tempNextPosition._x),2) + pow((currGoal._y - tempNextPosition._y),2)) )
                        {
                        if ((step._x < 0) || (step._x > getWorld()->getBoundaries()._size._width-1) || (step._y < 0) || (step._y > getWorld()->getBoundaries()._size._height-1) || (getWorld()->getValue(eObstacles, step) == 1) || (getWorld()->getValue(eRoomOrCoridor, step) != 0) ) {continue;}
                        tempNextPosition = step;
                        }
                    }
                }
            getWorld()->setValue(eTempCells, tempNextPosition, getWorld()->getValue(eTempCells, (tempNextPosition)+1));
            }        
        }
}




void EvacAgent::NextPosition() // I BELIEVE THIS SHOULD BE IN WORLD CXX
{
    const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
        Engine::Point2D<int> curPos = getPosition();    
        if (getWorld()->getDynamicRaster(eTempCells).getValue(curPos) == 1)
        {
            
            evacDist = evacDist + sqrt(pow((tempNextPosition._x - curPos._x),2) + pow((tempNextPosition._y - curPos._y),2));
            setPosition(tempNextPosition);
            evacTime++;
            getWorld()->setValue(eChemoTaxiTrails, tempNextPosition, getWorld()->getValue(eChemoTaxiTrails, (tempNextPosition)+1));
        }
        //else {continue;}
}


void EvacAgent::updateState()
{

       // IS THIS THE CORRECT WAY TO REMOVE EXITED AGENTS ?!?!??!?!!?!?
       if (exited == true)
        {
        return;
        }
    else if (getWorld()->getValue(eExits, getPosition()) == 1)
        {
        exited = true;
        getWorld()->removeAgent(this);
        }
    
    else if(panicked >= 1) // panic increases with more not moving
        {
        if (notMoved > notMovedPanicTreshold) 
            {
            panicked +=1;
            notMoved = 0;
            }
        }
        
    else if((notMoved > notMovedPanicTreshold) && (panicked == 0))
        {
        panicked = 1;
        knowledge = 0;
        notMoved = 0;
        }

    
        SetTempNextPosition(); // NOT SURE IF THIS HAS TO BE HERE -
        NextPosition();

// IDE on windows for methods ( or devian c++)


}

void EvacAgent::registerAttributes()
{
    const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
        registerIntAttribute("panicLevel");
        registerIntAttribute("speed");
	registerIntAttribute("notMoved");
        registerFloatAttribute("evacDist"); 
	registerIntAttribute("evacTime");               
}

void EvacAgent::serialize()
{
    const EvacConfig & evacConfig = (const EvacConfig &)getWorld()->getConfig();
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


