#pragma once
#include <rendell/rendell.h>

class RenderingServer final
{
private:
	RenderingServer();
public:
	~RenderingServer() = default;

	static void init();
	static void release();
	static RenderingServer* getInstance();
};