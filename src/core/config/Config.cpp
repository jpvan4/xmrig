
#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <uv.h>


#include "core/config/Config.h"
#include "3rdparty/rapidjson/document.h"
#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "base/net/dns/Dns.h"
#include "crypto/common/Assembly.h"


#ifdef XMRIG_ALGO_RANDOMX
#   include "crypto/rx/RxConfig.h"
#endif


#ifdef XMRIG_FEATURE_OPENCL
#   include "backend/opencl/OclConfig.h"
#endif


#ifdef XMRIG_FEATURE_CUDA
#   include "backend/cuda/CudaConfig.h"
#endif


namespace xmrig {


constexpr static uint32_t kIdleTime     = 60U;


const char *Config::kPauseOnBattery     = "pause-on-battery";
const char *Config::kPauseOnActive      = "pause-on-active";


#ifdef XMRIG_FEATURE_OPENCL
const char *Config::kOcl                = "opencl";
#endif

#ifdef XMRIG_FEATURE_CUDA
const char *Config::kCuda               = "cuda";
#endif

#if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
const char *Config::kHealthPrintTime    = "health-print-time";
#endif

#ifdef XMRIG_FEATURE_DMI
const char *Config::kDMI                = "dmi";
#endif


class ConfigPrivate
{
public:
    bool pauseOnBattery = false;
    CpuConfig cpu;
    uint32_t idleTime   = 0;

#   ifdef XMRIG_ALGO_RANDOMX
    RxConfig rx;
#   endif

#   ifdef XMRIG_FEATURE_OPENCL
    OclConfig cl;
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    CudaConfig cuda;
#   endif

#   if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
    uint32_t healthPrintTime = 60U;
#   endif

#   ifdef XMRIG_FEATURE_DMI
    bool dmi = true;
#   endif

    void setIdleTime(const rapidjson::Value &value)
    {
        if (value.IsBool()) {
            idleTime = value.GetBool() ? kIdleTime : 0U;
        }
        else if (value.IsUint()) {
            idleTime = value.GetUint();
        }
    }
};

}


xmrig::Config::Config() :
    d_ptr(new ConfigPrivate())
{
}


xmrig::Config::~Config()
{
    delete d_ptr;
}


bool xmrig::Config::isPauseOnBattery() const
{
    return d_ptr->pauseOnBattery;
}


const xmrig::CpuConfig &xmrig::Config::cpu() const
{
    return d_ptr->cpu;
}


uint32_t xmrig::Config::idleTime() const
{
    return d_ptr->idleTime * 1000U;
}


#ifdef XMRIG_FEATURE_OPENCL
const xmrig::OclConfig &xmrig::Config::cl() const
{
    return d_ptr->cl;
}
#endif


#ifdef XMRIG_FEATURE_CUDA
const xmrig::CudaConfig &xmrig::Config::cuda() const
{
    return d_ptr->cuda;
}
#endif


#ifdef XMRIG_ALGO_RANDOMX
const xmrig::RxConfig &xmrig::Config::rx() const
{
    return d_ptr->rx;
}
#endif


#if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
uint32_t xmrig::Config::healthPrintTime() const
{
    return d_ptr->healthPrintTime;
}
#endif


#ifdef XMRIG_FEATURE_DMI
bool xmrig::Config::isDMI() const
{
    return d_ptr->dmi;
}
#endif


bool xmrig::Config::isShouldSave() const
{
    if (!isAutoSave()) {
        return false;
    }

#   ifdef XMRIG_FEATURE_OPENCL
    if (cl().isShouldSave()) {
        return true;
    }
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    if (cuda().isShouldSave()) {
        return true;
    }
#   endif

    return (m_upgrade || cpu().isShouldSave());
}


bool xmrig::Config::read(const IJsonReader &reader, const char *fileName)
{
    if (!BaseConfig::read(reader, fileName)) {
        return false;
    }

    d_ptr->pauseOnBattery = reader.getBool(kPauseOnBattery, d_ptr->pauseOnBattery);
    d_ptr->setIdleTime(reader.getValue(kPauseOnActive));

    d_ptr->cpu.read(reader.getValue(CpuConfig::kField));

#   ifdef XMRIG_ALGO_RANDOMX
    if (!d_ptr->rx.read(reader.getValue(RxConfig::kField))) {
        m_upgrade = true;
    }
#   endif

#   ifdef XMRIG_FEATURE_OPENCL
    d_ptr->cl.read(reader.getValue(kOcl));
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    d_ptr->cuda.read(reader.getValue(kCuda));
#   endif

#   if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
    d_ptr->healthPrintTime = reader.getUint(kHealthPrintTime, d_ptr->healthPrintTime);
#   endif

#   ifdef XMRIG_FEATURE_DMI
    d_ptr->dmi = reader.getBool(kDMI, d_ptr->dmi);
#   endif

    return true;
}


void xmrig::Config::getJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;

    doc.SetObject();

    auto &allocator = doc.GetAllocator();

    Value api(kObjectType);
    api.AddMember(StringRef(kApiId),                    m_apiId.toJSON(), allocator);
    api.AddMember(StringRef(kApiWorkerId),              m_apiWorkerId.toJSON(), allocator);

    doc.AddMember(StringRef(kApi),                      api, allocator);
    doc.AddMember(StringRef(kHttp),                     m_http.toJSON(doc), allocator);
    doc.AddMember(StringRef(kAutosave),                 isAutoSave(), allocator);
    doc.AddMember(StringRef(kBackground),               isBackground(), allocator);
    doc.AddMember(StringRef(kColors),                   Log::isColors(), allocator);
    doc.AddMember(StringRef(kTitle),                    title().toJSON(), allocator);

#   ifdef XMRIG_ALGO_RANDOMX
    doc.AddMember(StringRef(RxConfig::kField),          rx().toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(CpuConfig::kField),         cpu().toJSON(doc), allocator);

#   ifdef XMRIG_FEATURE_OPENCL
    doc.AddMember(StringRef(kOcl),                      cl().toJSON(doc), allocator);
#   endif

#   ifdef XMRIG_FEATURE_CUDA
    doc.AddMember(StringRef(kCuda),                     cuda().toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(kLogFile),                  m_logFile.toJSON(), allocator);

    m_pools.toJSON(doc, doc);

    doc.AddMember(StringRef(kPrintTime),                printTime(), allocator);
#   if defined(XMRIG_FEATURE_NVML) || defined (XMRIG_FEATURE_ADL)
    doc.AddMember(StringRef(kHealthPrintTime),          healthPrintTime(), allocator);
#   endif

#   ifdef XMRIG_FEATURE_DMI
    doc.AddMember(StringRef(kDMI),                      isDMI(), allocator);
#   endif

    doc.AddMember(StringRef(kSyslog),                   isSyslog(), allocator);

#   ifdef XMRIG_FEATURE_TLS
    doc.AddMember(StringRef(kTls),                      m_tls.toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(DnsConfig::kField),         Dns::config().toJSON(doc), allocator);
    doc.AddMember(StringRef(kUserAgent),                m_userAgent.toJSON(), allocator);
    doc.AddMember(StringRef(kVerbose),                  Log::verbose(), allocator);
    doc.AddMember(StringRef(kWatch),                    m_watch, allocator);
    doc.AddMember(StringRef(kPauseOnBattery),           isPauseOnBattery(), allocator);
    doc.AddMember(StringRef(kPauseOnActive),            (d_ptr->idleTime == 0U || d_ptr->idleTime == kIdleTime) ? Value(isPauseOnActive()) : Value(d_ptr->idleTime), allocator);
}
