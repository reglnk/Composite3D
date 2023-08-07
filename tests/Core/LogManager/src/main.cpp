#include <cm3d/Core/LogManager.hpp>
#include <cm3d/Utility/SysBasic.hpp>

#include <thread>
#include <mutex>
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace cm3d;

static const char *threadNames[16] = {
	"lorem",
	"ipsum",
	"dolor",
	"varius",
	"velit",
	"ornare",
	"massa",
	"dictum",
	"tellus",
	"porta",
	"egestas",
	"vitae",
	"suspicit",
	"libero",
	"integer",
	"sapien"
};

int main()
{
	LogManager *mgr = new LogManager("latest.log");

	std::thread threads[16];
	for (int i = 0; i < 16; ++i)
	{
		threads[i] = std::thread([i](LogManager *m){
			for (int j = 0; j < 100; ++j)
			{
				char buf[256];
				snprintf(buf, 256, "%s: Iteration #%d\n", threadNames[i], j);
				
				// Add more asynchrony
				System::sleepms(10 + i);
				m->push(buf);
			}
		}, mgr);
	}

	mgr->push("main:\nHello World! #1\n");
	mgr->push("main:\nHello World! #2\n");

	for (int i = 0; i < 16; ++i)
		threads[i].join();
	
	mgr->join();
	puts("JOINED");
	System::sleepms(800);
	return mgr->collectErrors();
}
