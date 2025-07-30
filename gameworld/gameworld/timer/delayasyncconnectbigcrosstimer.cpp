#include "delayasyncconnectbigcrosstimer.hpp"
#include "global/bigcross/bigcrossmanager.hpp"

DelayAsyncConnectBigcrossTimer::DelayAsyncConnectBigcrossTimer()
{

}

void DelayAsyncConnectBigcrossTimer::OnTimer()
{
	// ���жϳɹ����ɹ�������������������
	BigCrossManager::Instance().ConnectToBigCrossServer();
}

void DelayAsyncConnectBigcrossTimer::Free()
{
	delete this;
}
