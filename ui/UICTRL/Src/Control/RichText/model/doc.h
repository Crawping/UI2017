#pragma once
#include "Inc\Interface\irichtext.h"
#include "node.h"
#include "style.h"
#include "selection.h"
#include "mousekey.h"
#include "layout_context.h"
#include "..\UISDK\Inc\Util\iimage.h"


//
// richtext����ģ�ͣ�
// ���ݶ���  
//  Doc, Element, Paragraph
// ���ֶ���
//  Page, Line, Run
//
// Doc������һ��RichText��ά��page�б�
// Unit: �����ⲿ�����һ�����ݵ�Ԫ���糬���ӣ���Ϊһ��������unit����ͳһˢ�¡������// ά��Element�б�
//       ��������Ϊһ��������й������������ռ����б�
// Element: ��\r\n��uniscribe��һ��Unit���ݲ��Ϊ���Element��Element����������������
// Page: ���ı���һҳ��ά��Line�б�
// Line: һ�С�ά��Run�б�
// Run: һ��Element�ڲ���ʱ��ֳ����ĵ�Ԫ�����һ��װ����һ��Element���������Ϊ���Run
// Param: ���������Ű棬���ö����ʽ
//
// Element �����֣� TextElement/ObjectElement
// Unit �����������ࣺ TextUnit/ObjectUnit���������͵�unitֱ�Ӵ�������unit��������
// 
namespace UI
{
namespace RT
{
class Page;
class Element;
class TextElement;
class ImageUnit;
class Line;
class Doc;
class Paragraph;

class Doc : public Node
{
public:
	Doc();
	~Doc();

    virtual NodeType  GetNodeType() override { return NodeTypeDoc; }
    virtual const CharFormat*  GetCharFormat() override;

	void  Init(RichText* pObj);
	void  Clear();
	RichText*  GetControl();
    IRichText*  GetIControl();
    Style&  GetStyle();
    Selection&  GetSelection();
    MouseKey&  GetMouseKey();
    Paragraph*  GetSelectionAnchorParagraph();
	IScrollBarManager*  GetScrollBarManager();

    PageMode  GetPageMode();
    EDITABLE    GetEditMode();
    void  SetPageMode(PageMode e);
    void  SetEditMode(EDITABLE e);

	void  Render2DC(HDC hDC, LPCRECT rcClip);
    void  LayoutAll(uint nControlWidth, uint nControlHeight);
    SIZE  GetDesiredSize();

    void  CopyDefaultCharFormat(CharFormat* pcf);
	void  CopyDefaultLinkCharFormat(CharFormat* pcf);
	const CharFormat*  GetDefaultCharFormat();
	const CharFormat*  GetDefaultLinkCharFormat();

    void  InvalidRegion(RECT* prcInDoc);
    void  ClearInvlidRegion();
    void  Refresh();
	void  ReleasePage();

	void  SetDefaultFont(CharFormat* pcf);
	void  SetDefaultLinkFont(CharFormat* pcf);
	void  CreateDefaultFont();
	void  CreateDefaultLinkFont();
     
    // selection ����
    void  CopySelection();
    void  SelectAll();
	void  SelectEnd();
	void  SelectBegin();
    void  SetNeedUpdateElementIndex();
    void  UpdateElementIndex();
	void  ScrollEnd();

    void  ReplaceText(LPCTSTR szText);
    void  ReplaceTextEx(LPCTSTR szText, const CharFormat* pcf);
    ILink*  ReplaceLink(LPCTSTR szText);
    ILink*  ReplaceLinkEx(LPCTSTR szText, const CharFormat* pcf);
    void  ReplaceImage(LPCTSTR szPath);
	bool  ReplaceCacheImage(const CCacheImage& image);
	void  DeleteParagraph(Paragraph*);

    IChatMessage*  BeginInsertMessage();
    void  EndInsertMessage(IChatMessage*);

    Page*  GetFirstPage();
    Page*  GetLastPage();
    Page*  PageHitTest(POINT ptDoc, __out POINT* ptInPage);

	// style
	void  SetStyle_LineSpace(int linespace);

	BOOL  ProcessMessage(UIMSG*, int, bool);
	void  OnNodeRemove(Node* p);

private:
    void  replaceTextNoCarriageReturn(
            LPCTSTR szText, uint length, const CharFormat* pcf);
	void  insertSimpleNodeAtSelection(Node*);
	void  insertParaNodeAtSelection(Paragraph*);

	void  SetContentSize(int cx, int cy);
	void  SetViewSize(int cx, int cy);

private:
	void  initBlank();
	
	virtual void  PreLayoutAll();
	void  layoutAllSingleLine(
            uint nControlWidth,
            uint nControlHeight
            );
    void  layoutAllSingleLineEachUnit(
            Unit* pUnit,
            SingleLineLayoutContext* context
            );
	void  layoutAllMultiLine(
            uint nControlWidth,
            uint nControlHeight
            );
	void  layoutAllMultiPage(
            uint nControlWidth, 
            uint nControlHeight
            );
    virtual void  PostLayoutAll() {};
    virtual void  PreRender2DC(HDC) {};
    virtual void  PostRender2DC(HDC) {}

	// bool  layout_all_multiline_imageelement(MultiLineLayoutContext* pContext);
    // void  append_text(LPCTSTR szText, TextUnit* pUnit, RichTextCharFormat* pcf);


protected:
    RichText*  m_pRichText;

    // ����
	CharFormat  m_defaultcf;  
	CharFormat  m_defaultcf_link;  
    Style  m_style; 

	Page*  m_pFirstPage;
    PageMode  m_ePageMode;

    EDITABLE  m_eEditMode;
    Selection  m_selection;
    MouseKey  m_mousekey;

	// ���ݴ�С
	SIZE  m_sizeContent;
	SIZE  m_sizeView;
	IScrollBarManager*   m_pMgrScrollBar;

    // ��Ч����
    enum InvalidState
    {
        InvalidStateEmpty,
        InvalidStateFull,
        InvalidStatePart,
    };
    InvalidState  m_eInvalidState;
    RECT  m_rcInvalidInDoc;  // ȫ0��ʾ��ˢ�£�ȫ-1��ʾȫ��ˢ�£�������ʾˢ��ˢ��

    // Ϊÿ��element���򣬷���Ƚ�����element��ǰ��˳��
    // ��Element�๹�캯�������ø�ֵ��Ŀǰֻ����ÿ������һ��elementʱ���¡�
    bool  m_bNeedUpdateElementIndex;

public:
    ImageWrap  m_imageLeftBubble;
    ImageWrap  m_imageRightBubble;
    
};

}
}