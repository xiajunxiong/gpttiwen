#include "delayqueryuserchatrecord.hpp"
#include "chat/chatmanager.h"
#include "world.h"
#include "scene/scenemanager.hpp"

DelayQueryUserChatRecordTimer::DelayQueryUserChatRecordTimer(int uid)
	: m_uid(uid)
{

}

void DelayQueryUserChatRecordTimer::OnTimer()
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_uid));
	if (NULL == role) return;

	ChatManager::Instance().QueryUserChatRecord(role);
}

void DelayQueryUserChatRecordTimer::Free()
{
	delete this;
}
