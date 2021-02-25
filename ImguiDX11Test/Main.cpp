#include "Main.h"

bool JattoImGui::CreateDevice(HWND hWindow) {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width  = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWindow;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		&featureLevel,
		&pDeviceContext
		) != S_OK) return false;

	CreateRenderTarget();
	return true;
}

void JattoImGui::CleanupDevice() {
	CleanupRenderTarget();
	if (pSwapChain)
	{
		pSwapChain->Release();
		pSwapChain = NULL;
	}
	if (pDeviceContext)
	{
		pDeviceContext->Release();
		pDeviceContext = NULL;
	}
	if (pDevice)
	{
		pDevice->Release();
		pDevice = NULL;
	}
}

void JattoImGui::CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();
}

void JattoImGui::CleanupRenderTarget() {
	if (pRenderTargetView)
	{
		pRenderTargetView->Release();
		pRenderTargetView = NULL;
	}
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT JattoImGui::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg) {
		case WM_SIZE:
			if (pDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool JattoImGui::GetDesktopSize(int& x, int& y) {
	RECT rect;
	HWND hDesktop = GetDesktopWindow();
	if (hDesktop == INVALID_HANDLE_VALUE || hDesktop == NULL) return false;
	else
	{
		if (GetWindowRect(hDesktop, &rect))
		{
			x = rect.right; y = rect.bottom;
			return true;
		}
		else { return false; }
	}
}

int main(int, char**) {
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = JattoImGui::WndProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = L"JattoImGui";
	RegisterClassEx(&wc);

	int x = 0 ; int y = 0;
	if (!JattoImGui::GetDesktopSize(x, y))
	{
		std::cout << "Failed to get desktop solution.\n";
		x = 1920; y = 1080;
	}

	JattoImGui::hWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, wc.lpszClassName, L"JattoImGui", WS_POPUP, 0, 0, x, y, NULL, NULL, wc.hInstance, NULL);

	SetLayeredWindowAttributes(JattoImGui::hWindow, 0, 255, ULW_COLORKEY | LWA_ALPHA);

	if (JattoImGui::hWindow == NULL)
	{
		std::cout << "hWindow Object is Null.\n";
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	if (!JattoImGui::CreateDevice(JattoImGui::hWindow))
	{
		std::cout << "CreateDevice() Failed.\n";
		JattoImGui::CleanupDevice();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	ShowWindow(JattoImGui::hWindow, SW_SHOW);
	UpdateWindow(JattoImGui::hWindow);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();

	if (!ImGui_ImplWin32_Init(JattoImGui::hWindow))
	{
		std::cout << "ImGui_ImplWin32_Init () Failed.\n";
		ImGui::DestroyContext();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	if (!ImGui_ImplDX11_Init(JattoImGui::pDevice, JattoImGui::pDeviceContext))
	{
		std::cout << "ImGui_ImplWin32_Init () Failed.\n";
		ImGui::DestroyContext();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	io.IniFilename = NULL;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	float clear_color[4]  = { 0.45f, 0.55f, 0.60f, 1.00f };
	float clear_color_transparent[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float color_picker[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_Once);
		if (ImGui::Begin("JattoImGuiTest Title Bar", &JattoImGui::show_gui))
		{
			ImGui::Text(u8"HogehogeFugafuga");

			ImGui::Separator();

			ImGui::Text(u8"Content Test");
			ImGui::SameLine();
			ImGui::Text(u8"Content Test");

			ImGui::Separator();

			ImGui::Checkbox(u8"Checkbox Test", &JattoImGui::checkbox);
			
			ImGui::Separator();

			ImGui::ColorPicker4(u8"Color Picker Test", color_picker);

			ImGui::Text("App Average %.3f [ms/frame] (%.1 fps)", 1000.0f / ImGui::GetIO().Framerate);

			ImGui::End();
		}


		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		JattoImGui::pDeviceContext->OMSetRenderTargets(1, &JattoImGui::pRenderTargetView, NULL);

		JattoImGui::pSwapChain->Present(1, 0);
		JattoImGui::pDeviceContext->ClearRenderTargetView(JattoImGui::pRenderTargetView, clear_color_transparent);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	JattoImGui::CleanupDevice();
	DestroyWindow(JattoImGui::hWindow);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}