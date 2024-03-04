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
    std::thread([&]() { Ov.OverlayManager(); }).detach();
    std::thread([&]() { Ct.AimBot(); }).detach();

    // MainLoop
    Ov.OverlayLoop();
    
    Ov.DestroyOverlay();
    CloseHandle(m.pHandle);

    return 0;
}

/*
// KeyBinder
void KeyBinder()
{
    while (Run)
    {
        // KeyBinder
        while (g.KeyBind)
        {
            while (true)
            {
                g.BindFlag = true;

                for (int i = 0; i < 0x87; i++)
                {
                    if (IsKeyDown(i))
                    {
                        // Escape - 割り当て解除
                        if (i == 27 || i == 35)
                        {
                            switch (g.BindID)
                            {
                            case 1:
                                g.AimKey0 = 0;
                                break;
                            case 2:
                                g.AimKey1 = 0;
                                break;
                            case 3:
                                g.FakeDuckKey = 0;
                                break;
                            default:
                                break;
                            }

                            g.BindID = 0;
                            g.BindFlag = false;
                            g.KeyBind = false;
                            break;
                        }
                        else
                        {
                            if (g.FakeDuckKey == 1 || g.FakeDuckKey == 2)
                                g.FakeDuckKey = 0;

                            switch (g.BindID)
                            {
                            case 1:
                                g.AimKey0 = i;
                                break;
                            case 2:
                                g.AimKey1 = i;
                                break;
                            case 3:
                                g.FakeDuckKey = i;
                                break;
                            default:
                                break;
                            }

                            g.BindID = 0;
                            g.BindFlag = false;
                            g.KeyBind = false;
                            break;
                        }
                    }
                }

                if (!g.BindFlag)
                    break;
            }
        }

        // Key Check
        if (g.AimKey0 == g.AimKey1)
            g.AimKey1 = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
*/