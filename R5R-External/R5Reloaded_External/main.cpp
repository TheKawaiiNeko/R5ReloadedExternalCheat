#include "Cheat\Cheat.h"
#include "Overlay\Overlay.h"
#include "Utils\Memory\Memory.h"

Cheat   Ct = Cheat();
Overlay Ov = Overlay();

void Overlay::OverlayLoop()
{
    while (g.Run)
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

        Ct.RenderInfo();

        if (ShowMenu)
            Ct.RenderMenu();

        if (g.ESP)
            Ct.RenderESP();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);

    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // DPI
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    // Mmeory Init
    if (!m.Init())
        return 1;

    if (!Ov.CreateOverlay())
        return 2;

    // Create threads
    g.Run = true;
     std::thread([&]() { Ct.AimBot(); }).detach();
    std::thread([&]() { Ov.OverlayManager(); }).detach();
   
    // MainLoop
    Ov.OverlayLoop();
    
    Ov.DestroyOverlay();
    CloseHandle(m.pHandle);

    return 0;
}
