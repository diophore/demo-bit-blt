#include <Windows.h>

LRESULT CALLBACK
MessageCallback(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_PAINT)
	{
		PAINTSTRUCT paint_struct;
		HDC device_context = BeginPaint(window_handle, &paint_struct);
		int left = paint_struct.rcPaint.left;
		int top = paint_struct.rcPaint.top;
		int width = paint_struct.rcPaint.right - left;
		int height = paint_struct.rcPaint.bottom - top;
		BitBlt(device_context, left, top, width, height, NULL, 0, 0, BLACKNESS);
		EndPaint(window_handle, &paint_struct);
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
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
