#pragma once
#include "Other\resource.h"
#include "..\Dialogs.h"
#include "UISDK\Inc\Interface\iwindow.h"
#include "Util\trackrect.h"
#include "Dialog\Framework\childframe.h"
#include "LayoutPropertyHandler.h"
#include "UISDK\Inc\Util\struct.h"
class CLayoutCodeDlg;

class CLayoutEditorDlg : public IWindow,
                       public ILayoutMDIClientView,
                       public IRectTrackerRsp,
                       public IWindowCommitListener
{
public:
    enum {
        WM_USER_REFRESH = WM_USER + 0x100,
    };

    enum MOUSE_ACTION
    {
        MOUSE_ACTION_NONE,
        MOUSE_ACTION_MOVE_WINDOW_DRAW_POS = 1,
    };

	CLayoutEditorDlg();
	~CLayoutEditorDlg();
    virtual void Destroy() override {
        this->Release();
    }

    UI_BEGIN_MSG_MAP_Ixxx(CLayoutEditorDlg)

        if (m_eMouseAction != MOUSE_ACTION_MOVE_WINDOW_DRAW_POS)
        {
            if (m_recttracker.ProcessMessage(uMsg, wParam, lParam, &lResult))
                return TRUE;
        }

        UIMSG_PAINT(OnPaint)
        UIMSG_ERASEBKGND(OnEraseBkgnd)
        UIMSG_SIZE(OnSize)

        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_RBUTTONUP(OnRButtonUp)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)

        UIMSG_DROPTARGETEVENT(OnDropTargetEvent)
        UIMSG_HANDLER_EX(WM_USER_REFRESH, OnRefresh)

        UIMSG_HANDLER_EX(MDIVIEW_MESSAGE_PREVIEW, OnBtnClickPreView)

        // todo:
		// UIMSG_HANDLER_EX(UI_WM_PREVIEW_WINDOW_DESTROY, OnPrevViewWindowDestroy)

        UIMSG_KILLFOCUS(OnKillFocus)
        MSG_WM_CANCELMODE(OnCancelMode)

        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    void OnPaint(IRenderTarget* pRenderTarget);
    void OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void OnInitialize();
	void DoPaintBkgnd(CDCHandle& dc);
	void DoPaint(CDCHandle& dc);
    void OnSize(UINT nType, int cx, int cy);
    LRESULT  OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
    BOOL OnSetCursor(HWND wnd, UINT nHitTest, UINT message);

    void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void OnKillFocus(IObject* pNewFocusObj);
    void OnCancelMode();

    void OnDropTargetEvent(DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData);
	LRESULT OnBtnClickPreView(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPrevViewWindowDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma  region
    virtual void**  QueryInterface(REFIID riid);
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { m_pSkin = hSkin; }

	virtual UI_RESOURCE_TYPE  GetResourceType(){ return UI_RESOURCE_LAYOUT; }
    virtual HWND  GetHWND() { return IWindow::GetHWND(); }
	virtual void  OnMDIActivate(bool bActivate);
	virtual long*  GetKey();

	virtual bool ShowLayoutNode(LayoutTreeItemData* pData);
    virtual IWindow*  GetWindow() { return m_pWindow; }
	virtual void  OnObjectDeleteInd(IObject* p);
#pragma  endregion

#pragma region
    virtual void  OnRectTrackerDragStart();
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta);
    virtual void  OnRectTrackerDragEnd();
    virtual void  DP2LP(POINT* p);
#pragma endregion

#pragma region
    virtual void PreCommit(LPCRECT dirtyarray, int count) override;
    virtual void PostCommit(LPCRECT dirtyarray, int count) override;
#pragma endregion

protected:
	IObject*  CreateObject(LPCTSTR szNodeName, LPCTSTR szNodeID);
	void     LoadObjectChildNode(IObject* pParent);

	void  SelectObject(IObject* pObject);
    void  PrepareDC(HDC hDC);
    void  UnPrepareDC(HDC hDC);
    void  Render();

    IObject*  GetContainerObject(IObject* pCurObj);
	void  UpdateTitle();

    void  SetCurSelectObject(IObject*  pSelectObj);
    void  SetWindowDrawRect(RECT* prc);
    void  UpdateRectTracker();
	void  OffsetWindowDrawRect(int x, int y);
    
private:
    LayoutPropertyHandler  m_PropertyHandler;

	// Ч��ͼ��ˢ
	HBRUSH  m_hBkBrush;

	// ����͸����
	int  m_nWindowRenderAlpha;

	ISkinRes*  m_pSkin;
	IWindow*   m_pWindow;
	IObject*   m_pCurSelectObject;  // ���ñ�����ֵ����ʹ��SetCurSelectObject

    HCURSOR  m_hCursorHand;
    HCURSOR  m_hCursorHandPress;
    
	RECT  m_rcWindowDraw;  // UI���ڻ��Ƶ���������, �߼����ꡣ���ñ�����ֵ�������SetWindowDrawRect
    int   m_nZoom;     // �Ŵ����  100%
    bool  m_bCenter;   // ���л���

    RectTracker  m_recttracker;
    CRect  m_rectObjectSave;    // ��ק�ؼ���Сǰ����ÿؼ�λ��

    POINT  m_ptLastClick;
    POINT  m_ptLastMouseMove;
    MOUSE_ACTION  m_eMouseAction;

	

};

