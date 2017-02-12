#include "stdafx.h"
#include "long_attribute.h"
#include "attribute.h"
#include "Src\Util\DPI\dpihelper.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateLongAttribute()
    {
        return new LongAttribute();
    }

}

LongAttribute::LongAttribute() 
{
    m_pILongAttribute = NULL;
    m_pBindValue = NULL;
    m_lDefault = 0;

	m_eDpiScaleType = LONGATTR_DPI_SCALE_DISABLE;

    _this = NULL;
    _setter = NULL;
    _getter = NULL;
}

LongAttribute::~LongAttribute()
{
    SAFE_DELETE(m_pILongAttribute);
}

ILongAttribute*  LongAttribute::GetILongAttribute()
{
    if (!m_pILongAttribute)    
        m_pILongAttribute = new ILongAttribute(this);

    return m_pILongAttribute;
}

void  LongAttribute::SetBindValue(void* p)
{
    m_pBindValue = (long*)p;
}

void  LongAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnLongSetter)_setter;
    this->_getter = (pfnLongGetter)_getter;
}

LongAttribute*  LongAttribute::SetDefault(long l)
{
	m_lDefault = l;

	return this;
}

//
// ע������ֱ��д��b = _getter();
//     �����Ľ���ǽ�LongAttribute��this��mov��ecx��
//
long  LongAttribute::GetLong()
{
	long lReturn = m_lDefault;
    if (m_pBindValue)
    {
        lReturn = (*m_pBindValue);
    }
    else if (_getter && _this)
    {
        lReturn = m_lDefault;
        long localVarThis = (long)_this;
        long localVarGetter = (long)_getter;

        __asm
        {
            mov ecx, localVarThis;  // ����this
            call localVarGetter;    // ����
            mov lReturn, eax;       // ��ȡ����ֵ
        }

    }
    else
    {
        lReturn = m_lDefault;
    }
	
	switch (m_eDpiScaleType)
	{
	case LONGATTR_DPI_SCALE_ENABLE:
		lReturn = UI::RestoreByDpi(lReturn);
		break;

	case LONGATTR_DPI_SCALE_GRATETHAN_0:
		lReturn = UI::RestoreByDpi_if_gt0(lReturn);
		break;
	}

	return lReturn;
}

void  LongAttribute::SetLong(long l)
{
	switch (m_eDpiScaleType)
	{
	case LONGATTR_DPI_SCALE_ENABLE:
		l = UI::ScaleByDpi(l);
		break;

	case LONGATTR_DPI_SCALE_GRATETHAN_0:
		l = UI::ScaleByDpi_if_gt0(l);
		break;
	}

    if (m_pBindValue)
    {
        (*m_pBindValue) = l;
    }
    else if (_setter && _this)
    {
        long localVarThis = (long)_this;
        long localVarSetter = (long)_setter;

        __asm
        {
            mov eax, dword ptr [l]   // ѹ�����
            push eax;

            mov ecx, localVarThis;   // ����this
            call localVarSetter;     // ����
        }
    }
}
LPCTSTR  LongAttribute::Get()
{
    long lValue = GetLong();
    
    LPCTSTR szAlias = m_mapAlias.GetAlias(lValue);
    if (szAlias)
    {
        return szAlias;
    }
    else
    {
		TCHAR* szText = GetTempBuffer(64);
        _stprintf(szText, TEXT("%d"), lValue);
        return szText;
    }
}

void  LongAttribute::Set(LPCTSTR szValue)
{
    long lValue = m_lDefault;

    if (m_mapAlias.GetAlias(szValue, &lValue))
    {
        SetLong(lValue);
    }
    else
    {
        lValue = _ttoi(szValue);
        SetLong(lValue);
    }
}

void  LongAttribute::Reset()
{
	SetLong(m_lDefault);
}

bool  LongAttribute::IsDefaultValue()
{
    return GetLong() == m_lDefault;
}

LongAttribute*  LongAttribute::AddAlias(long l, LPCTSTR sz)
{
    m_mapAlias.AddAlias(l, sz);
    return this;
}

void  LongAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Long2Editor(this, e);
}

long  LongAttribute::EnumAlias(pfnEnumAliasCallback c, WPARAM w, LPARAM l)
{
	return m_mapAlias.EnumAlias(c, w, l);
}
uint  LongAttribute::GetAliasCount()
{
	return m_mapAlias.GetCount();
}

LongAttribute* LongAttribute::SetDpiScaleType( LONGATTR_DPI_SCALE_TYPE e )
{
	m_eDpiScaleType = e;
	return this;
}