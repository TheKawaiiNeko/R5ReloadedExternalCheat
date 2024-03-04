#include "Overlay.h"

void LoadStyle();
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

void CleanupRenderTarget();
void CleanupDeviceD3D();
void CreateRenderTarget();
bool CreateDeviceD3D(HWND hWnd);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// オーバーレイの作成
bool Overlay::CreateOverlay()
{
	wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, TitleName, ClassName, NULL };
    RegisterClassExA(&wc);
    Hwnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, wc.lpszClassName, wc.lpszMenuName, WS_POPUP | WS_VISIBLE, 100, 100, 100, 100, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(Hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(Hwnd, &margin);

    if (!CreateDeviceD3D(Hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    ShowWindow(Hwnd, SW_SHOWDEFAULT);
    UpdateWindow(Hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.LogFilename = nullptr;
    io.IniFilename = nullptr;

    // ImGuiのスタイルをロードする
    LoadStyle();

    ImGui_ImplWin32_Init(Hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
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

void Overlay::OverlayManager()
{
    while (g.Run)
    {
        // ゲームが実行されているかをチェックする
        HWND CheckHwnd = FindWindowA(NULL, "Apex Legends");

        if (!CheckHwnd)
        {
            g.Run = false;
            break;
        }

        // Window Style Changer
        static LONG MenuStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
        static LONG ESPStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST;
        LONG TmpLong = GetWindowLong(Hwnd, GWL_EXSTYLE);
        HWND ForegroundWindow = GetForegroundWindow();

        if (ShowMenu && MenuStyle != TmpLong)
        {
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);

            if (ForegroundWindow != Hwnd)
                SetForegroundWindow(Hwnd);
        }
        else if (!ShowMenu && ESPStyle != TmpLong)
        {
            SetWindowLong(Hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST);

            if (ForegroundWindow != Hwnd)
                SetForegroundWindow(Hwnd);
        }

        // メニュー表示の切り替え (デフォルトではENDキー)
        static bool menu_key = false;
        if (IsKeyDown(g.MenuKey) && !menu_key)
        {
            ShowMenu = !ShowMenu;

            menu_key = true;
        }
        else if (!IsKeyDown(g.MenuKey) && menu_key)
        {
            menu_key = false;
        }

        // 対象のウィンドウのサイズや位置を取得/比較して、差異があればそれに合わせる
        RECT TmpRect = {};
        POINT TmpPoint = {};
        GetClientRect(CheckHwnd, &TmpRect);
        ClientToScreen(CheckHwnd, &TmpPoint);

        if (TmpRect.left != g.GameSize.left || TmpRect.bottom != g.GameSize.bottom || TmpRect.top != g.GameSize.top || TmpRect.right != g.GameSize.right || TmpPoint.x != g.GamePoint.x || TmpPoint.y != g.GamePoint.y)
        {
            g.GameSize = TmpRect;
            g.GamePoint = TmpPoint;

            SetWindowPos(Hwnd, nullptr, TmpPoint.x, TmpPoint.y, g.GameSize.right, g.GameSize.bottom, SWP_NOREDRAW);
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
    // Style
    ImGuiStyle& style = ImGui::GetStyle();
    // Window
    style.WindowPadding = ImVec2(8.f, 8.f);
    style.WindowRounding = 0.f;
    style.WindowBorderSize = 0.f;
    style.WindowMinSize = ImVec2(100.f, 100.f);

    // Child
    style.ChildRounding = 0.f;
    style.ChildBorderSize = 1.f; // BORDER

    // Popup
    style.PopupRounding = 0.f;
    style.PopupBorderSize = 1.f; // BORDER

    // Frame
    style.FramePadding;
    style.FrameRounding = 0.f;
    style.FrameBorderSize = 0.f;

    // Spacing
    style.ItemSpacing = ImVec2(8.f, 4.f);                // Horizontal and vertical spacing between widgets/lines.
    style.ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    style.CellPadding;                // Padding within a table cell. CellPadding.y may be altered between different rows.
    style.TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    style.IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    style.ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).

    style.ScrollbarSize = 10.f;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    style.ScrollbarRounding;          // Radius of grab corners for scrollbar.
    style.GrabMinSize = 5.f;                // Minimum width/height of a grab box for slider/scrollbar.
    style.GrabRounding = 0.f;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.

    style.LogSliderDeadzone;          // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
    style.TabRounding = 0.f;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    style.TabBorderSize = 1.f;              // Thickness of border around tabs.
    style.TabMinWidthForCloseButton;  // Minimum width for close button to appear on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.

    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    style.SelectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
    style.SeparatorTextBorderSize = 1.f;    // Thickkness of border in SeparatorText()
    style.SeparatorTextAlign;         // Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
    style.SeparatorTextPadding;       // Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    style.DisplayWindowPadding = ImVec2(0.f, 0.f);       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    style.DisplaySafeAreaPadding = ImVec2(100.f, 100.f);     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.5f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.5f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
}
