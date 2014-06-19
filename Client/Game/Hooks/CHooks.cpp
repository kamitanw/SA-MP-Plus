#include <stdafx.h>
#include "CHooks.h"
#include "../Utility/CLog.h"
#include <Detours/detours.h>

Direct3DCreate9_t  CHooks::m_pfnDirect3DCreate9 = NULL;
DirectInput8Create_t CHooks::m_pfnDirectInput8Create = NULL;
SetCursorPos_t  CHooks::m_pfnSetCursorPos = NULL;

void CHooks::Apply()
{
	ApplyCursorPos();
	ApplyDirect3D();
	ApplyDirectInput();
}

void CHooks::Remove()
{
	RemoveDirectInput();
	RemoveDirect3D();
	RemoveCursorPos();
}

void CHooks::ApplyDirectInput()
{
	if (!m_pfnDirectInput8Create)
		m_pfnDirectInput8Create = (DirectInput8Create_t)DetourFunction(DetourFindFunction("dinput8.dll", "DirectInput8Create"), (BYTE*)CHooks::HOOK_DirectInput8Create);
}

void CHooks::RemoveDirectInput()
{
	if (m_pfnDirectInput8Create)
		DetourRemove((BYTE*)m_pfnDirectInput8Create, (BYTE*)HOOK_DirectInput8Create);
}

void CHooks::ApplyDirect3D()
{
	if (!m_pfnDirect3DCreate9)
		m_pfnDirect3DCreate9 = (Direct3DCreate9_t)DetourFunction(DetourFindFunction("d3d9.dll", "Direct3DCreate9"), (BYTE*)HOOK_Direct3DCreate9);
}

void CHooks::RemoveDirect3D()
{
	if (m_pfnDirect3DCreate9)
		DetourRemove((BYTE*)m_pfnDirect3DCreate9, (BYTE*)HOOK_Direct3DCreate9);
}

void CHooks::ApplyCursorPos()
{
	if(!m_pfnSetCursorPos)
		m_pfnSetCursorPos = (SetCursorPos_t)DetourFunction(DetourFindFunction("user32.dll", "SetCursorPos"), (BYTE*)HOOK_SetCursorPos);
}

void CHooks::RemoveCursorPos()
{
	if (m_pfnSetCursorPos)
		DetourRemove((BYTE*)m_pfnSetCursorPos, (BYTE*)HOOK_SetCursorPos);
}

HRESULT CHooks::HOOK_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, void** ppvOut, IUnknown* punkOuter)
{
	HRESULT hr = m_pfnDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	IDirectInput8A* pDInput = (IDirectInput8A*)*ppvOut;
	CDInput8Proxy* pDInputHook = new CDInput8Proxy(pDInput);
	*ppvOut = pDInputHook;
	return hr;
}

IDirect3D9* WINAPI CHooks::HOOK_Direct3DCreate9(UINT SDKVersion)
{
	IDirect3D9* Direct3D = m_pfnDirect3DCreate9(SDKVersion);
	IDirect3D9* Mine_Direct3D = new CD3D9Proxy(Direct3D);
	return Mine_Direct3D;
}

BOOL WINAPI CHooks::HOOK_SetCursorPos(int iX, int iY)
{
	if (CMessageProxy::OnSetCursorPos(iX, iY))
		m_pfnSetCursorPos(iX, iY);
		
	return false;
}
