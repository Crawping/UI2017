#ifndef  _INCLUDED_IRECTTRACKER_H_
#define  _INCLUDED_IRECTTRACKER_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{


enum RectTrackerHit
{
    hitNothing = -1,
    hitTopLeft = 0, 
    hitTopRight = 1, 
    hitBottomRight = 2, 
    hitBottomLeft = 3,
    hitTop = 4, 
    hitRight = 5, 
    hitBottom = 6, 
    hitLeft = 7, 
    hitMiddle = 8,   // ���ڱ�ʾ��קmove
    hitCount = 9,    // 
};

#define XML_RECTTRACKER_MASK   TEXT("mask")
#define XML_RECTTRACKER_MASK_NONE  TEXT("none")
#define XML_RECTTRACKER_MASK_MOVE  TEXT("move")
#define XML_RECTTRACKER_MASK_SIZE  TEXT("size")
#define XML_RECTTRACKER_MASK_SIZEMOVE  TEXT("sizemove")
enum RectTrackerMask
{
    recttrackermask_none = 0,
    recttrackermask_move = 1,
    recttrackermask_size = 2,
    recttrackermask_sizemove = 3,
    recttrackermask_iscontainer = 4,  // ����һ���������ڲ���������Ԫ��
};

#define XML_RECTTRACKER_HANDLE_SIZE   TEXT("handlesize")
#define XML_RECTTRACKER_LINE_SPACE    TEXT("linespace")
#define XML_RECTTRACKER_DRAW_BORDER    TEXT("drawborder")
// ��BORDERҲ�����ڸ������ڲ�����������real rect��
#define XML_RECTTRACKER_LIMIT_BORDER_IN_PARENT    TEXT("limitborderinparent")


// ������֪ͨ
// wParam/lParamͬWM_MOUSEWHEEL
// return: 0δ���� 1�Ѵ���
#define RectTracker_NM_MOUSEWHEEL  153201717

interface  IRectTrackerRsp
{
    // ֪ͨ�ⲿ��ʼ�ƶ����ȱ���һ��ԭʼλ��
    virtual void  OnRectTrackerDragStart() = 0;
    // ��ԭʼλ�õĻ������ƶ��ľ���
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta) = 0;
    // ֪ͨ�ⲿ�ƶ����������浱ǰֵ
    virtual void  OnRectTrackerDragEnd() = 0;
    // ����
    virtual void  Draw(HDC) = 0;
    // ����ת��
    virtual void  DP2LP(POINT* p) = 0;
	// �ͷ�
	virtual void  Release() = 0;
};

class RectTracker;
interface __declspec(uuid("5B6657F0-9944-4DFA-9B1C-538AB64B005E")) UICTRL_API IRectTracker : public IControl
{
    void  SetRsp(IRectTrackerRsp* prsp);
    void  SetRealRectInParent(RECT* prc);
    void  SetHandleMask(UINT nMask);
    void  GetRealRectInWindow(RECT* prc);
    void  GetRealRectInControl(RECT* prc);
	void  GetRealRectInParent(RECT* prc);
    int   GetHandleSize();
    int   GetLineSpace(); 
    void  SetDrawBorder(bool b);
    void  SetCursor(int nIndex, HCURSOR hCursor);
	void  SetHandleMask(RectTrackerMask mask);

	signal2<IRectTracker*, LPCRECT>&  RectChangedEvent();
	signal<IRectTracker*>&  ClickEvent();

	UI_DECLARE_INTERFACE_ACROSSMODULE(RectTracker);
};


}


#endif // _INCLUDED_IRECTTRACKER_H_