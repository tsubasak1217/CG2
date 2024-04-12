#include <windows.h>
#include <cstdint>

LRESULT CALLBACK WndProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE,
    LPSTR, int nCmdShow) {

    TCHAR szAppName[] = L"test";
    WNDCLASS wc{};
    HWND hwnd;
    MSG msg{};

    // クライアント領域の決定
    const int32_t kCliantWidth = 1280;
    const int32_t kCliantHeight = 720;
    RECT rect = { 0,0,kCliantWidth,kCliantHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    // ウインドウクラスの登録
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = szAppName;

    if (!RegisterClass(&wc)) { return 0; }

    // ウインドウの生成
    hwnd = CreateWindow(
        szAppName, L"CG2",
        WS_OVERLAPPEDWINDOW,
        //ウィンドウ左上座標
        CW_USEDEFAULT, CW_USEDEFAULT,
        //ウィンドウ幅と高さ
        rect.right - rect.left,
        rect.bottom - rect.top,
        //
        nullptr,
        nullptr,
        hInstance, 
        nullptr
    );

    if (!hwnd) { return 0; }
    
    // ウインドウの表示
    ShowWindow(hwnd, nCmdShow);
    // ウインドウの更新
    UpdateWindow(hwnd);

    while (msg.message != WM_QUIT) {

        // ウインドウにメッセージがある場合、優先して処理する
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        } else {
            // ゲームの処理
        }
    }
    //a
    return 0;
}

LRESULT CALLBACK WndProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        // 標準のメッセージ処理を行う
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}