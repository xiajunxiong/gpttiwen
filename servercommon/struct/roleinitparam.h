#ifndef CHARAINITPARAM_H
#define CHARAINITPARAM_H

#include <string>

#include "common/tlvprotocol.h"

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

// ����ṹ���Բ��������string���ֶ��� ��Ϊ����ṹ��memset��
// ����ṹ��������Ҫ������ֶζ�Ҫ��ʽ�ر��浽Role��RoleAttrDetail�����ֶ�Ҫ�ǵü����ݱ�
// �����ֶ�������г�ʼֵ����Ҫ��roleinitattr��RMIRole��CreateRole�м��߼�
class RoleInitParam 
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int				role_id;								// ID
	GameName		role_name;								// ����
	
	char			authority_type;							// Ȩ�� 
	char			vip_level;								// VIP�ȼ�
	char			reserve_ch;								// ������
	char			color;									// ��ɫ

	short			avatar_type;							// ��������
	short			headshot_id;							// ͷ��ID
	int				top_level;								// �۷�ȼ�
	int				profession;								// ְҵ
	unsigned int    next_top_level_refresh_timestamp;		// �۷�ȼ�����ʱ���

	short			reserve_sh;
	short			role_fly_flag;							// ��ɫ�����׶�

	int				level;									// �ȼ�
	int				scene_id;								// ����ID
	int				scene_x;								// X
	int				scene_y;								// Y
	int				last_scene_id;							// �ϴ���ͨ����ID
	int				last_scene_x;							// X
	int				last_scene_y;							// Y

	int				online_time;							// ����ʱ��
	int				today_online_time;						// ��������ʱ��
	int				lastday_online_time;					// ��������ʱ��
	long long		create_time;							// ����ʱ��
	
	Int64			coin_bind;								// �󶨽��
	Int64			immortal_coin;							// �ɱ�
	Int64			gold;									// Ԫ������ʯ��

	Int64           history_recharge_gold;					// ��ʷ��ֵ��

	long long		forbid_talk_time;						// ����ʱ��
	int				forbid_talk_type;							// ����ģʽ \see FORBID_TYPE_MAX

	unsigned int	last_online_dayid;						// �ϴθ��µ�dayid
	unsigned int    last_reset_module_timestamp;			// �ϴ�����ģ���ʱ���

	long long		exp;									// ����
	long long		max_exp;								// �����

	int				capability;								// ս����
	int				max_capability;							// ��ʷ���ս����

	long long 		avatar_timestamp;						// ͷ��ʱ����� ʵ�����ڿͻ��˴���Զ����ϴ���ͷ�����

	int				plat_spid;								// ƽ̨����id

	int				weapon_id;								// ����ID

	int				account_user_num;						// ���˺��µڼ�����ɫ

	int				mini_game_id;							// С��ϷID
private:

	
};

#endif

