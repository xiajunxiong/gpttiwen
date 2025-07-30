#ifndef __MSG_DRAW_HPP__
#define __MSG_DRAW_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/shanhaibossdef.hpp"
#include "servercommon/miyidef.hpp"
#include "servercommon/wildbossdef.hpp"
#include "servercommon/rolecrystalfbdef.h"

#pragma pack(push,4)

namespace Protocol
{
	enum DrawReqType
	{
		DRAW_REQ_TYPE_ROLE_DRAW = 0,	//领取请求(手动翻牌) p_1:奖励组id   p_2:选中的位置[0,DRAW_GROUP_MAX_ITEM_NUM]
	};
	class CSDrawReq			//!< 3560 翻牌请求
	{
	public:
		CSDrawReq();
		MessageHeader header;

		int type;				//操作类型
		int param_1;
		int param_2;
	};

	
	class SCRoleDrawInfo	//!< 3561 角色翻牌选择信息
	{
	public:
		SCRoleDrawInfo();
		MessageHeader header;

		int m_draw_group_id;	//奖励组id

		int uid;				//翻牌者uid
		int draw_index;			//翻牌者选中的位置[0,DRAW_GROUP_MAX_ITEM_NUM]
	};
	
	struct DrawPoker	
	{
		DrawPoker()
		{
			memset(data, 0, sizeof(data));
		}	
		
		char data[MAX_ITEM_LENGTH_NUM];
	};
	class SCDrawPokerInfo : public IMessageSerializer2		//!< 3565	奖励翻牌信息
	{
	public:
		SCDrawPokerInfo();
		virtual ~SCDrawPokerInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool result = true;

			if (NULL == outstream)
			{
				return false;
			}
			
			TLVSerializer2 & out = *outstream;
			result = result && header.Serialize(outstream);
			result = result && out.Push(draw_group_id);
			result = result && out.Push(timeout);
			result = result && out.Push(draw_group_type);
			for (int i = 0; i < SIDE_MAX_ROLE_NUM; i++)
			{
				UNSTD_STATIC_CHECK(DRAW_GROUP_TYPE_MAX == 9);
				switch (draw_group_type)
				{
					case DRAW_GROUP_TYPE_MI_YI:
					{
						MiYiPoker * mi_yi_poker = (MiYiPoker *)draw_poker[i].data;
						result = result && mi_yi_poker->Serialize(out);
						UNSTD_STATIC_CHECK(sizeof(MiYiPoker) <= MAX_ITEM_LENGTH_NUM);
					}
					break;
					case DRAW_GROUP_TYPE_FIELD_BOSS:
					{
						WildbossPoker * boss_poker = (WildbossPoker *)draw_poker[i].data;
						result = result && boss_poker->Serialize(out);
						UNSTD_STATIC_CHECK(sizeof(WildbossPoker) <= MAX_ITEM_LENGTH_NUM);
					}
					break;
					case DRAW_GROUP_TYPE_SHAN_HAI:
					case DRAW_GROUP_TYPE_CRYSTAL:
					case DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS:
					case DRAW_GROUP_TYPE_PET_GOD_FB:
					case DRAW_GROUP_TYPE_PET_GOD_FB_AUTO:
					case DRAW_GROUP_TYPE_HUNDRED_GHOST:
					{
						DrawCrystalPoker * draw_crystal_poker = (DrawCrystalPoker *)draw_poker[i].data;
						result =  result && draw_crystal_poker->Serialize(out);
						UNSTD_STATIC_CHECK(sizeof(DrawCrystalPoker) <= MAX_ITEM_LENGTH_NUM);
					}
					break;
					default:
					result = false;
					break;
				}
			}
			return result;
		}

		int draw_group_id;
		unsigned int timeout;
		int draw_group_type;
		DrawPoker draw_poker[SIDE_MAX_ROLE_NUM];
	};

}

#pragma pack(pop)

#endif // !__MSG_DRAW_HPP__