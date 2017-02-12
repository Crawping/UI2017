#include "stdafx.h"
#include "thread.h"
#include "print.h"
#include "memoryleakdetect.h"
#include "wnd.h"
#include "utility.h"
#include <atlstr.h>
#include <time.h> 

void  filte_block_type();
void  statistics_leak(list<AllocInfo*>& alloc_list, LEAK_REPORT_TYPE eSortType, HMODULE hModuleFilter);

// ע����Ҫֱ�ӽ�map��Ϊȫ�ֱ�����������MemoryLeakDetect����ǰ��������

unsigned int  g_thread_id;
HANDLE  g_thread_handle;
CRITICAL_SECTION  g_thread_cs;

struct ModuleAllocInfo
{
    HMODULE  module;
    LONG  allocbytes;

    bool operator == (const ModuleAllocInfo &moduleInfo) const
    {
        if (module == moduleInfo.module)
            return 1;
        else
            return 0;
    }
};

static list<ModuleAllocInfo> preListModuleAlloc;

HeapAllocs*  get_heap_alloc(HANDLE hHeap, bool bCreate)
{
    map<HANDLE, HeapAllocs*>::iterator iter = g.map_heaps.find(hHeap);
    if (iter != g.map_heaps.end())
        return iter->second;

    HeapAllocs* p = new HeapAllocs;
    p->hHeap = hHeap;
    g.map_heaps[hHeap] = p;
    return p;
}

void  on_heap_destroy(HANDLE hHeap)
{
    SCOPE_LOCK;

    map<HANDLE, HeapAllocs*>::iterator iter = g.map_heaps.find(hHeap);
    if (iter == g.map_heaps.end())
        return;

    HeapAllocs* pHeapAllocs = iter->second;
	delete pHeapAllocs;
	g.map_heaps.erase(iter);
}

void  on_malloc(HANDLE hHeap, AllocInfo* pInfo)
{
    SCOPE_LOCK;

	HeapAllocs* pHeapAllocs = get_heap_alloc(hHeap, true);
    map<LPVOID, AllocInfo*>::iterator iter =
        pHeapAllocs->map_allocs.find(pInfo->lpMem);

    if (iter != pHeapAllocs->map_allocs.end())
    {
        //assert (0 && "malloc / free dismatch");
        trace_stack(iter->second->framepointArray);
        DebugBreak();
    }

    pInfo->dwHash = hash_stack_framepoint(pInfo->framepointArray);

    pHeapAllocs->map_allocs[pInfo->lpMem] = pInfo;
}

void  on_free(HANDLE hHeap, LPVOID lpMem)
{
    SCOPE_LOCK;

	HeapAllocs* pHeapAllocs = get_heap_alloc(hHeap, true);
	map<LPVOID, AllocInfo*>::iterator iter =
		pHeapAllocs->map_allocs.find(lpMem);
     
	if (iter == pHeapAllocs->map_allocs.end())
    {
        //assert (0 && "malloc / free dismatch");
        //mld_printf("dismatch free, no malloc info: 0x%08X\r\n", lpMem);
        return;
    }

	delete iter->second;
    pHeapAllocs->map_allocs.erase(iter);
}

#if 0



// void  destroy_mld_thread()
// {
//     post_mld_thread_msg(WM_QUIT, 0, 0);
// 
//     HANDLE hThread = OpenThread(SYNCHRONIZE, 0, g_thread_id);
//     if (hThread)
//     {
//         WaitForSingleObject(hThread, INFINITE);
//         CloseHandle(hThread);
//     }
// }

// �������߳��е���һ����������ȫ����mld�߳��е����ݡ�
void  suspend_thread_call(pfnInvokeMLDRes p)
{
    HANDLE hEventSuspendMLDThread = CreateEvent(0,0,0,0);
    HANDLE hEventWaitReportFinish = CreateEvent(0,0,0,0);
    post_mld_thread_msg(THREAD_MSG_ID_SUSPEND, 
        (WPARAM)hEventSuspendMLDThread, 
        (LPARAM)hEventWaitReportFinish);

    // �ȴ�MLD�߳̿���
    WaitForSingleObject(hEventSuspendMLDThread, INFINITE);
    CloseHandle(hEventSuspendMLDThread);

    // ���ڿ��Է��ĵĶ�ȡmld�߳��е�������
    p();

    // ֪ͨMLD�߳̿���������
    SetEvent(hEventWaitReportFinish);
    Sleep(10);
}

