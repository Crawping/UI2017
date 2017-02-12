#pragma once
#include "Inc\Interface\irectracker.h"

namespace UI
{
//
// 2017/1/25
// ע�� IRectTrackerRsp�����𲽱��������������¼��ķ�ʽ���ڸ���ά��һ��
//
class RectTracker : public MessageProxy
{
public:
    RectTracker(IRectTracker*);
    ~RectTracker();

    void Init();
    void Release();

    UI_BEGIN_MSG_MAP()
        if (m_bStartup)
        {
            MSG_WM_SETCURSOR(OnSetCursor)
            UIMSG_PAINT(OnPaint)
            MESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK, OnLButtonDown)
            MESSAGE_HANDLER_EX(WM_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLER_EX(WM_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLER_EX(WM_MOUSEMOVE, OnMouseMove)
            MESSAGE_HANDLER_EX(WM_KEYDOWN, OnKeyDown)
            MESSAGE_HANDLER_EX(WM_KEYUP, OnKeyUp)
            MESSAGE_HANDLER_EX(WM_CANCELMODE, OnCancelMode)
            MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel) 
        }
		MESSAGE_HANDLER_EX(WM_WINDOWPOSCHANGED, OnPosChanged)
		UIMSG_SERIALIZE(OnSerialize)
        UIMSG_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RectTracker, IControl)

public:
	IRectTracker*  GetIRectTracker();

    void  SetRsp(IRectTrackerRsp* prsp);
    void  SetRealRectInParent(RECT* prcRealObj);
	void  SetHandleMask(UINT nMask);
	UINT  GetHandleMask();
    void  GetRealRectInWindow(RECT* prc);
    void  GetRealRectInControl(RECT* prc);
	void  GetRealRectInParent(RECT* prc);
    int   GetHandleSize() { return m_nHandleSize; }
    int   GetLineSpace() { return m_nLineSpace; }
    void  SetDrawBorder(bool b) { m_bDrawBroder = b; }

    void  SetCursor(int nIndex, HCURSOR hCursor);

private:
    void  OnPaint(IRenderTarget*);
	void  OnInitialize();
	void  OnSerialize(SERIALIZEDATA* pData);

    BOOL  OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void  DrawGdi(HDC hDC);
    void  DrawGdiplus(HDC hDC);

    RectTrackerHit  HitTest(POINT  pt);
    RectTrackerHit  HitTestUnderCursor();

    bool   IsContainer();
    bool   CanSize();
    bool   CanMove();

	void   GetHandleRect(RectTrackerHit nIndex, RECT* prc, bool bControlRect);
    void   GetBolderRectInControl(RECT* prc);
    void   GetBolderRectInWindow(RECT* prc);
	void   UpdateRealRect();
    void   CalcWindowRectFromRealRect(RECT* prcReal, RECT* prcCtrl);
	// void   CalcParentRectFromRealRect(RECT* prcReal, RECT* prcCtrl);

	void  OnRectTrackerDragStart();
	void  OnRectTrackerDragEnd();
	void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta);

public:
	bool     m_bStartup;           // �Ƿ���ѡ�ж���
    bool     m_bDrawBroder;        // �Ƿ���Ʊ߿��ߡ�

    CRect    m_realRect;           // �������꣬ʵ�ʵĶ���λ�ã����ǿؼ�λ�á����������ſؼ�����
    int      m_nHandleSize;  
    int      m_nLineSpace;         // �߿����������֮����

    UINT     m_nMask;   
    IRectTrackerRsp*  m_prsp;      // ��Ӧ��
	UI::Color  m_colorBorder;      // �߿���ɫ

    RectTrackerHit  m_eActionHit;  // ��ǰ���/������קʱ�Ĳ���

    bool     m_bMouseDraging;      // ��־���Ƿ��������ק
    POINT    m_ptLButtonDown;      // ��¼��갴��ʱ��λ�ã������ƶ��ؼ�
    
    bool     m_bKeyboardDraging;   // ��־���Ƿ񿪻�������ק
    UINT     m_nKeyPushdown;       // �������ĸ���
    long     m_nKeyboardHitCount;  // �����¼���������(��ס����)

    HCURSOR  m_hCursor[10];        // ��ӦRectTrackerHit�����ⲹ��һ��hitMiddle��קʱ��갴�µ���ʽ

	CRect  m_rectObjectSave;       // ��ק�ؼ���Сǰ����ÿؼ�λ��
	bool   m_bLimitBorderInParent; // ���߿�Ҳ�����ڸ������ڲ�������ؼ��Ҷ��볡����BUG

private:
    IRectTracker*  m_pIRectTracker;

public:
	signal2<IRectTracker*, LPCRECT>  rect_changed;
	signal<IRectTracker*>  click;
};

}