#include "rolebattledatavermgr.h"
#include "common/tlvprotocol.h"
#include "servercommon/servercommon.h"

bool RoleBattleDataVerMgr::HandlerVerUnSerializer(short ver, RoleBattleData & data, TLVUnserializer &instream)
{
	if (ver < 0) return false;

	VerHandlerItem *list = GetVerHandlerItemList();
	if (NULL == list) return false;
		
	if (ver < ROLE_BATTLE_DATA_VER_MAX)
	{
		if (list[ver].ver != ver) 
		{
			return false;
		}
		else 
		{	
			if (NULL != list[ver].func)
			{
				bool ret = (data.*list[ver].func)(instream);
				if (ret) this->Update_TO_Max_Version(ver, data);
				return ret;
			}
		}
	}
	else
	{
		return (data.*list[ROLE_BATTLE_DATA_VER_MAX - 1].func)(instream);
	}

	return false;
}

void RoleBattleDataVerMgr::Update_TO_Max_Version(short ver_cur, RoleBattleData & data)
{
	if (ver_cur < 0 || ver_cur >= ROLE_BATTLE_DATA_CURRENT_VERSION) return;

	int idx = 0;

	VerFuncVerChangeItem *list = GetVerFuncChangeInitList();

	if (NULL != list)
	{
		while (list[idx].ver_cur != ver_cur) idx++;
		
		for (; NULL != list[idx].func; idx++)
		{
			(this->*list[idx].func)(data);
		}
	}
}
