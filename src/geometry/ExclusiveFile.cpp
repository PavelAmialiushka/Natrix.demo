#include "ExclusiveFile.h"

#include <windows.h>
#include <io.h>

ExclusiveFile::ExclusiveFile(QString name)
    : QFile(name)
    , filename_(name)
    , busy_(false)
    , notified_(false)
{
}

bool ExclusiveFile::open(QIODevice::OpenMode mode)
{
    if (!QFile::open(mode))
    {
        return false;
    }

    if (!lock())
    {
        busy_ = true;

        notified_ = false;
        emit fileIsBusy(&notified_);

        close();
        return false;
    }

    emit fileLocked();
    return true;
}

void ExclusiveFile::close()
{
    unlock();
    QFile::close();
    emit fileClosed();
}

bool ExclusiveFile::lock()
{
    Q_ASSERT(isOpen());
    locked_ = true;
    return (bool)::LockFile((HANDLE)_get_osfhandle(handle()), 0, 0, -1, -1);
}

bool ExclusiveFile::isLocked() const
{
    return locked_;
}

void ExclusiveFile::unlock()
{
    locked_ = false;
    ::UnlockFile((HANDLE)_get_osfhandle(handle()), 0, 0, -1, -1);
}

bool ExclusiveFile::isBusy() const
{
    return busy_;
}

bool ExclusiveFile::isNotified() const
{
    return notified_;
}


