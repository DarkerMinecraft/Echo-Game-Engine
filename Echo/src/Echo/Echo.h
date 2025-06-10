#pragma once

#include "Core/Application.h"
#include "Core/Log.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Core/Timestep.h"
#include "Core/Layer.h"
#include "Scene/ScriptableEntity.h"
#include "Scene/Components.h"

#include "Graphics/CommandList.h"
#include "Graphics/Primitives/Framebuffer.h"
#include "Graphics/Primitives/Texture.h"
#include "Graphics/Primitives/Buffer.h"
#include "Graphics/Primitives/Pipeline.h"
#include "Graphics/Primitives/Material.h"

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/Event.h"

#include "Graphics/NamedRenderer/Renderer2D.h"

#include "AssetManager/AssetRegistry.h"
#include "AssetManager/Assets/ShaderAsset.h"
#include "AssetManager/Assets/TextureAsset.h"

#include "Debug/Instrumentor.h"