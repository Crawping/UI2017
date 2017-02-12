#pragma once
#include "mallocspy.h"

class CApiHook
{
public:
    CApiHook(
            PSTR pszCalleeModName, 
            PROC pfnCurrent, 
            PROC pfnNew
        );
    ~CApiHook();
    void  UnHookModule(HMODULE hModule);
    void  HookModule(HMODULE hModule);


private:
    PCSTR  m_pszCalleeModName;
    PROC   m_pfnOrig; 
    PROC   m_pfnHook;
};

// TODO: û�д���ģ�鱻ж��(freelibrary)�����
enum MODULE_TYPE
{
    MODULE_SYSTEM,
    MODULE_CRT,
    MODULE_GDIPLUS,
    MODULE_MLD,
    MODULE_APPLICATION,    
};
class CHookManager
{
public:
    CHookManager();
    ~CHookManager();

    void  Init();
    void  Uninit();

    void  OnLoadLibrary(HMODULE hModule);
    bool  IsAppModule(HMODULE hModule);

    void  AddHook(const CApiHook& hook);

    void  CopyModuleMap(map<HMODULE, MODULE_TYPE>& m);

private:
    MODULE_TYPE  GetModuleType(HMODULE hModule);
    void  HookModule(HMODULE hModule);
    void  UnHookModule(HMODULE hModule);

    void  HookAllModules();
    void  UnHookAllModules();
    
    void  ClearModules();
    void  AddModule(HMODULE, MODULE_TYPE);

private:
    // ֻ�ڳ�ʼ��ʱwrite�ˣ�ƽʱ����read��������
    list<CApiHook>  m_apiHookList;

    TCHAR  m_szAppDir[MAX_PATH];

    CRITICAL_SECTION  m_csModuleMap;
    map<HMODULE, MODULE_TYPE>  m_mapAllModules;  // ���м��ص�ģ��

    // COM hook
    MallocSpy  m_oComHook;
};

extern CHookManager  g_hookMgr;