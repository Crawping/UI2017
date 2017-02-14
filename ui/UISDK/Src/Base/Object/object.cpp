#include "stdafx.h"
#include "object.h"
#include "object_layer.h"

#include "Inc\Interface\iwindow.h"
#include "Inc\Interface\ilayout.h"
#include "Inc\Interface\ipanel.h"
#include "Src\Resource\uiresource.h"
#include "Src\Renderbase\renderbase\renderbase.h"
#include "Src\Renderbase\textrenderbase\textrender.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\UIObject\HwndHost\HwndHost.h"
#include "Src\Resource\uicursor.h"
#include "Src\Helper\layout\layout.h"
#include "Src\Base\Application\uiapplication.h"
#include "Inc\Interface\ixmlwrap.h"
#include "Inc\Interface\iuires.h"
#include "Src\Resource\cursorres.h"
#include "Src\Resource\stylemanager.h"
#include "Src\Resource\skinres.h"
#include "Src\Resource\colorres.h"
#include "Src\Atl\image.h"
#include "Inc\Interface\imapattr.h"
#include <algorithm>
#include "..\Accessible\accessibleimpl.h"
#include "..\Accessible\object_accessible.h"
#include "object_desc.h"


using namespace UI;


Object::Object(IObject* p) : ObjTree(p), m_objLayer(*this)
{
    m_pIObject = p;
	::SetRectEmpty(&m_rcParent);
	::SetRectEmpty(&m_rcExtNonClient);
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcBorder);

	// m_lCanRedrawRef = 0;
	m_pSkinRes = NULL;
	m_ppOutRef = NULL;
    m_lzOrder = 0;

	m_nMaxWidth = NDEF;
	m_nMaxHeight = NDEF;
	m_lMinWidth = m_lMinHeight = NDEF;

	m_pBkgndRender = NULL;
	m_pForegndRender = NULL;
	m_pTextRender = NULL;

    m_pIMapAttributeRemain = NULL;
    m_pLayoutParam = NULL;
	m_pAccessible = NULL;
    m_pDescription = UndefineDescription::Get();

	memset(&m_objStyle, 0, sizeof(m_objStyle));
	memset(&m_objState, 0, sizeof(m_objState));
	m_objState.visibility_ = VISIBILITY_VISIBLE;
}

 // 注意：不要在构造或者析构函数中调用虚函数

Object::~Object(void)
{
	if (m_ppOutRef)
		*m_ppOutRef = NULL;

    SAFE_RELEASE(m_pIMapAttributeRemain);
    SAFE_RELEASE(m_pLayoutParam);
	if (m_pAccessible)
		SAFE_RELEASE(m_pAccessible);
}

HRESULT Object::FinalConstruct(ISkinRes* pSkinRes)
{
	m_pSkinRes = pSkinRes->GetImpl();
    return S_OK;
}

void Object::FinalRelease()
{
	// 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
    // 必须在DestroyChildObject前调用，否则parent关系被清空
    IMKMgr*  pMKMgr = this->GetIMKMgr();
    if (pMKMgr)
    {
        pMKMgr->OnObjectRemoveInd(this);
    }

	this->DestroyChildObject();  // 先删除子对象

	IUIEditor* pEditor = GetUIApplication()->GetUIEditorPtr();
	if (pEditor)
	{
		// 由子往父进行通知，便于从skinbuilder的tree中一个个删除
		pEditor->OnObjectDeleteInd(m_pIObject);
	}

	//	清理自己的邻居关系
	RemoveMeInTheTree();

	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);

	// 在析构之前销毁，避免窗口的window compositor已经销毁了，但layer还没有被销毁
	m_objLayer.DestroyLayer();
}

IObject*  Object::GetIObject() 
{ 
    return m_pIObject; 
}

// 注：如果在其它模块直接调用 pCtrl->m_strID=L"..."的话，在对象释放时将会崩溃
void Object::SetId(LPCTSTR szText)
{
	if (szText)
		m_strId = szText;
	else
		m_strId.clear();
}
LPCTSTR  Object::GetId() 
{
    return m_strId.c_str(); 
}

Layer* Object::GetSelfLayer() const
{
	return m_objLayer.GetLayer();
}

// 获取对象所在Layer
Layer*  Object::GetLayer()
{
	ObjectLayer*  pLayer = GetLayerEx();
    if (pLayer)
        return pLayer->GetLayer();

    return NULL;
}

ObjectLayer*  Object::GetLayerEx()
{
    Object* pObj = this;
    while (pObj)
    {
        if (pObj->GetSelfLayer())
            return &pObj->m_objLayer;

        pObj = pObj->m_pParent;
    }

    return nullptr;
}

// Layer Tree 生成关键函数，参照webkit：
// RenderLayer* RenderObject::findNextLayer(RenderLayer* parentLayer, RenderObject* startPoint, bool checkParent)
// http://opensource.apple.com//source/WebCore/WebCore-658.28/rendering/RenderObject.cpp
// 获取pStartObj的layer应该插入在pParentLayer的哪个位置
// 注：这里并没有处理自己的子对象已经有layer在pParentLayer下面的情况，需要单独处理
// Layer*  Object::FindNextLayer(
//             Layer* pParentLayer, 
//             Object* pStartObj,
//             bool bCheckParent)
// {
//     if (!pParentLayer)
//         return NULL;
//     
//     Layer* pThisLayer = m_pLayer ? m_pLayer->GetLayer() : NULL;
// 
//     // 找到一个同级的layer，这个对象是在pStartObj的后面，因此可以将pStartObj
//     // 对应的layer插入到这个对象的layer前面。
//     if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
//         return pThisLayer;
// 
//     // 如果这个对象没有layer，则向后递归遍历
//     if (!pThisLayer || pThisLayer == pParentLayer)
//     {
//         Object* p = pStartObj ? pStartObj->GetNextObject() : GetChildObject();
//         for (; p; p = p->GetNextObject())
//         {
//             Layer* pReturn = p->FindNextLayer(
//                 pParentLayer, NULL, false);
// 
//             if (pReturn)
//                 return pReturn;
//         }
//     }
// 
//     // 找到顶了，没有其它layer
//     if (pThisLayer == pParentLayer)
//         return NULL;
// 
//     // 向上找
//     if (m_pParent && bCheckParent)
//         return m_pParent->FindNextLayer(pParentLayer, this, true);
// 
//     return NULL;
// }

