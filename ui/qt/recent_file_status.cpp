/* recent_file_status.cpp
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "recent_file_status.h"

#include <ui/qt/main_application.h>

RecentFileStatus::RecentFileStatus(const QString filename, QObject *parent) :
    QObject(parent),
    // Force a deep copy.
    filename_(QString::fromStdU16String(filename.toStdU16String()))
{
    // We're a QObject, which means that we emit a destroyed signal,
    // which might happen at the wrong time when automatic deletion is
    // enabled. This will trigger an assert in debug builds (bug 14279).
    setAutoDelete(false);
    // Qt::QueuedConnection creates a copy of our argument list. This
    // squelches what appears to be a ThreadSanitizer false positive.
    connect(this, &RecentFileStatus::statusFound, qobject_cast<MainApplication *>(parent),
            &MainApplication::itemStatusFinished, Qt::QueuedConnection);
}

void RecentFileStatus::run() {
    fileinfo_.setFile(filename_);

    if (fileinfo_.isFile() && fileinfo_.isReadable()) {
        emit statusFound(filename_, fileinfo_.size(), true);
    } else {
        emit statusFound(filename_, 0, false);
    }
    deleteLater();
}
