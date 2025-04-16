#include <Core/Application.h>
#include <Core/EntryPoint.h>

#include "BloomwoodLayer.h"

namespace Bloomwood 
{

	class BloomwoodApp : public Echo::Application
	{
	public:
		BloomwoodApp()
			: Echo::Application(1280, 720, "Bloomwood")
		{
			PushLayer(new BloomwoodLayer());
		}

		~BloomwoodApp()
		{}
	};
}


Echo::Application* Echo::CreateApplication()
{
	return new Bloomwood::BloomwoodApp();
}


