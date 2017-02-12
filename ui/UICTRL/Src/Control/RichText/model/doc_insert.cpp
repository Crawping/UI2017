#include "stdafx.h"
#include "doc.h"
#include "paragraph.h"
#include "element\imageelement.h"
#include "element\textelement.h"
#include "font.h"
#include "message.h"
#include "link.h"

using namespace UI;
using namespace UI::RT;

//  ȫѡ�߼�
//	anchor = doc
//  anchoroffset = 0
//  focus = last node
//  focusoffset = -1
// 
// �����߼�����
//  text,image,link,message
//  1. �����ѡ����Χ������ɾ��ѡ���ڵ�����
//  2. ���ѡ��Ϊ������offset=0�������addfront��offset!=0����addback
//  3. ���ѡ��Ϊ�ı������ı���֣��������ı�����
//  4. ...


void  Doc::ReplaceText(LPCTSTR szText)
{
    ReplaceTextEx(szText, nullptr);
}

void  stringReplaceR(TCHAR* start, TCHAR* end)
{
    TCHAR* p = start;
    while (p <= end)
    {
        if (*p == TEXT('\r'))
            *p = TEXT(' ');

        p++;
    }
}
void  stringReplaceN(TCHAR* start, TCHAR* end)
{
    TCHAR* p = start;
    while (p <= end)
    {
        if (*p == TEXT('\n'))
            *p = TEXT(' ');

        p++;
    }
}

void  Doc::ReplaceTextEx(LPCTSTR _szText, const CharFormat* pcf)
{
    if (!_szText)
        return;
  
    // �ڲ�Ҫ�޸�����ַ��������¿���һ��
    String strText(_szText);


    // 1. ��ǰѡ�������ݣ���ɾ����ǰѡ��
    if (!m_selection.IsEmpty())
    {
        // todo:
    }

    //  �ֲ�س��ַ�
    int  nLength = strText.length();
    if (0 == nLength)
        return;

    LPTSTR pStart = (LPTSTR)strText.c_str();
    LPTSTR pEnd = pStart + nLength;
    LPTSTR p = NULL;

    // ���β���
    do
    {
        p = _tcschr(pStart, TEXT('\n'));

        int nParaLength = 0;
        if (p)
        {
            nParaLength = p - pStart;

            // \nǰ���\r���˵�
            if (nParaLength > 0 && *(p - 1) == TEXT('\r'))  
                nParaLength--;

            // �ٽ��м��\r�滻Ϊ�ո񣬲�֧�ֵ�����\r���س����س�ֻʶ��\n
            stringReplaceR(pStart, pStart + nParaLength);

            if (nParaLength)
                replaceTextNoCarriageReturn(pStart, nParaLength, pcf);
            
            if (m_ePageMode != PageMode_singleline)
            {
				// ����һ��para
				Paragraph* para = new Paragraph;
                insertParaNodeAtSelection(para);
            }

            pStart = p + 1;
        }
        else
        {
            nParaLength = pEnd - pStart;

            stringReplaceR(pStart, pStart + nParaLength);
            replaceTextNoCarriageReturn(pStart, nParaLength, pcf);
        }
    } while (p);
}

// �����ı������ӡ�ͼƬ���ȼ򵥶������ֱ�ӵ��øú�������Щ�������λ��һ��para����
// ����message���ܵ��á���ΪmessageҪ���ڵڶ�����㡣
void  Doc::insertSimpleNodeAtSelection(Node* node)
{
	UIASSERT(node);
	if (!node)
		return;

	Node* cp = m_selection.GetAnchor();
	UIASSERT(cp);
	if (!cp)
		return;

	uint selPos = m_selection.GetAnchorOffset().pos;
	
	if (cp->IsRealNode())
	{
		// ������ı�������ǰ�ı����в�֡��������������ֱ�Ӽӵ�����ȥ
		if (cp->GetNodeType() == NodeTypeText)
		{
			static_cast<TextElement*>(cp)->SplitTwoElement(selPos);
		}

		node->InsertAfter(cp);
		m_selection.SetAnchorFocus(node, 0);
		return;
	}

	// ��ǰλ�����ڸ���� 
	if (cp == this)
	{
		Paragraph* para = new Paragraph;
		para->AddChildNode(node);

		if (0 == selPos)
		{
			// ������ǰ�� SelectBegin ״̬
			this->AddFrontChildNode(para);
		}
		else
		{
			// �����ں���
			this->AddChildNode(para);
		}
		m_selection.SetAnchorFocus(node, 0);
		return;
	}

	// �����һ����������棬����message
	// 			if (selPos == -1)
	// 			{
	// 				Paragraph* para = new Paragraph;
	// 				para->InsertAfter(cp);
	// 				para->AddChildNode(pText);
	// 			}
	// 			else 
	// �����ǰ����һ��������ȼ�һ������
	if (!cp->GetParagraphNode())
	{
		Paragraph* para = new Paragraph;
		cp->AddChildNode(para);
		para->AddChildNode(node);
	}
	else
	{
		cp->AddChildNode(node);
	}

	m_selection.SetAnchorFocus(node, 0);
}

