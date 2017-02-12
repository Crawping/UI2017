#ifndef _UI_IRICHEDIT_H_
#define _UI_IRICHEDIT_H_
#include "iricheditole.h"
#include "iricheditbubble.h"
#include <richedit.h>
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
    
// ���ⲿת��֪ͨ��Ϣ
// message: UI_WM_RICHEDIT_TXNOTIFY   
// wParam:  DWORD iNotify
// lParam:  void* pv
// return:  HRESULT
#define UI_WM_RICHEDIT_TXNOTIFY   137181715

// ֪ͨ�ⲿ����༭������Ӧ��С
// wParam: RECT*  pRequestSize
// lParam: int* nDefaultDesiredWidth/Height (�ⲿ���޸�)
// return: 1�Ѵ��� 0δ��������nDefaultDesiredWidth���޸Ĵ�С
#define UI_EN_REQUESTAUTOSIZE  149251035

// ֪ͨ�ⲿ�������Ӧ��СĬ�ϴ���
#define UI_EN_AUTOSIZE_DONE  149251053

// Ĭ�ϲ˵���ID����
#define RE_MENU_ID_CUT     152791728
#define RE_MENU_ID_COPY    152791729
#define RE_MENU_ID_PASTE   152791730
#define RE_MENU_ID_SELALL  152791731

// seltype
//  Selection type. The value, which specifies the contents of the new
//  selection, can be one or more of the following values.
//  SEL_EMPTY
//      The selection is empty.
//  SEL_TEXT
//      Text.
//  SEL_OBJECT
//      At least one Component Object Model (COM) object.
//  SEL_MULTICHAR
//      More than one character of text.
//  SEL_MULTIOBJECT
//      More than one COM object.
//  GCM_RIGHTMOUSEDROP
//      Indicates that a context menu for a right-mouse drag drop should 
//      be generated. lpoleobj parameter is a pointer to the IDataObject 
//      interface for the object being dropped.
//    
// lpoleobj
//  Pointer to an interface. If the seltype parameter includes the 
//  SEL_OBJECT flag, lpoleobj is a pointer to the IOleObject interface for
//  the first selected COM object. If seltype includes the
//  GCM_RIGHTMOUSEDROP flag, lpoleobj is a pointer to an IDataObject 
//  interface. Otherwise, lpoleobj is NULL. If you hold on to the interface
//  pointer, you must call AddRef to increment the object's reference count. 
//
// lpchrg
//  Pointer to a CHARRANGE structure containing the current selection. 
struct  ENCONTEXTMENUDATA
{
    WORD seltype;
    LPOLEOBJECT lpoleobj;
    CHARRANGE* lpchrg;
};
// �����Ĳ˵���������
// wParam: ENCONTEXTMENUDATA*
// return: HANDLED,�ⲿ�Ѵ����ڲ�����
//         NOT_HANLED,�ⲿδ�������ݵ���Ĭ�ϲ˵�
#define UI_REN_PRE_CONTEXTMENU   152791710

// �����Ĳ˵����������������Ĭ�ϲ˵���
// wParam: ENCONTEXTMENUDATA*
// wParam: IMenu*
#define UI_REN_POST_CONTEXTMENU  152791711

// �����Ĳ˵������������
// wParam: LONG lCmd
#define UI_REN_HANDLE_CONTEXTMENU_CMD 152791712

// �ڿ��������У������һ��δ�����ͼƬ����Ҫ�Ȼ��棬�õ�һ������·����������ճ��ʱ�޷�����
#define UI_REN_SAVE_COPYED_PICTURE  162761853

// wParam: LPCTSTR path  __in
// lParam: CCachedImage* __out
#define UI_REN_LOAD_CACHED_IMAGE  162761854
// wParam: LPCSTR url __in
// lParam: CCachedImage* __out
#define UI_REN_LOAD_CACHED_URL    162761855



