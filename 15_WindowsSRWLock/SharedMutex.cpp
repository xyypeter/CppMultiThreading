#include "SharedMutex.h"

//构造函数:初始化读写锁
SharedMutex::SharedMutex(){
#ifdef WIN32
    ::InitializeSRWLock(&m_SRWLock);
#else
    ::pthread_rwlock_init(&m_SRWLock, nullotr);
#endif
}

//析构函数:清理资源
SharedMutex::~SharedMutex(){
#ifdef WIN32
    // windows平台:SRW锁不需要显示销毁，系统自动管理，SRW锁的优点，减少资源管理复杂性
#else
    // Linux/Unix平台:需要显示销毁pthread读写锁
    ::pthread_rwlock_destroy(&m_SRWLock);
#endif
}

//获取读锁(共享锁)
void SharedMutex::acquireReadLock(){
#ifdef WIN32
    // Windows平台：使用AcquireSRWLockShared获取共享（读）锁
    // 多个线程可以同时持有读锁，只要没有线程持有写锁
    ::AcquireSRWLockShared(&m_SRWLock);
#else
    // Linux/Unix平台：使用pthread_rwlock_rdlock获取读锁
    ::pthread_rwlock_rdlock(&m_SRWLock);
#endif  
}

//释放读锁
void SharedMutex::unlockReadLock(){

}

//获取写锁(独占锁)
void SharedMutex::acquireWriteLock(){
#ifdef WIN32
    // Windows平台：使用AcquireSRWLockExclusive获取独占（写）锁
    // 写锁是独占的，有线程持有写锁时，其他线程不能获取读锁或写锁
    ::AcquireSRWLockExclusive(&m_SRWLock);
#else
    // Linux/Unix平台：使用pthread_rwlock_wrlock获取写锁
    ::pthread_rwlock_wrlock(&m_SRWLock);
#endif   
}

//释放写锁
void SharedMutex::unlockWriteLock(){
#ifdef WIN32   
    // Windows平台：使用ReleaseSRWLockShared释放共享锁
    ::ReleaseSRWLockShared(&m_SRWLock);
#else
    // Linux/Unix平台：使用pthread_rwlock_unlock释放锁
    // 注意：pthread读写锁的读锁和写锁使用相同的释放函数
    ::pthread_rwlock_unlock(&m_SRWLock);
#endif
}

//SharedLockGuard类的实现
//构造函数:自动获取读锁
SharedLockGuard::SharedLockGuard(SharedMutex& sharedMutex):m_SharedMutex(sharedMutex){
    m_SharedMutex.acquireReadLock();
}

//析构函数:自动释放读锁
SharedLockGuard::~SharedLockGuard(){
    m_SharedMutex.unlockReadLock();
}

//UniqueLockGuard类的实现
//构造函数:自动获取写锁
UniqueLockGuard::UniqueLockGuard(SharedMutex& sharedMutex):m_SharedMutex(sharedMutex){
    m_SharedMutex.acquireWriteLock();
}

//析构函数:自动释放读锁
UniqueLockGuard::~UniqueLockGuard(){
    m_SharedMutex.unlockWriteLock();
}
