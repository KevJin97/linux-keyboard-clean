#include <iostream>
#include <string>
#include <chrono>


#include "keyboard_lock.hpp"
#include "Device.hpp"

int main(int argc, char* argv[])
{
	std::chrono::seconds period;

	switch(argc)
	{
		default:
			// Too many input parameters. Using default.
		case 1:
			period = default_period;
			break;
		
		case 2:
			for (unsigned n = 0; argv[1][n] != '\0'; ++n)
			{
				if (argv[1][n] < '0' || argv[1][n] > '9')
				{
					// Not a number
					return -1;
				}
			}

			unsigned time_frame = std::stoul(argv[1]);
			if (time_frame > 120)
			{
				// Too big
				return -1;
			}
			else
			{
				period = std::chrono::seconds(time_frame);
			}
			break;
	}

	Device devices;
	
	std::cout << "Freezing for " << period << std::endl;

	devices.freeze(period);
	devices.wait_for_threads();

	return 0;
}