// richedit
#define XML_RICHEDIT_MSG_BUBBLE_PREFIX        _T("msgbubble.")  // ��Ϣ���ݶ���
#define XML_RICHEDIT_MSG_BUBBLE_ENABLE        _T("enable")      // msgbubble.enable �Ƿ��������ݡ�ֻ�п����˸�������ڱ༭������������image
#define XML_RICHEDIT_MSG_BUBBLE_LEFT_PREFIX   _T("left.")       // ������ݶ���
#define XML_RICHEDIT_MSG_BUBBLE_RIGHT_PREFIX  _T("right.")      // �Ҳ����ݶ���
#define XML_RICHEDIT_MSG_BUBBLE_IMAGE         _T("image")       // ���ݱ���ͼƬ
#define XML_RICHEDIT_MSG_BUBBLE_IMAGE_9REGION _T("image.9region") // ����9������

#define XML_RICHEDIT_OLE_PREFIX               _T("reole.")  
#define XML_REOLE_PICTURE_PREFIX              _T("picture.")  
#define XML_REOLE_PICTURE_LOADING_IMAGE       _T("loading.image")
#define XML_REOLE_PICTURE_FAILURE_IMAGE       _T("failure.image")
#define XML_REOLE_MAGNIFIER_PREFIX            _T("magnifier.")
#define XML_REOLE_NORMAL                      _T("normal")
#define XML_REOLE_HOVER                       _T("hover")
#define XML_REOLE_PRESS                       _T("press")

#define XML_RICHEDIT_PROMPT_TEXT              _T("prompt")
#define XML_RICHEDIT_PROMPT_FONT_PREFIX       _T("prompt.")

class RichEdit;
class WindowlessRichEditEx;
interface __declspec(uuid("52D94210-595B-404E-8892-53EE2DC42EFA")) UICTRL_API IRichEdit : public IControl
{
    void  SetWindowlessRichEdit(WindowlessRichEditEx* pRE);

    bool  SetText(LPCTSTR szText);
    int   GetText(TCHAR* szBuf, int nLen);
	void  GetRangeText(BSTR* pbstr, int nStart, int nSize);
	void  GetText(BSTR* bstr);
	bool  InsertText(LPCTSTR szText, int nLength=-1);
    bool  AppendText(LPCTSTR szText, int nLength=-1);
    // bool  GetEncodeTextW(IBuffer** ppBuffer);
    const char*  GetRtfBuffer(int nFlags=0);
    void   ReleaseRtfBuffer(const char*);
    bool  AppendEncodeText(LPCTSTR szText, int nSize);
    bool  InsertEncodeText(int nPos, LPCTSTR szText, int nSize);
    bool  InsertEncodeText(LPCTSTR szText, int nSize);
    HANDLE  InsertMessageStart(BubbleType eType, BubbleAlign eAlign);
    void  InsertMessageFinish(HANDLE hInsertStart);

    LONG  GetSelBarWidth();
    LONG  SetSelBarWidth(LONG l_SelBarWidth);
    bool  GetRichTextFlag();
    void  SetRichTextFlag(bool b);
    void  SetMultiLine(bool b);
    bool  IsMultiLine();
    bool  IsReadOnly();
    void  SetReadOnly(bool fReadOnly);
    void  SetAutoResize(bool b, int nMaxSize);
    bool  SetCharFormatByLogFont(LOGFONT* plf, COLORREF color);
    bool  SetCharFormat(CHARFORMAT2* pcf);
	void  SetSelectionCharFormat(CHARFORMAT2* pcf);
	void  SetSelectionParaFormat(PARAFORMAT2* ppf);
    void  GetDefaultCharFormat(CHARFORMAT2* pcf);
	void  SetCharFormatEx(UINT nPart, CHARFORMAT2* pcf);
    void  GetDefaultParaFormat(PARAFORMAT2* ppf);
    void  SetDefaultParaFormat(PARAFORMAT2* ppf);
	void  SetSel(int nPos, int nLen);
    void  SelectEnd();
    void  GetSel(int* pnPos, int* pnLen);
    void  DisableUndoOp();
    void  EnableDragDrop(bool b);
    bool  CanDragDrop();
    void  ScrollEnd();
	void  EnableWordBreak(bool b);
	void  Paste();
	void  SetLimitText(int nLimit);
    
