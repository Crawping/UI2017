#ifndef _UI_CONTROLDEFINE_H_
#define _UI_CONTROLDEFINE_H_

// ���뵼������
#ifdef UICTRL_EXPORTS
#define UICTRL_API __declspec(dllexport)
#else
#define UICTRL_API __declspec(dllimport)
#endif

#define UICTRL_API_UUID(guid)  __declspec(uuid(#guid)) UICTRL_API

namespace UI
{
// �����λ��Control Sub Type��ʶ

#if 0

// richedit style
#define RICHEDIT_STYLE_TEXTSHADOW       0x0100    // �����ı���Ӱ������͸�������ϣ����������뱳����ɫ̫�ӽ���������Ӱ��������

    // tree style
#define TREE_STYLE_HASLINE              0x0100    // ��������

    // listbox style

    // menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // ����������ʱ���Ƿ��Զ��ͷ��Ӳ˵��ڴ档��Ҫ���ڴ�xml��load�Ĳ˵�
    // ��Ϊ���ڲ��������Ӳ˵����ڲ����������Ӳ˵������ⲿ�Լ�create����

// tab ctrl
#define TABCTRL_STYLE_BOTTOM            0x0100    // ��ťλ���²�
#endif
}

#endif  // _UI_CONTROLDEFINE_H_