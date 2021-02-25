#pragma once

// Standard Library Includes
#include <iostream>
#include <string>

// ImGui Library Includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

// Direct3D / DirectX Library Includes
#include "D3D11.h"
#include "D3DX11.h"

namespace JattoImGui {
	// Direct3D Implement objects
	static ID3D11Device*						pDevice						= NULL;
	static ID3D11DeviceContext*			pDeviceContext		= NULL;
	static IDXGISwapChain*					pSwapChain				= NULL;
	static ID3D11RenderTargetView*	pRenderTargetView = NULL;

	//
	HWND hWindow;

	// Flags
	bool bShowGui = true;
	bool bChekcbox;

	// ClassMethods
	static bool CreateDevice(HWND hWindowInst);
	static void CleanupDevice();
	static void CreateRenderTarget();
	static void CleanupRenderTarget();

	static LRESULT WINAPI WindowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);
};
