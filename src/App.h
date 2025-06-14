#ifndef XMRIG_APP_H
#define XMRIG_APP_H


#include "base/kernel/interfaces/IConsoleListener.h"
#include "base/kernel/interfaces/ISignalListener.h"
#include "base/tools/Object.h"


#include <memory>


namespace xmrig {


class Console;
class Controller;
class Network;
class Process;
class Signals;


class App : public IConsoleListener, public ISignalListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(App)

    App(Process *process);
    ~App() override;

    int exec();

protected:
    void onConsoleCommand(char command) override;
    void onSignal(int signum) override;

private:
    bool background(int &rc);
    void close();

    std::shared_ptr<Console> m_console;
    std::shared_ptr<Controller> m_controller;
    std::shared_ptr<Signals> m_signals;
};


} /* namespace xmrig */


#endif /* XMRIG_APP_H */
