#include "delayasyncconnectbigcrosstimer.hpp"
#include "global/bigcross/bigcrossmanager.hpp"

DelayAsyncConnectBigcrossTimer::DelayAsyncConnectBigcrossTimer()
{

}

void DelayAsyncConnectBigcrossTimer::OnTimer()
{
	// 不判断成功不成功，后续靠心跳来重连
	BigCrossManager::Instance().ConnectToBigCrossServer();
}

void DelayAsyncConnectBigcrossTimer::Free()
{
	delete this;
}
