#include "EditorLayer.h"

#include <Echo.h>

namespace Echo
{

	class Editor : public Application 
	{
	public: 
		Editor(unsigned int width, unsigned int height, const char* title)
			: Application(width, height, title)
		{
			PushLayer(new EditorLayer());
		}
		~Editor() 
		{
	
		}
	};

	Application* CreateApplication()
	{
		return new Editor(
			1280,
			720,
			"Echo Editor"
		);
	}

}