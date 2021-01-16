#pragma once

// Standard Library Includes
#include <iostream>
#include <string>
#include <tchar.h>

// ImGui Library Includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

// Direct3D / DirectX Library Includes
#include <d3d11.h>
#include <dinput.h>

namespace JattoImGui {
	static ID3D11Device*				pDevice 				= NULL;
	static ID3D11DeviceContext*			pDeviceContext			= NULL;
	static IDXGISwapChain*				pSwapChain				= NULL;
	static ID3D11RenderTargetView*		pRenderTargetView		= NULL;

	HWND hWindow;

	bool show_gui = true;
	bool checkbox;

	static bool CreateDevice(HWND hWindow);
	static void CleanupDevice();
	static void CreateRenderTarget();
	static void CleanupRenderTarget();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}