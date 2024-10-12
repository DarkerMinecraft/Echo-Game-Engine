#include "pch.h"
#include "DirectX12GraphicsContext.h"

namespace Echo
{

	DirectX12GraphicsContext::DirectX12GraphicsContext(HWND window, unsigned int width, unsigned int height)
		: m_Window(window), m_Width(width), m_Height(height)
	{
		EC_CORE_ASSERT(window, "Window is null!")
	}

	void DirectX12GraphicsContext::Init()
	{
		CreateDevice();
		CreateResources();
	}

	void DirectX12GraphicsContext::SwapBuffers()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_FenceValues[m_BackBufferIndex];
		DX::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), currentFenceValue));

		// Update the back buffer index.
		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_Fence->GetCompletedValue() < m_FenceValues[m_BackBufferIndex])
		{
			DX::ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValues[m_BackBufferIndex], m_FenceEvent.Get()));
			std::ignore = WaitForSingleObjectEx(m_FenceEvent.Get(), INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_FenceValues[m_BackBufferIndex] = currentFenceValue + 1;
	}

	void DirectX12GraphicsContext::GetAdapter(IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_DXGIFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf()); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			DX::ThrowIfFailed(adapter->GetDesc1(&desc));

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_FeatureLevel, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

#if !defined(NDEBUG)
		if (!adapter)
		{
			if (FAILED(m_DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
			{
				throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
			}
		}
#endif

		if (!adapter)
		{
			throw std::runtime_error("No Direct3D 12 device found");
		}

		*ppAdapter = adapter.Detach();
	}

	void DirectX12GraphicsContext::CreateDevice()
	{
		DWORD dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	//
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
			{
				debugController->EnableDebugLayer();
			}

#ifndef __MINGW32__
			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
			}
#endif // __MINGW32__
		}
#endif

		DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_DXGIFactory.ReleaseAndGetAddressOf())));

		ComPtr<IDXGIAdapter1> adapter;
		GetAdapter(adapter.GetAddressOf());

		// Create the DX12 API device object.
		DX::ThrowIfFailed(D3D12CreateDevice(
			adapter.Get(),
			m_FeatureLevel,
			IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf())
		));

#ifndef NDEBUG
	// Configure debug device (if active).
		ComPtr<ID3D12InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(m_Device.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D12_MESSAGE_ID hide[] =
			{
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				// Workarounds for debug layer issues on hybrid-graphics systems
				D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
			};
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
#endif

	// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		DX::ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.ReleaseAndGetAddressOf())));

		// Create descriptor heaps for render target views and depth stencil views.
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
		rtvDescriptorHeapDesc.NumDescriptors = m_SwapBufferCount;
		rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
		dsvDescriptorHeapDesc.NumDescriptors = 1;
		dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_RTVDescriptorHeap.ReleaseAndGetAddressOf())));
		DX::ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_DSVDescriptorHeap.ReleaseAndGetAddressOf())));

		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create a command allocator for each back buffer that will be rendered to.
		for (UINT n = 0; n < m_SwapBufferCount; n++)
		{
			DX::ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocators[n].ReleaseAndGetAddressOf())));
		}

		// Create a command list for recording graphics commands.
		DX::ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(m_CommandList.ReleaseAndGetAddressOf())));
		DX::ThrowIfFailed(m_CommandList->Close());

		// Create a fence for tracking GPU execution progress.
		DX::ThrowIfFailed(m_Device->CreateFence(m_FenceValues[m_BackBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())));
		m_FenceValues[m_BackBufferIndex]++;

		m_FenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
		if (!m_FenceEvent.IsValid())
		{
			throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");
		}

		// Check Shader Model 6 support
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
			|| (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
		{
#ifdef _DEBUG
			OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
			throw std::runtime_error("Shader Model 6.0 is not supported!");
		}

		// If using the DirectX Tool Kit for DX12, uncomment this line:
#ifdef ECHO_PLATFORM_XBOX
		m_GraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(m_Device.Get());
#endif

		// TODO: Initialize device dependent objects here (independent of window size).
	}

	void DirectX12GraphicsContext::CreateResources()
	{
		// Wait until all previous GPU work is complete.
		WaitForGPU();

		// Release resources that are tied to the swap chain and update fence values.
		for (UINT n = 0; n < m_SwapBufferCount; n++)
		{
			m_RenderTargets[n].Reset();
			m_FenceValues[n] = m_FenceValues[m_BackBufferIndex];
		}

		constexpr DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		constexpr DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
		const UINT backBufferWidth = static_cast<UINT>(m_Width);
		const UINT backBufferHeight = static_cast<UINT>(m_Height);

		// If the swap chain already exists, resize it, otherwise create one.
		if (m_SwapChain)
		{
			HRESULT hr = m_SwapChain->ResizeBuffers(m_SwapBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				OnDeviceLost();

				// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method
				// and correctly set up the new device.
				return;
			}
			else
			{
				DX::ThrowIfFailed(hr);
			}
		}
		else
		{
			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = backBufferFormat;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = m_SwapBufferCount;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
			fsSwapChainDesc.Windowed = TRUE;

			// Create a swap chain for the window.
			ComPtr<IDXGISwapChain1> swapChain;
			DX::ThrowIfFailed(m_DXGIFactory->CreateSwapChainForHwnd(
				m_CommandQueue.Get(),
				m_Window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				swapChain.GetAddressOf()
			));

			DX::ThrowIfFailed(swapChain.As(&m_SwapChain));

			// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut
			DX::ThrowIfFailed(m_DXGIFactory->MakeWindowAssociation(m_Window, DXGI_MWA_NO_ALT_ENTER));
		}

		// Obtain the back buffers for this window which will be the final render targets
		// and create render target views for each of them.
		for (UINT n = 0; n < m_SwapBufferCount; n++)
		{
			DX::ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(m_RenderTargets[n].GetAddressOf())));

			wchar_t name[25] = {};
			swprintf_s(name, L"Render target %u", n);
			m_RenderTargets[n]->SetName(name);

