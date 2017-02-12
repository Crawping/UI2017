#pragma once
#include "element.h"
#include "unit.h"
#include "..\UISDK\Inc\Util\iimage.h"
#include "include\imagecache_def.h"

// ����֧�ֶ���
namespace UI
{
namespace RT
{
class ImageElement : 
	public Element, 
	public ICacheImageUpdateCallback,
	public IImageElement
{
public:
    ImageElement();

private:
    virtual NodeType  GetNodeType() {
        return NodeTypeImage; 
    }
    virtual void  LayoutMultiLine(MultiLineLayoutContext*) override;
	virtual void  OnLButtonDBClick(UINT nFlags, POINT ptDoc, bool& bHandled) override;

    virtual void  Draw(HDC hDC, Run* run, RECT* prcRun) override;
    virtual uint  GetWidth() override;
    virtual uint  GetHeight() override;

	virtual void OnCacheImageUpdate(ICacheImage*) override;

	virtual LPCTSTR  GetPath() override;

public:
    uint  GetImageWidth();
    uint  GetImageHeight();
	
public:
//     virtual void  Draw(HDC hDC, RECT* prcRun) override;
     SIZE  GetLayoutSize(SIZE pageContentSize, int lineRemain);

public:
    bool  Load(LPCTSTR szPath);
	void  SetCacheImage(const CCacheImage& image);

protected:
    ImageWrap  m_image;
	CCacheImage  m_cacheImage;

    // ���Ƹ߶ȡ���ʵ�ʸ߶ȿ��ܲ�һ��������ͼƬ�ܴ�����
    SIZE  m_sizeDraw;
};
}
}