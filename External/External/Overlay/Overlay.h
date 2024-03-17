#pragma once
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_impl_dx11.h"
#include "..\Utils\Config\Config.h"
#include <dwmapi.h>
#include <thread>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class Overlay
{
private:
	// OverlayWindow
	WNDCLASSEXA wc;
	HWND Hwnd;
	char Title[16] = "Overlay";
	char Class[16] = "WND";
	char TargetTitle[64] = "Apex Legends";
public:
	HWND GHwnd;

	bool CreateOverlay();
	void OverlayLoop();
	void DestroyOverlay();
	void OverlayManager();
};

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

extern void CleanupDeviceD3D();
extern void CreateRenderTarget();
extern void CleanupRenderTarget();
extern bool CreateDeviceD3D(HWND hWnd);
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);