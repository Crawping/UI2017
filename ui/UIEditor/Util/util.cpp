#include "stdafx.h"
#include "util.h"
#include "UISDK\Inc\Interface\iuires.h"

void  MsgBox(LPCTSTR szText, LPCTSTR szTitle)
{
    ::MessageBox(
        g_pGlobalData->m_hWndMainFrame,
        szText, szTitle?szTitle:_T("UIEditor"), MB_OK|MB_ICONINFORMATION);
}

// ��exe�е�ģ�屣�浽ָ��Ŀ¼s
bool  SaveTemplateResFileTo(LPCTSTR szDstPath, UINT nId, LPCTSTR szResType)
{
    HRSRC hResource = FindResource(_Module.m_hInst, MAKEINTRESOURCE(nId), szResType);
    if (NULL == hResource)
    {
        MsgBox(_T("����Ƥ���ļ���Դʧ��"));
        return false;
    }
    HGLOBAL hGlobal = LoadResource(_Module.m_hInst, hResource);
    {
        fstream  f;
        f.open(szDstPath, ios_base::out|ios_base::binary);
        if (f.fail())
        {
            FreeResource(hGlobal);
            return false;
        }

        int  nSize = SizeofResource(_Module.m_hInst, hResource);
        byte*  pData = (byte*)LockResource(hGlobal);
        f.write((char*)pData, nSize);
        UnlockResource(hGlobal);
        f.close();
    }
    FreeResource(hGlobal);
    return true;
}

// ��ȡUIEditor.xml����·��
void  GetUIEditorConfigFilePath(TCHAR* pszPath)
{
    GetAppPath_(pszPath);
    _tcscat(pszPath, _T("Editor\\"));
    _tcscat(pszPath, UIEDITOR_CONFIG_FILE_NAME);
}

// ���������е�������
void  ParsePropertyCommand(LPCTSTR  szCmdLine, String& strCommand, String& strArgs)
{
    if (NULL == szCmdLine)
        return;

    String strText(szCmdLine);
    TrimStringLeft(strText);

    LPCTSTR p = strText.c_str();
    while (*p)
    {
        if (*p == _T(' '))
            break;
        p++;
    }
    strCommand.insert(0, strText.c_str(), p-strText.c_str());

    // �������������֮��Ŀո�
    while (*p)
    {
        if (*p != _T(' '))
            break;
        p++;
    }

    strArgs.assign(p);
}

// ���������е����Ժ�ֵ
void  ParsePropertyCommandArg(LPCTSTR  szArgs, String& strKey, String& strValue)
{
    if (!szArgs || _T('\0') == szArgs[0])
        return;

   
    TCHAR*  szSep = NULL; 
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            szSep = _T("=");   // 1. ���� = �ָ�
        else 
            szSep = _T(" ");   // 2. ���� �ո� �ָ�

        LPCTSTR  szPos = _tcsstr(szArgs, szSep);
        if (szPos)
        {
            strKey.clear();
            strKey.insert(0, szArgs, szPos-szArgs);

            strValue.assign(szPos+1);

            TrimString(strKey);
            TrimString(strValue);
            break;
        }
    }
}


void  TrimStringLeft(String&  str, const TCHAR c)
{
    int nLeft = 0;
    int nLength = str.length();
    for (int i = 0; i < nLength; i++)
    {
        if (str[i] != c)
            break;

        nLeft++;
    }
    str = str.substr(nLeft, nLength-nLeft);
}
void  TrimStringRight(String&  str, const TCHAR c)
{
    int nRight = 0;

    int nLength = str.length();
    for (int i = nLength-1; i >= 0; i--)
    {
        if (str[i] != c)
            break;

        nRight++;
    }

    str = str.substr(0, nLength-nRight);
}

void  TrimString(String&  str, const TCHAR c)
{
    int nLeft = 0;
    int nRight = 0;

    int nLength = str.length();
    for (int i = 0; i < nLength; i++)
    {
        if (str[i] != c)
            break;

        nLeft++;
    }

    for (int i = nLength-1; i >= 0; i--)
    {
        if (str[i] != c)
            break;

        nRight++;
    }

    if (nRight + nLeft >= nLength)
    {
        str.clear();
    }
    else
    {
        str = str.substr(nLeft, nLength-nLeft-nRight);
    }
}

// �ȴ��û����»س�����ȷ��
bool  WaitForEnter2Confirm(HWND hWnd)
{
    bool bOk = false;
    MSG  msg;
    while (GetMessage(&msg, hWnd, 0, 0))
    {
        if (msg.message == WM_KEYDOWN)
        {
            if (VK_RETURN == msg.wParam)
            {
                bOk = true;
            }
            break;
        }
        else if (msg.message > WM_MOUSEFIRST && msg.message < WM_MOUSELAST)
        {
            break;
        }
    }
    return bOk;
}

