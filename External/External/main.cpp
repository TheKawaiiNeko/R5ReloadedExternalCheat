#include "Overlay\Overlay.h"
#include "Cheat\Cheat.h"
#include "Utils\Memory\Memory.h"

// EntryPoint : main.cpp line 52(debug)/54(release)

Cheat CWare;
Overlay Ov;

void Overlay::OverlayLoop()
{
	while (cfg.Run)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == GHwnd)
		{
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		CWare.RenderInfo();

		if (cfg.ShowMenu)
			CWare.RenderMenu();
			
		if (cfg.PlayerESP)
			CWare.RenderESP();

		ImGui::Render();
		const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}
}

// Debug時のみコンソールを表示
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	// Fix DPI Scale
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	// Memory
	if (!m.Init())
		return 1;

	// Overlay
	if (!Ov.CreateOverlay())
		return 2;

	// Cheat
	if (!CWare.Init())
		return 3;

	cfg.Run = true;
	std::thread([&]() {CWare.UpdateList(CWare.entitylist); }).detach();
	std::thread([&]() {CWare.AimBot(); }).detach();
	std::thread([&]() {CWare.Misc(); }).detach();
	std::thread([&]() {CWare.RainbowGenerator(); }).detach();
	std::thread([&]() {Ov.OverlayManager(); }).detach();

	Ov.OverlayLoop();
	Ov.DestroyOverlay();
	CloseHandle(m.pHandle);

	return 0;
}