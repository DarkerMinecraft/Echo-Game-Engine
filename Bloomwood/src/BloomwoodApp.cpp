#include <Core/Application.h>

#include "BloomwoodLayer.h"

namespace Bloomwood 
{

	class BloomwoodApp : public Echo::Application 
	{
	public:
		BloomwoodApp() : 
			Echo::Application("C:/Dev/Echo Projects/Bloomwood", "Bloomwood") 
		{
			PushLayer(new BloomwoodLayer());
		}

		~BloomwoodApp() {}
	};

}

Echo::Application* CreateApplication()
{
	return new Bloomwood::BloomwoodApp();
}