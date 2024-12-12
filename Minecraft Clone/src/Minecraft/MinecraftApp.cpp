#include "Echo.h"

#include "MinecraftLayer.h"

namespace Minecraft
{

	class MinecraftApp : public Echo::Application 
	{
	public:
		MinecraftApp()
			: Echo::Application(1280, 720, "Minecraft")
		{
			PushLayer(new MinecraftLayer());
		}
	};

}

Echo::Application* Echo::CreateApplication()
{
	return new Minecraft::MinecraftApp();
}
