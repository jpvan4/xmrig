
#include "core/Controller.h"
#include "backend/cpu/Cpu.h"
#include "core/config/Config.h"
#include "core/Miner.h"
#include "crypto/common/VirtualMemory.h"
#include "net/Network.h"


#ifdef XMRIG_FEATURE_API
#   include "base/api/Api.h"
#   include "hw/api/HwApi.h"
#endif


#include <cassert>


xmrig::Controller::Controller(Process *process) :
    Base(process)
{
}


xmrig::Controller::~Controller()
{
    VirtualMemory::destroy();
}


int xmrig::Controller::init()
{
    Base::init();

    VirtualMemory::init(config()->cpu().memPoolSize(), config()->cpu().hugePageSize());

    m_network = std::make_shared<Network>(this);

#   ifdef XMRIG_FEATURE_API
    m_hwApi = std::make_shared<HwApi>();
    api()->addListener(m_hwApi.get());
#   endif

    return 0;
}


void xmrig::Controller::start()
{
    Base::start();

    m_miner = std::make_shared<Miner>(this);

    network()->connect();
}


void xmrig::Controller::stop()
{
    Base::stop();

    m_network.reset();

    m_miner->stop();
    m_miner.reset();
}


xmrig::Miner *xmrig::Controller::miner() const
{
    assert(m_miner);

    return m_miner.get();
}


xmrig::Network *xmrig::Controller::network() const
{
    assert(m_network);

    return m_network.get();
}


void xmrig::Controller::execCommand(char command)
{
    miner()->execCommand(command);
    network()->execCommand(command);
}