// 判断思路：这个对象的下一个层，只会在这个对象的下一个Object中出现。
Layer*  Object::FindNextLayer(Layer* pParentLayer)
{
    Object* pNextTreeObject = GetNextTreeItemObject();
    while (pNextTreeObject)
    {
        Layer* pThisLayer = pNextTreeObject->GetSelfLayer();

        // 找回到父layer了，说明自己就是最后一个，没有next
        if (pThisLayer && pThisLayer == pParentLayer)
            return NULL;

        // 在后面的结点中，找到了一个同级层，插在它的前面 
        if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
            return pThisLayer;

        // 否则继续往下一个树结点遍历
        pNextTreeObject = pNextTreeObject->GetNextTreeItemObject();
    }
    
    return NULL;
}

// 获取自己所在层的层对象
Object*  Object::GetLayerCreator()
{
    ObjectLayer*  p = GetLayerEx();
    if (p)
        return &p->GetObjet();

    return NULL;
}


// 获取一个控件所在窗口句炳
WindowBase* Object::GetWindowObject()
{
    Object*  pParent = this;
    do 
    {
        if (!pParent->m_pParent)
            break;
        pParent = pParent->m_pParent;
    } 
    while (1);

    IWindowBase*  pWindowBase = (IWindowBase*)pParent->
			GetIObject()->QueryInterface(__uuidof(IWindowBase));

    if (pWindowBase)
        return pWindowBase->GetImpl();
    return NULL;
}

HWND Object::GetHWND()
{
	WindowBase* pWindow = this->GetWindowObject();
	if (!pWindow)
		return NULL;

	return pWindow->GetHWND();
}

// TODO: 函数名修改为find decendant object
//
//	在所有的子子孙孙中查找id为objId的对象，只返回第一个找到的对象
//
//	Parameter
//		objId
//			[in]	要查找的对象id
//
//	Return
//		成功返回对象地址，失败返回NULL
//
Object*  Object::FindObject(LPCTSTR szObjId)
{
    if (!szObjId)
        return NULL;

	Object* pRet = this->find_child_object(szObjId, true);
	if (!pRet)
	{
		UI_LOG_WARN(_T("Find \"%s\" from \"%s\" failed."), szObjId, this->m_strId.c_str() );
        UIASSERT(0);
	}
	return pRet;
}

// 不写日志
Object*  Object::TryFindObject(LPCTSTR szObjId)
{
    if (!szObjId)
        return NULL;

    Object* pRet = this->find_child_object(szObjId, true);
    return pRet;
}

// 没有递归
Object*  Object::FindNcObject(LPCTSTR szobjId)
{
	if (NULL == szobjId)
		return NULL;

	Object* pObjChild = NULL;
	while (pObjChild = this->EnumNcChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
		{
			return pObjChild;
		}
	}	
	return NULL;
}

Object*  Object::FindObject(UUID uuid)
{
    return this->find_child_object(uuid, true);
}

Object* Object::find_child_object(LPCTSTR szobjId, bool bFindDecendant)
{
    if (NULL == szobjId)
        return NULL;

	Object* pObjChild = NULL;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
        {
			return pObjChild;
        }
	}	

	if (!bFindDecendant)
		return NULL;

	pObjChild = NULL;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		Object* pRet = ((Object*)pObjChild)->find_child_object(szobjId, bFindDecendant);
		if (pRet )
			return pRet;
	}	
	return NULL;
}


Object* Object::find_child_object(UUID uuid, bool bFindDecendant)
{
    Object* pObjChild = NULL;
    while (pObjChild = this->EnumChildObject(pObjChild))
    {
        IObjectDescription* pDesc = pObjChild->GetDescription();
        if (IsEqualGUID(pDesc->GetGUID(), uuid))
        {
            return pObjChild;
        }
    }

    if (!bFindDecendant)
        return NULL;

    pObjChild = NULL;
    while (pObjChild = this->EnumChildObject(pObjChild))
    {
        Object* pRet = ((Object*)pObjChild)->
            find_child_object(uuid, bFindDecendant);
        if (pRet)
            return pRet;
    }
    return NULL;
}


void  Object::LoadAttributeFromMap(IMapAttribute* pMapAttrib, bool bReload)
{
    if (!pMapAttrib)
        return;

    String strStyle;
    String strId;
    
    LPCTSTR szText = pMapAttrib->GetAttr(XML_STYLECLASS, false);
    if (szText)
        strStyle = szText;
    
    szText = pMapAttrib->GetAttr(XML_ID, false);
    if (szText)
        strId = szText;

    StyleRes& styleRes = m_pSkinRes->GetStyleRes();
    styleRes.LoadStyle(
                m_pDescription->GetTagName(),
                strStyle.c_str(),
                strId.c_str(),
                pMapAttrib);

    SERIALIZEDATA data = { 0 };
    data.pUIApplication = GetIUIApplication();
	data.pSkinRes = m_pSkinRes->GetISkinRes();
	data.pMapAttrib = pMapAttrib;
	data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
	if (bReload)
		data.nFlags |= SERIALIZEFLAG_RELOAD;
	

	// 保存属性，用于扩展。
	SAFE_RELEASE(m_pIMapAttributeRemain);
	m_pIMapAttributeRemain = pMapAttrib;
	if (m_pIMapAttributeRemain)
		m_pIMapAttributeRemain->AddRef();

	UISendMessage(m_pIObject, UI_MSG_SERIALIZE, (WPARAM)&data);

	// 如果没有多余的属性，直接释放，节省内存
	if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount())
	{
		SAFE_RELEASE(m_pIMapAttributeRemain);
	}
	else
	{
		int a = 0;
	}
}

// 从xml获取自己的属性
void  Object::LoadAttributeFromXml(UIElement* pElement, bool bReload)
{
	if (!pElement)
		return;

	IMapAttribute*  pMapAttrib = NULL;
	pElement->GetAttribList(&pMapAttrib);
	{
		this->LoadAttributeFromMap(pMapAttrib, bReload);
	}
	SAFE_RELEASE(pMapAttrib);

	// 通知编辑器关联控件和xml结点.
	// 将通知放在这里，而不是layoutmanager中，是为了解决复合控件中的子控件加载问题
	// 这种子控件不会在layoutmanager中被加载属性，而是由复合控件自己解决，最终调用
	// 到子控件的SetAttributeFromXmlElement函数，因此将通知放在这里。
	IUIEditor* pEditor = GetUIApplication()->GetUIEditorPtr();
	if (pEditor)
	{
		pEditor->OnObjectAttributeLoad(m_pIObject, pElement->GetIUIElement());
	}
}

