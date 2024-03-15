// cl main.c /Zi /nologo

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

#define Assert(c) do { if (!(c)) __debugbreak(); } while (0)

#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")

typedef uint32_t U32;

void *memory;
U32 client_width;
U32 client_height;

void draw_pixel(U32 x, U32 y, U32 color) {
    U32 *pixel = (U32 *)memory;
    pixel += y * client_width + x;
    *pixel = color;
}

void clear_screen(U32 color) {
    U32 *pixel = (U32 *)memory;
    for(U32 i = 0; i < client_width * client_height; ++i) {
        *pixel++ = color;
    }
}

LRESULT CALLBACK win_proc(HWND window, UINT msg, 
                          WPARAM w_param, LPARAM l_param) {
    switch(msg) {
        case WM_KEYDOWN: {
            switch(w_param) {
                case 'O': { 
                    DestroyWindow(window); 
                } break;
            }
        } break;
        
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        
        default: {
            return DefWindowProcW(window, msg, w_param, l_param);
        }
    }
    
    return 0;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, 
                   PSTR cmd, int cmd_show) {
    
    // Window creation.
    
    WNDCLASSW wc = {
        .lpszClassName = L"MyWindowClass",
        .lpfnWndProc = win_proc,
        .hInstance = instance,
        .hCursor = LoadCursor(0, IDC_CROSS)
    };
    
    ATOM atom = RegisterClassW(&wc);
    Assert(atom && "Failed to register a window");
    
    HWND window = CreateWindowW(wc.lpszClassName, 
                                L"Drawing Pixels", 
                                WS_OVERLAPPEDWINDOW, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                NULL, NULL, instance, NULL);
    Assert(window && "Failed to create a window");
    
    ShowWindow(window, cmd_show);
    
    // Allocate memory.
    
    RECT rect;
    GetClientRect(window, &rect);
    client_width = rect.right - rect.left;
    client_height = rect.bottom - rect.top;
    
    memory = VirtualAlloc(0, 
                          client_width * client_height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );
    
    // Create BITMAPINFO struct for StretchDIBits.
    
    BITMAPINFO bitmap_info = {
        .bmiHeader.biSize = sizeof(bitmap_info.bmiHeader),
        .bmiHeader.biWidth = client_width,
        .bmiHeader.biHeight = client_height,
        .bmiHeader.biPlanes = 1,
        .bmiHeader.biBitCount = 32,
        .bmiHeader.biCompression = BI_RGB
    };
    
    HDC hdc = GetDC(window);
    
    for(;;) {
        MSG msg;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        } 
        
        clear_screen(0x333333);
        
        // Draw white pixel at 100, 100 (from bottom left).
        
        draw_pixel(100, 100, 0xffffff);
        
        StretchDIBits(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      client_width,
                      client_height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
    }
    
    return 0;
}
