#include <Exception.hxx>
#include <iostream>

#include "EvacWorld.hxx"
#include "EvacConfig.hxx"
#include <cstdlib>
//#include <Config.hxx>

int main(int argc, char *argv[])
{
	try
	{	
                if (argc >2)
                {
                     throw Engine::Exception("Compile in the following way: Evacuation [config file]");
                }
                std::string fileName("config.xml");
                if (argc == 2)
                {
                     fileName = argv[1];
                }

		Evacuation::EvacWorld myWorld (new Evacuation::EvacConfig(fileName), myWorld.useOpenMPSingleNode());
		
		myWorld.initialize(argc, argv);
		myWorld.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}