// 获取一个未解析的属性。如果bErase==true，则将返回一个临时的字符串指针，调用者应该尽快保存或者仅临时使用
LPCTSTR  Object::GetAttribute(LPCTSTR szKey, bool bErase)
{
    if (NULL == szKey || NULL == m_pIMapAttributeRemain)
        return NULL;

    return m_pIMapAttributeRemain->GetAttr(szKey, bErase);
}
void  Object::AddAttribute(LPCTSTR szKey, LPCTSTR  szValue)
{
    if (NULL == m_pIMapAttributeRemain) 
    {
        UICreateIMapAttribute(&m_pIMapAttributeRemain);
    }
    m_pIMapAttributeRemain->AddAttr(szKey, szValue);

}
void  Object::GetMapAttribute(IMapAttribute** ppMapAttribute)
{
    if (ppMapAttribute && m_pIMapAttributeRemain)
    {
        *ppMapAttribute = m_pIMapAttributeRemain;
        m_pIMapAttributeRemain->AddRef();
    }
}
void  Object::ClearMapAttribute()
{
	if (m_pIMapAttributeRemain)
	{
		m_pIMapAttributeRemain->Release();
		m_pIMapAttributeRemain = nullptr;
	}
}


// 设置padding的值，同时更新非客户区的大小
void Object::SetPaddingRegion(REGION4* prc)
{
	m_rcPadding.CopyRect(prc);
}


void Object::SetBorderRegion(RECT* prc)
{
	m_rcBorder.CopyRect(prc);
}

// void*  Object::QueryInterface(const IID* pIID)
// {
//     if (IsEqualIID(*pIID, uiiidof(IObject)) ||
//         IsEqualIID(*pIID, uiiidof(IMessage)) )
//     {
//         return m_pIObject;
//     }
// 
//     return NULL;
// }
//
// 
// void  Object::OnCreateByEditor(CREATEBYEDITORDATA* pData)
// {
//     // 在编辑框中创建的对象，也给创建一个空属性列表，用于序列化默认的TEXTSTYLE等。
//     assert(NULL == m_pIMapAttributeRemain);
//     UICreateIMapAttribute(&m_pIMapAttributeRemain);
// }
//

// TODO: 优化
//
//	设置Object的默认鼠标样式
//
//	Remark:
//		如果不实现该消息，可能导致鼠标移动到一个Object上面时，仍然显示的是Window的OnSetCursor
//
// 
// BOOL Object::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
// {
// 	if (NULL == m_pCursor || NULL == m_pCursor->GetCursor())
// 	{
// 		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
//  		return TRUE;
// 	}
// 
// 	::SetCursor(m_pCursor->GetCursor());
// 	return TRUE;
// }

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
// IRenderFont* Object::GetRenderFont()
// {
//     IRenderFont* pRenderFont = NULL;
// 
//     // 向自己的textrender获取
//     if (m_pTextRender)
//         pRenderFont = m_pTextRender->GetRenderFont();
// 
//     if (pRenderFont)
//         return pRenderFont;
// 
//     // 向窗口获取默认
//     WindowBase* pWindow = GetWindowObject();
//     if (pWindow)
//         return pWindow->GetWindowDefaultRenderFont();
// 
//     return NULL;
// }

//
//	[public] [virtual]  UINT HitTest( POINT* ptInParent )
//
//	Parameters
//		ptInParent
//			[in]  要试探的位置，通常为鼠标当前位置
//
//	Return
//		鼠标不在对象所有警围内返回HTNOWHERE，否则返回HTCLIENT，或其它（子类可扩展）
//
//	Remark
//		这个函数是用来解决一个问题，是为Object配备一个RECT对象表示范围，还是给一个更强大的RGN表示范围?
//		由于RGN更加消耗内存，而有时候就得使用RGN来实现某些功能，因此在这里用这个函数来告诉调用者，
//		你这个位置处于我什么部位。同时给Object配备一个RECT，来表示每一个对象的范围，因为大部分时候使用的都是RECT区域。
//

UINT Object::OnHitTest(POINT* ptInParent, __out POINT* ptInChild)
{
	BOOL  bIn = FALSE;

// 	if (this->m_hRgn)
// 	{
//      UIASSERT(0);  // 未实现
// 		bIn = ::PtInRegion(m_hRgn, ptInParent->x, ptInParent->y);
//      if (bIn)
// 		    return HTCLIENT;
// 	}

	Layer* layer = GetSelfLayer();
    if (layer)
    {
		POINT ptObj = 
		{
			ptInParent->x-m_rcParent.left, 
			ptInParent->y-m_rcParent.top
		};

		layer->MapView2Layer(&ptObj);

		RECT rcObj = {0, 0, m_rcParent.Width(), m_rcParent.Height()};
		if (PtInRect(&rcObj, ptObj))
		{
			if (ptInChild)
				*ptInChild = ptObj;

			return HTCLIENT;
		}
    }
	else
	{
		bIn = ::PtInRect(&m_rcParent, *ptInParent);
		if (bIn)
		{
			if (ptInChild)
			{
				ptInChild->x = ptInParent->x - m_rcParent.left;
				ptInChild->y = ptInParent->y - m_rcParent.top;
			}

			return HTOBJECT;
		}
	}

	return HTNOWHERE;
}

// void Object::OnThemeChanged()
// {
// 	SetMsgHandled(FALSE);
//     
//     if (m_pBkgndRender)
//         m_pBkgndRender->CheckThemeChanged();
//     if (m_pForegndRender)
//         m_pForegndRender->CheckThemeChanged();
// }
// 
// LRESULT  Object::OnSkinTextureChanged(UINT, WPARAM, LPARAM)
// {
//     if (m_pTextRender)
//     {
//         m_pTextRender->CheckSkinTextureChanged();
//     }
//     return 0;
// }


IMKMgr*  Object::GetIMKMgr()
{
    IMKMgr*  pMKMgr = NULL;

    Object*  pParent = m_pParent;
    while (pParent)
    {
		pMKMgr = pParent->virtualGetIMKMgr();;
        if (pMKMgr)
           return pMKMgr;

        pParent = pParent->m_pParent;
    }
    return NULL;
}

void  Object::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
//     if (!bVisible)
//     {
//         // 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
//         WindowBase* pWindow = GetWindowObject();
//         if (pWindow)  // 必须在DestroyChildObject前调用，否则parent关系被清空
//         {
//             pWindow->OnObjectHideInd(this);
//         }
//     }

    // 如果隐藏的对象是一个焦点对象，则将焦点重新切回到第一个对象
    // 注，这里的object也可能是位于一个listctrl中，则其对应的mouse/keyboard
    //     manager则不再是窗口的，而是listctrl自己的

    IMKMgr*  pMKMgr = GetIMKMgr();
    if (pMKMgr)
    {
        pMKMgr->OnObjectVisibleChangeInd(this, bVisible?true:false);
    }
}

