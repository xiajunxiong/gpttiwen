#ifndef __SPIRIT_PACT_ITEM_HPP__
#define __SPIRIT_PACT_ITEM_HPP__
//���--Ԫ����Լ

#include "item/itembase.h"
//Ҫ�󲻷�����Ʒ����  �����ֶ����ֿⱣ��
//��Ը��������������  ����Ҫͨ����Ʒidȡ
//Ҫ��ֿⴢ��������  ���ϵͳ��������ӳ�� save_index <-> item_id

//I_TYPE_PARTNER_PACT
//ITEM_SUB_TYPE_SPIRIT_PACT
//�ֿⱣ��	SERIALIZE_OTHER_PARAM(partner_pact_data, partner_pact_param);

class SpiritPactItem : public ItemBase
{
public:
	SpiritPactItem();
	virtual ~SpiritPactItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	//��Ӧ���id
	int GetPartnerID()const { return m_partner_id; };
	//�Ǽ�
	int GetStar()const { return m_star; };
	//��������(�޷����� ��0)
	ItemID GetUpStarCostID()const { return m_up_star_cost_id; };
	int GetUpStarCostNum()const { return m_up_star_cost_num; };

	//���ǲ���
	int GetUpStarOutID()const { return m_up_star_out_id; };

	//��������ǧ�ֱ�
	int GetAttrUpPerMil()const { return m_attr_up_per_mil; };

private:
	int m_partner_id;	//��Ӧ���id
	int m_star;		//�Ǽ�

	//��������(�޷����� ��0)
	ItemID m_up_star_cost_id;
	int m_up_star_cost_num;

	//���ǲ���
	ItemID m_up_star_out_id;

	//��������ǧ�ֱ�
	int m_attr_up_per_mil;
};

#endif 