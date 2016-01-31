#include <Windows.h>

HBITMAP bitmap_handle;
void *buffer;
RECT client_rect;

int red = 0xFF;
int green = 0x00;
int blue = 0x00;

void
next_rainbow_hue()
{
	// start at max red  FF 00 00
	if (red == 0xFF && green < 0xFF && blue == 0x00)
	{
		green += 0x01;  // animate to max green:  FF 00+ 00
	}
	else if (red > 0x00 && green == 0xFF && blue == 0x00)
	{
		red -= 0x01;  // animate to zero red:  FF- FF 00
	}
	else if (red == 0x00 && green == 0xFF && blue < 0xFF)
	{
		blue += 0x01;  // animate to max blue:  00 FF 00+
	}
	else if (red == 0x00 && green > 0x00 && blue == 0xFF)
	{
		green -= 0x01;  // animate to zero green:  00 FF- FF
	}
	else if (red < 0xFF && green == 0x00 && blue == 0xFF)
	{
		red += 0x01;  // animate to max red:  00+ 00 FF
	}
	else if (red == 0xFF && green == 0x00 && blue > 0x00)
	{
		blue -= 0x01;  // animate to zero blue:  FF 00 FF-
	}
}

void
render(void *buffer, RECT rect)
{
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int pixels = width * height;

	unsigned long *pixel = (unsigned long *)buffer;
	unsigned long color = (red << 16) | (green << 8) | blue;

	for (int i = 0; i < pixels; i++, pixel++)
	{
		*pixel = color;
	}
}

void
paint(HDC window_device_context, RECT rect)
{
	HDC memory_device_context = CreateCompatibleDC(window_device_context);
	HGDIOBJ replaced_gdi_object = SelectObject(memory_device_context, bitmap_handle);

	BitBlt(
		window_device_context,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		memory_device_context,
		0,
		0,
		SRCCOPY);

	SelectObject(memory_device_context, replaced_gdi_object);
	DeleteObject(memory_device_context);
}

LRESULT CALLBACK
MessageCallback(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_PAINT)
	{
		PAINTSTRUCT paint_struct;
		HDC window_device_context = BeginPaint(window_handle, &paint_struct);
		paint(window_device_context, client_rect);
		EndPaint(window_handle, &paint_struct);
	}
	else if (message == WM_SIZE)
	{
		if (bitmap_handle)
		{
			DeleteObject(bitmap_handle);
		}

		GetClientRect(window_handle, &client_rect);

		BITMAPINFO bitmap_info;
		memset(&bitmap_info, 0, sizeof(bitmap_info));
		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = client_rect.right - client_rect.left;
		bitmap_info.bmiHeader.biHeight = -(client_rect.bottom - client_rect.top);  // negative height for a top-down DIB
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		bitmap_handle = CreateDIBSection(NULL, &bitmap_info, DIB_RGB_COLORS, &buffer, NULL, 0);

		render(buffer, client_rect);
	}
	else if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else
	{
		result = DefWindowProc(window_handle, message, wParam, lParam);
	}

	return result;
}

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS window_class;
	memset(&window_class, 0, sizeof(window_class));
	window_class.lpfnWndProc = MessageCallback;
	window_class.hInstance = hInstance;
	window_class.hCursor = LoadCursor(hInstance, IDC_ARROW);
	window_class.lpszClassName = L"DemoBitBltWindowClass";

	if (!RegisterClass(&window_class)) { return (int) GetLastError(); }

	HWND window_handle = CreateWindow(
		window_class.lpszClassName,
		L"DemoBitBlt",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	MSG msg;
	BOOL running = TRUE;

	while (running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) { running = FALSE; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		next_rainbow_hue();
		render(buffer, client_rect);
		HDC window_device_context = GetDC(window_handle);
		paint(window_device_context, client_rect);
		ReleaseDC(window_handle, window_device_context);
	}

	return msg.wParam;
}
