
#ifndef XMRIG_IJOBRESULTLISTENER_H
#define XMRIG_IJOBRESULTLISTENER_H


namespace xmrig {


class Client;
class JobResult;


class IJobResultListener
{
public:
    virtual ~IJobResultListener() = default;

    virtual void onJobResult(const JobResult &result) = 0;
};


} /* namespace xmrig */


#endif // XMRIG_IJOBRESULTLISTENER_H
