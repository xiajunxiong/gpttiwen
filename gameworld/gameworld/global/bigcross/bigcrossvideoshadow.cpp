#include "bigcrossvideoshadow.hpp"
#include "servercommon/serverclmempool.h"
#include <cstring>

BigCrossVideoShadow& BigCrossVideoShadow::Instance()
{
	static BigCrossVideoShadow instance;
	return instance;
}

bool BigCrossVideoShadow::GetBigCrossVideo(long long video_id, const char** out_video_data, int* out_video_length)
{
	VideoMap::iterator it = m_video_map.find(video_id);
	if (it != m_video_map.end())
	{
		if (NULL != out_video_data) *out_video_data = it->second.video_buffer;
		if (NULL != out_video_length) *out_video_length = it->second.video_len;

		return true;
	}

	return false;
}

void BigCrossVideoShadow::SetBigCrossVideo(long long video_id, const char* video_data, int video_length)
{
	if (video_length <= 0) return;

	VideoMap::iterator it = m_video_map.find(video_id);
	if (it != m_video_map.end()) return;

	VideoData video;
	video.video_buffer = (char*)(new ServerMsgMem[video_length]);
	memset(video.video_buffer, 0, video_length);
	video.video_len = video_length;
	memcpy(video.video_buffer, video_data, video_length);

	m_video_map.insert(VideoMap::value_type(video_id, video));
}

BigCrossVideoShadow::BigCrossVideoShadow()
{

}

void BigCrossVideoShadow::Release()
{
	VideoMap::iterator it = m_video_map.begin();
	for (; it != m_video_map.end(); ++it)
	{
		if (NULL != it->second.video_buffer)
		{
			ServerMsgMem *tmp = (ServerMsgMem*)it->second.video_buffer;
			delete[]tmp;
			it->second.video_buffer = NULL;
		}
	}
	m_video_map.clear();
}

BigCrossVideoShadow::~BigCrossVideoShadow()
{
	this->Release();
}