#ifdef __MINGW32__
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
			std::ignore = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandle);
#else
			auto cpuHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#endif

			const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(cpuHandle, static_cast<INT>(n), m_RTVDescriptorSize);
			m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvDescriptor);
		}

		// Reset the index to the current back buffer.
		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
		// on this surface.
		const CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			depthBufferFormat,
			backBufferWidth,
			backBufferHeight,
			1, // Use a single array entry.
			1  // Use a single mipmap level.
		);
		depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		const CD3DX12_CLEAR_VALUE depthOptimizedClearValue(depthBufferFormat, 1.0f, 0u);

		DX::ThrowIfFailed(m_Device->CreateCommittedResource(
			&depthHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(m_DepthStencil.ReleaseAndGetAddressOf())
		));

		m_DepthStencil->SetName(L"Depth stencil");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthBufferFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

#ifdef __MINGW32__
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		std::ignore = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandle);
#else
		auto cpuHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#endif

		m_Device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, cpuHandle);
	}

	void DirectX12GraphicsContext::WaitForGPU()
	{
		if (m_CommandQueue && m_Fence && m_FenceEvent.IsValid())
		{
			// Schedule a Signal command in the GPU queue.
			UINT64 fenceValue = m_FenceValues[m_BackBufferIndex];
			if (SUCCEEDED(m_CommandQueue->Signal(m_Fence.Get(), fenceValue)))
			{
				// Wait until the Signal has been processed.
				if (SUCCEEDED(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent.Get())))
				{
					std::ignore = WaitForSingleObjectEx(m_FenceEvent.Get(), INFINITE, FALSE);

					// Increment the fence value for the current frame.
					m_FenceValues[m_BackBufferIndex]++;
				}
			}
		}
	}

	void DirectX12GraphicsContext::OnDeviceLost()
	{
		for (UINT n = 0; n < m_SwapBufferCount; n++)
		{
			m_CommandAllocators[n].Reset();
			m_RenderTargets[n].Reset();
		}

		m_DepthStencil.Reset();
		m_Fence.Reset();
		m_CommandList.Reset();
		m_SwapChain.Reset();
		m_RTVDescriptorHeap.Reset();
		m_DSVDescriptorHeap.Reset();
		m_CommandQueue.Reset();
		m_Device.Reset();
		m_DXGIFactory.Reset();

		// If using the DirectX Tool Kit for DX12, uncomment this line:
#ifdef ECHO_PLATFORM_XBOX
		m_GraphicsMemory.reset();
#endif

		CreateDevice();
		CreateResources();
	}

}