void  Object::ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove)
{

#define __REMOVE(x) \
    if (remove->x) m_objStyle.x = 0;
#define __ADD(x) \
	if (add->x) m_objStyle.x = 1;

	if (add)
	{
		__ADD(transparent);
		__ADD(float_on_parent_content);
		__ADD(post_paint);
		__ADD(hscroll);
		__ADD(vscroll);
		__ADD(ncobject);
		__ADD(reject_all_mouse_msg);
		__ADD(reject_self_mouse_msg);
		__ADD(receive_dragdrop_event);
		__ADD(enable_ime);
		__ADD(zindex_overlap);
		__ADD(noclip);
		__ADD(clip_client);
		__ADD(tabstop);

		if (add->layer)
			EnableLayer(true);

		// 默认值为1时，如果没有在xml中配置，不会触发setter函数
		// 因此在设置默认值的时候，应该同步一次该值
		if (add->default_ncobject)
		{
			m_objStyle.default_ncobject = 1;
			m_objStyle.ncobject = 1;
		}
		if (add->default_reject_all_mouse_msg)
		{
			m_objStyle.default_reject_all_mouse_msg = 1;
			m_objStyle.reject_all_mouse_msg = 1;
		}
		if (add->default_reject_self_mouse_msg)
		{
			m_objStyle.default_reject_self_mouse_msg = 1;
			m_objStyle.reject_self_mouse_msg = 1;
		}
		if (add->default_transparent)
		{
			m_objStyle.default_transparent = 1;
			m_objStyle.transparent = 1;
		}
		if (add->default_tabstop)
		{
			m_objStyle.default_tabstop = 1;
			m_objStyle.tabstop = 1;
		}
	}

	if (remove)
	{
		__REMOVE(transparent);
		__REMOVE(float_on_parent_content);
		__REMOVE(post_paint);
		__REMOVE(hscroll);
		__REMOVE(vscroll);
		__REMOVE(ncobject);
		__REMOVE(reject_all_mouse_msg);
		__REMOVE(reject_self_mouse_msg);
		__REMOVE(receive_dragdrop_event);
		__REMOVE(enable_ime);
		__REMOVE(zindex_overlap);
		__REMOVE(noclip);
		__REMOVE(clip_client);
		__REMOVE(tabstop);
		if (remove->layer)
			EnableLayer(false);

		if (remove->default_ncobject)
		{
			m_objStyle.default_ncobject = 0;
			m_objStyle.ncobject = 0;
		}
		if (remove->default_reject_all_mouse_msg)
		{
			m_objStyle.default_reject_all_mouse_msg = 0;
			m_objStyle.reject_all_mouse_msg = 0;
		}
		if (remove->default_reject_self_mouse_msg)
		{
			m_objStyle.default_reject_self_mouse_msg = 0;
			m_objStyle.reject_self_mouse_msg = 0;
		}
		if (remove->default_transparent)
		{
			m_objStyle.default_transparent = 0;
			m_objStyle.transparent = 0;
		}
		if (remove->default_tabstop)
		{
			m_objStyle.default_tabstop = 0;
			m_objStyle.tabstop = 0;
		}
	}
}

bool Object::TestObjectStyle(const OBJSTYLE& test)
{

#define TEST(x) \
    if (test.x && !m_objStyle.x) return false; 

    TEST(transparent);
	TEST(float_on_parent_content);
    TEST(post_paint);
	TEST(hscroll);
	TEST(vscroll);
	TEST(ncobject);
	TEST(reject_all_mouse_msg);
	TEST(reject_self_mouse_msg);
	TEST(receive_dragdrop_event);
	TEST(enable_ime);
	TEST(zindex_overlap);
	TEST(noclip);
    TEST(clip_client);
	TEST(tabstop);
	TEST(layer);

    return true;
}

// void Object::ModifyStyleEx(UINT nStyleAdd, UINT nStyleRemove, bool bNotify)
// {
//     STYLESTRUCT s;
//     s.styleOld = m_nStyle2;
//     s.styleNew = m_nStyle2;
// 
//     if (nStyleAdd != 0)
//     {
//         s.styleNew |= nStyleAdd;
//     }
//     if (nStyleRemove != 0)
//     {
//         s.styleNew &= ~nStyleRemove;
//     }
// 
//     if (s.styleNew == s.styleOld)
//         return;
// 
//     if (bNotify)
//     {
//         ::UISendMessage(this, WM_STYLECHANGING, (WPARAM)GWL_EXSTYLE, (LPARAM)&s);
//     }
// 
//     m_nStyle2 = s.styleNew;
// 
//     if (bNotify)
//     {
//         s.styleOld = s.styleOld;
//         ::UISendMessage(this, WM_STYLECHANGED, (WPARAM)GWL_EXSTYLE, (LPARAM)&s);
//     }
// }

bool Object::IsTransparent()
{
	if (m_objStyle.transparent)
        return true;

    return false;
}
void  Object::SetTransparent(bool b)
{
	m_objStyle.transparent = b;
}

bool  Object::IsNoClip()
{
	return !NeedClip();
}

bool  Object::NeedClip()
{
	if (m_objStyle.noclip)
		return false;

//  TODO: 需要重写。
// 	// 如果子对象是一个层，并且有变换，则不更新剪裁区域，而是切换到父对象的剪裁区域
// 	// 2015.9.23 add 如果是硬件合成模式，则不用管，层与层之间的数据没有关联。
// 	if (m_pRenderLayer && m_pRenderLayer->HasTransform())
// 	{
// 		WindowBase* pWindow = GetWindowObject();
// 		if (!pWindow || !pWindow->IsGpuComposite())
// 			return false;
// 	}
// 
	return true;
}

void  Object::SetNoClip(bool b)
{
	m_objStyle.noclip = b;
}

void  Object::SetClipClient(bool b)
{
    m_objStyle.clip_client = b;
}
bool  Object::NeedClipClient()
{
    return m_objStyle.clip_client;
}

bool Object::IsFocus()
{
	return m_objState.focus;
}


