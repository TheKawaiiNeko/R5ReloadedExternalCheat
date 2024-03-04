#pragma once
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_impl_dx11.h"
#include "..\Utils\Config\Config.h"
#include <thread>
#include <dwmapi.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class Overlay
{
public:
	bool CreateOverlay();
	void OverlayLoop();
	void DestroyOverlay();
	void OverlayManager();
private:
	bool ShowMenu = false;

	// Overlay
	WNDCLASSEXA wc;
	HWND Hwnd;
	char ClassName[16] = "NULL";
	char TitleName[16] = "Overlay";
};

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;