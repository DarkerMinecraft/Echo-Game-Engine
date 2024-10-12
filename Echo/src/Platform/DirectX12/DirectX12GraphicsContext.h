#pragma once

#include "Echo/Graphics/GraphicsContext.h"


namespace Echo
{
	using Microsoft::WRL::ComPtr;

	class DirectX12GraphicsContext : public GraphicsContext
	{
	public:
		DirectX12GraphicsContext(HWND window, unsigned int width, unsigned int height);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		void GetAdapter(IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateResources();

		void WaitForGPU();
		void OnDeviceLost();
	private:
		HWND m_Window;
		unsigned int m_Width, m_Height;

		D3D_FEATURE_LEVEL m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

		static const UINT m_SwapBufferCount = 2;
		UINT m_BackBufferIndex;
		UINT m_RTVDescriptorSize;

		ComPtr<ID3D12Device2>				m_Device;
		ComPtr<IDXGIFactory4>				m_DXGIFactory;
		ComPtr<ID3D12CommandQueue>          m_CommandQueue;
		ComPtr<ID3D12DescriptorHeap>        m_RTVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap>        m_DSVDescriptorHeap;
		ComPtr<ID3D12CommandAllocator>      m_CommandAllocators[m_SwapBufferCount];
		ComPtr<ID3D12GraphicsCommandList>   m_CommandList;
		ComPtr<ID3D12Fence>                 m_Fence;
		UINT64                              m_FenceValues[m_SwapBufferCount];
		Microsoft::WRL::Wrappers::Event     m_FenceEvent;

#ifdef ECHO_PLATFORM_XBOX
		Scope<DirectX::GraphicsMemory> m_GraphicsMemory;
#endif

		ComPtr<IDXGISwapChain3>             m_SwapChain;
		ComPtr<ID3D12Resource>              m_RenderTargets[m_SwapBufferCount];
		ComPtr<ID3D12Resource>              m_DepthStencil;
	};
}