void Object::SetFocus(bool b, bool bNoitfy)
{
    if (m_objState.focus == b)
        return;

    m_objState.focus = b;
    if (bNoitfy)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

// 注：控件有可能位于列表项内(listitem)，给该项里的控件设置焦点不应该抢走listtrl的焦点
bool  Object::SetFocusInWindow()
{
    IMKMgr*  pMKMgr = GetIMKMgr();
    if (!pMKMgr)
        return false;

    pMKMgr->SetFocusObject(this);

// 	WindowBase* pWnd = GetWindowObject();
// 	if (!pWnd)
// 		return false;
// 
// 	pWnd->SetFocusObject(this);
	return true;
}


bool Object::IsTabstop()
{
	return m_objStyle.tabstop;
}

bool Object::CanTabstop()
{
	return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

void Object::SetTabstop(bool b)
{
	m_objStyle.tabstop = b;
}

bool Object::IsSelfVisible()
{
	bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE?true:false;
	return bVisible;
}

bool Object::IsVisible()
{
	bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE?true:false;
	if (false == bVisible || NULL == m_pParent)
	{
		return bVisible;
	}
	// 判断父对象不可见的情况 
	else
	{
		// 注：在这里不对最外层的窗口进行判断的原因是：在类似于窗口初始化的函数里面，
		// 虽然窗口暂时是不可见的，但里面的对象的IsVisible应该是返回true才好处理
		if (m_pParent->GetDescription() && 
			OBJ_WINDOW == m_pParent->GetDescription()->GetMajorType())
		{ 
			return true;       
		}
		else
		{
			return m_pParent->IsVisible();
		}
	}
    return true;
}


bool  Object::IsCollapsed()
{
    bool bCollapsed = m_objState.visibility_ == VISIBILITY_COLLAPSED?true:false;
    if (bCollapsed || NULL == m_pParent)
    {
        return bCollapsed;
    }
    // 判断父对象不可见的情况 
    else
    {
        return m_pParent->IsCollapsed();
    }

    return false;
}
bool  Object::IsSelfCollapsed()
{
    if (m_objState.visibility_ == VISIBILITY_COLLAPSED)
        return true;

    return false;
}


bool Object::IsEnable()
{
	return !m_objState.disable;
}

void  Object::LoadVisibleEx(long l)
{
	m_objState.visibility_ = static_cast<VISIBILITY_TYPE>(l);
}
long  Object::SaveVisibleEx()
{
	return m_objState.visibility_;
}

void Object::SetVisible(bool b)
{
    SetVisibleEx(b?VISIBILITY_VISIBLE:VISIBILITY_COLLAPSED);
}

void  Object::virtualSetVisibleEx(VISIBILITY_TYPE eType)
{
}


//
//	[static]
//   给pParent的子孙对象递归转发消息
//
void Object::ForwardMessageToChildObject(
	Object* pParent, UIMSG* pMsg)
{
	Object*  pChild = NULL;
	while (pChild = pParent->EnumChildObject(pChild))
	{
		pMsg->pMsgTo = pChild->GetIMessage();
		::UISendMessage(pMsg);
		Object::ForwardMessageToChildObject(pChild, pMsg);
	}

	Object* pNcChild = NULL;
	while (pNcChild = pParent->EnumNcChildObject(pNcChild))
	{
		pMsg->pMsgTo = pNcChild->GetIMessage();
		::UISendMessage(pMsg);
		Object::ForwardMessageToChildObject(pNcChild, pMsg);
	}
}

// 发给父对象，再发给子对象，最后再通知一次父对象
// 应用场景：UI_WM_INITIALIZE/UI_WM_INITIALIZE2
void Object::ForwardMessageToChildObject2(
	Object* pParent, UIMSG* pMsg, UIMSG* pMsg2)
{
	Object*  pChild = NULL;
	while (pChild = pParent->EnumChildObject(pChild))
	{
		pMsg->pMsgTo = pChild->GetIMessage();
		::UISendMessage(pMsg);

		Object::ForwardMessageToChildObject2(pChild, pMsg, pMsg2);

		pMsg2->pMsgTo = pChild->GetIMessage();
		::UISendMessage(pMsg2);
	}

	Object* pNcChild = NULL;
	while (pNcChild = pParent->EnumNcChildObject(pNcChild))
	{
		pMsg->pMsgTo = pNcChild->GetIMessage();
		::UISendMessage(pMsg);

		Object::ForwardMessageToChildObject2(pNcChild, pMsg, pMsg2);

		pMsg2->pMsgTo = pNcChild->GetIMessage();
		::UISendMessage(pMsg2);
	}
}
void  Object::SetVisibleEx(VISIBILITY_TYPE eType)
{
    if (m_objState.visibility_ == eType)
        return;

	bool bOld = IsVisible();
	bool bVisibleCompatible = eType==VISIBILITY_VISIBLE ? true:false;

	m_objState.visibility_ = eType;
    virtualSetVisibleEx(eType);
	
	// 通知子对象
	UISendMessage(this, UI_MSG_VISIBLE_CHANGED, 
        bVisibleCompatible?TRUE:FALSE, 
        (LPARAM)this);

    UIMSG msg;
	msg.message = UI_MSG_VISIBLE_CHANGED;
    msg.wParam = bVisibleCompatible?TRUE:FALSE;
    msg.lParam = (LPARAM)this;
	Object::ForwardMessageToChildObject(this, &msg);

    if (m_pParent)
    {
		ILayout*  pLayout = (ILayout*)UISendMessage(m_pParent, UI_MSG_GETLAYOUT);
        if (pLayout)
        {
            pLayout->ChildObjectVisibleChanged(m_pIObject);
        }
    }

	if (bVisibleCompatible != bOld)
	{
        // 确实存在在初始化中调用setvisible，但此时还没有 m_pParent 的情况，如SystemScrollBar
// 		if (bUpdateLayout)
// 		{
// 			this->UpdateLayout(bRedraw);
//         }
//         else if (bRedraw)
//         {
//             if (bVisibleCompatible)
//                 this->Invalidate(); 
//             else
//                 this->UpdateObjectBkgnd(true); 
//         }
//         else
//         {
//         }
	}
}



void  Object::SetDisableDirect(bool b)
{
    m_objState.disable = b;
}
bool  Object::IsSelfDisable()
{
	return m_objState.disable;
}

void  Object::virtualSetEnable(bool b)
{
	SetDisableDirect(!b);
}

void Object::SetEnable(bool b, bool bNoitfy)
{
	bool bOld = IsEnable();
	
	virtualSetEnable(b);
	
    if (bNoitfy && b != bOld)
	    UISendMessage(GetIMessage(), UI_MSG_STATECHANGED, (WPARAM)OSB_DISABLE);

	if (b != bOld)
    {
		// [注] 如果没有指定刷新，则需要外部显示调用UpdateObject，因为该控件所在层
		//      并没有设置为dirty，直接刷新整个窗口也不会更新该控件
		    this->Invalidate();

        // 重新发送鼠标消息，例如鼠标正好位于该控件上面，则需要将该控件设置为hover，否则点击无效
        POINT pt = {0, 0};
        ::GetCursorPos(&pt);
        HWND hWnd = GetHWND();
        ::MapWindowPoints(NULL, hWnd, &pt, 1);
        ::PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    }
}


bool Object::IsDefault()
{
	return m_objState.default_;
}

// bool Object::IsReadonly()
// {
// 	return this->testStateBit(OSB_READONLY);
// }

bool Object::IsHover()
{
	return m_objState.force_hover || m_objState.hover;
}

bool Object::IsPress()
{
	return m_objState.force_press || m_objState.press;
}

bool Object::IsForceHover()
{
	return m_objState.force_hover;
}

bool Object::IsForcePress()
{
	return m_objState.force_press;
}
bool  Object::IsSelected()
{
    return m_objState.selected;
}

void  Object::SetDefault(bool b, bool bNotify)
{
    if (m_objState.default_ == b)
        return;

    m_objState.default_ = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_DEFAULT;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetSelected(bool b, bool bNotify)
{
    if (m_objState.selected == b)
        return;
    
    m_objState.selected = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void Object::SetForceHover(bool b, bool bNotify)
{
    if (m_objState.force_hover == b)
        return;

    bool bOldHover = IsHover();
    
    m_objState.force_hover = b;
    if (bNotify && IsHover() != bOldHover)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void Object::SetForcePress(bool b, bool bNotify)
{
    if (m_objState.force_press == b)
        return;

    bool bOldPress = IsPress();

    m_objState.force_press = b;
    if (bNotify && IsPress() != bOldPress)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetHover(bool b, bool bNotify)
{
    if (m_objState.hover == b)
        return;

    m_objState.hover = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetPress(bool b, bool bNotify)
{
    if (m_objState.press == b)
        return;

    m_objState.press = b;
    if (bNotify)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetAsNcObject(bool b) 
{
	m_objStyle.ncobject = b;
}
bool  Object::IsNcObject()   
{
	return m_objStyle.ncobject;
}
bool  Object::IsRejectMouseMsgAll()  
{
	return m_objStyle.reject_all_mouse_msg;
}
bool  Object::IsRejectMouseMsgSelf() 
{
	return m_objStyle.reject_self_mouse_msg;
}
void  Object::SetRejectMouseMsgAll(bool b)
{
	m_objStyle.reject_all_mouse_msg = b;
}
void  Object::SetRejectMouseMsgSelf(bool b)
{   
    m_objStyle.reject_self_mouse_msg = b;
}

// 当手动创建一个对象（非从xml中加载时，可以调用该函数完全默认属性的加载)
void  Object::InitDefaultAttrib()
{
    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);

    UIASSERT(m_strId.empty() && TEXT("将setid放在该函数之后调用，避免覆盖"));
	// pMapAttrib->AddAttr(XML_ID, m_strId.c_str()); // 防止id被覆盖??

    // 解析样式
    UIASSERT(m_pSkinRes);
	
    StyleRes& styleRes = m_pSkinRes->GetStyleRes();
    styleRes.LoadStyle(
        m_pDescription->GetTagName(),
        nullptr,
        nullptr,
        pMapAttrib);
	

	SERIALIZEDATA data = {0};
    data.pUIApplication = GetIUIApplication();
	data.pSkinRes = m_pSkinRes->GetISkinRes();
	data.pMapAttrib = pMapAttrib;
	data.nFlags = SERIALIZEFLAG_LOAD;

	SAFE_RELEASE(m_pIMapAttributeRemain);
	m_pIMapAttributeRemain = pMapAttrib;
	if (m_pIMapAttributeRemain)
		m_pIMapAttributeRemain->AddRef();

	UISendMessage(m_pIObject, UI_MSG_SERIALIZE, (WPARAM)&data);

	// 如果没有多余的属性，直接释放，节省内存
	if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount())
	{
		SAFE_RELEASE(m_pIMapAttributeRemain);
	}
    SAFE_RELEASE(pMapAttrib);
}
// 
// void Object::SetUserData(LPVOID p)
// {
// 	m_pUserData = p;
// }
// 
// LPVOID Object::GetUserData()
// {
// 	return m_pUserData;
// }


void  Object::SetOutRef(void** ppOutRef)
{
	m_ppOutRef = ppOutRef;
}

SkinRes*  Object::GetSkinRes()
{
	return m_pSkinRes;
}

ISkinRes*  Object::GetISkinRes()
{
	if (!m_pSkinRes)
		return NULL;
	
	return m_pSkinRes->GetISkinRes();
}

UIApplication*  Object::GetUIApplication() 
{ 
	UIASSERT(m_pSkinRes);
	UIASSERT(m_pSkinRes->GetUIApplication());

	return m_pSkinRes->GetUIApplication();
}

IUIApplication*  Object::GetIUIApplication()
{
	UIASSERT(m_pSkinRes);
	UIASSERT(m_pSkinRes->GetUIApplication());

	return m_pSkinRes->GetUIApplication()->GetIUIApplication();
}


int  Object::GetZorder() 
{
    return m_lzOrder; 
}

void  Object::SetZorderDirect(int z) 
{
    m_lzOrder = z;

    //bool bOldHasLayer = GetSelfLayer()?true:false;
    bool bPosChanged = false;

    // 更新了zorder，修改自己在parent中的位置
    if (m_pParent)
    {
        if ((m_pPrev && m_pPrev->m_lzOrder > m_lzOrder) ||
            (m_pNext && m_pNext->m_lzOrder < m_lzOrder))
        {
            RemoveMeInTheTree();
            if (IsNcObject())
                m_pParent->AddNcChild(this);
            else
                m_pParent->AddChild(this);

            bPosChanged = true;
        }
    }
    
//     update_layer_ptr();
//     
//     bool bNowHasLayer = GetSelfLayer() ? true : false;
    if (/*bOldHasLayer && bNowHasLayer && */bPosChanged)
    {
        m_objLayer.OnObjPosInTreeChanged();
    }
}

bool  SortByZorder(Object* p1, Object* p2)
{
	return p1->GetZorder() < p2->GetZorder();
}

void  Object::SortChildByZorder()
{
	if (!m_pChild || !m_pChild->m_pNext)
		return;

	// 排序
	vector<Object*> vec;
	Object* p = m_pChild;
	while (p)
	{
		vec.push_back(p);
		p = p->m_pNext;
	}
	std::sort(vec.begin(), vec.end(), SortByZorder);

	// 应用
	vector<Object*>::size_type count = vec.size();
	m_pChild = vec[0];
	m_pChild->m_pPrev = NULL;

	for (uint i = 1; i < count; i++)
	{
		vec[i]->m_pNext = NULL;
		vec[i]->m_pPrev = vec[i-1];

		vec[i-1]->m_pNext = vec[i];
	}
}

void Object::SetBackRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pBkgndRender);
	m_pBkgndRender = p;

	if (m_pBkgndRender)
		m_pBkgndRender->AddRef();
}


void Object::SetForegndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pForegndRender);
	
	m_pForegndRender = p;
	if (m_pForegndRender)
		m_pForegndRender->AddRef();
}


void Object::SetTextRender(ITextRenderBase* p)
{
	SAFE_RELEASE(m_pTextRender);
	
	m_pTextRender = p;
	if (m_pTextRender)
		m_pTextRender->AddRef();
}

ITextRenderBase*  Object::GetTextRender()
{
    return m_pTextRender; 
}
IRenderBase*  Object::GetBackRender() 
{ 
    return m_pBkgndRender; 
}
IRenderBase*  Object::GetForeRender()
{
    return m_pForegndRender; 
}

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
IRenderFont* Object::GetRenderFont()
{
	IRenderFont* pRenderFont = NULL;

	// 向自己的textrender获取
	if (m_pTextRender)
		pRenderFont = m_pTextRender->GetRenderFont();

	if (pRenderFont)
		return pRenderFont;

	// 向窗口获取默认
	WindowBase* pWindow = GetWindowObject();
	if (pWindow)
		return pWindow->GetWindowDefaultRenderFont();

	return NULL;
}

void  Object::load_renderbase(LPCTSTR szName, IRenderBase*& pRender)
{
    SAFE_RELEASE(pRender);
    if (szName)
	{
		GetUIApplication()->GetRenderBaseFactory().CreateRenderBaseByName(
            m_pSkinRes->GetISkinRes(), szName, m_pIObject, &pRender);
	}
}

void  Object::load_textrender(LPCTSTR szName, ITextRenderBase*& pTextRender)
{
    SAFE_RELEASE(pTextRender);
    if (szName)
		GetUIApplication()->GetTextRenderFactroy().CreateTextRenderBaseByName(
		m_pSkinRes->GetISkinRes(), szName, m_pIObject, &pTextRender);
}

LPCTSTR  Object::get_renderbase_name(IRenderBase*& pRender)
{
    if (!pRender)
        return NULL;

    return GetUIApplication()->GetRenderBaseFactory().GetRenderBaseName(pRender->GetType());
}
LPCTSTR  Object::get_textrender_name(ITextRenderBase*& pTextRender)
{
    if (!pTextRender)
        return NULL;

	return GetUIApplication()->GetTextRenderFactroy().GetTextRenderBaseName(pTextRender->GetType());
}

void  Object::LoadBkgndRender(LPCTSTR szName)   
{ 
    load_renderbase(szName, m_pBkgndRender); 
}
void  Object::LoadForegndRender(LPCTSTR szName) 
{ 
    load_renderbase(szName, m_pForegndRender);
}
void  Object::LoadTextRender(LPCTSTR szName)  
{ 
    load_textrender(szName, m_pTextRender); 
}
LPCTSTR  Object::SaveBkgndRender()  
{ 
    return get_renderbase_name(m_pBkgndRender);
}
LPCTSTR  Object::SaveForegndRender() 
{ 
    return get_renderbase_name(m_pForegndRender); 
}
LPCTSTR  Object::SaveTextRender() 
{ 
    return get_textrender_name(m_pTextRender); 
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   窗口收到WM_DESTROY消息之后，传递给所有的子对象
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_DESTROY消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// 在接收到OnSize消息的时候，需要改变自己的pos,rectP,rectW成员，
// 并通知自己的子对象去更新
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_ERASEBKGND消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

// 注：该函数不负责将关联的对象的相应指针置空，例如不会将parent中的child中的自己清除
//     目前仅是在换肤时被调用

void Object::ClearMyTreeRelationOnly()
{
	this->m_pParent = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

	OBJ_TYPE eType = OBJ_UNKNOWN;
	if (m_pDescription)
		eType = m_pDescription->GetMajorType();
	if (OBJ_CONTROL != eType) // 控件类型在xml中只占一个结点，不能清除 
	{
		this->m_pChild = NULL;
		this->m_pNcChild = NULL;
	}

	// 不要清除hook/notify，因为换肤时需要保留这些信息
	// 			pThis->ClearHook();
	// 			pThis->ClearNotify();
}


//
//  根据对象所在的背景，计算对象的文本颜色。用于图片换肤
//
// Two colors provide good color visibility if the brightness difference and the color difference between the two colors are greater than a set range.
// Color brightness is determined by the following formula:
// ((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
// Color difference is determined by the following formula:
// (max (Red 1, Red 2) - min (Red 1, Red 2)) + (max (Green 1, Green 2) - min (Green 1, Green 2)) + (max (Blue 1, Blue 2) - min (Blue 1, Blue 2))
//  The rage for color brightness difference is 125. The range for color difference is 500.
// 
//  google text background color black white
//  http://juicystudio.com/services/aertcolourcontrast.php
//  http://www.colourlovers.com/web/blog/2010/03/11/calculating-color-contrast-for-legible-text
//
#if 0
DWORD  Object::CalcContrastTextColor()
{
    WindowBase*  pWindowBase = GetWindowObject();
    if (NULL == pWindowBase)
        return 0;

	assert (0 && TEXT("TODO"));
	return 0;
#if 0
    HDC hDC = pWindowBase->GetRenderLayer2()->GetRenderTarget()->GetHDC();
    if (NULL == hDC)
        return 0;

    HBITMAP  hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    if (NULL == hBitmap)
        return 0;

    CRect rcLayer;
    GetRectInWindow(&rcLayer, true);

    Image  image;
    image.Attach(hBitmap, Image::DIBOR_TOPDOWN);
// #ifdef _DEBUG
//     image.Save(L"C:\\aa.png", Gdiplus::ImageFormatPNG);
// #endif
    DWORD  dwAverageColor = image.GetAverageColor(&rcLayer);
    image.Detach();

    // 将alpha值应用上
//    int a = dwAverageColor >> 24;
    int r = GetRValue(dwAverageColor);
    int g = GetGValue(dwAverageColor);
    int b = GetBValue(dwAverageColor);

    // TODO ?? 是否需要做这一步，这应该看颜色预乘了没？？
//     if (a)
//     {
//         r = (r<<8)/a;
//         g = (g<<8)/a;
//         b = (b<<8)/a;
// 
//         r = min(255, r);
//         g = min(255, g);
//         b = min(255, b);
//     }

//     int v = ((r * 299) + (g * 587) + (b * 114)) >> 10;
//     if (v < 125)
//         return 0xFFFFFFFF;
//     else
//         return 0xFF000000;

    // 这里得计算亮度，而不是灰色
    int max = max(max(r,g),b);
    int min = min(min(r,g),b);
    int light = (min + max) >> 1;
    if (light < 128)
        return 0xFFFFFFFF;
    else
        return 0xFF000000;
#endif
}
#endif

Object*  Object::GetObjectByPos(POINT* pt)
{
    WindowBase* pWindow = GetWindowObject();
    if (NULL == pWindow)
        return NULL;

    return pWindow->GetObjectByPos(this, pt, false);
}

bool  Object::SetMouseCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetMouseCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseMouseCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIObject);
    return true;
}
bool  Object::SetKeyboardCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseKeyboardCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIObject);
    return true;
}