void on_suspend(HANDLE hEventSuspend, HANDLE hEventWait)
{
    // ֪ͨ���߳̿��Է���������
    if (hEventSuspend)
        SetEvent(hEventSuspend);

    WaitForSingleObject(hEventWait, INFINITE);
    // ���߳�д�ɲ��������߳̿���������
    CloseHandle(hEventWait);
}
void  uninit_thread()
{
    CloseHandle(g_thread_handle);
    g_thread_handle = 0;
    g_thread_id = 0;
    g.map_allocs.clear();
}

unsigned int __stdcall mld_thread(LPVOID lpParameter)
{
    init_thread();
    SetEvent((HANDLE)lpParameter);

    thread_run();
    uninit_thread();
    return 0;
}

void  init_thread()
{
    // ������Ϣ����
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
}

void  thread_run()
{
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {       
        handle_message(msg.message, msg.wParam, msg.lParam);
    }
}

// ��ʼ��ʱ����list��������??
void post_mld_thread_msg( UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (0 == PostThreadMessage(g_thread_id, msg, wParam, lParam))
    {
        // �ڳ�ʼ��ʱ���̻߳�û�д�����ɾ��Ѿ��������ڴ���䡣
        // �������Ϣ������
        DWORD dwErr = GetLastError();
        if (g_thread_id && dwErr == 0x5a4) // ��Ч���̱߳�ʶ��
        {
            if (msg == THREAD_MSG_ID_MALLOC || msg == THREAD_MSG_ID_FREE)
            {
                MSG _msg = {0};
                _msg.message = msg;
                _msg.wParam = wParam;
                _msg.lParam = lParam;
                g.list_thread_delay_msg.push_back(_msg);
            }
        }
    }
    else
    {
        if (g.list_thread_delay_msg.size())
        {
            list<MSG>::iterator iter = g.list_thread_delay_msg.begin();
            for (; iter != g.list_thread_delay_msg.end(); ++iter)
            {
                PostThreadMessage(g_thread_id, 
                    iter->message, iter->wParam, iter->lParam);
            }
            g.list_thread_delay_msg.clear();
        }
    }
}

void  create_mld_thread()
{
    if (g_thread_id)
        return;

    HANDLE hEvent = CreateEvent(0,0,FALSE,0);
    g_thread_handle = (HANDLE)_beginthreadex(0, 0, mld_thread, hEvent, 0, 
        &g_thread_id);
    //WaitForSingleObject(hEvent, INFINITE);
    CloseHandle(hEvent);

    // ���ﲻ��wait�����ڳ�ʼ��ȫ�ֱ�����
}
#endif


void  on_clear()
{
    SCOPE_LOCK;

	g.clear_map_heaps();
    preListModuleAlloc.clear();
}

bool sort_by_modulealloc_callback(ModuleAllocInfo& o1, ModuleAllocInfo& o2)
{
    return o1.allocbytes > o2.allocbytes;
}

// ��ȡ��ģ�������ڴ���Ϣ
void on_moduleallocinfo_req()
{
    // ���˵�CRT����
    filte_block_type();

    SCOPE_LOCK;  // alloc_list��������ָ�룬���Ի�Ҫ����

    // ����HASHֵ���й���
    list<AllocInfo*> alloc_list;
    statistics_leak(alloc_list, LEAK_REPORT_TYPE_FAST, NULL);
    if (alloc_list.empty())
        return;

    // ���������ÿ��ģ���������ֽ���
    list<ModuleAllocInfo>  listModuleAlloc;
    for (list<AllocInfo*>::iterator iter = alloc_list.begin();
        iter != alloc_list.end(); ++iter)
    {
        AllocInfo* p = *iter;

        // �Ӷ�ջ��ȡ��һ������ĵ����ڴ�����ģ��
        HMODULE hAppModule = get_appmodule_from_stack(p->framepointArray);
        if (!hAppModule)
            continue;

        list<ModuleAllocInfo>::iterator iter2 = listModuleAlloc.begin();
        for (; iter2 != listModuleAlloc.end(); ++iter2)
        {
            if (iter2->module == hAppModule)
            {
                iter2->allocbytes += (p->dwBytes * p->dwSameHashCount);
                hAppModule = NULL; 
                break; 
            }
        }
        if (hAppModule) // û�ҵ����ӽ�ȥ 
        {
            ModuleAllocInfo  mai = {0};
            mai.module = hAppModule;
            mai.allocbytes = (p->dwBytes * p->dwSameHashCount);
            listModuleAlloc.push_back(mai);
        }
    }

    // ����
    listModuleAlloc.sort(sort_by_modulealloc_callback);

    // ���
    mld_printf("\r\nApplication Module HeapAlloc Info:\r\n");

    long total = 0;
    list<ModuleAllocInfo>::iterator iter2 = listModuleAlloc.begin();
    for (; iter2 != listModuleAlloc.end(); ++iter2)
    {
        long lDifferByte = 0;
        HMODULE hModule = iter2->module;
        TCHAR szName[MAX_PATH]; 
        get_module_name(hModule, szName);
        ULONG uByte = iter2->allocbytes;

        // �Ա���һ�ε��ڴ����������
        list<ModuleAllocInfo>::iterator iterPre = find(preListModuleAlloc.begin(), preListModuleAlloc.end(), *iter2);
        if (iterPre != preListModuleAlloc.end())
        {
            lDifferByte = uByte - iterPre->allocbytes;
        }

        if (lDifferByte > 0)
            mld_printf(TEXT("%25s :  %-7d  ��+%d��\r\n"),
                szName, uByte, lDifferByte);
        else if(lDifferByte < 0)
            mld_printf(TEXT("%25s :  %-7d  ��%d��\r\n"),
            szName, uByte, lDifferByte);
        else
            mld_printf(TEXT("%25s :  %-7d\r\n"),
            szName, uByte);

        total += iter2->allocbytes; 
    }

    preListModuleAlloc.clear();
    preListModuleAlloc = listModuleAlloc;

    mld_printf("Report Finish, total alloc: %d\r\n", total);
}

