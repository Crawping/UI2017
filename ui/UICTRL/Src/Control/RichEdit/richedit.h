#pragma once
#include "windowlessricheditex.h"
#include "UI\UICTRL\Inc\Interface\irichedit.h"
#include "..\ScrollBar\vscrollbar_creator.h"

//
//	RichEdit�༭��ؼ�
//
//  ��������ڰ�װWindowlessRichEdit��ΪWindowlessRichEdit�ṩһ����������Ϣת����
//  ΪWindowlessRichEdit�ṩ���귶Χ���ⲿ��Ϣ�ȡ�
//  RichEdit��Ӧ�ķ����ͽӿ�ȫ����WindowlessRichEditʵ�֣��ⲿ���в���ʱ����ֱ�ӻ�
//  ȡWindowlessRichEdit��ָ����е��á�
//
namespace UI
{
interface IScrollBarManager;
interface ISystemVScrollBar;

class  RichEdit : public MessageProxy
{
public:
	RichEdit(IRichEdit* p);
    ~RichEdit();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT( OnPaint )
		UIMSG_ERASEBKGND( OnEraseBkgnd )
		// TO DELETE UIMSG_HANDLER_EX( UI_WM_REDRAWOBJECT, OnRedrawObject )

		UIMSG_HANDLER_EX( WM_SETCURSOR, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_CHAR, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_KEYDOWN, OnKeyDown )
		UIMSG_HANDLER_EX( WM_KEYUP, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_WINDOWPOSCHANGED, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_TIMER, OnForwardMessage )
		UIMSG_HANDLER_EX( UI_MSG_GIFFRAME_TICK, OnForwardMessage)
		UIMSG_VISIBLE_CHANGED( OnVisibleChanged )
		UIMSG_SETFOCUS( OnSetFocus )
		UIMSG_KILLFOCUS( OnKillFocus )
		UIMSG_SIZE( OnSize ) 

		// ��Scrollmgr����֮ǰ������UI_WM_GETSCROLLOFFSET
		// ��Ϊrichedit�ڲ����Լ���ƫ����Ϣ��������ʹ��UI�ؼ���
		// ƫ����Ϣ������ͻᵼ�¹�꣬������ʾ����

		UIMSG_HANDLER_EX( WM_VSCROLL, OnScroll )
		UIMSG_HANDLER_EX( WM_HSCROLL, OnScroll )
		UIMSG_HANDLER_EX( UI_MSG_GETSCROLLOFFSET,OnGetScrollOffset )
		UIMSG_HANDLER_EX( WM_MOUSEWHEEL, OnForwardMessage )
		// TODO: UIMSG_WM_GETDLGCODE( OnGetDlgCode )
		UIMSG_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnForwardMessage)
		UIMSG_HANDLER_EX( WM_IME_ENDCOMPOSITION, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_IME_COMPOSITION, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_IME_REQUEST, OnForwardMessage )
		UIMSG_HANDLER_EX( WM_IME_CHAR,  OnForwardMessage )

	//	UICHAIN_MSG_MAP_POINT_MEMBER(m_pMgrScrollbar)  
		MSG_WM_RBUTTONDOWN( OnRButtonDown )
		MESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST,WM_MOUSELAST, OnForwardMessage )
        UIMSG_HANDLER_EX( WM_MOUSELEAVE, OnForwardMessage )

        UIMSG_HANDLER_EX( UI_MSG_DROPTARGETEVENT, OnForwardMessage )

		UIMSG_INITIALIZE( OnInitialize )
        UIMSG_QUERYINTERFACE(RichEdit)
        UIMSG_SERIALIZE( OnSerialize )
        UIMSG_FINALCONSTRUCT(FinalConstruct)

    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RichEdit, IControl)

    IRichEdit*  GetIRichEdit();

public:
	WindowlessRichEdit& GetRichEdit();
	IScrollBarManager*  GetScrollMgr();
    CCaret*  GetCaret();
    void  SetReadOnly(bool fReadOnly);
    void  EnableBubbleMode(bool b);
    bool  IsBubbleMode();

    void  InvalidateRect(RECT* prcWnd);
    void  InvalidateRect2(RECT* prcWndArray, int nCount);
    HRESULT  OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled);
    void  OnContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg);

    void  RegisterDropEvent();
    void  RevokeDropEvent();

protected:
    HRESULT  FinalConstruct(ISkinRes* p);
    void  OnSerialize(SERIALIZEDATA* pData);
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

	void  OnInitialize();
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnPaint(IRenderTarget* pRenderTarget);
	LRESULT  OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT  OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void  OnSetFocus(IObject* pOldFocusObj);
	void  OnKillFocus(IObject* pNewFocusObj);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnRButtonDown(UINT nFlags, CPoint point);
	UINT  OnGetDlgCode(LPMSG lpMsg);
	
	void  OnVisibleChanged(BOOL bVisible, IObject* pParent);

    long  OnRequestAutoSize(RECT* prcRequest);
    void  OnEnChanged(BOOL& bHandled);
	void  OnEnLink(ENLINK *pLink);

private:
    void  set_bubble_left_image(LPCTSTR);
    void  set_bubble_right_image(LPCTSTR);
    LPCTSTR  get_bubble_left_image();
    LPCTSTR  get_bubble_right_image();

protected:
    IRichEdit*  m_pIRichEdit;

	WindowlessRichEditEx   m_wrapRichEidt;
	IScrollBarManager*   m_pMgrScrollBar;

    VScrollbarCreator  m_vscrollbarCreator;

    CCaret   m_caret;         // ���
	bool   m_bRedrawing;      // �����������ⲿֱ�ӵ��õ�UpdateObject������OnRedrawObject���õ�UpdateObject
    bool   m_bPostRedrawMsg;  // ��ǵ�ǰ�Ѿ�post��һ��redraw��Ϣ�����һ�û����Ӧ���ڴ��ڼ䲻��post�ڶ���redraw��Ϣ
    CRect  m_rcInvalidate;    // TxInvalidateRect������������Ч����ļ���

    bool  m_bByteLimit;

    // ����Ӧ��С�༭��
    bool  m_bAutoSize;
    long  m_lSingleLineMaxWidth;
    long  m_lSingleLineMinWidth;
    long  m_lMultiLineMaxHeight;
    long  m_lMultiLineMinHeight;

    // ����ģʽ
    bool  m_bBubbleMode;
    
	// ������ʾ
	ITextRenderBase*  m_pPromptFont;
	String  m_strPrompt;

public:
	signal2<IRichEdit*, IDataObject*>  QueryPaste;
	signal2<IRichEdit*, ENLINK*>  EnLink;
	signal_r2<bool, IRichEdit*, UINT>  Keydown;
};

}