#pragma once
#include "Inc\Interface\isliderctrl.h"

namespace UI
{
interface IButton;

// ����ؼ�(����ť�Ĺ�����)
class SliderCtrl : public MessageProxy
{
public:
	SliderCtrl(ISliderCtrl* p);
	~SliderCtrl();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		UIMSG_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)
		UIMSG_STATECHANGED(OnStateChanged)
		//UIMSG_STYLECHANGED(OnStyleChanged)
        UIMSG_QUERYINTERFACE(SliderCtrl)
        UIMSG_SERIALIZE(OnSerialize)
		UIMSG_INITIALIZE(OnObjectLoaded)

		// ��ť���¼�
	UIALT_MSG_MAP(1)   
		MSG_WM_LBUTTONDOWN(OnBtnLButtonDown)	
		MSG_WM_MOUSEMOVE(OnBtnMouseMove)	
		MSG_WM_LBUTTONUP(OnBtnLButtonUp)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)

    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SliderCtrl, IControl)

public:
	void  OnSerialize(SERIALIZEDATA* pData);
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine){ m_nLine = nLine; }
    int   GetPos(){ return m_nCur; }
    int   GetPage(){ return m_nPage; }
    int   GetLine(){ return m_nLine; }
    void  GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
    int   GetRange() { return m_nMax - m_nMin; }

    void  SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
    PROGRESSBAR_SCROLL_DIRECTION_TYPE  GetDirectionType() { return m_eDirectionType; }

	void  UpdateButtonRect();
	int   SetTickFreq(int nFreq);

protected:
	void  OnObjectLoaded();
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnMouseMove(UINT nFlags, POINT point);
	void  OnBtnLButtonUp(UINT nFlags, POINT point);
	void  OnMove(POINT pt );
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSize(UINT nType, int cx, int cy );
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	void  OnStateChanged(UINT nMask);

	void  UpdateTicksData();
	void  UpdateUIData(bool bCalBeginEnd=false, bool bUpdateButtonRect=true);
	void  CalcCurPosByUITrackPos(int nWindowPos);

	int   WindowPoint2UITrackPos(const POINT* ptWindow);
	
    int   FixCurValue(int nCur);

    void  show_tooltip();
    void  update_tooltip();
    void  hide_tooltip();

	void  firePosChanged(int pos, int type);

protected:
    ISliderCtrl*  m_pISliderCtrl;

    int  m_nMin;        // ��Сֵ
    int  m_nMax;        // ���ֵ
    int  m_nCur;        // ��ǰλ��
    int  m_nLine;       // ÿ�ΰ���ʱ�������ľ���
    int  m_nPage;       // һ��ҳ��Ĵ�С
    PROGRESSBAR_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // ����������,����

	// ������ʾ����
	int  m_nBegin;     // ��Ӧm_nMin����������ʼ���Ƶ����꣬PADDING + half of button size
	int  m_nEnd;       // ��Ӧm_nMax���������������Ƶĵط���Control Size - (PADDING + half of button size )
	int  m_nTrack;     // ��Ӧm_nCur����ǰ��ť���ڵ����꣬middle of button

	// show
	IButton*  m_pButton;    // ��ק�õİ�ť�ؼ������ฺ��ʵ����
	int  m_nDiffFix;   // ����������϶�ʱ�������core m_nTrack��ƫ��

	// �̶Ȼ������
	IRenderBase*  m_pTickRender; // �̶Ȼ���
	int  m_nFreq;       // auto tickʱ��ָ����ʾ�̶ȵ�Ƶ��
	struct TickInfo
	{
		int    nPos;              // ��ʾ��һ���̶�
		CRect  rcRect;            // �ÿ̶����Ҫ��ʾλ�ã����ڴ�С�������ı����Ҫ���¼��㣩
	};
	vector<TickInfo*>  m_vTickInfo;

    // TOOLTIP
    HWND  m_hToolTip;
    TOOLINFO m_toolinfo;

public:
	signal<SliderPosChangedParam*>  PosChanged;
};
}