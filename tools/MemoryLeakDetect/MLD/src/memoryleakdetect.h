#pragma once


// ��Ҫ�����ݷ�������൱�У���Ҫ������������Ϊȫ�ֱ�����
// �����˳�ʱMemoryLeakDetect����ͷţ�Ҫ��֤�������ݲ����ȱ��ͷš�

#define MAXSTACKDEPTH  32

struct AllocInfo
{
    LPVOID  lpMem;            // �ڴ��ַ
    SIZE_T  dwBytes;          // �����ֽ���
    int     nBlockType;       // �ڴ�����
    DWORD   dwHash;           // HASHֵ
    DWORD   dwSameHashCount;  // �����������ʱ������ֻ��ÿ��ͳ��ʱ���á�����ͬһHASHֻ��һ��AllocInfo��dwSameHashCount��ֵ
    UINT_PTR  framepointArray[MAXSTACKDEPTH];  // ����ʱ�Ķ�ջ
};

class HeapAllocs
{
public:
    HeapAllocs();
    ~HeapAllocs();

    HANDLE hHeap;
    map<LPVOID, AllocInfo*>  map_allocs;
};

class MemoryLeakDetect
{
public:
    MemoryLeakDetect();
    ~MemoryLeakDetect();

    void clear_map_heaps();

public:
    map<HANDLE, HeapAllocs*>  map_heaps;
	DWORD  dwMainThreadId;
    bool  bPause;  // ��ͣ��¼malloc�����Լ�¼free/destroy��δ����
    bool  bFilterOneCount;  // ֻ����1�ε�ʱ�򲻴�ӡ
};

extern __declspec(dllexport) MemoryLeakDetect g;

inline bool IsMainThread()
{
	return GetCurrentThreadId() == g.dwMainThreadId;
}