
#ifndef XMRIG_JOBRESULTS_H
#define XMRIG_JOBRESULTS_H


#include <cstddef>
#include <cstdint>


namespace xmrig {


class IJobResultListener;
class Job;
class JobResult;


class JobResults
{
public:
    static void done(const Job &job);
    static void setListener(IJobResultListener *listener, bool hwAES);
    static void stop();
    static void submit(const Job &job, uint32_t nonce, const uint8_t *result);
    static void submit(const JobResult &result);

#   if defined(XMRIG_FEATURE_OPENCL) || defined(XMRIG_FEATURE_CUDA)
    static void submit(const Job &job, uint32_t *results, size_t count, uint32_t device_index);
#   endif
};


} // namespace xmrig


#endif /* XMRIG_JOBRESULTS_H */
