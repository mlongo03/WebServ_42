#include "Location.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include "Worker.hpp"

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		try
		{
			ConfigParser configParser;
			std::vector<Server> servers = configParser.parseConfig(argv[1]);
			Worker worker = Worker(servers);
			worker.run();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
