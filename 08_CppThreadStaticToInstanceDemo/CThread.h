#ifdef _WIN32
#include <windows.h>
typedef HANDLE THREAD_HANDLE;
#else
#include <pthread.h>
typedef pthread_t THREAD_HANDLE;
#endif

/// <summary>
/// 定义了一个线程对象
/// </summary>
class  CThread{
public:
    /// <summary>
    /// 构造函数
    /// </summary>
    CThread();

    /// <summary>
    /// 析构函数
    /// </summary>
    virtual ~CThread();

    /// <summary>
    /// 创建一个线程
    /// @return true:创建成功 false:创建失败
    /// </summary>
    virtual bool Create();

    /// <summary>
    /// 获得本线程对象存储的线程句柄
    /// @return 本线程对象存储的线程句柄线程句柄
    /// </summary>
    THREAD_HANDLE GetHandle();

    /// <summary>
    /// 线程睡眠seconds秒
    /// @param seconds 睡眠秒数
    /// </summary>
    void OSSleep(int nSeconds);

    void SleepMs(int nMilliseconds);

    bool Join();

    bool IsCurrentThread();

    void ExitThread();

private:
#ifdef WIN32
    static DWORD WINAPI _ThreadEntry(LPVOID pParam);
#else
    static void* _ThreadEntry(void* pParam);
#endif

    /// <summary>
    /// 虚函数，子类可做一些实例化工作
    /// @return true:创建成功 false:创建失败
    /// </summary>
    virtual bool InitInstance();

    /// <summary>
    /// 虚函数，子类清楚实例
    /// </summary>
    virtual void ExitInstance();

    /// <summary>
    /// 线程开始运行，纯虚函数，子类必须继承实现
    /// </summary>
    virtual void Run() = 0;

private:
    //线程句柄
    THREAD_HANDLE  m_hThread;
    DWORD          m_IDThread;
};