void  Object::load_layer_config(bool b)
{
    m_objStyle.layer = b;
}

void  Object::EnableLayer(bool b)
{
	if (b)
	{
		m_objLayer.CreateLayer();
	}
	else
	{
		m_objLayer.ReleaseLayer();
	}
	
	// 通知父layer更新缓存，子对象有自己的缓存，或者需要缓存子对象
	if (m_pParent && m_objStyle.layer != b)
	{
		m_pParent->Invalidate(&m_rcParent);
	}

	m_objStyle.layer = b;
}

bool  Object::HasLayer()
{
	return m_objStyle.layer;
}

void  Object::OnLayerDestory()
{
	m_objStyle.layer = false;

	// 通知父layer更新缓存，子对象有自己的缓存，或者需要缓存子对象
	if (m_pParent)
	{
		m_pParent->Invalidate(&m_rcParent);
	}
}


// 序列化辅助函数
/*
void  Object::LoadColor(LPCTSTR szColorId, Color*& pColor)
{
    SAFE_RELEASE(pColor);
    if (!szColorId || !m_pUIApplication)
        return;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (!pColorRes)
        return;

    pColorRes->GetColor(szColorId, &pColor);
}

LPCTSTR  Object::SaveColor(Color*& pColor)
{
    if (!pColor || !m_pUIApplication)
        return NULL;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (pColorRes)
    {
        LPCTSTR szId = pColorRes->GetColorId(pColor);
        if (szId)
            return szId;
    }

    TCHAR* szBuffer = GetTempBuffer();
    pColor->ToHexString(szBuffer);
    return szBuffer;
}
*/