void  Doc::insertParaNodeAtSelection(Paragraph* para)
{
	Node* selNode = m_selection.GetAnchor();
	if (selNode == this)
	{
		CaretPos pos = m_selection.GetAnchorOffset();
		if (0 == pos.pos)
		{
			// ������ǰ�� SelectBegin ״̬
			this->AddFrontChildNode(para);
		}
		else
		{
			// �����ں���
			this->AddChildNode(para);
		}
	}
	else
	{
		Paragraph* pThisPara = nullptr;
		if (selNode)
			pThisPara = selNode->GetParagraphNode();

		if (pThisPara)
		{
			para->InsertAfter(pThisPara);
		}
		// ��������������Է�para�ӽ��
		else if (selNode && selNode->CanAddParagraphAsChildNode())
		{
			selNode->AddChildNode(para);
		}
		else
		{
			this->AddChildNode(para);
		}
	}                              

	m_selection.SetAnchorFocus(para, CaretPos(0));
}

// �����ı������ı��в������س�����
void  Doc::replaceTextNoCarriageReturn(
	LPCTSTR szText, uint length, const CharFormat* pcf)
{
	// 2. �жϵ�ǰλ�ã��Ƿ���Ժϲ���һ��textelement��
	Node* cp = m_selection.GetAnchor();
	UIASSERT(cp);

	bool bCanMerge = false;

	// �涨��������ֱ�������������,ʵ����������������.
	if (cp->GetNodeType() == NodeTypeText)
	{
		const CharFormat* pcf_cp =
			static_cast<TextElement*>(cp)->GetOriginCharFormat();

		if (!pcf)
		{
			pcf = pcf_cp;
			bCanMerge = true;
		}
		else if (IsSameFormat(pcf_cp, pcf))
		{
			bCanMerge = true;
		}
	}


	uint selPos = m_selection.GetAnchorOffset().pos;

	if (bCanMerge)
	{
		// 3. ����ܺϲ�������
		static_cast<TextElement*>(cp)->InsertTextAt(selPos, szText, length);

		selPos += length;
		m_selection.SetAnchorFocus(cp, CaretPos(selPos));
	}
	else
	{
		// 3. ������ܺϲ�������һ���µ�
		TextElement* pText = new TextElement();
		insertSimpleNodeAtSelection(pText);

		pText->SetCharFormat(pcf);
		pText->SetText(szText, length);

		uint pos = length;
		m_selection.SetAnchorFocus(pText, CaretPos(pos));
	}
}

void  Doc::ReplaceImage(LPCTSTR szPath)
{
	ImageElement* image = new ImageElement;
	insertSimpleNodeAtSelection(image);

	if (image)
	{
		image->Load(szPath);
	}
}

bool  Doc::ReplaceCacheImage(const CCacheImage& image)
{
	if (!image)
		return false;

	ImageElement* pImage = new ImageElement;
	insertSimpleNodeAtSelection(pImage);
	if (pImage)
	{
		pImage->SetCacheImage(image);
	}

	return true;
}


IChatMessage*  Doc::BeginInsertMessage()
{
	ChatMessage* message = new ChatMessage(*this);

	// ��ȡ��ǰλ�õĶ�����㣬���ں��档
	// ĿǰԼ����message���ֻ������doc�������

	// ֱ�������µ��У����ڵ�ǰ������
	Node* p = m_selection.GetAnchor();
	if (p == this)
	{
		CaretPos pos = m_selection.GetAnchorOffset();
		if (0 == pos.pos)
		{
			// ������ǰ�� SelectBegin ״̬
			this->AddFrontChildNode(message);
		}
		else
		{
			// �����ں���
			this->AddChildNode(message);
		}
	}
	else
	{
		while (p)
		{
			if (p->GetParentNode() == this)
				break;

			p = p->GetParentNode();
		}

		if (p)
		{
			message->InsertAfter(p);
		}
		else
		{
			this->AddChildNode(message);
		}
	}

	m_selection.SetAnchorFocus(message, CaretPos(0));
	return static_cast<IChatMessage*>(message);
}

ILink*  Doc::ReplaceLinkEx(LPCTSTR szText, const CharFormat* pcf)
{
	// Լ�������Ӳ�����\r\n��ֻ����һ��

	Link* link = new Link;
	insertSimpleNodeAtSelection(link);
	link->SetCharFormat(pcf);

	ReplaceTextEx(szText, nullptr);

	return static_cast<ILink*>(link);
}


// �����ŵ�message����ȥ
void  Doc::EndInsertMessage(IChatMessage* pMsg)
{
	UIASSERT(pMsg);
	ChatMessage* msg = static_cast<ChatMessage*>(pMsg);

	// ��-1����ʾ�������������棬������������  ???????
	m_selection.SetAnchorFocus(
		static_cast<Node*>(msg), CaretPos(-1));
}

ILink*  Doc::ReplaceLink(LPCTSTR szText)
{
	return ReplaceLinkEx(szText, nullptr);
}