#include "pch.h"
#include "Echo/Utils/PlatformUtils.h"
#include "Echo/Core/Application.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlobj.h>
#include <propkey.h>
#include <atlbase.h>
#include <vector>
#include <memory>

namespace Echo
{
	// Helper to convert between UTF-8 and wide strings
	static std::wstring Utf8ToWide(const char* utf8)
	{
		if (!utf8) return L"";
		int size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
		if (size <= 0) return L"";

		std::wstring result(size - 1, 0); // -1 to skip null terminator in size
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &result[0], size);
		return result;
	}

	static std::string WideToUtf8(const WCHAR* wide)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
		if (size <= 0) return "";

		std::string result(size - 1, 0); // -1 to skip null terminator in size
		WideCharToMultiByte(CP_UTF8, 0, wide, -1, &result[0], size, nullptr, nullptr);
		return result;
	}

	// Filter data class to own filter strings throughout dialog lifetime
	class DialogFilterData
	{
	public:
		std::vector<std::wstring> wideStrings;
		std::vector<COMDLG_FILTERSPEC> filterSpecs;

		// Parse filter string (format: "Description\0*.ext\0Description\0*.ext\0\0")
		void ParseFilter(const char* filter)
		{
// Clear any previous data
			wideStrings.clear();
			filterSpecs.clear();

			if (!filter || !filter[0])
			{
// Default filter if none provided
				wideStrings.push_back(L"All Files");
				wideStrings.push_back(L"*.*");
				filterSpecs.push_back({ wideStrings[0].c_str(), wideStrings[1].c_str() });
				return;
			}

			// Filter format is pairs of null-terminated strings
			const char* ptr = filter;

			while (*ptr)
			{
				std::string name = ptr;
				ptr += name.length() + 1; // Skip to next string after null

				if (!*ptr) break; // Unexpected end

				std::string pattern = ptr;
				ptr += pattern.length() + 1; // Skip to next pair

				// Convert strings to wide
				wideStrings.push_back(Utf8ToWide(name.c_str()));
				wideStrings.push_back(Utf8ToWide(pattern.c_str()));

				// Add to filter specs
				size_t idx = wideStrings.size() - 2;
				filterSpecs.push_back({ wideStrings[idx].c_str(), wideStrings[idx + 1].c_str() });
			}

			// If no filters were parsed successfully, add a default
			if (filterSpecs.empty())
			{
				wideStrings.push_back(L"All Files");
				wideStrings.push_back(L"*.*");
				filterSpecs.push_back({ wideStrings[0].c_str(), wideStrings[1].c_str() });
			}
		}
	};

	std::string FileDialogs::OpenFile(const char* filter)
	{
		std::string result;

		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr)) return result;

		// Create a scope to ensure all COM objects are released before CoUninitialize
		{
			// Parse filter string
			auto filterData = std::make_unique<DialogFilterData>();
			filterData->ParseFilter(filter);

			// Get parent window handle
			HWND hwndOwner = (HWND)Application::Get().GetWindow().GetNativeWindow();

			// Set up shell COM object
			CComPtr<IFileOpenDialog> pFileOpen;
			hr = pFileOpen.CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL);
			if (SUCCEEDED(hr))
			{
				// Set options
				FILEOPENDIALOGOPTIONS options;
				hr = pFileOpen->GetOptions(&options);
				if (SUCCEEDED(hr))
				{
					hr = pFileOpen->SetOptions(options | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
				}

				// Set filters
				if (!filterData->filterSpecs.empty())
				{
					pFileOpen->SetFileTypes(static_cast<UINT>(filterData->filterSpecs.size()),
											filterData->filterSpecs.data());
				}

				// Show the dialog
				hr = pFileOpen->Show(hwndOwner);
				if (SUCCEEDED(hr))
				{
					CComPtr<IShellItem> pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						LPWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr))
						{
							// Convert and store result
							result = WideToUtf8(pszFilePath);
							CoTaskMemFree(pszFilePath);
						}
					}
				}
			}
		} // All COM objects released here

		CoUninitialize();
		return result;
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		std::string result;

		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr)) return result;

		// Create a scope to ensure all COM objects are released before CoUninitialize
		{
			// Parse filter string
			auto filterData = std::make_unique<DialogFilterData>();
			filterData->ParseFilter(filter);

			// Get parent window handle
			HWND hwndOwner = (HWND)Application::Get().GetWindow().GetNativeWindow();

			// Set up shell COM object
			CComPtr<IFileSaveDialog> pFileSave;
			hr = pFileSave.CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL);
			if (SUCCEEDED(hr))
			{
				// Set options
				FILEOPENDIALOGOPTIONS options;
				hr = pFileSave->GetOptions(&options);
				if (SUCCEEDED(hr))
				{
					hr = pFileSave->SetOptions(options | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT);
				}

				// Set filters
				if (!filterData->filterSpecs.empty())
				{
					pFileSave->SetFileTypes(static_cast<UINT>(filterData->filterSpecs.size()),
											filterData->filterSpecs.data());

					 // Set default extension based on first filter
					if (!filterData->filterSpecs.empty())
					{
						std::wstring extSpec = filterData->filterSpecs[0].pszSpec;
						size_t starPos = extSpec.find(L"*.");
						if (starPos != std::wstring::npos)
						{
							std::wstring ext = extSpec.substr(starPos + 2);
							// Handle multiple extensions like "*.jpg;*.jpeg"
							size_t semicolonPos = ext.find(L';');
							if (semicolonPos != std::wstring::npos)
							{
								ext = ext.substr(0, semicolonPos);
							}
							pFileSave->SetDefaultExtension(ext.c_str());
						}
					}
				}

				// Show the dialog
				hr = pFileSave->Show(hwndOwner);
				if (SUCCEEDED(hr))
				{
					CComPtr<IShellItem> pItem;
					hr = pFileSave->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						LPWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr))
						{
							// Convert and store result
							result = WideToUtf8(pszFilePath);
							CoTaskMemFree(pszFilePath);
						}
					}
				}
			}
		} // All COM objects released here

		CoUninitialize();
		return result;
	}
}