// 辅助函数
unsigned long  Object::GetChildCount()
{
	long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		lCount++;
		p = p->GetNextObject();
	}

	return lCount;
}

Object*  Object::GetChildObjectByIndex(unsigned long lIndex)
{
	unsigned long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		if (lCount == lIndex)
			return p;

		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		if (lCount == lIndex)
			return p;
		lCount++;
		p = p->GetNextObject();
	}

	return NULL;
}

unsigned long  Object::GetChildObjectIndex(Object* pChild)
{
	unsigned long lCount = 0;
	Object* p = this->GetChildObject();
	while (p)
	{
		if (p == pChild)
			return lCount;

		lCount++;
		p = p->GetNextObject();
	}
	p = this->GetNcChildObject();
	while (p)
	{
		if (p == pChild)
			return lCount;
		lCount++;
		p = p->GetNextObject();
	}

	return 0;
}

// 外部添加IUIAccessible的接口实现类
class UIAccessibleCreator : public IUIAccessibleCreator
{
public:
	UIAccessibleCreator(AccessibleImpl* p)
	{
		m_pAccessible = p;
	}
	virtual void  Add(IUIAccessible* p) override
	{
		if (m_pAccessible)
			m_pAccessible->AddUIAccessible(p);
	}
private:
	AccessibleImpl*  m_pAccessible;
};

