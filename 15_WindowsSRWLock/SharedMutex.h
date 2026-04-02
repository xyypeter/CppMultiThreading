#pragma once
#ifndef __SHARED_MUTEX_H__
#define __SHARED_MUTEX_H__

#ifdef WIN32
#include <Windows.h>
#else
#include <pthred.h>
#endif

class SharedMutex final {
public:
    SharedMutex();
    ~SharedMutex();
    //读锁相关接口
    void acquireReadLock(); //获取读锁(共享锁)
    void unlockReadLock();  //释放读锁
    //写锁相关接口
    void acquireWriteLock();//获取写锁(独占锁)
    void unlockWriteLock(); //释放写锁
private:
    SharedMutex(const SharedMutex& rhs) = delete;           //禁用拷贝构造函数
    SharedMutex& operator=(const SharedMutex& rhs) = delete;//禁用赋值运算符
private:
#ifdef WIN32
    SRWLOCK          m_SRWLock;//windows平台 slim Reader/Writer
#else
    pthread_rwlock_t m_SRWLock;//Linux/Unix平台:pthread读写锁
#endif // WIN32
};

// 模拟std::shared_lock的RAII读锁守卫类
class SharedLockGuard final {
public:
    explicit SharedLockGuard(SharedMutex& sharedMutex);
    ~SharedLockGuard();
private:
    SharedLockGuard(const SharedLockGuard& rhs) = delete;
    SharedLockGuard& operator=(const SharedLockGuard& rhs) = delete;
private:
    SharedMutex& m_SharedMutex;//引用管理的SharedMutex对象
};

// 模拟std::unique_lock的RAII写锁守卫类
class UniqueLockGuard final{
public:
    // 构造函数：获取写锁
    explicit UniqueLockGuard(SharedMutex& sharedMutex);
    // 析构函数：自动释放写锁
    ~UniqueLockGuard();
private:
    // 禁用拷贝构造函数和赋值运算符
    UniqueLockGuard(const UniqueLockGuard& rhs) = delete;
    UniqueLockGuard operator=(const UniqueLockGuard& rhs) = delete;
private:
    SharedMutex& m_SharedMutex;  // 引用管理的SharedMutex对象
};


#endif