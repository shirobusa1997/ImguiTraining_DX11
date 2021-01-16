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

int main(int, char**) {
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = JattoImGui::WndProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"JattoImGui";
	RegisterClassEx(&wc);

	// �E�B���h�E�I�u�W�F�N�g����
	JattoImGui::hWindow = CreateWindowW(wc.lpszClassName, L"JattoImGui", WS_OVERLAPPEDWINDOW, 100, 100, 800, 800, NULL, NULL, wc.hInstance, NULL);

	// ImGui����
	if (JattoImGui::hWindow == NULL)
	{
		std::cout << "�E�B���h�E�̍쐬�Ɏ��s���܂����D\n";
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	if (!JattoImGui::CreateDevice(JattoImGui::hWindow))
	{
		std::cout << "�f�o�C�X�̍쐬�Ɏ��s���܂����D\n";
		JattoImGui::CleanupDevice();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		std::exit(1);
	}

	// �E�B���h�E�`��
	ShowWindow(JattoImGui::hWindow, SW_SHOW);
	UpdateWindow(JattoImGui::hWindow);

	// ImGui�o�[�W�����`�F�b�N�ƃR���e�L�X�g����
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();

	// �C���^�t�F�[�X�̃C�j�V�����C�Y�`�F�b�N
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

	// ini�t�@�C���𐶐����Ȃ��悤�ɐݒ�
	io.IniFilename = NULL;
	// �C���^�t�F�[�X�̓��{��t�H���g�ɑΉ�������悤�ɐݒ�
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	float clear_color[4]  = { 0.45f, 0.55f, 0.60f, 1.00f };
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

		// ���ɕ`�悷��t���[���𐶐�
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ���ɕ`�悷��ImGui�R���e���c��ݒ�
		ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_Once);
		if (ImGui::Begin("JattoImGuiTest Title Bar", &JattoImGui::show_gui))
		{
			// �e�L�X�g
			ImGui::Text(u8"�ق��ق��ӂ��ӂ�");

			// ��؂��
			ImGui::Separator();

			// �����s�ւ̃R���e���c�̒ǉ�
			ImGui::Text(u8"�����s�ւ� ");
			ImGui::SameLine();
			ImGui::Text(u8"�R���e���c�̒ǉ��e�X�g ");

			ImGui::Separator();

			// �`�F�b�N�{�b�N�X�G�������g
			ImGui::Checkbox(u8"�`�F�b�N�{�b�N�X�͂���Ȋ����炵��", &JattoImGui::checkbox);
			
			ImGui::Separator();

			// �J���[�s�b�J�[�G�������g
			ImGui::ColorPicker4(u8"�J���[�s�b�J�[�e�X�g", color_picker);

			ImGui::Text("App Average %.3f [ms/frame] (%.1 fps)", 1000.0f / ImGui::GetIO().Framerate);

			ImGui::End();
		}

		// �����_���֕`��f�[�^�𓊂���
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		JattoImGui::pDeviceContext->OMSetRenderTargets(1, &JattoImGui::pRenderTargetView, NULL);
		// ���������ݒ�(1st args)
		JattoImGui::pSwapChain->Present(1, 0);
		JattoImGui::pDeviceContext->ClearRenderTargetView(JattoImGui::pRenderTargetView, clear_color);
	}

	// �I������

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	JattoImGui::CleanupDevice();
	DestroyWindow(JattoImGui::hWindow);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}