// ���ض���˳���������
bool sort_by_count_callback(AllocInfo* p1, AllocInfo* p2)
{
    return p1->dwSameHashCount > p2->dwSameHashCount;
}
bool sort_by_bytes_callback(AllocInfo* p1, AllocInfo* p2)
{
    return p1->dwBytes*p1->dwSameHashCount > p2->dwBytes*p2->dwSameHashCount;
}


// ͳ��ͬ��HASHֵ���ڴ����
void  statistics_leak(list<AllocInfo*>& alloc_list, LEAK_REPORT_TYPE eSortType, HMODULE hModuleFilter)
{
    SCOPE_LOCK;

    map<DWORD,AllocInfo*>  map_by_hash;
    map<HANDLE, HeapAllocs*>::iterator iter = g.map_heaps.begin();
    for (; iter != g.map_heaps.end(); ++iter)
    {
		HeapAllocs* pHeapAllocs = iter->second;

		map<LPVOID, AllocInfo*>::iterator iter2 = pHeapAllocs->map_allocs.begin();
		for (; iter2 != pHeapAllocs->map_allocs.end(); ++ iter2)
		{
			DWORD dwHash = iter2->second->dwHash;

			map<DWORD,AllocInfo*>::iterator iter_count = map_by_hash.find(dwHash);
			if (iter_count == map_by_hash.end())
			{
				map_by_hash[dwHash] = iter2->second;
				iter2->second->dwSameHashCount = 1;
			}
			else
			{
				map_by_hash[dwHash]->dwSameHashCount++;
			}
		}
    }

    // �浽һ��list�У�������ģ��
    {
        map<DWORD,AllocInfo*>::iterator iter = map_by_hash.begin();
        for (; iter != map_by_hash.end(); ++iter)
        {
            if (hModuleFilter)
            {
                if (hModuleFilter != get_appmodule_from_stack(iter->second->framepointArray))
                    continue;
            }
            alloc_list.push_back(iter->second);
        }
    }

    // ��������
    if (g_report_type == LEAK_REPORT_TYPE_COUNT)
    {
        alloc_list.sort(sort_by_count_callback);
    }
    else if (g_report_type == LEAK_REPORT_TYPE_BYTES)
    {
        alloc_list.sort(sort_by_bytes_callback);
    }
}



bool  test_is_crt_block(LPVOID lpMem, SIZE_T size)
{
    long* pFill1 = (long*)((byte*)lpMem + sizeof(_CrtMemBlockHeader) - nNoMansLandSize);
    long* pFill2 = (long*)((byte*)lpMem + size - nNoMansLandSize);
    
    if (IsBadReadPtr(pFill1, nNoMansLandSize))
        return false;
    if (IsBadReadPtr(pFill2, nNoMansLandSize))
        return false;

    if (*pFill1 == 0xFDFDFDFD && *pFill2 == 0xFDFDFDFD)
        return true;

    return false;
}

