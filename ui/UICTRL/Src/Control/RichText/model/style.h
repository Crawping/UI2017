#pragma once
#include "Inc\Interface\irichtext.h"

namespace UI
{
namespace RT
{

struct Style
{
    Style()
    {
		draw_link_underline = true;
		page_content_valign = VALIGN_top;
		page_content_halign = HALIGN_left;
		line_space = 0;
        SetRectEmpty(&page_padding);
    }

    // hover/pressʱ�Զ����������»���
    bool  draw_link_underline;  

    // ����������뷽ʽ
    VALIGN  page_content_valign;  

    // ���ݺ�����뷽ʽ
    HALIGN  page_content_halign;  

    // ҳ�����ܿհ�
    REGION4  page_padding;

    // �м��
    long  line_space;  
};

}
}