#include "EditorLayer.h"

#include <Core/EntryPoint.h>
#include <Core/Application.h>

namespace Echo
{

	class Editor : public Application 
	{
	public: 
		Editor()
			: Application("C:/Dev/Echo Projects/Testing", "Horizon")
		{
			PushLayer(new EditorLayer());
		}
		~Editor() 
		{
	
		}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}

}