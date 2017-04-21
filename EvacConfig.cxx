#include <EvacConfig.hxx>

namespace Evacuation
{

EvacConfig::EvacConfig( const std::string & xmlFile ) : Config(xmlFile), _obstacleFile("not loaded"), _initAgentsDistributionFile("not loaded")
{
}

EvacConfig::~EvacConfig()
{
}

void EvacConfig::loadParams()
{

    // Newly Added parameters still not in the config file || FINISH THE CORRECT PATH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    _numAgents = getParamInt("agents", "num");
    childPercentage = getParamInt("agents", "childPercentage");
    elderlyPercentage = getParamInt("agents", "elderlyPercentage");
    malePercentage = getParamInt("agents", "malePercentage");
    notMovedPanicTreshold = getParamInt("agents", "notMovedPanicTreshold");


    floorNumber = getParamInt("environment", "floorNumber");

    // signs
    Engine::Point2D<int> sign;
    sign._x = getParamInt("signs/sign", "x");
    sign._y = getParamInt("signs/sign", "y");
    signList.push_back(sign);

    // doors
    Engine::Point2D<int> sign;
    door._x = getParamInt("doors/door", "x");
    door._y = getParamInt("doors/door", "y");
    doorList.push_back(door);    

    // exits
    Engine::Point2D<int> ext;
    ext._x = getParamInt("exits/exit", "x");
    ext._y = getParamInt("exits/exit", "y");
    exitconfiglist.push_back(ext);
}

} // namespace Evacuation


