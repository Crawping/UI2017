#pragma once
#include "Other\resource.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "UISDK\Inc\Interface\ilistctrlbase.h"
#include "UIEditorCtrl\Inc\iprojecttreeview.h"

class CMainFrame;
class ObjectEditorData;

namespace UI
{
    interface ITreeView;
    interface IRadioButton;
    interface IProjectTreeView;
    interface IButton;
}

// ����һ����־ģ��
#define WM_INSERT_LOG_ITEM   (WM_USER+101)
// �޸�һ����־ģ��
#define WM_MODIFY_LOG_ITEM   (WM_USER+102)

enum TREEITEM_DATA_TYPE
{
    TREEITEM_DATA_TYPE_UNKNOWN,

    TREEITEM_DATA_TYPE_LAYOUT_ROOT,
    TREEITEM_DATA_TYPE_LAYOUT_SKIN,
	TREEITEM_DATA_TYPE_LAYOUT_WINDOW,
    TREEITEM_DATA_TYPE_LAYOUT_OBJECT,
//    TREEITEM_DATA_TYPE_LAYOUT_MENU,

	TREEITEM_DATA_TYPE_RESOURCE_ROOT,
    TREEITEM_DATA_TYPE_RESOURCE_SKIN,
    TREEITEM_DATA_TYPE_RESOURCE_IMAGE,
    TREEITEM_DATA_TYPE_RESOURCE_FONT,
    TREEITEM_DATA_TYPE_RESOURCE_COLOR,
    TREEITEM_DATA_TYPE_RESOURCE_STYLE,
	TREEITEM_DATA_TYPE_RESOURCE_I18N,

	TREEITEM_DATA_TYPE_EXPLORER,
	TREEITEM_DATA_TYPE_LOG,
};
class TreeItemData
{
public:
	virtual ~TreeItemData() = 0 {};
	virtual TREEITEM_DATA_TYPE GetDataType() = 0;
};
class LayoutTreeItemData : public TreeItemData
{
public:
	LayoutTreeItemData();
	~LayoutTreeItemData();

	TREEITEM_DATA_TYPE GetDataType()
    {
        return m_eType;
    }

public:
    TREEITEM_DATA_TYPE  m_eType;
    IListItemBase*      m_pListItem;

    ISkinRes*  m_pSkin;
	IObject*   m_pObject;

	String  m_strNodeName;
	String  m_strNodeID;
    String  m_strXmlPath;

    bool  m_bLazyLoad;  // �ô����������� <layoutconfig>��
};

class ResourceTreeItemData : public TreeItemData
{
public:
	ResourceTreeItemData();
	~ResourceTreeItemData();

	TREEITEM_DATA_TYPE GetDataType() 
    { 
        return m_eType; 
    }

public:
    TREEITEM_DATA_TYPE  m_eType;
    IListItemBase*      m_pListItem;
	ISkinRes*  m_pSkin;
};

class ExplorerTreeItemData : public TreeItemData
{
public:
	ExplorerTreeItemData();
	~ExplorerTreeItemData();

	TREEITEM_DATA_TYPE GetDataType() { return TREEITEM_DATA_TYPE_EXPLORER; }

public:
	ISkinRes* m_pSkin;
	HTREEITEM m_hTreeItem;
	String    m_strFilePath;
	bool  m_bDirty;
};
// ��Ƥ����һЩ������ݱ�������
class SkinResData
{
public:
    SkinResData();

public:
    ISkinRes*  m_pSkinRes;
    IListItemBase*  m_pItem_LayoutTree;   // ��layout tree�еĽ��
    IListItemBase*  m_pItem_ResourceTree; // ��resource tree�еĽ��
};

//
//	��������� 
//
class CProjectTreeDialog : public IWindow
{
public:

	CProjectTreeDialog();
	~CProjectTreeDialog(void);

    UI_BEGIN_MSG_MAP_Ixxx(CProjectTreeDialog)
        UIMSG_NOTIFY_CODE_FROM(UI_PROJECTTREEVIEW_NOTIFY_CONTROLITEM_DROPEVENT, m_pTreeLayout, OnControlDropEvent)
        UIMSG_INITIALIZE(OnInitialize)
        MSG_WM_DESTROY(OnDestroy)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void  SetMainFrame(CMainFrame* pFrame) { m_pMainFrame = pFrame; }
	void  OnInitialize();
    void  OnDestroy( );
	void  OnSize(UINT nType, CSize size);
	
	void  CalcLayout( int cx, int cy );

	void  InsertSkinItem(ISkinRes* pSkin);
    void  InsertWindowItem(
            ISkinRes* pSkin, LPCTSTR szPath, 
            LPCTSTR szName, LPCTSTR szId);
    IListItemBase*  InsertWindowXmlTreeItem(ISkinRes* pSkin, LPCTSTR szXmlPath);
   
    void  OnLayoutTreeSelChanged(UI::IListCtrlBase*);

	LRESULT  OnExplorerTreeDBClick(LPNMHDR pnmh);
	LRESULT  OnExplorerTreeRClick(LPNMHDR pnmh);
    void  OnLayoutTreeRClick(IListCtrlBase*, IListItemBase*);
    void  OnLayoutTreeKeyDown(IListCtrlBase*, UINT, bool&);
    LRESULT  OnControlDropEvent(WPARAM w, LPARAM l);
    void  OnResourceTreeDBClick(IListCtrlBase*, IListItemBase*);
    void  OnBtnLayout(UI::IButton*);
    void  OnBtnResource(UI::IButton*);

	bool  InsertLayoutChildObjectNode(
            IObject* pChild,
            IListItemBase* hParentTreeItem, 
            IListItemBase* pInsertAfter=UITVI_LAST);
    bool  RemoveLayoutObjectNode(IObject* pChild);
	bool  SelectLayoutObjectNode(IObject* pChild);
    void  LoadObjectChildNode(IObject* pParent);
    IListItemBase*  GetLayoutObjectTreeItem(IObject* pObj);
    void  UpdatLayoutObjectTreeItemText(IObject* pObj);


	HTREEITEM FindExplorerItem(ISkinRes* hSkin, TCHAR* szFilePath);

    void  OnObjectDeleteInd(ObjectEditorData* pObjData);

protected:
	String    GetResName(UI_RESOURCE_TYPE);
    IObject*  GetObjectByHTREEITEM(IListItemBase*  pItem);
    void      DeleteWindowItem(IListItemBase*  pItem);
	void      DeleteObjectItem(IListItemBase*  pItem);

    SkinResData*  GetSkinResData(ISkinRes* pSkinRes);
    LayoutTreeItemData*  GetLayoutTreeItemData(IListItemBase* pItem);

public:
	CMainFrame*         m_pMainFrame ;         // ������
    IProjectTreeView*   m_pTreeLayout;
    IProjectTreeView*   m_pTreeResource; 

private:
	CTreeViewCtrl   m_treeCtrlExplorer;
	CTreeViewCtrl*  m_pCurTreeCtrl;

	// ������ʹ��map���������������ݣ�����鲼�����̫����
	map<IListItemBase*, LayoutTreeItemData*>    m_mapLayoutTree;     // m_treeCtrlLayout������������������
    map<IListItemBase*, ResourceTreeItemData*>  m_mapResTree;        // m_treeCtrlRes������������������
    map<ISkinRes*, SkinResData*>  m_mapSkinResData;   // Ƥ���ڸ������еĽ��λ��

	map<HTREEITEM, ExplorerTreeItemData*>  m_mapTreeCtrlExplorer;   // m_treeCtrlRes������������������

	CTabCtrl        m_tabCtrl;
};


