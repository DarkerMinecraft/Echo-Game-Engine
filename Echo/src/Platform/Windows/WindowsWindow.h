#pragma once

#include "Echo/Core/Window.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>


namespace Echo 
{
	using namespace Microsoft::WRL;

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props, HINSTANCE hInst);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void* GetNativeWindow() const override;
	public:
		struct WindowData
		{
			const char* Title;
			unsigned int Width, Height;
			bool VSync;
		};
	private:
		void Init(const WindowProps& props, HINSTANCE hInst);
		void Shutdown();
	private:
		HWND m_Window;
		HINSTANCE m_HInst;
		RECT m_WindowRect; 
		WindowData m_Data;

		bool m_UseWarp = false;
		bool m_IsDirectXInitalized = false;

		ComPtr<ID3D12Device2> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<IDXGISwapChain4> m_SwapChain;
		ComPtr<ID3D12Resource> m_BackBuffers[3];
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocators[3];
		ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
	};

}