bool  Object::CreateAccesible(IAccessible** pp)
{
	if (!pp)
		return false;

	if (!m_pAccessible)
	{
		AccessibleImpl* p = new AccessibleImpl;
		m_pAccessible = static_cast<IAccessible*>(p);
		m_pAccessible->AddRef();

		UIAccessibleCreator creator(p);

		// 添加其它accessible
		UISendMessage(m_pIObject, UI_WM_CREATE_ACCESSIBLE, 
			(WPARAM)static_cast<IUIAccessibleCreator*>(&creator), 0);

		// 添加默认的object accessible
		// 内部会调用addref

		// TODO1: Object销毁时，怎么确保这些接口也销毁，而不仅仅是release？
		// TODO2: 在使用触摸屏滚动页面时，这里居然会造成内存泄露，无法理解
		p->AddUIAccessible(new ObjectAccessible(*this));
	}

	*pp = m_pAccessible;
	m_pAccessible->AddRef();
	return true;
}

void  Object::SetDescription(IObjectDescription* p)
{
    m_pDescription = p;
}
IObjectDescription*  Object::GetDescription()
{
    return m_pDescription;
}

// 自己在树中的位置改变。如在编辑器中，拖拽控件到另一个panel下面
void  Object::position_in_tree_changed()
{
    if (!m_pLayoutParam)
        return;

    // 根据父对象，重新生成布局参数
    SAFE_RELEASE(m_pLayoutParam);
    GetSafeLayoutParam();
}

int UI::Object::GetMaxWidth()
{
	return m_nMaxWidth;
}

int UI::Object::GetMaxHeight()
{
	return m_nMaxHeight;
}

void  Object::SetMaxWidth(int n)
{
	m_nMaxWidth = n;
}
void  Object::SetMaxHeight(int n)
{
	m_nMaxHeight = n;
}

int   Object::GetMinWidth()
{
	return m_lMinWidth;
}
int   Object::GetMinHeight()
{
	return m_lMinHeight;
}
void  Object::SetMinWidth(int n)
{
	m_lMinWidth = n;
}
void  Object::SetMinHeight(int n)
{
	m_lMinHeight = n;
}