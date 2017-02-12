#pragma once
#include "../Base/icommand.h"
#include "Business\Project\ProjectData\projectdata.h"
#include "UISDK\Inc\Interface\iuires.h"

// ���ͼƬ��
// wParam:  IImageResItem*
#define WM_IMAGE_EDITOR_ADD     (WM_USER+100)   
// ɾ��ͼƬ��
// wParam:  IImageResItem*
#define WM_IMAGE_EDITOR_DELETE  (WM_USER+101)

class ImageItemInfo
{
public:
    ImageItemInfo()
    {
        bImageList = false;
        bLayoutH = true;
        nImageListItemCount = 1;
        bHls = false;
        bAntialiasing = false;
    }

	String  strId;
	String  strPath;
	bool    bImageList;
	bool    bLayoutH;
	int     nImageListItemCount;
	bool    bHls;
	bool    bAntialiasing;
};

class ImageItemOpCommand : public CommandBase
{
public:
    ImageItemOpCommand()
    {
        m_pSkin = NULL;
    }
    ~ImageItemOpCommand()
    {
    }

    // ��ȡIImageResItem������Ϣ
    void  GetItemInfo(__in IImageResItem* pImageResItem, __out ImageItemInfo* pInfo)
    {
        if (!pImageResItem || !pInfo)
            return;

        pInfo->strId = pImageResItem->GetId();
        pInfo->strPath = pImageResItem->GetPath();
        pInfo->bAntialiasing = pImageResItem->GetNeedAntiAliasing();
        pInfo->bHls = pImageResItem->GetUseSkinHLS();
    }

    bool  Add()
    {
        IImageManager&  rImageManager = m_pSkin->GetImageManager();
        IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);

        if (!pImageXmlElem)
            return false;

        // ��ӵ�xml
        IUIElementProxy pUIElement;
        if (m_imageiteminfo.bImageList)
        {
            pUIElement = pImageXmlElem->AddChild(XML_IMAGE_ITEM_TYPE_IMAGELIST);
        }
        else
        {
            pUIElement = pImageXmlElem->AddChild(XML_IMAGE_ITEM_TYPE_IMAGE);
        }

        // ��ӵ��ڴ�
        IImageResItem*  pImageResItem = NULL;
        if (m_imageiteminfo.bImageList)
        {
            pImageResItem = rImageManager.InsertImageItem(
                IMAGE_ITEM_TYPE_IMAGE_LIST,
                m_imageiteminfo.strId.c_str(), 
                m_imageiteminfo.strPath.c_str());
        }
        else
        {
            pImageResItem = rImageManager.InsertImageItem(
                IMAGE_ITEM_TYPE_IMAGE,
                m_imageiteminfo.strId.c_str(), 
                m_imageiteminfo.strPath.c_str());
        }
        if (!pImageResItem)
        {
            String  str = _T("���ͼƬʧ�ܣ�Id=");
            str.append(m_imageiteminfo.strId);
            str.append(_T("  path="));
            str.append(m_imageiteminfo.strPath);
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return false;
        }

        UpdateAttribute(pUIElement.get(), pImageResItem, &m_imageiteminfo);

        // ͬ�������Կؼ�
		g_pGlobalData->m_pProjectData->m_uieditor.OnImageItemLoad(pImageResItem, pUIElement.get());

		HWND hWnd = g_pGlobalData->m_pMainFrame->GetImageWnd(m_pSkin);
		if (hWnd)
		{
			SendMessage(hWnd, WM_IMAGE_EDITOR_ADD, (WPARAM)pImageResItem, 0);
		}
        SetDirty(pUIElement.get());
        return true;
    }
  
    void UpdateAttribute(IUIElement* pUIElement, IImageResItem* pImageResItem, ImageItemInfo* pItemInfo)
    {
        if (!pUIElement || !pItemInfo || !pImageResItem)
            return;

        // �ڴ�����
        pImageResItem->SetId(pItemInfo->strId.c_str());
        pImageResItem->SetPath(pItemInfo->strPath.c_str());
        pImageResItem->SetNeedAntiAliasing(pItemInfo->bAntialiasing);
        pImageResItem->SetUseSkinHLS(pItemInfo->bHls);
        if (pItemInfo->bImageList)
        {
            IImageListResItem* pImageListResItem = static_cast<IImageListResItem*>(pImageResItem);
            pImageListResItem->SetItemCount(pItemInfo->nImageListItemCount);
            if (pItemInfo->bLayoutH)
                pImageListResItem->SetLayoutType(IMAGELIST_LAYOUT_TYPE_H);
            else
                pImageListResItem->SetLayoutType(IMAGELIST_LAYOUT_TYPE_V);
        }

        // xml����
        pUIElement->AddAttrib(XML_ID, pItemInfo->strId.c_str());
        pUIElement->AddAttrib(XML_PATH, pItemInfo->strPath.c_str());
        
        if (pItemInfo->bAntialiasing)
            pUIElement->AddAttrib(XML_IMAGE_NEED_ANTIALIASING, XML_BOOL_VALUE_1);
        else
            pUIElement->RemoveAttrib(XML_IMAGE_NEED_ANTIALIASING);

        if (pItemInfo->bHls)
            pUIElement->AddAttrib(XML_IMAGE_THEMEHLS, XML_BOOL_VALUE_1);
        else
            pUIElement->RemoveAttrib(XML_IMAGE_THEMEHLS);

        if (pItemInfo->bImageList)
        {
            if (!pItemInfo->bLayoutH)
                pUIElement->AddAttrib(XML_IMAGE_IMAGELIST_LAYOUT, XML_IMAGE_IMAGELIST_LAYOUT_V);
            else
                pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_LAYOUT);

            TCHAR szCount[16] = {0};
            _stprintf(szCount, _T("%d"), pItemInfo->nImageListItemCount);
            pUIElement->AddAttrib(XML_IMAGE_IMAGELIST_COUNT, szCount);
        }
        else
        {
            pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_LAYOUT);
            pUIElement->RemoveAttrib(XML_IMAGE_IMAGELIST_COUNT);
        }
    }

    bool Delete()
    {
        IUIElement* pUIElement = NULL;
        bool bOk = false;
        do 
        {
		    if (!m_pSkin || m_imageiteminfo.strId.empty())
			    break;

		    IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
		    if (!pImageXmlElem)
			    break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;
    	
            pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;
            pUIElement->AddRef();

		    // ͬ�����ؼ�
		    HWND hWnd = g_pGlobalData->m_pMainFrame->GetImageWnd(m_pSkin);
		    if (hWnd)
		    {
			    SendMessage(hWnd, WM_IMAGE_EDITOR_DELETE, (WPARAM)pImageResItem, 0);
		    }
		    g_pGlobalData->m_pProjectData->m_uieditor.OnImageItemDeleteInd(pImageResItem);

		    // ���ڴ���ɾ��
		    if (!rImageManager.RemoveImageItem(m_imageiteminfo.strId.c_str()))
                break;

		    // ��xml��ɾ��
            SetDirty(pUIElement);
		    pImageXmlElem->RemoveChild(pUIElement);
            
            bOk = true;
        }
        while (0);

        SAFE_RELEASE(pUIElement);
        if (!bOk)
        {
            String  str = _T("ɾ��ͼƬʧ��: ");
            str.append(m_imageiteminfo.strId.c_str());
            str.append(_T(" = \""));
            str.append(m_imageiteminfo.strPath.c_str());
            str.append(_T("\""));
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return false;
        }

		return true;
    }

	long*  GetKey(void)
	{
		UIASSERT(m_pSkin);
		IImageRes&  rImgRes = m_pSkin->GetImageRes();
        return (long*)&rImgRes;
	}

