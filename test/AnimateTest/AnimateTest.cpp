//
// libo 2017/2/8 
// ����Ч����������
//

#include "stdafx.h"
#include "AnimateTest.h"
#include "window.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UI::IUIApplication* pUIApp = NULL;
	UI::CreateUIApplication(&pUIApp);
	UICTRL_RegisterUIObject(pUIApp);

	pUIApp->EnableGpuComposite();

	// ������Դ
	TCHAR szResPath[MAX_PATH] = { 0 };
	UI::Util::PathInBin(L"AniamteTestSkin", szResPath);
	UI::ISkinRes* pRootSkinRes = pUIApp->LoadSkinRes(szResPath);

	{
		LoginWindow m;
		m.Create(pRootSkinRes);
		pUIApp->MsgHandleLoop();
	}
	pUIApp->Release();

	return 0;
}