#ifndef __MSG_RA_ZHUI_YUE_SHANG_DIAN_HPP__
#define __MSG_RA_ZHUI_YUE_SHANG_DIAN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityzhuiyueshangdianparam.hpp"

//////////////////////////////////////////// 2208  ����  �����۱�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	class SCRAZhuiYueShangDianInfo						//<! 5330 ��Ϣ
	{
	public:
		SCRAZhuiYueShangDianInfo();
		MessageHeader header;
	
		unsigned short ra_day_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
		RAZhuiYueShangDianItem item_list_select[ZHUI_YUE_SHANG_DIAN_SELECT_NUM_SELECT];		//3 ��ѡ�����б�
	};

	class SCRAZhuiYueShangDianDrawRet						//<! 5331 �齱����
	{
	public:
		SCRAZhuiYueShangDianDrawRet();
		MessageHeader header;

		int count;
		int item_list[10];		//-1 �Ǵ󽱣������Ǹ���index
	};

	class SCRAZhuiYueShangDianDrawRecrod						//<! 5332 �齱��¼
	{
	public:
		SCRAZhuiYueShangDianDrawRecrod();
		MessageHeader header;

		int count;
		RAZhuiYueShangDianDrawRecord record_list[ZHUI_YUE_SHANG_DIAN_RECORD_NUM];		//-1 �Ǵ󽱣������Ǹ���index
	};
}

#pragma pack(pop)

#endif 
