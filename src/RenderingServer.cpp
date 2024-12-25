#include "RenderingServer.h"
#include <cassert>

namespace rendell_text
{
	static RenderingServer* s_renderingServer = nullptr;

	RenderingServer::RenderingServer()
	{

	}

	void RenderingServer::init()
	{
		assert(!s_renderingServer);
		s_renderingServer = new RenderingServer();
	}

	void RenderingServer::release()
	{
		assert(s_renderingServer);
		delete s_renderingServer;
		s_renderingServer = nullptr;
	}

	RenderingServer* RenderingServer::getInstance()
	{
		assert(s_renderingServer);
		return s_renderingServer;
	}
}
