
#include "alphatrack.hpp"

#include <fstream>
#include <iostream>

int main()
{
	
	std::ifstream file;
	file.open("/dev/alphatrack0", std::ios::in|std::ios::binary|std::ios::ate);
	
	if ( !file.is_open() )
  {
		std::cout << "Warning, file not opened" << std::endl;
	}
	else
	{
		char* memblock;
		for (;;)
		{
			std::ifstream::pos_type size = file.tellg();
			
			if (size != 0)
			{
				memblock = new char [size];
				
				file.seekg (0, std::ios::beg);
				file.read (memblock, size);
			}
		}
	}
	
	
	Alphatrack alphatrack;
	
	alphatrack.connect("Lupppppppppp");
	
	for(;;)
	{
		
		alphatrack.read();
		
	}
	
	return 0;
	
	
}
