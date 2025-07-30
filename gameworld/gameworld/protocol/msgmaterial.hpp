#ifndef __MSG_MATERIAL_HPP__
#define __MSG_MATERIAL_HPP__

#pragma pack(push, 4) 

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/rolematerialdef.hpp"

namespace Protocol
{
	class CSMaterialReq					//!< 2910 玉虚境操作请求
	{
	public:
		CSMaterialReq();
		MessageHeader header;
		
		enum MATERIAL_OP_TYPE
		{
			MATERIAL_OP_TYPE_CHALLENGE = 0,				//挑战			param1:seq(配置副本seq)
			MATERIAL_OP_TYPE_FAST_PASS = 1,				//扫荡			param1:seq(配置副本seq)
			MATERIAL_OP_TYPE_FETCH_BOX = 2,				//领取宝箱		param1:seq(配置seq)
			MATERIAL_OP_TYPE_BUY_TIMES = 3,				//购买关卡次数	param1:seq(配置副本seq)

			MATERIAL_OP_TYPE_MAX
		};

		short op_type;
		short param1;
	};
	
	enum MATERIAL_OBJ_TYPE
	{
		MATERIAL_OBJ_TYPE_BOSS = 0,			
		MATERIAL_OBJ_TYPE_NPC = 1,		

		MATERIAL_OBJ_TYPE_MAX
	};

	struct SpecialLogicMaterialObjInfo
	{
		SpecialLogicMaterialObjInfo() : npc_id(0), is_need_talk(0), obj_type(0), obj_id(0), pos_x(0), pos_y(0)
		{}

		int npc_id;
		char is_need_talk;		//0:不需要对话  1：需要对话
		char obj_type;
		ObjID obj_id;
		short pos_x;
		short pos_y;
	};

	class SCMaterialSceneInfo				//!< 2911 玉虚境场景信息
	{
	public:
		SCMaterialSceneInfo();
		MessageHeader header;

		static const int MAX_SCENE_OBJ_NUM = 4;

		char chapter_id;		// 章节ID
		char level_seq;			// 关卡seq
		char kill_boss_flag;	// 0:未击杀  1：已击杀
		char count;
		SpecialLogicMaterialObjInfo obj_list[MAX_SCENE_OBJ_NUM];
	};

	class SCMaterialAllInfo : public IMessageSerializer						//!< 2912 玉虚境角色所有信息
	{
	public:
		SCMaterialAllInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(level_count, buffer, buffer_size, out_length);
			for (int i = 0; i < level_count && i < ARRAY_LENGTH(level_data_list); ++i)
			{
				MaterialLevelNode & data_node = level_data_list[i];

				result = result && WriteChar(data_node.chapter_id, buffer, buffer_size, out_length);
				result = result && WriteChar(data_node.level_seq, buffer, buffer_size, out_length);
				result = result && WriteUChar(data_node.condition_flag, buffer, buffer_size, out_length);
				result = result && WriteUChar(data_node.achievenment_condition_flag, buffer, buffer_size, out_length);
				result = result && WriteChar(data_node.day_pass_times, buffer, buffer_size, out_length);
				result = result && WriteChar(data_node.buy_times, buffer, buffer_size, out_length);
			}

			result = result && WriteShort(chapter_count, buffer, buffer_size, out_length);
			for (int i = 0; i < chapter_count && i < ARRAY_LENGTH(intergral_list); ++i)
			{
				MaterialChapterNode & data_node = intergral_list[i];

				result = result && WriteShort(data_node.chapter_id, buffer, buffer_size, out_length);
				result = result && WriteShort(data_node.integral_num, buffer, buffer_size, out_length);
			}

			return result;
		}

		short level_count;
		short chapter_count;
		MaterialLevelNode level_data_list[MAX_MATERIAL_LEVEL_NUM];		//关卡数据   MAX_MATERIAL_LEVEL_NUM = 400
		MaterialChapterNode intergral_list[MAX_MATERIAL_CHAPTER_NUM];	//章节总积分 MAX_MATERIAL_CHAPTER_NUM - 20
	};

	class SCMaterialSingleInfo				//!< 2913 玉虚境单个关卡信息
	{
	public:
		SCMaterialSingleInfo();
		MessageHeader header;

		short reserve_sh;
		short integral_count;						//当前章节总积分
		MaterialLevelNode data;
	};

	class SCMaterialOtherInfo				//!< 2914 玉虚境其它信息下发
	{
	public:
		SCMaterialOtherInfo();
		MessageHeader header;

		char max_chapter_id;							//玩家当前到达的章节
		char max_level_seq;								//玩家当前到达对应章节的关卡	
		short day_pass_time;							//今天总通关次数
		BitMap<MAX_MATERIAL_BOX_FLAG_NUM> box_flag;		//宝箱领取标识
	};


}

#pragma pack(pop)

#endif