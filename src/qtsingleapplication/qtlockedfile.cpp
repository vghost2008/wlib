#include "qtlockedfile.h"

QtLockedFile::QtLockedFile()
    : QFile()
{
#ifdef Q_OS_WIN
    m_semaphore_hnd = 0;
    m_mutex_hnd = 0;
#endif
    m_lock_mode = NoLock;
}

QtLockedFile::QtLockedFile(const QString &name)
    : QFile(name)
{
#ifdef Q_OS_WIN
    m_semaphore_hnd = 0;
    m_mutex_hnd = 0;
#endif
    m_lock_mode = NoLock;
}

bool QtLockedFile::isLocked() const
{
    return m_lock_mode != NoLock;
}

QtLockedFile::LockMode QtLockedFile::lockMode() const
{
    return m_lock_mode;
}

