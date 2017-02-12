#pragma once
#include "selection.h"

namespace UI {
    struct CharFormat;
class RichText;

    namespace RT {

class Doc;
class Paragraph;
struct SingleLineLayoutContext;
struct MultiLineLayoutContext;

struct NodeStyle
{
    bool hover : 1;
    bool press : 1;
};

enum  NodeType
{
    NodeTypeUnknown,
    NodeTypeVirtualNodeBegin = 100,

        NodeTypeDoc,
		NodeTypeChatMessage,
		NodeTypeChatMessageBubble,

		// ������paraǰ������ͣ���ʾpara���Է�����������Ϊ�ӽ��
        NodeTypeParagraph = 200,   

        NodeTypeLink,

//         NodeTypeChatMessageHead,
//         NodeTypeChatMessagePortrait,
    NodeTypeVirtualNodeEnd,

    NodeTypeRealNodeBegin = 300,
        NodeTypeText,
        NodeTypeImage,
        NodeTypeObject,
    NodeTypeRealNodeEnd,
};

class Node
{
public:
    Node();
    virtual NodeType  GetNodeType() = 0;

	// node��������ͳһ��Destroy����Destroy�ڲ��ٵ���delete this;
	void  Destroy();
protected:
	 virtual ~Node();
public:

    Node*  GetParentNode() { return parent; }
    Node*  GetChildNode() { return child; }
    Node*  GetPrevNode() { return prev; }
    Node*  GetNextNode() { return next; }

    Node*  GetLastChildNode();
    Node*  GetPrevTreeNode();
    Node*  GetNextTreeNode();

    Node*  GetRootNode();
    Paragraph*  GetParagraphNode();
    Node* GetAncestorNodeByType(NodeType);
    Doc*  GetDocNode();
	RichText* GetControl();
    
	void  DestroyDecendant();

    void  AddChildNode(Node*);
	void  AddFrontChildNode(Node*);
	void  InsertAfter(Node* pInsertAfter);
    void  TakePlaceAsMyParent(Node* pNewParent);
	void  RemoveFromTree(Node*);
	void  DeleteFromTree(Node*);
	bool  IsMyChild(Node* pChild, bool bFindInGrand);

    bool IsVirtualNode();
    bool IsRealNode();
	bool CanAddParagraphAsChildNode();
	bool IsPress();
	bool IsHover();

    void  SetSelectionState(SelectionState e);
    SelectionState  GetSelectionState();
    void  SetIndex(int);
    int   GetIndex();


    virtual void PreLayoutSingleLine(SingleLineLayoutContext*);
    virtual void LayoutSingleLine(SingleLineLayoutContext*);
    virtual void PostLayoutSingleLine(SingleLineLayoutContext*);

    virtual void PreLayoutMultiLine(MultiLineLayoutContext*);
    virtual void LayoutMultiLine(MultiLineLayoutContext*);
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*);

    virtual void PreDraw(HDC hDC, LPCRECT rcClip);
    virtual BOOL OnSetCursor();
    virtual void OnLButtonDown(UINT nFlags, POINT ptDoc);
	virtual void OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled);
    virtual void OnLButtonUp(UINT nFlags, POINT ptDoc);
    virtual void OnClick(UINT nFlags, POINT ptDoc);

    virtual void OnMouseEnter() {};
    virtual void OnMouseLeave() {};
    virtual void OnMouseMove(UINT nFlags, POINT ptDoc) {}

    void  NotifyMouseLeave(Node* pBubbleUntil);
    void  NotifyMouseMove(UINT nFlags, POINT ptDoc, Node* pBubbleUntil);

    virtual const CharFormat*  GetCharFormat();
    virtual uint  GetCharacters() { return 0; }
    virtual void  ToString(
            CaretPos start,
            CaretPos end,
            int format,
            String& out) {};

private:
    Node*  parent;
    Node*  child;
    Node*  prev;
    Node*  next;

    NodeStyle  m_style;

    SelectionState  m_eSelectionState;
    int  m_nIndex;  // ��doc�е�����������element֮���ǰ��˳��Ƚ�
};

}
}