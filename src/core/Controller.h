

#ifndef XMRIG_CONTROLLER_H
#define XMRIG_CONTROLLER_H


#include "base/kernel/Base.h"


#include <memory>


namespace xmrig {


class HwApi;
class Job;
class Miner;
class Network;


class Controller : public Base
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Controller)

    Controller(Process *process);
    ~Controller() override;

    int init() override;
    void start() override;
    void stop() override;

    Miner *miner() const;
    Network *network() const;
    void execCommand(char command);

private:
    std::shared_ptr<Miner> m_miner;
    std::shared_ptr<Network> m_network;

#   ifdef XMRIG_FEATURE_API
    std::shared_ptr<HwApi> m_hwApi;
#   endif
};


} // namespace xmrig


#endif /* XMRIG_CONTROLLER_H */
