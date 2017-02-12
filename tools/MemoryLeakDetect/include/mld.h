#ifndef MEMORY_LEAK_DETECT_H_LEEIHCY
#define MEMORY_LEAK_DETECT_H_LEEIHCY
#ifdef _DEBUG
#ifdef MLD_EXPORTS
#define MLD_API __declspec(dllexport)
#else
#define MLD_API __declspec(dllimport)
#endif

////////////////////////////////////////////////////////////////////////// 
//
//   Copy From: Visual Leak Detector (Open Source)
//
//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "mld.lib")

// �ⲿ����ֻ��Ҫ���#include "mld.h"���ɴ���MemoryLeakDetectȫ�ֺ������죬
// �����ٵ�������������API
// 
// Force a symbolic reference to the global VisualLeakDetector class object from
// the DLL. This ensures that the DLL is loaded and linked with the program,
// even if no code otherwise imports any of the DLL's exports.
#pragma comment(linker, "/include:__imp_?g@@3VMemoryLeakDetect@@A")

namespace MLD
{
    extern "C"
    {
        void MLD_API PauseLeakDetect(bool bPause);
		void MLD_API ShowMldWindow();
    }
}

#endif
#endif // MEMORY_LEAK_DETECT_H_LEEIHCY


//////////////////////////////////////////////////////////////////////////
/* Ҫ����ȫ�ֱ�����ʵ������ʱ�������ڴ����֮ǰ��ʼ�������˳�ʱ������
 * �ڴ��ͷŽ����󱨸�й¶�ǳ��鷳��
 * ��Ҫ������:
 *
 *  1. ���ȷ���Լ���ȫ�ֱ���������ȫ�ֱ���ǰ��ʼ����
 *     ����VLD��������ʹ��#pragmaָ�������һ������
 *     #pragma comment(linker, "/include:__imp_?g_mld@@3VMemoryLeakDetect@@A")
 *     �����ⲿģ��ֻҪ������"mld.h"�ͻᴥ��ȫ����Ĺ��캯��
 *
 *  2. ��ȫ����MLD�д����̣߳������������ɹ���ʹ��waitforsingleobject(thread)
 *     ���������������
 *     ��������Ϊȫ���๹�캯���л�����crt��ʼ���׶Σ��������������ɹ���
 *
 *  3. ��ȫ��������������У�����ȫ������Ҳ�ѱ��ͷţ������Լ����̡߳��Լ���
 *     ���ȫ�����ݵȡ�
 *
 *  4. ��ν��ȫ����/��̬�������з�����ڴ棬ֻ���ڳ����˳�ʱ�Ż��ͷŵ����⣿
 *     �����������ʹ��ȫ�ֱ���������leak report
 *  
 *  MLD���ö�̬��msvcr90d.dll��_CrtSetAllocHookֻ��Ϊ���ж�̬����msvcr90d.dll
 *  ��ģ�����ø�HOOK����̬����msvcr90d.dll��ģ�����Լ���hookȫ�ֱ���������
 *  ������ΪMLD�е�HOOK��
 *  ��˷���ʹ��_CrtSetAllocHook����ΪHOOK RtlAllocateHeap
 *  
 *  ��ν����report_leak�ڼ䣬ֹͣ���Իᵼ��VS��ס�����⡣
 *  1. [����VLD]ÿ��report֮�����һ�� sleep(50)��
 *     sleep(10)Ҳ���������ǻῨסһ�¡�
 *
 *  ���ʵ��˫��������ݣ����ɶ�λ����Ӧ�Ĵ����У�
 *  1. ʹ��OutputDebugStringʱ�����ݵ�ǰ׺��ʽΪ��
 *     filepath(line)
 *  2. ǰ��ֻ�ܸ��ո񣬲���������������
 *  3. �����������������
 *
 *
 *  ʲôʱ��ȥΪMODULE���ط��ţ�
 *  1. û����hook_loadlibrary��Ϊÿ����ģ�����һ�η��ţ����������
 *     leak_report��ʱ��ȥ����
 *  
 *  
 *
 *    ԭ������ר��Ϊmld����һ���߳�������malloc/free/report�����飬��
 *  ����Ҫ��momoryleakdetectȫ����Ĺ���/�����д�����Щ���飬�߳��ڹ��캯��
 *  �л��������ˣ�����������������ǰ�������ˡ�
 *    ���ֱ��ʹ�����߳������������е�malloc/freeͨ��postmessage�ŵ�һ��
 *  mld_wnd��ȥ�ŶӴ���
 *    ʹ��postmessage�ķ�����һ���ܴ��ȱ�ݣ�ÿ����Ϣ���е��������Ϊ��10000.
 *  ����������ʱ����Ϣ���ͽ�ʧ�ܡ�
 *    ����޸�Ϊʹ��critical_section��ͬ��map�Ķ��̹߳������⡣
 *
 *
 *  ��Hook_HeapAlloc�У���ôȷ������Ƿ���CRT���õ��ڴ档������ǣ�����
 *  ����pHdr��������ȡblock type�� ���Ŀǰ����֧���Զ���heap�������ڴ档
 *
 *
 * �������⣺
 *  1. ���ȷ��mldģ������������ģ���ȱ�exe���أ�����memoryleakdetectȫ�ֶ���
 *     ���ܹ���ǰ����������ͷš�
 *     ��mld.lib���õ�exe���̵ģ�Linker->Input->Additional Dependencies�ĵ�һλ
 *
 *  2. Ŀǰ�ж�һ��block�Ƿ���crt����ķ�������׼ȷtest_is_crt_block
 *
 */