// ��ȡ�������͵���ʾͼ��
void  GetObjectTypeBitmap(OBJ_TYPE eType, UINT nControlType, IRenderBitmap** ppRenderBitma)
{
    IImageRes& pImgRes = g_pGlobalData->m_pMyUIApp->
        GetDefaultSkinRes()->GetImageRes();

    LPCTSTR  szImgId = NULL;
    if (eType == OBJ_WINDOW)
    {
        if (nControlType == WINDOW_TOOLTIP)
            szImgId = _T("control_tooltip");
        else
            szImgId = _T("control_AppWindow");
    }
    else if (eType == OBJ_PANEL)
    {
        szImgId = _T("control_panel");
    }
    else if (eType == OBJ_CONTROL)
    {
        switch (nControlType)
        {
        case CONTROL_BUTTON:
            szImgId = _T("control_button");
            break;
        case CONTROL_MENU:
            szImgId = _T("control_menu");
            break;
        case CONTROL_CHECKBUTTON:
            szImgId = _T("control_checkbox");
            break;
        case CONTROL_COMBOBOX:
            szImgId = _T("control_combobox");
            break;
        case CONTROL_GROUPBOX:
            szImgId = _T("control_groupbox");
            break;
        case CONTROL_HSCROLLBAR:
            szImgId = _T("control_hscrollbar");
            break;
        case CONTROL_VSCROLLBAR:
            szImgId = _T("control_vscrollbar");
            break;
        case CONTROL_HYPERLINK:
            szImgId = _T("control_hyperlink");
            break;
        case CONTROL_LABEL:
        case CONTROL_LEDCTRL:
            szImgId = _T("control_label");
            break;
        case CONTROL_LISTBOX:
            szImgId = _T("control_listbox");
            break;
        case CONTROL_PICTURE:
            szImgId = _T("control_picturebox");
            break;
        case CONTROL_RADIOBUTTON:
            szImgId = _T("control_radiobutton");
            break;
        case CONTROL_RICHEDIT:
            szImgId = _T("control_richtextbox");
            break;
        case CONTROL_TABCTRL:
            szImgId = _T("control_tabcontrol");
            break;
        case CONTROL_EDIT:
            szImgId = _T("control_textbox");
            break;
        case CONTROL_PROGRESSBAR:
            szImgId = _T("control_progress");
            break;
        case CONTROL_SLIDERCTRL:
            szImgId = _T("control_trackbar");
            break;
        case CONTROL_TREEVIEWCTRL:
            szImgId = _T("control_treeview");
            break;
        case CONTROL_IE:
            szImgId = _T("control_webbrowser");
            break;
        case CONTROL_LISTVIEWCTRL:
            szImgId = _T("control_listview");
            break;
        }
    }

    if (szImgId)
    {
        pImgRes.GetBitmap(szImgId, GRAPHICS_RENDER_LIBRARY_TYPE_GDI, ppRenderBitma);
    }
}


//
//	����:	void GetAppPath_( TCHAR* strPath )
//	
//	Ŀ��:	��ȡӦ�ó��������ļ��У����������\\
//
//	����:
//		strPath
//			[out]:	�����ļ���·��
//
void GetAppPath_(TCHAR* strPath)
{
    int		dwLength = 0;
    TCHAR	*p = NULL;
    TCHAR	szFilePath[MAX_PATH] = { 0 };

    dwLength = GetModuleFileName(GetModuleHandle(NULL), szFilePath, MAX_PATH);
    p = _tcsrchr(szFilePath, '\\');
    if (p != NULL)
        *(p + 1) = 0;

    _tcscpy(strPath, szFilePath);
}


//
//	����szPath������ļ�szFile�����·��szOutRelative
//
BOOL CalcRelativePathToFile(
        LPCTSTR szFile, LPCTSTR szPath, TCHAR* szOutRelativePath)
{
    if (NULL == szFile || NULL == szPath || NULL == szOutRelativePath)
    {
        return FALSE;
    }

    BOOL bRet = ::PathRelativePathTo(szOutRelativePath,
        szFile, FILE_ATTRIBUTE_NORMAL,
        szPath, FILE_ATTRIBUTE_NORMAL);

    if (FALSE == bRet)
    {
        _tcscpy(szOutRelativePath, szPath);
    }

    // ȥ����ǰ���.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp + 2);
    }
    return bRet;
}


//
//	����szPath������ļ���szDir�����·��szOutRelative
//
BOOL CalcRelativePathToDir(
        LPCTSTR szDir, LPCTSTR szPath, TCHAR* szOutRelativePath)
{
    if (NULL == szDir || NULL == szPath || NULL == szOutRelativePath)
    {
        return FALSE;
    }

    BOOL bRet = ::PathRelativePathTo(szOutRelativePath,
        szDir, FILE_ATTRIBUTE_DIRECTORY,
        szPath, FILE_ATTRIBUTE_NORMAL);

    if (FALSE == bRet)
    {
        _tcscpy(szOutRelativePath, szPath);
    }
    // ȥ����ǰ���.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp + 2);
    }
    return bRet;
}

