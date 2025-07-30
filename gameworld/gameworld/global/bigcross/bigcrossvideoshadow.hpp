#ifndef __BIG_CROSS_VIDEO_SHADOW_HPP__
#define __BIG_CROSS_VIDEO_SHADOW_HPP__

#include <map>

struct VideoData
{
	VideoData() : video_len(0), video_buffer(0) {}

	void Reset()
	{
		video_len = 0;
		video_buffer = 0;
	}

	int video_len;
	char* video_buffer;
};

class BigCrossVideoShadow
{
public:
	typedef std::map<long long, VideoData> VideoMap; // key is video_file_id

public:
	static BigCrossVideoShadow& Instance();

	bool GetBigCrossVideo(long long video_id, const char** out_video_data, int* out_video_length);
	void SetBigCrossVideo(long long video_id, const char* video_data, int video_length);

private:
	BigCrossVideoShadow();
	~BigCrossVideoShadow();
	BigCrossVideoShadow(const BigCrossVideoShadow&);
	BigCrossVideoShadow& operator=(const BigCrossVideoShadow&) const;

	void Release();

	VideoMap m_video_map;
};

#endif