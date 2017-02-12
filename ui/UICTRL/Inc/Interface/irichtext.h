#ifndef _UI_IRICHTEXT_H_
#define _UI_IRICHTEXT_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
	class CCacheImage;

struct CharFormat
{
    // HFONT����
    TCHAR  szFontName[LF_FACESIZE];
    long  lFontSize;
    bool  bBold;
    bool  bItalic;
    bool  bUnderline;
	bool  bDeleteline;

    // ��������
    long  textColor;
    long  backColor;
    bool  bTextColor;  // �Ƿ���������ɫtextColor
    bool  bBackColor;  // �Ƿ����ñ���ɫbackColor

    // �ڲ�ʹ��
    HFONT  hFont;
    unsigned int  nFontHeight;
    unsigned int  nFontDescent;  // �����»��ߵĻ����������
};


enum /*class*/ PageMode
{
    PageMode_singleline,
    PageMode_multiline,
    PageMode_multipage,
    PageMode_error,
};

namespace RT
{
// Ŀǰֻʵ���˵���ģʽ
enum /*class*/ VALIGN // : int
{
    VALIGN_top,
    VALIGN_bottom,
    VALIGN_center,
};

enum /*class*/ HALIGN // : int
{
    HALIGN_left,
    HALIGN_right,         // ÿһ�����ݶ��Ҷ���
    HALIGN_center,        // ÿһ�����ݶ����ж���
    HALIGN_paraRight,     // ���������Ҷ��룬�����е��л��������
    HALIGN_paraCenter,    // ����������ж��룬�����е��л��������
};

// �༭ģʽ
enum EDITABLE
{
	EDITABLE_Disable,   // ��֧�ֱ༭������ʾ��꣬��֧�ָ���
	EDITABLE_Readonly,  // ��ʾ��֧꣬�ָ��ƣ�δʵ�֣�
	EDITABLE_Enable,    // ֧�ֱ༭��δʵ�֣�
};

interface IParagraph
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
    virtual void  SetLeftIndent(int indent) = 0;
    virtual void  SetRightIndent(int indent) = 0;
    virtual void  SetTopSpace(int space) = 0;
    virtual void  SetBottomSpace(int space) = 0;
    virtual void  SetCharformat(CharFormat*) = 0;
    virtual void  ShowBubbleBackgournd(bool b) = 0;
};

interface IChatMessage
{
    virtual void  SetAlign(RT::HALIGN align) = 0;
	virtual void  SetPortrait(const CCacheImage& portrait) = 0;
    virtual HANDLE  EnterBubble() = 0;
    virtual void  LeaveBubble(HANDLE) = 0;
};

// interface INode
// {
//     virtual IParagraph*  GetParagraphNode() = 0;
// };

// interface ISelection
// {
//     virtual INode*  GetAnchorNode() = 0;
//     virtual INode*  GetFocusNode() = 0;
// };


interface ILink
{
	virtual int  GetType() = 0;
	virtual void  SetType(int) = 0;

    virtual LPCTSTR  GetId() = 0;
    virtual void  SetId(LPCTSTR) = 0;

    virtual LPCTSTR  GetToolTip() = 0;
    virtual void  SetToolTip(LPCTSTR) = 0;

    virtual LPCTSTR  GetReference() = 0;
    virtual void  SetReference(LPCTSTR) = 0;

	virtual IParagraph*  GetParagraph() = 0;
};

interface IImageElement
{
	virtual LPCTSTR  GetPath() = 0;
};

}


#ifdef ReplaceText
#undef ReplaceText
#endif

class CCacheImage;
class RichText;
interface UICTRL_API_UUID(F1B67364-BA72-44CD-BA63-53DA6B23CB4A) IRichText : public IControl
{
    void  Clear();
    void  SetText(LPCTSTR szText);
    void  GetDefaultCharFormat(CharFormat* pcf);
	void  SetDefaultCharFormat(CharFormat* pcf);
	void  GetDefaultLinkCharFormat(CharFormat* pcf);
	void  SetDefaultLinkCharFormat(CharFormat* pcf);
    
	void  LoadHtml(LPCTSTR szHtml);

	void  SelectEnd();
	void  SelectBegin();
	void  ScrollEnd();

    void  ReplaceText(LPCTSTR szText);
    void  ReplaceTextEx(LPCTSTR szText, CharFormat* pcf);
    void  ReplaceImage(LPCTSTR szPath);
	void  ReplaceCacheImage(const CCacheImage& image);
    RT::ILink*  ReplaceLink(LPCTSTR);
    RT::ILink*  ReplaceLinkEx(LPCTSTR, CharFormat* pcf);
    RT::IChatMessage*  BeginInsertMessage();
    void  EndInsertMessage(RT::IChatMessage*);
	void  DeleteParagraph(RT::IParagraph*);

    void  SetPageMode(PageMode e);
    PageMode  GetPageMode();
	void  SetEditMode(RT::EDITABLE e);
	RT::EDITABLE  GetEditMode();

    //RT::ISelection*  GetSelection();
    RT::IParagraph*  GetSelectionAnchorParagraph();

    void  SetStyle_PageContentVAlign(RT::VALIGN);
    void  SetStyle_PageContentHAlign(RT::HALIGN);
    void  SetStyle_DrawLinkUnderline(bool b);
	void  SetStyle_LineSpace(int linespace);


	signal2<IRichText*, RT::ILink*>&  LinkClickedEvent();
	signal2<IRichText*, RT::IImageElement*>&  ImageDBClickEvent();

	UI_DECLARE_INTERFACE_ACROSSMODULE(RichText);
};


}

#endif // _UI_IRICHTEXT_H_