    void  EnableBubbleMode(bool b);
    bool  IsBubbleMode();
	void  AppendBubbleSpace();

    bool  InsertGif(LPCTSTR szGifFile);
    bool  InsertSkinGif(LPCTSTR szGifId);

	IREOleBase*  GetOleByCharPos(int cp);

    IPictureREOle*  InsertPicture(PictureREOleInfo* pInfo);
	signal2<IRichEdit*, ENLINK*>&  EnLinkEvent();
	signal_r2<bool, IRichEdit*, UINT>&  KeydownEvent();

private:
    WindowlessRichEditEx*  m_pWindowlessRE;

	UI_DECLARE_INTERFACE_ACROSSMODULE(RichEdit);
};


enum TextFilterResult
{
    TFR_NOT_HANDLED,  // δ����ʹ��Ĭ��RE����
    TFR_HANDLED,      // �Ѵ�������ҪRE����
    TFR_REPLACE_TEXT  // δ���������ı������滻���ٴ���
};
enum TextFilterType
{
    TFT_NONE,
    TFT_DIGIT,
    TFT_UINT,
    TFT_PHONENUMBER,
    TFT_ACCOUNT,
	TFT_PASSWORD,
};
interface ITextFilter
{   
    // ��RE����ʱ������Ҫ�ٻ�ԭ״̬�ˣ�bRestoreState����Ϊfalse
    // ���滻textfilterʱ������Ҫ��bRestoreState����Ϊtrue
    virtual void  Release(bool bRestoreState) = 0;
    virtual TextFilterType  GetType() = 0;

    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled) = 0;
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled) = 0;
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled) = 0;
};


// ������ʽ����
#define RE_CF_ROOT             _T("RE")            // �����
#define RE_CF_VERSION          _T("version")       // �汾��Ϣ
#define RE_CF_FONT_BOLD        _T("bold")          // ����(1/0)
#define RE_CF_FONT_ITALIC      _T("italic")        // ����(1/0)
#define RE_CF_FONT_UNDERLINE   _T("underline")     // ����(1/0)
#define RE_CF_FONT_COLOR       _T("color")         // ��ɫ
#define RE_CF_FONT_FACE        _T("face")          // ����
#define RE_CF_FONT_HEIGHT      _T("height")        // �����С
#define RE_CF_PARA_ALIGN       _T("align")         // ���뷽ʽ
#define RE_CF_LINESPACING      _T("linespacing")   // �и�
#define RE_CF_LINEBEFORE       _T("linebefore")    // 
#define RE_CF_LINEAFTER        _T("lineafter")     // 

#define RE_CF_OLE_TEXT         _T("text")
#define RE_CF_OLE_GIF          _T("gif")
#define RE_CF_OLE_EMOTION      _T("emotion")
#define RE_CF_OLE_COM          _T("com")
#define RE_CF_OLE_PICTURE      _T("picture")
enum RICHEDIT_OLE_TYPE
{
    RICHEDIT_OLE_GIF_TEXT = 1,
    RICHEDIT_OLE_GIF_FILE = 2,
    RICHEDIT_OLE_EMOTION  = 3,
    RICHEDIT_OLE_COM  = 4,
    RICHEDIT_OLE_PICTURE  = 5,
};

#define RE_CF_OLE_TEXT_LENGTH   _T("length")       // �ı��ڵ㣬���ݴ�С
#define RE_CF_OLE_COM_CLSID     _T("clsid")        // com���clsid
#define RE_CF_OLE_ID            _T("id")           // ����id
#define RE_CF_OLE_PATH          _T("path")         // ·��
#define RE_CF_OLE_SUBTYPE       _T("type")         // ���ole��������
#define RE_CF_OLE_SUBDATA       _T("data")         // ���ole����չ����

}

namespace UI
{
	extern "C"
	UICTRL_API void UpdateREOleRes(LPCTSTR  szMD5, REOleLoadStatus e, LPCTSTR szPath);
}

#endif // _UI_IRICHEDIT_H_