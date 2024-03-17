#include "Overlay.h"

void LoadStyle();

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

bool Overlay::CreateOverlay()
{
    // Get ClientSize
    GHwnd = FindWindowA(nullptr, TargetTitle);
    GetClientRect(GHwnd, &cfg.GameRect);
    ClientToScreen(GHwnd, &cfg.GamePoint);

    // Create Overlay
    wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, Title, Class, NULL};
    RegisterClassExA(&wc);
    Hwnd = CreateWindowExA(NULL, wc.lpszClassName, wc.lpszMenuName, WS_POPUP | WS_VISIBLE, cfg.GamePoint.x, cfg.GamePoint.y, cfg.GameRect.right, cfg.GameRect.bottom, NULL, NULL, wc.hInstance, NULL);
    SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    SetLayeredWindowAttributes(Hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(Hwnd, &margin);

    if (!CreateDeviceD3D(Hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        
        return false;
    }

    ShowWindow(Hwnd, SW_SHOWDEFAULT);
    UpdateWindow(Hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    LoadStyle();

    ImGui_ImplWin32_Init(Hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

void Overlay::DestroyOverlay()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(Hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
}

// 占有スレッドで動かす
void Overlay::OverlayManager()
{
    while (cfg.Run)
    {
        // Window Check
        HWND CheckHwnd = FindWindowA(nullptr, TargetTitle);
        if (!CheckHwnd)
        {
            cfg.Run = false;
            break;
        }

        // StreamProof
        DWORD Flag = NULL;
        GetWindowDisplayAffinity(Hwnd, &Flag);
        if (cfg.StreamProof && Flag == WDA_NONE)
            SetWindowDisplayAffinity(Hwnd, WDA_EXCLUDEFROMCAPTURE);
        else if (!cfg.StreamProof && Flag == WDA_EXCLUDEFROMCAPTURE)
            SetWindowDisplayAffinity(Hwnd, WDA_NONE);

        // Window Style Changer
        static LONG MenuStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
        static LONG ESPStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW;
        LONG TmpLong = GetWindowLong(Hwnd, GWL_EXSTYLE);
        HWND ForegroundWindow = GetForegroundWindow();

        if (cfg.ShowMenu && MenuStyle != TmpLong)
        {
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);

            if (ForegroundWindow != Hwnd)
                SetForegroundWindow(Hwnd);
        }
        else if (!cfg.ShowMenu && ESPStyle != TmpLong)
        {
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

            if (ForegroundWindow != CheckHwnd)
                SetForegroundWindow(CheckHwnd);
        }

        // ShowMenu
        static bool menu_key = false;
        if (IsKeyDown(cfg.MenuKey) && !menu_key)
        {
            cfg.ShowMenu = !cfg.ShowMenu;
            menu_key = true;
        }
        else if (!IsKeyDown(cfg.MenuKey) && menu_key)
        {
            menu_key = false;
        }

        // Window Resizer
        RECT TmpRect = {};
        POINT TmpPoint = {};
        GetClientRect(CheckHwnd, &TmpRect);
        ClientToScreen(CheckHwnd, &TmpPoint);

        // Resizer
        if (TmpRect.left != cfg.GameRect.left || TmpRect.bottom != cfg.GameRect.bottom || TmpRect.top != cfg.GameRect.top || TmpRect.right != cfg.GameRect.right || TmpPoint.x != cfg.GamePoint.x || TmpPoint.y != cfg.GamePoint.y)
        {
            cfg.GameRect  = TmpRect;
            cfg.GamePoint = TmpPoint;
            SetWindowPos(Hwnd, nullptr, TmpPoint.x, TmpPoint.y, cfg.GameRect.right, cfg.GameRect.bottom, SWP_NOREDRAW);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void LoadStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Border
    style.WindowBorderSize  = 0.f;
    style.ChildBorderSize   = 1.f;
    style.PopupBorderSize   = 1.f;
    style.FrameBorderSize   = 0.f;
    style.TabBorderSize     = 1.f;
    style.TabBarBorderSize  = 0.f;

    // Rounding
    style.WindowRounding = 0.f;
    style.ChildRounding = 0.f;
    style.FrameRounding = 0.f;
    style.PopupRounding = 0.f;
    style.TabRounding = 0.f;

    // Misc
    style.GrabMinSize = 5.f;
    style.SeparatorTextBorderSize = 1.f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.DisplayWindowPadding = ImVec2(0.f, 0.f);       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    style.DisplaySafeAreaPadding = ImVec2(50.f, 50.f);     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.95f);
    colors[ImGuiCol_Border] = ImVec4(0.34f, 0.34f, 0.34f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.08f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.09f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.09f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.09f, 0.55f, 1.00f, 0.88f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.09f, 0.55f, 1.00f, 0.39f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
}