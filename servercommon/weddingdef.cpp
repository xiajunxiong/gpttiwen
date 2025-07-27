#include "weddingdef.hpp"

#define PAMRAM_SERIALIZE(PARAM)\
{\
	TLVSerializer t_data;\
	t_data.Reset((char*)&PARAM, sizeof(PARAM));\
	t_data.MoveCurPos(sizeof(PARAM));\
	ret = outstream.Push(t_data);\
	if (!ret)\
	{\
		return false;\
	}\
}

#define PAMRAM_UN_SERIALIZE(PARAM)\
{\
	TLVUnserializer t_data;\
	ret = instream.Pop(&t_data);\
	if (!ret)\
	{\
		return false;\
	}\
	\
	if (t_data.Size() <= sizeof(PARAM))\
	{\
		memcpy(&PARAM, t_data.Ptr(), t_data.Size());\
	}\
}

bool WeddingInfoDBList::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(m_save_index_begin)
		&& outstream.Push(m_count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < m_count && i < MAX_SIZE; ++i)
	{
		const WeddingFB::WeddingInfoDBParam & info = m_wedding_info_list[i];

		bool ret = outstream.Push(info.change_state)
			&& outstream.Push(info.wedding_id)
			&& outstream.Push(info.save_index)
			&& outstream.Push(info.begin_time)
			&& outstream.Push(info.cfg_ver)
			&& outstream.Push(info.is_finsih);
		if (!ret)
		{
			return false;
		}

		UNSTD_STATIC_CHECK(2 == WeddingFB::HOST_NUM);

		PAMRAM_SERIALIZE(info.host_info_[0]);
		PAMRAM_SERIALIZE(info.host_info_[1]);

		PAMRAM_SERIALIZE(info.invite_info);
		PAMRAM_SERIALIZE(info.common_info);

	}

	return true;
}

bool WeddingInfoDBList::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&m_save_index_begin)
		&& instream.Pop(&m_count);
	if (!ret) return false;

	for (int i = 0; i < m_count && i < MAX_SIZE; ++i)
	{
		WeddingFB::WeddingInfoDBParam & info = m_wedding_info_list[i];

		bool ret = instream.Pop(&info.change_state)
			&& instream.Pop(&info.wedding_id)
			&& instream.Pop(&info.save_index)
			&& instream.Pop(&info.begin_time)
			&& instream.Pop(&info.cfg_ver)
			&& instream.Pop(&info.is_finsih);
		if (!ret)
		{
			return false;
		}

		UNSTD_STATIC_CHECK(2 == WeddingFB::HOST_NUM);

		PAMRAM_UN_SERIALIZE(info.host_info_[0]);
		PAMRAM_UN_SERIALIZE(info.host_info_[1]);

		PAMRAM_UN_SERIALIZE(info.invite_info);
		PAMRAM_UN_SERIALIZE(info.common_info);

	}
	return true;
}
