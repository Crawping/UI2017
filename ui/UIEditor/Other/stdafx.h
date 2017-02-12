// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <stack>
#include <deque>
#include <map>
#include <algorithm>
#include <fstream>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)
#pragma warning(disable:4302)
#pragma warning(disable:4838)

#include <atltypes.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <Shellapi.h>

using namespace std;

#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

// ��������
#define  WINDOW_PADDING             10   // �����ڿؼ����봰�ڱ�Ե�Ĵ�С
#define  LEFT_TREE_WINDOW_WIDTH    300   // ���Ĵ��ڿ��

#define  WIDTH_IMAGEBUILD_LISTCTRL 300
#define  HEIGHT_TOOLBAR_BTN         22
#define  WIDTH_TOOLBAR_BTN          50
#define  HEIGHT_TAB                 20  // TAB�ؼ��ĸ߶�
#define  STATUSBAR_HEIGHT           22
#define  TOOLBAR_HEIGHT             26
#define  PROPERTY_DLG_WIDTH         300

#define  UIEDITOR_CONFIG_FILE_NAME _T("uieditor.xml")   // �����ļ�����

// #include "App\MemoryLeakDetect\include\mld.h"
// #pragma comment(lib, "mld.lib")

// ������
#include "UISDK\Inc\inc.h"
#include "UICTRL\Inc\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrl.lib")

#include "UISDK\Inc\Util\util.h"
#include "UISDK\Inc\Interface\ilistitembase.h"
#include "UISDK\Inc\Interface\iuires.h"
#include "common/define.h"

#include "UIEditorCtrl\Inc\inc.h"
#pragma comment(lib, "uieditorctrl.lib")

#define MAX_STRING_LEN 256

using namespace UI;


// ��Դ���Ͷ���
typedef enum
{
    UI_RESOURCE_NULL = 0,
    UI_RESOURCE_PROJECT = 0x0001,
    UI_RESOURCE_SKIN = 0x0002,
    UI_RESOURCE_IMAGE = 0x0004,
    UI_RESOURCE_FONT = 0x0008,
    UI_RESOURCE_COLOR = 0x0010,
    UI_RESOURCE_STYLE = 0x0020,
    UI_RESOURCE_LAYOUT = 0x0040,
    UI_RESOURCE_I18N = 0x0080,
    UI_RESOURCE_SOURCEFILE = 0x0400,  // ��Դ�������ڵ��ļ�������UIBuilder�鿴�ļ�
    UI_RESOURCE_MENU = 0x0800,
}
UI_RESOURCE_TYPE;

#ifndef uint
typedef unsigned int uint;
#endif


#include "Util/util.h"
#include "Business\GlobalData.h"
#include "Business\Command\Base\commandhistorymgr.h"

extern CAppModule      _Module;
extern CGlobalData*    g_pGlobalData;

IUIApplication*  GetEditUIApplication();
ISkinManager*    GetEditSkinManager();
CProjectData*    GetProjectData();
CDependsCtrlDllMgr*  GetProjDependsCtrlDllMgr();
CUIEditor*  GetUIEditor();
CPropertyDialog*  GetPropertyDialog();
namespace UI
{
	interface IPropertyCtrl;
}

UI::IPropertyCtrl*   GetPropertyCtrl();


extern const GUID IID_IMDIClietnView;
extern const GUID IID_IExplorerMDIClientView;
extern const GUID IID_ILogMDIClientView;
extern const GUID IID_ILayoutMDIClientView;
extern const GUID IID_IMenuMDIClientView;

void UIEDITOR_LOG_DEBUG( TCHAR* szContent, ... );
void UIEDITOR_LOG_INFO( TCHAR* szContent, ... );
void UIEDITOR_LOG_WARN( TCHAR* szContent, ... );
void UIEDITOR_LOG_ERROR( TCHAR* szContent, ... );
void UIEDITOR_LOG_FATAL( TCHAR* szContent, ...);

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Ԥ������
#define MDIVIEW_MESSAGE_PREVIEW  (WM_USER+1000)