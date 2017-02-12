#pragma once

struct DependsCtrlDllItem
{
    String  strRelativePath;  // �����uiproj��·�������ڱ����·��
    String  strAbsolutePath;  // ͨ��strRelativePath��������ľ���·��
};

class CDependsCtrlDllMgr
{
public:
    CDependsCtrlDllMgr();
    ~CDependsCtrlDllMgr();

    bool AddCtrlDll(LPCTSTR szPath);
    bool DelCtrlDll(LPCTSTR szPath);

    void  OnLoadItem(LPCTSTR szPath);

public:
    vector<DependsCtrlDllItem*>  m_vStrDlls;
};