#include <Echo.h>

#include "EditorLayer.h"

namespace Echo
{

	class Editor : public Application 
	{
	public: 
		Editor()
		{
			PushLayer(new EditorLayer());
		}
		~Editor() {}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}

}