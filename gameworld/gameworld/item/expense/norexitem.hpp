#ifndef __NOREX_ITEM_H__
#define __NOREX_ITEM_H__

#include "item/itembase.h"
static const int GIFT_ITEM_MAX_NUM = 100;		//�����������Ʒ���������

class NorexItem : public ItemBase
{
public:
	enum NOREX_ITEM_TYPE
	{
		I_NOREX_ITEM_INVALID = 0,

		I_NOREX_ITEM_JINGYAN_DAN,						// ���鵤
		I_NOREX_ITEM_GIFT,								// ���(���������Χ������)
		I_NOREX_ITEM_IMMORTAL_COIN_CARD,				// ħʯ
		I_NOREX_ITEM_PET_EGGS,							// ���ﵰ
		I_NOREX_ITEM_PET_SKILL,							// ���＼����
		I_NOREX_ITEM_PARTNER_LICENCE,					// ���ƾ֤
		I_NOREX_ITEM_WORKSHOP_RECIPE,					// �����䷽
		I_NOREX_ITEM_SURFACE,							// �û�
		I_NOREX_ITEM_HP_RECOVER,						// ��Ѫҩ	param1 ��ս��״̬��Ѫ��   param2 ս���лظ��̶�ֵ  param3 ս���о����쳣Ч���ٷֱ�
		I_NOREX_ITEM_BIND_COIN_CARD,					// ͭ�ҿ�
		I_NOREX_ITEM_TREASURE_MAP,						// û�о�ֵ�Ĳر�ͼ,ʹ�ú����ĸõ��߲���������о�ֵ�Ĳر�ͼ
		I_NOREX_ITEM_RECOVER_MP,						// ����ҩ	param1 ��ս��״̬������ param2 ս��״̬�»�����
		I_NOREX_ITEM_GATHER_POWER,						// �ɼ������ҩ	param1 �ӻ�����ֵ
		I_NOREX_ITEM_VILLAGE_PRESTIGE,					// ���Ӵ�ׯ����	param1 ��ׯid param2 ���ӵ�ֵ
		I_NOREX_ITEM_ADVTASK_TRIGGER,					// �������񴥷����� - ������ʵ�⣬���������� R-����-�������� �µġ��������񼤻ʽ����ҳ��.
		I_NOREX_ITEM_TITLE_CATD,						// �ƺſ�
		I_NOREX_ITEM_REALIVE,							// ����ҩ   param1 Ŀ����ҵ�����������ްٷֱ�   param2 ս���л�Ѫϵ���ٷֱ�(ʹ�õ���Ŀ��ظ�ֵ*P2)
		I_NOREX_ITEM_OPTIONAL_ITEM,						// ��ѡ���(��)
		I_NOREX_ITEM_FABAO,								// ����������� param1�������index
		I_NOREX_ITEM_INST_KILL_MONSTER,					// ������   param1 ����ID   param2 �ɹ��ʰٷֱ�[0, 100]
		I_NOREX_ITEM_FLOWERS,							// �߲�ͼ��(�ʻ�)
		I_NOREX_ITEM_GOLD_CHONGZHI_CARD,				// Ԫ���� - �ȼ��ڳ�ֵ
		I_NOREX_ITEM_EXP,								// ������ param1�ȼ�����	param2 ���Ӿ���
		I_NOREX_LUCKYLOTTERY_TICKET,					// ���˳齱ȯ  -- ��Ӧ����ת�̻
		I_NOREX_ITEM_FACESROE_RES,						// ��ֵ��Դ
		I_NOREX_ITEM_PARTNER_QUALITYUP,					// �����Ʒ
		I_NOREX_ITEM_MEDAL_SP,							// ����ʯ (����ѫ����Ч����Ʒ)
		I_NOREX_ITEM_PET_SKIN,							// ����Ƥ��  p1 ����ĳ���Ƥ��id
		I_NOREX_ITEM_PARTNER_SKIN,						// ���Ƥ��  p1 ���id	p2�����Ƥ��id
		I_NOREX_ITEM_EXP_PERSENT,						// �ٷֱȾ��鵤  p1 �ٷֱ�
		I_NOREX_ITEM_AUTO_QIYUN,						// �Զ����˿�		p1  ����ֵ
		I_NOREX_ITEM_QIYUN,								// ���˿�		p1  ����ֵ
		I_NOREX_ITEM_JIFEN,								// ����ֿ�   p1	����ֵ
		I_NOREX_ITEM_INTIMACY,							// ����ܿ�   p1	����ֵ
		I_NOREX_ITEM_RECOVERY_POWER,					// �ؾ�����ҩ��
		I_NOREX_ITEM_EQUIP,								// δ����װ��	p1 װ���ȼ�	p2����id
		I_NOREX_ITEM_BUY_GIFT_ITEM,						// ֱ��������
		I_NOREX_ITEM_DREAM_NOTES_CHALLENGE,				// ��Ԩ����  param1:���Ӵ���
		I_NOREX_ITEM_MONEY_AUTO_USE,					// ���ҵ����Զ�ʹ�� p1:money_type p2:money_num
		I_NOREX_ITEM_CRESTAL_XIYOU_TIMES,				// ˮ������ϡ��ģʽ��������
		I_NOREX_ITEM_CONVERT_ITEM,						// ת������ p1:item_id  p2:item_num
		I_NOREX_ITEM_PET_GOD_PRINT,						// ������ӡ
		I_NOREX_ITEM_DUEL_REDUCE,						// ����ɱ¾ֵ
		I_NOREX_ITEM_FORTUNE_MAZE,						// �����Թ�����
		I_NOREX_ITEM_ROLE_ATTR_PELEET,					// ��ɫ���Ե����
		I_NOREX_ITEM_NEW_LIFE_SKILL_PROPS,				// �������(NEW_LIFE_SKILL_PROPS_TYPE) p1:seq
		I_NOREX_ITEM_CHANGE_CARD,						// ���� p1:seq
		I_NOREX_ITEM_GUILD_BUILD_MATERIAL,				// ���彨��, p1:�����ʽ�
		I_NOREX_ITEM_SILVER_COIN_CARD,					// ����(����)��  p_1:��������
		I_NOREX_ITEM_MONEY_BOX,							// ���Һ��  p_1:�������
		I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD,			// ��ʱ�޵ĳƺſ� p1:��ֵID(�ͻ�����) p2:�ƺ�ID(�������)
		I_NOREX_ITEM_DAILY_CUMULATIVE_RECHARGE,			// ÿ���۳俨
		I_NOREX_ITEM_JIN_YI_TIAN_XIANG_ADD_VALUE,		// ����������ӳ齱����
		I_NOREX_ITEM_HP_PERCENT_RECOVER,				// �ٷֱȻ�Ѫҩ param1 ��ս��״̬��Ѫ�ٷֱ� param2 ս���л�Ѫ�ٷֱ�  (���Ѫ��ȡĿ��)
		I_NOREX_ITEM_MP_PERCENT_RECOVER,				// �ٷֱȻ���ҩ param1 ��ս��״̬�����ٷֱ� param2 ս��״̬�»����ٷֱ� (�������ȡĿ��)
		I_NOREX_ITEM_SMART_MOUNTS_SKILL,				// ���＼���� P1:����ID P2:���ܿ�λ P3:���ܵȼ�
		I_NOREX_ITEM_HUANSHOU_EGG,						// ָ�����޵� P1:huanshou_id

		I_NOREX_ITEM_MAX,
	};

	NorexItem();
	~NorexItem();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	int GetParam1() const { return m_param1; }
	int GetParam2() const { return m_param2; }
	int GetParam3() const { return m_param3; }
	short GetUseType() const { return m_use_type; }

private:
	short m_use_type;
	int m_param1;
	int m_param2;
	int m_param3;
};

#endif //__NOREX_ITEM_H__