//
//	�������szDir��·��szRelative��չΪȫ·��szOut
//
BOOL CalcFullPathByRelative(LPCTSTR _szDir, LPCTSTR szRelative, TCHAR* szOut)
{
    if (NULL == _szDir || NULL == szRelative || NULL == szOut)
        return FALSE;

    TCHAR szDir[MAX_PATH] = _T("");
    _tcscpy(szDir, _szDir);
    if (!PathIsDirectory(szDir))
    {
        PathRemoveFileSpec(szDir);
    }

    TCHAR szPath[2 * MAX_PATH] = _T("");

    int nLenDir = _tcslen(szDir);
    if (szRelative[0] != _T('\\') &&
        szDir[nLenDir - 1] != _T('\\'))
    {
        _stprintf(szPath, _T("%s\\%s"), szDir, szRelative);
    }
    else
    {
        _stprintf(szPath, _T("%s%s"), szDir, szRelative);
    }

    PathCanonicalize(szOut, szPath);

    return TRUE;
}


//
//	��ȡһ��·��szPath���ڵ�Ŀ¼szOutDir
//
BOOL GetPathDir(LPCTSTR szPath, TCHAR* szOutDir)
{
    if (NULL == szPath || NULL == szOutDir)
        return FALSE;

    TCHAR szCopy[MAX_PATH] = _T("");
    _tcsncpy(szCopy, szPath, MAX_PATH - 1);

    TCHAR* p = _tcsrchr(szCopy, _T('\\'));
    if (NULL == p)
        return FALSE;

#ifdef _UNICODE
    *(p + 1) = _T('\0');
#else
    *(p + 1) = 0;
#endif
    _tcscpy(szOutDir, szCopy);
    return TRUE;
}

//
//	�ж�һ��·���Ƿ���ȫ·��
//
BOOL IsFullPath(LPCTSTR szPath)
{
    return !::PathIsRelative(szPath);
}


//
//	��ȡһ��·��szPath�е��ļ���
//
BOOL GetPathFileName(LPCTSTR szPath, TCHAR* szOutDir)
{
    if (NULL == szPath || NULL == szOutDir)
        return FALSE;

    LPCTSTR p = _tcsrchr(szPath, _T('\\'));
    if (NULL == p)
        return FALSE;

    _tcscpy(szOutDir, p + 1);
    return TRUE;
}

BOOL GetPathFileExt(LPCTSTR szPath, TCHAR* szOutExt)
{
    if (NULL == szPath || NULL == szOutExt)
        return FALSE;

    LPCTSTR p = _tcsrchr(szPath, _T('.'));
    if (NULL == p)
        return FALSE;

    _tcscpy(szOutExt, p + 1);
    return TRUE;
}



//
//	������Ĵ�Сת����CreateFont��Ҫʹ�õ�����߶�
//
//	Parameter
//		nSize	
//			[in]	�����������ʾ�Ĵ�С
//
//	Return
//		������������Ӧ��height;
//
int FontSize2Height(int nSize)
{
    int nHeight = 0;
    HDC hDC = ::GetDC(NULL);
    int dpi = GetDeviceCaps(hDC, LOGPIXELSY);
    nHeight = -MulDiv(nSize, dpi, 72);
    ::ReleaseDC(NULL, hDC);
    return nHeight;
}

//
//	������LOGFONT.lfHeightת���������С
//
//	Parameter
//		nHeight	
//			[in]	�����������ʾ�Ĵ�С
//
//	Return
//		������������Ӧ��size;
//
int FontHeight2Size(int nHeight)
{
    int nSize = 0;
    HDC hDC = ::GetDC(NULL);
    nSize = -MulDiv(nHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));  // 96
    ::ReleaseDC(NULL, hDC);
    return nSize;
}


//
//	����һ���յ�xml�ļ����ļ����ݰ��� <? ?> ����rootԪ��<ui>,�����һ�����<node>
//
bool CreateEmptyXmlFile(LPCTSTR szPath, LPCTSTR szRoot, LPCTSTR szNode)
{
#ifdef _UNICODE
    wfstream f;
#else
    fstream  f;
#endif
    f.open(szPath, ios_base::out);
    if (f.fail())
    {
        return false;
    }

    // �������һ��rootԪ�أ�����markup�᷵��ʧ��
    f << _T("<?xml version=\"1.0\" encoding=\"utf-8\" ?>") << endl;
    f << _T("<") << szRoot << _T(">") << endl;
    if (NULL != szNode)
    {
        f << _T("<") << szNode << _T("/>") << endl;
    }
    f << _T("</") << szRoot << _T(">");

    f.close();

    return true;
}