void  filte_block_type()
{
    SCOPE_LOCK;

	map<HANDLE, HeapAllocs*>::iterator iter = g.map_heaps.begin();
	for (; iter != g.map_heaps.end(); ++iter)
	{
		HeapAllocs* pHeapAllocs = iter->second;

		map<LPVOID, AllocInfo*>::iterator iter2 = pHeapAllocs->map_allocs.begin();
		for (; iter2 != pHeapAllocs->map_allocs.end(); iter2)
		{
			AllocInfo* p = iter2->second;
            
            if (!test_is_crt_block(p->lpMem, p->dwBytes))
            {
                p->nBlockType = -1;
                ++iter2;
                continue;
            }

            _CrtMemBlockHeader* pHeader = (_CrtMemBlockHeader*)p->lpMem;
            p->nBlockType = pHeader->nBlockUse;
            if (p->nBlockType == _FREE_BLOCK ||
                p->nBlockType == _IGNORE_BLOCK ||
                p->nBlockType == _CRT_BLOCK)
            {
                delete p;
                iter2 = pHeapAllocs->map_allocs.erase(iter2);
            }
            else
            {
                ++iter2;
            }
		}
	}
}

LPCSTR get_blocktype_text(int n)
{
	switch (n)
	{
	case _FREE_BLOCK:
		return ("Free");

	case _NORMAL_BLOCK:
		return ("Normal");

	case _CRT_BLOCK:
		return ("Crt");

	case _IGNORE_BLOCK:
		return ("Ignore");

	case _CLIENT_BLOCK:
		return ("Client");
	}

	return ("Unknown");
}

// ���˳�����ʱ�����ֹͣ���԰�ť��Ҫ���ܹ������˳������ܿ�ס��
//
// ֧�����������
// ֧��ģ��������
// ֧�ְ�ESC�����˳�
//
void  leak_report(bool bQuitDebugMode)
{
	// flush_mld_wnd_msg();
	filte_block_type();

    // ����HASHֵ���й���
    HMODULE hModuleFilter = get_report_filter_module();

    SCOPE_LOCK;  // alloc_list��������ָ�룬���Ի�Ҫ����

    list<AllocInfo*> alloc_list;
    statistics_leak(alloc_list, g_report_type, hModuleFilter);

	if (alloc_list.empty())
		return;

    time_t t = time(0);
    char s[32];
    char time[64];
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&t));
    sprintf_s(time, sizeof(time), "----------  Start Check Leak  %s  ----------\r\n", s);
    ::mld_print(time);

    list<AllocInfo*>::iterator iter = alloc_list.begin();
    for (; iter != alloc_list.end(); ++iter)
    {
		if (is_key_down(VK_ESCAPE))
			break;

        AllocInfo* pInfo = *iter;
		// ֻ����1�ε�ʱ�򲻴�ӡ
        if (pInfo->dwSameHashCount > 1 || !g.bFilterOneCount)
        {
            mld_printf("%s block at 0x%08X, %d bytes, hash: 0x%08X, count: %d. callstack:\r\n\r\n", 
                get_blocktype_text(pInfo->nBlockType),
                pInfo->lpMem,
                pInfo->dwBytes,
                pInfo->dwHash,
                pInfo->dwSameHashCount);

            trace_stack(pInfo->framepointArray);
            mld_print("\r\n");
        }

		if (bQuitDebugMode)
		{
			// �˳�����ʱ��һֱreport�ẻסVS������ȴ�һ�¡�ԭ��δ֪��
			// Sleep(10)���������ǻῨסVS
			Sleep(50);
		}
    }
    
    ::mld_printf("----------  Leak Count: %d  ----------\r\n",
        alloc_list.size());
}

bool  handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case THREAD_MSG_ID_MALLOC:
        on_malloc((HANDLE)wParam, (AllocInfo*)lParam);
        break;

    case THREAD_MSG_ID_FREE:
        on_free((HANDLE)wParam, (LPVOID)lParam);
        break;

    case THREAD_MSG_ID_REPORT:
        leak_report(!!wParam);
        break;

        // 	case THREAD_MSG_ID_SUSPEND:
        // 		on_suspend((HANDLE)wParam, (HANDLE)lParam);
        // 		break;

    case THREAD_MSG_ID_CLEAR:
        on_clear();
        break;

	case THREAD_MSG_ID_HEAPDESTROY:
		on_heap_destroy((HANDLE)wParam);
		break;

    case THREAD_MSG_ID_MODULEALLOCINFO_REQ:
        on_moduleallocinfo_req();
        break;

    default:
        return false;
    }

    return true;
}

void  lock_handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // ����������ֱ��lock���׳��������⣬���������
    //EnterCriticalSection(&g_thread_cs);

    handle_message(uMsg, wParam, lParam);

    //LeaveCriticalSection(&g_thread_cs);
}