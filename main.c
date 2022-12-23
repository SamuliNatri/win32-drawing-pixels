#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

typedef uint32_t u32;
void* Memory;
int ClientWidth;
int ClientHeight;

void DrawPixel(int X, int Y, u32 Color) {
    u32 *Pixel = (u32 *)Memory;
    Pixel += Y * ClientWidth + X;
    *Pixel = Color;
}

void ClearScreen(u32 Color) {
    u32 *Pixel = (u32 *)Memory;
    for(int Index = 0; 
        Index < ClientWidth * ClientHeight; 
        ++Index) 
    {
        *Pixel++ = Color;
    }
}

LRESULT CALLBACK 
WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch(Message) {
        case WM_KEYDOWN: {
            switch(WParam) {
                case 'O': { 
                    DestroyWindow(Window); 
                } break;
            }
        } break;
        
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        
        default: {
            return DefWindowProc(Window, Message, WParam, LParam);
        }
    }
    
    return 0;
}

int WINAPI 
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, int CmdShow) {
    
    // Window creation
    
    WNDCLASS WindowClass = {0};
    
    const char ClassName[] = "MyWindowClass";
    
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = ClassName;
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClass(&WindowClass)) {
        MessageBox(0, "RegisterClass failed", 0, 0);
        return GetLastError();
    }
    
    HWND Window = CreateWindowEx(0,
                                 ClassName,
                                 "Window",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0,
                                 0,
                                 Instance,
                                 0);
    
    if(!Window) {
        MessageBox(0, "CreateWindowEx failed", 0, 0);
        return GetLastError();
    }
    
    // Allocate memory
    
    RECT Rect;
    GetClientRect(Window, &Rect);
    ClientWidth = Rect.right - Rect.left;
    ClientHeight = Rect.bottom - Rect.top;
    
    Memory = VirtualAlloc(0, 
                          ClientWidth * ClientHeight * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );
    
    // Create BITMAPINFO struct for StretchDIBits
    
    BITMAPINFO BitmapInfo;
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = ClientWidth;
    BitmapInfo.bmiHeader.biHeight = ClientHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    HDC HDC = GetDC(Window);
    
    int Running = 1;
    
    while(Running) {
        MSG Message;
        while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            if(Message.message == WM_QUIT) {
                Running = 0;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
        // Clear screen
        
        ClearScreen(0x333333);
        
        // Draw white pixel at 100, 100 (from bottom left)
        
        DrawPixel(100, 100, 0xffffff);
        
        StretchDIBits(HDC,
                      0,
                      0,
                      ClientWidth,
                      ClientHeight,
                      0,
                      0,
                      ClientWidth,
                      ClientHeight,
                      Memory,
                      &BitmapInfo,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
    }
    
    return 0;
}
