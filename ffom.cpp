// ffom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Painter.h"



int main()
{
	try 
	{
		Painter painter;
		painter.loop();
	}
	catch (std::string log) {
		fprintf(stderr, "%s\n", log.c_str());
	}

    return 0;
}

