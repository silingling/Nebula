/*******************************************************************************
 * Project:  Nebula
 * @file     Chain.hpp
 * @brief 
 * @author   Bwar
 * @date:    2019年6月7日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_ACTOR_CHAIN_CHAIN_HPP_
#define SRC_ACTOR_CHAIN_CHAIN_HPP_

#include <queue>
#include "labor/Worker.hpp"
#include "actor/DynamicCreator.hpp"
#include "ChainModel.hpp"

namespace neb
{

class Chain: public ChainModel
{
public:
    Chain(ev_tstamp dChainTimeout = 60.0);
    Chain(const Chain&) = delete;
    Chain& operator=(const Chain&) = delete;
    virtual ~Chain();

    /**
     * @brief 会话超时回调
     */
    virtual E_CMD_STATUS Timeout() = 0;

    /**
     * @brief 检查Chain内数据是否已加载
     * @note 为满足数据共享并确保同一数据在同一个Worker内只需从
     * 外部存储中加载一次，提供了IsReady()，SetReady()，IsLoading()，
     * SetLoading()四个方法。如果一个或若干个Step获取到一个已创建好
     * 的Chain，则需先调用IsReady()判断数据是否就绪，若就绪则直接
     * 从Chain中读取，若未就绪则调用IsLoading()判断数据是否正在加
     * 载，若正在加载则直接return(CMD_STATUS_RUNNING)（框架会在数据
     * 加载完毕后调用该Step的Callback），否则加载数据并且SetLoading()，
     * 数据加载完毕后SetReady()。
     * @param pStep 调用IsReady()方法的调用者Step指针，用于记录
     * 哪些Step依赖于Chain的数据，在数据就绪时由框架主动调用
     * 依赖这些数据的Step回调而不需要等到超时才回调。
     */
    bool IsReady(Step* pStep);

protected:
    template <typename ...Targs> void Logger(int iLogLevel, const char* szFileName, unsigned int uiFileLine, const char* szFunction, Targs... args);
    template <typename ...Targs> std::shared_ptr<Step> MakeSharedStep(const std::string& strStepName, Targs... args);
    template <typename ...Targs> std::shared_ptr<Session> MakeSharedSession(const std::string& strSessionName, Targs... args);

private:
    friend class WorkerImpl;
    std::queue<uint32> m_vecWaitingStep;
};

template <typename ...Targs>
void Chain::Logger(int iLogLevel, const char* szFileName, unsigned int uiFileLine, const char* szFunction, Targs... args)
{
    m_pWorker->Logger(m_strTraceId, iLogLevel, szFileName, uiFileLine, szFunction, std::forward<Targs>(args)...);
}

template <typename ...Targs>
std::shared_ptr<Step> Chain::MakeSharedStep(const std::string& strStepName, Targs... args)
{
    return(m_pWorker->MakeSharedStep(this, strStepName, std::forward<Targs>(args)...));
}

template <typename ...Targs>
std::shared_ptr<Session> Chain::MakeSharedSession(const std::string& strSessionName, Targs... args)
{
    return(m_pWorker->MakeSharedSession(this, strSessionName, std::forward<Targs>(args)...));
}

} /* namespace neb */

#endif /* SRC_ACTOR_CHAIN_CHAIN_HPP_ */
