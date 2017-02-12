#include "stdafx.h"
#include "listbox.h"
#include "Inc\Interface\iedit.h"
#include "..\UISDK\Inc\Interface\iwindow.h"

using namespace UI;
#define EDIT_ID  _T("__edit")

// �����ؼ�ʱ��Ҳ����edit�ؼ����ڱ༭
void  ListBox::AddEditUIRes(CREATEBYEDITORDATA* pData)
{
	EditorAddObjectResData data = {0};
	data.pUIApp = pData->pUIApp;
	data.pParentXml = pData->pXml;
	data.pParentObj = m_pIListBox;
	data.bNcChild = false;

	data.objiid = __uuidof(IInstantEdit);
	data.ppCreateObj = (void**)&m_pUIEdit;
	data.szId = EDIT_ID;
	pData->pEditor->AddObjectRes(&data);
}

void  ListBox::GetEditUIRes()
{
    if (!m_pUIEdit)
    {
        m_pUIEdit = static_cast<IInstantEdit*>(
			m_pIListBox->TryFindObject(EDIT_ID));
        if (m_pUIEdit)
        {
            m_pUIEdit->SetNotify(m_pIListBox, LISTCTRL_EDIT_MSGMAP_ID);
            m_pUIEdit->SetVisible(false);
        }
    }
}

void  ListBox::StartEdit(IListItemBase* pEditItem)
{
    if (!m_pUIEdit || !pEditItem)
        return;

    bool bNeedUpdate = false;
    m_pIListBox->MakeItemVisible(pEditItem, &bNeedUpdate);

    CRect rcItem;
    pEditItem->GetParentRect(&rcItem);
    int nHeight = m_pUIEdit->GetHeight();
    if (nHeight)
    {
        // ���������edit�ߣ�����ж��롣��Ϊedit�ı���ͼƬ�����Ǵ�ֱ���������
        int y = rcItem.top + (rcItem.Height()-nHeight)/2;
		m_pUIEdit->SetObjectPos(
			rcItem.left,y, rcItem.Width(), nHeight, SWP_NOREDRAW);
    }
    else
    {
		m_pUIEdit->SetObjectPos(
			rcItem.left,rcItem.top, rcItem.Width(), rcItem.Height(), SWP_NOREDRAW);
    }
    m_pEditingItem = pEditItem;

    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_EN_STARTEDIT;
    msg.pMsgTo = msg.pMsgFrom = m_pIListBox;
    msg.wParam = (WPARAM)m_pUIEdit;
    if (START_EDIT_CANCEL == UISendMessage(&msg, LISTCTRL_EDIT_MSGMAP_ID))
    {
        m_pEditingItem = NULL;
        if (bNeedUpdate)
            m_pIListBox->Invalidate();

        return;
    }

    m_pUIEdit->SetVisible(true); 

	IWindowBase* wnd = m_pUIEdit->GetWindowObject();
	if (wnd)
	{
		wnd->SetFocusObject(m_pUIEdit); // ֱ�ӵ���SetFocusInWindow�Ǹ�listctrl���ý���ؼ�
		// m_pUIEdit->SetFocusInWindow();  // �ᴥ��listctrl��ʧ������ˢ��
	}
	m_pUIEdit->StartEdit();
    m_pUIEdit->SelectAll();

	m_pIListBox->InvalidateItem(pEditItem);
}

LRESULT  ListBox::OnDiscardEditReq(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DiscardEdit();
	return 0;
}

// ֹͣ�༭���ú������жϵ�ǰ�༭�����Ƿ���ȷ���ɵ�����ͨ��IsItemEditing�Լ��ж�
void  ListBox::DiscardEdit()
{
    if (!m_pUIEdit)
        return;

    DoEditFinishWork(); 
}

// �༭��ɺ�������
void  ListBox::DoEditFinishWork()
{
    m_pUIEdit->SetVisible(false);
	m_pUIEdit->SetText(nullptr);
    if (m_pEditingItem)
    {
        UIMSG  msg;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_EN_ENDEDIT;
        msg.pMsgTo = msg.pMsgFrom = m_pIListBox;
        msg.wParam = (WPARAM)m_pUIEdit;
        UISendMessage(&msg, LISTCTRL_EDIT_MSGMAP_ID);

		m_pIListBox->InvalidateItem(m_pEditingItem);
    }
    
    m_pEditingItem = NULL;

    // ��������й����ܱ������ˡ�������ʾ���� 
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

LRESULT  ListBox::OnEditFinish(WPARAM w, LPARAM l)
{
    if (m_pEditingItem)
    {
        long lRet = UISendMessage(m_pEditingItem, 
            UI_MSG_NOTIFY, w, l, UI_EN_ENDEDIT, NULL,
            LISTCTRL_EDIT_MSGMAP_ID);
    }
	DoEditFinishWork(); 
    return 0;
}

bool  ListBox::IsEditing()
{
    if (m_pUIEdit && m_pEditingItem)
        return true;

    return false;
}

bool  ListBox::IsItemEditing(IListItemBase* pEditItem)
{
    if (!m_pUIEdit || !pEditItem)
        return false;

    if (m_pUIEdit->IsSelfVisible() && m_pEditingItem == pEditItem)
        return true;

    return false;
}

IInstantEdit*  ListBox::GetEditCtrl()
{
    return m_pUIEdit;
}

LRESULT  ListBox::OnPreStartEdit(WPARAM w, LPARAM l)
{
    if (!m_pEditingItem)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    long lRet = UISendMessage(
		m_pEditingItem, 
        UI_MSG_NOTIFY, w, l, UI_EN_STARTEDIT, NULL,
        LISTCTRL_EDIT_MSGMAP_ID);

    if (START_EDIT_CANCEL != lRet)
    {
        m_pIListBox->InvalidateItem(m_pEditingItem);
    }

    return lRet;
}

LRESULT  ListBox::OnAcceptEdit(WPARAM w, LPARAM l)
{
    if (!m_pEditingItem)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    m_pIListBox->InvalidateItem(m_pEditingItem);

    // ��item�������ⲿ����accept֪ͨ
    ACCEPT_EDIT_RESULT lRet = (ACCEPT_EDIT_RESULT)UISendMessage(
        m_pEditingItem, 
        UI_MSG_NOTIFY, 
        w,  // old text
        (LPARAM)m_pUIEdit->GetText(), // new text
        UI_EN_ACCEPTEDIT, NULL, LISTCTRL_EDIT_MSGMAP_ID);

    if (lRet == ACCEPT_EDIT_ACCEPT || lRet == ACCEPT_EDIT_REJECT)
    {
        DoEditFinishWork();
        return lRet;
    }
    else if (lRet == ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT)
    {
    }
    
    return lRet;
}

LRESULT  ListBox::OnCancelEdit(WPARAM w, LPARAM l)
{
    if (!m_pEditingItem || !m_pUIEdit)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    m_pIListBox->InvalidateItem(m_pEditingItem);

    BOOL bHandled = FALSE;
    long lRet = UISendMessage(m_pEditingItem, 
        UI_MSG_NOTIFY, 
        w, l,
        UI_EN_CANCELEDIT, 0, LISTCTRL_EDIT_MSGMAP_ID, &bHandled);

    if (bHandled)
    {
        DoEditFinishWork();
        return lRet;
    }

    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.pMsgFrom = m_pIListBox;
    msg.nCode = UI_EN_CANCELEDIT;
    msg.wParam = (WPARAM)m_pEditingItem;
    m_pIListBox->DoNotify(&msg);

    DoEditFinishWork();
    return 0;
}
