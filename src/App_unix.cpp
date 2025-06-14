

#include <cstdlib>
#include <csignal>
#include <cerrno>
#include <unistd.h>


#include "App.h"
#include "base/io/log/Log.h"
#include "core/Controller.h"


bool xmrig::App::background(int &rc)
{
    if (!m_controller->isBackground()) {
        return false;
    }

    int i = fork();
    if (i < 0) {
        rc = 1;

        return true;
    }

    if (i > 0) {
        rc = 0;

        return true;
    }

    i = setsid();

    if (i < 0) {
        LOG_ERR("setsid() failed (errno = %d)", errno);
    }

    i = chdir("/");
    if (i < 0) {
        LOG_ERR("chdir() failed (errno = %d)", errno);
    }

    return false;
}