public:
    ISkinRes*     m_pSkin;

    // ������ָ�롣��Ϊ��һ���Ĳ���������һ�����ܱ�ɾ����������Щָ��ֱ��ʧЧ�ˡ�
    // ������Idȥ����
//     IImageResItem*  m_pImageResItem;
//     IUIElement*   m_pUIElement;
  
    ImageItemInfo  m_imageiteminfo;
};

// ���ͼƬ����
class AddImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, ImageItemInfo* pInfo)
    {
        if (NULL == pSkinRes || NULL == pInfo)
            return NULL;

        if (pSkinRes->GetImageRes().GetImageResItem(pInfo->strId.c_str()))
        {
            String  str = _T("Id�ظ�: ");
            str.append(pInfo->strId.c_str());
            g_pGlobalData->m_pStatusBar->SetStatusText2(str.c_str());
            return NULL;
        }

        AddImageItemCommand* p = new AddImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;
        p->m_imageiteminfo = *pInfo;
        return p;
    }
    virtual long execute()
    {
        Add();
		return 0;
    }
    virtual long undo()
    {
        Delete();
		return 0;
    }
};


// ɾ��ͼƬ����
class DeleteImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IImageResItem* pItem)
    {
        if (NULL == pSkinRes || NULL == pItem)
            return NULL;

        DeleteImageItemCommand* p = new DeleteImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;
        p->GetItemInfo(pItem, &p->m_imageiteminfo);
        return p;
    }
    virtual long execute()
    {
        Delete();
        return 0;
    }
    virtual long undo()
    {
        Add();
        return 0;
    }
};

// �޸�ͼƬ����
class ModifyImageItemCommand : public ImageItemOpCommand
{
public:
    static ICommand*  CreateInstance(ISkinRes* pSkinRes, IImageResItem* pItem, ImageItemInfo* pModifyInfo)
    {
        if (NULL == pSkinRes || NULL == pItem || NULL == pModifyInfo)
            return NULL;

        ModifyImageItemCommand* p = new ModifyImageItemCommand;
        p->AddRef();

        p->m_pSkin = pSkinRes;

        p->m_imageiteminfo = *pModifyInfo;
        p->GetItemInfo(pItem, &p->m_OriginImageItemInfo);
        return p;
    }
    virtual long execute()
    {
        bool bOk = false;
        do 
        {
            IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
            if (!pImageXmlElem)
                break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;

            IUIElement* pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;

            UpdateAttribute(pUIElement, pImageResItem, &m_imageiteminfo);
            SetDirty(pUIElement);

            bOk = true;
        } 
        while (0);

        if (!bOk)
        {
            return -1;
        }
        return 0;
    }
    virtual long undo()
    {
        bool bOk = false;
        do 
        {
            IImageManager&  rImageManager = m_pSkin->GetImageManager();
            IUIElement* pImageXmlElem = rImageManager.GetImageXmlElem(NULL);
            if (!pImageXmlElem)
                break;

            IImageResItem* pImageResItem = rImageManager.GetImageRes().
                GetImageResItem(m_imageiteminfo.strId.c_str());
            if (!pImageResItem)
                break;

            IUIElement* pUIElement = GetUIEditor()->GetImageResItemXmlElem(pImageResItem);
            if (!pUIElement)
                break;

            UpdateAttribute(pUIElement, pImageResItem, &m_OriginImageItemInfo);
            SetDirty(pUIElement);

            bOk = true;
        } 
        while (0);

        if (!bOk)
        {
            return -1;
        }
        return 0;
    }

public:
    ImageItemInfo   m_OriginImageItemInfo;
};
