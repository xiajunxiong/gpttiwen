#ifndef __MSG_MARRIAGE_HPP__
#define __MSG_MARRIAGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/marrydef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSMarriageReq		//!<2551
	{
	public:
		enum MARRIAGE_REQ_TYPE
		{
			MARRIAGE_REQ_TYPE_INFO,						//!< 请求信息
			MARRIAGE_REQ_TYPE_PROPOSE,					//!< 求婚请求	p1 结婚类型 p2 目标uid
			MARRIAGE_REQ_TYPE_PROPOSE_REPLY,				//!< 回复求婚	p1  是否接受  p2 目标uid
			MARRIAGE_REQ_TYPE_DIVORCE,					//!< 离婚请求
			MARRIAGE_REQ_TYPE_DIVORCE_REPLY,				//!< 回复离婚	p1 是否接受离婚提议
			MARRIAGE_REQ_TYPE_DIVORCE_FORCE,				//!< 请求强制离婚
			MARRIAGE_REQ_TYPE_REGRET_DIVORCE_FORCE,		//!< 取消强制离婚
			MARRIAGE_REQ_TYPE_TASK_REQ_ROUTE,				//!< 情缘日常任务转发
			MARRIAGE_REQ_TYPE_REQ_TASK,					//!< 日常任务开始  p1：1	快乐拼图 ，2	心有灵犀，3	共渡难关，4	快乐拼图，5	育儿心得
			MARRIAGE_REQ_TYPE_AUTO_FB,					//!< 扫荡
			MARRIAGE_REQ_TYPE_ANSWSER_DUET,				//!< 双人回答 p1 是选择的答案
			MARRIAGE_REQ_TYPE_ANSWSER_SINGLE,				//!< 单人回答 p1 是选择的答案
			MARRIAGE_REQ_TYPE_QUERY_OTHER,				//!< 查询别人结婚信息 p1 role_id
			MARRIAGE_REQ_TYPE_CANCEL_PROPOSE,			//!< 取消求婚请求	p1  目标uid
			MARRIAGE_REQ_TYPE_DUET_PUZZLE,				//!< p1 是否完成了
			MARRIAGE_REQ_TYPE_NEXT_QUESTION_DUET,				//!< 双人请求下一题
			MARRIAGE_REQ_TYPE_NEXT_QUESTION_SINGLE,				//!< 单人请求下一题
		};

		CSMarriageReq();
		MessageHeader		header;

		short				req_type;
		short				param1;
		int				param2;
		int				param3;
	};

	class SCMarryReqRoute	//!<2552			求婚信息
	{
	public:
		SCMarryReqRoute();
		MessageHeader		header;

		int					marry_type;			//!< 结婚类型
		int					req_uid;				//!< 求婚者uid
		GameName			req_name;				//!< 求婚者名字
	};

	class SCMarryInfo	//!<2553			信息
	{
	public:
		SCMarryInfo();
		MessageHeader		header;

		int lover_uid;								//!< 伴侣UID
		GameName lover_name;						//!< 伴侣名字
		int avatar_type;
		unsigned int last_marry_time;					//!< 结婚时间
		unsigned int last_divorce_time;					//!< 离婚时间	
		unsigned int apply_divorce_time;				//!< 申请强制离婚时间

		int puzzle_play_times;			//快乐拼图玩的次数
		int question_play_times;			//答题玩的次数
		int max_pass_fb_wave;				//最大波数
		short curr_wave;			//当前波数
		short is_auto_fb;				//是否扫荡
	};

	class SCDivorceReqRoute		//!<2554			对方请求离婚
	{
	public:
		SCDivorceReqRoute();
		MessageHeader		header;
	};

	class SCMarryProposeNotice	//!<2555	
	{
	public:
		SCMarryProposeNotice();
		MessageHeader		header;

		enum OP_RESULT
		{
			OP_RESULT_PROPOSE_SUCC,					//!<通知求婚者请求已发送到对方
			OP_RESULT_REFUSE,							//!<对方拒绝
		};

		int result;
	};

	struct MarriageSeekingInfo
	{
		GameName			gamename;
		GuildName			guildname;
		int					user_id;
		char				reserve_ch;
		signed char				sex;
		char				prof;
		char				is_online;
		short				level;
		short				reserve_sh;
		unsigned int		timestamp;
		long long		m_avator_timestamp;
		int				avator_frame_id;
		MarriageSeekingNotice marriage_seeking_notice;
	};


	class CSMarriageSeekingOpera			//!<2556
	{
	public:
		CSMarriageSeekingOpera();
		MessageHeader header;

		short type;
		short reserve_sh;
		MarriageSeekingNotice marriage_seeking_notice;
	};

	class SCMarriageSeekingInfo						//!<2557
	{
	public:
		SCMarriageSeekingInfo();							
		MessageHeader header;

		int count;
		MarriageSeekingInfo	marriage_seeking_list[MARRIAGE_SEEKING_MAX_COUNT];
	};

	class SCMarriageTaskRoute						//!<2558  转发
	{
	public:
		SCMarriageTaskRoute();					//发2251 req=MARRIAGE_REQ_TYPE_TASK_REQ_ROUTE。对方就会收到这个
		MessageHeader header;

		int role_id;
		int param1;
		int param2;
		unsigned int param3;
	};

	class SCMarriageTaskQuestionInfo						//!<2559		题目信息
	{
	public:
		SCMarriageTaskQuestionInfo();
		MessageHeader header;

		char question_type;		//是否双人
		char question_num;		//第几题
		short question_id;  //问题id
		unsigned int end_time;//结束时间
	};

	class SCMarriageTaskQuestionResult						//!<2560  结果
	{
	public:
		SCMarriageTaskQuestionResult();
		MessageHeader header;

		char question_type;		//是否双人
		char is_right;		//是否正确
		short question_id;  //问题id
	};

	class SCMarriageQueryRet								//!<2561  结果
	{
	public:
		SCMarriageQueryRet();
		MessageHeader header;

		int role_id;				
		int lover_id;
		short prof;
		short sex;
	};

	class SCMarriageDuetPuzzleInfo								//!<2562  结果
	{
	public:
		SCMarriageDuetPuzzleInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			DUET_PUZZLE_START,		//开始		p1 结束时间
			DUET_PUZZLE_END,			//结束		p1 0:没两个完成   1:两个完成了
			DUET_PUZZLE_LOGOUT,		//其中有一个掉线,就会发
			DUET_PUZZLE_CANCEL,		//取消
		};


		int info_type;
		unsigned int param1;
	};

	class SCMarriageDuetTaskLogoutNotice								//!<2563 离线通知
	{
	public:
		SCMarriageDuetTaskLogoutNotice();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSG_MARRIAGE_HPP__

