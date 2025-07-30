#ifndef __RAND_ACTIVITY_SEVEN_COLOR_PICTURE_CONFIG_HPP__
#define __RAND_ACTIVITY_SEVEN_COLOR_PICTURE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasevencolorpictureparam.hpp"
#include "item/itembase.h"
#include "servercommon/maildef.hpp"

struct RASevenColorPictureOtherCfg
{
	RASevenColorPictureOtherCfg() :section_start(0), section_end(0), end_day_time(0), coin_num(0), gold_num(0), is_complement(0)
	{}

	int section_start;
	int section_end;

	ItemConfigData seven_color_collection_reward;	//�߲�ͼ�����뽱��
	int end_day_time;		//�󽱿ɲ�������
	int coin_num;			//ѡ��һ�ּ���
	int gold_num;			
	int is_complement;
};


struct RASevenColorPicturePigmentCfg
{
	RASevenColorPicturePigmentCfg() :seq(0), quality(0), is_special(0)
	{}

	int seq;
	int quality;				//��������Ʒ��
	int is_special;				//������������
};

struct RASevenColorPictureAwardCfg
{
	RASevenColorPictureAwardCfg() :seq(0), grade_start(0), grade_end(0), seq1(0), summation_score(0), hearsay(0), reward_num(0)
	{}

	int seq;				//����
	int grade_start;		//��ʼ�ȼ�
	int grade_end;			//�����ȼ�  (�ȼ�����)
	int seq1;				//�ȼ������
	int summation_score;	//���� ���������ڵȼ����ȡ����
	int hearsay;			//�Ƿ���  1:���� 0:������

	int reward_num;
	ItemConfigData reward[MAX_ATTACHMENT_ITEM_NUM];
};

struct RASevenColorPictureAwardLevelCfg
{
	RASevenColorPictureAwardLevelCfg() :count(0)
	{}

	int count;
	RASevenColorPictureAwardCfg node_list[SEVEN_COLOR_PICTURE_AWARD__LEVEL_MAX_NUM];
};

struct RASevenColorPictureAwardSectionCfg
{
	RASevenColorPictureAwardSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{}

	int section_start;
	int section_end;

	std::vector<RASevenColorPictureAwardCfg> m_seven_color_picture_award_cfg;						//�߲�ͼ������
	std::map<int, RASevenColorPictureAwardLevelCfg> m_seven_color_picture_award_level_cfg;			//�߲�ͼ���ȼ���(�����ȡ)��1000 * Grade_start + Grade_endΪkey
};

class RandActivityManager;

class RandActivitySevenColorPictureConfig : public RandActivityConfig
{
public:
	RandActivitySevenColorPictureConfig();
	virtual ~RandActivitySevenColorPictureConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASevenColorPictureOtherCfg & GetOtherCfg(RandActivityManager * ramgr)const;

	const RASevenColorPicturePigmentCfg* GetSevenColorPicturePigmentCfg(int seq) const;
	const RASevenColorPicturePigmentCfg * GetSevenColorPicturePigmentCfg() const { return m_seven_color_picture_pigment_cfg; }
	const RASevenColorPictureAwardCfg* GetSevenColorPictureAwardCfg(int level, int score, RandActivityManager * ramgr) const;

private:
	int InitSevenColorPictureOtherCfg(TiXmlElement *RootElement);
	int InitSevenColorPicturePigmentCfg(TiXmlElement *RootElement);
	int InitSevenColorPictureAwardCfg(TiXmlElement *RootElement);

	std::vector<RASevenColorPictureOtherCfg> m_other_cfg;

	RASevenColorPicturePigmentCfg m_seven_color_picture_pigment_cfg[SEVEN_COLOR_PIGMENT_TYPE_MAX];	// �߲�ͼ�� - ����
	std::vector<RASevenColorPictureAwardSectionCfg> m_seven_color_section_cfg;
};

#endif	//__RAND_ACTIVITY_SEVEN_COLOR_PICTURE_CONFIG_HPP__