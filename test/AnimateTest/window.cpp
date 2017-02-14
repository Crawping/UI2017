#include "stdafx.h"
#include "window.h"
#include "Animate\animate_base.h"


LoginWindow::LoginWindow()
{
	m_pWindow = nullptr;
}


LoginWindow::~LoginWindow()
{
	if (m_pWindow)
		m_pWindow->Release();
}

void LoginWindow::Create(UI::ISkinRes* p)
{
	m_pWindow = UI::ICustomWindow::CreateInstance(p);
	m_pWindow->SetWindowMessageCallback(this);
	m_pWindow->EnableGpuComposite(true);
	m_pWindow->Create(L"login");

	m_pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");
	m_pWindow->ShowWindow();
}

BOOL LoginWindow::OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
{
	if (msg == WM_DESTROY)
	{
		Animate::Quit();
		PostQuitMessage(0);
	}
	else if (msg == WM_MOUSEWHEEL)
	{

	}
	else if (msg == WM_NCLBUTTONDBLCLK)
	{
		
	}
	else if (msg == WM_KEYDOWN)
	{
		OnKeydown(wParam);
	}
	return FALSE;
}

void LoginWindow::Destroy()
{
	if (m_pWindow)
	{
		m_pWindow->DestroyWindow();
	}
}


void LoginWindow::ShowAnimate(long type)
{
	Animate::Create(type, m_pWindow);
}

void LoginWindow::OnKeydown(UINT key)
{
	if (key >= '1' && key <= '9')
	{
		int index = key - '1';
		ShowAnimate(index);
	}
// 	switch (key)
// 	{
// 	case '1':
// 		ShowAnimate(AnimateType_CustomTimingFunction);
// 		break;
// 
// 	default:
//		break;
// 	}
}