#pragma once

#include "Echo/Core/Base.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <limits>
#include <set>

#ifdef ECHO_PLATFORM_WIN
#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef __MINGW32__
#include <unknwn.h>
#endif

#include <wrl/client.h>

#ifdef USING_DIRECTX_HEADERS
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxguids/dxguids.h>
#else
#include <d3d12.h>

#include "d3dx12.h"
#endif

#include <dxgi1_4.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "GraphicsMemory.h"

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

#ifdef __MINGW32__
namespace Microsoft
{
	namespace WRL
	{
		namespace Wrappers
		{
			class Event
			{
			public:
				Event() noexcept : m_handle{} {}
				explicit Event(HANDLE h) noexcept : m_handle{ h } {}
				~Event() { if (m_handle) { ::CloseHandle(m_handle); m_handle = nullptr; } }

				void Attach(HANDLE h) noexcept
				{
					if (h != m_handle)
					{
						if (m_handle) ::CloseHandle(m_handle);
						m_handle = h;
					}
				}

				bool IsValid() const { return m_handle != nullptr; }
				HANDLE Get() const { return m_handle; }

			private:
				HANDLE m_handle;
			};
		}
	}
}
#else
#include <wrl/event.h>
#endif

#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#endif

#include "Echo/Core/Log.h"