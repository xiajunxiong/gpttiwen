#include <sstream>
#include <string>
#include <iostream>
#include <string.h>
#include "global/randactivity/randactivitymanager.hpp"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "protocol/msgactivity.hpp"
#include "protocol/msgother.h"
#include "global/mail/mailmanager.hpp"
#include "other/dayreward/dayreward.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "other/buffmanager/buffmanager.hpp"
#include "other/buffmanager/buffbase.hpp"
#include "obj/character/role.h"
#include "globalconfig/globalconfig.h"
#include "battle/battle_manager_local.hpp"
#include "equipment/jewelry.h"
#include "equipment/equipment.h"
#include "task/taskmanager.h"
#include "task/taskrecorder.h"
#include "item/money.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "protocol/msgbosschapter.h"
#include "other/boss_chapter/boss_chapter.hpp"
#include "other/gm/gm.hpp"
#include "other/schooltask/schooltask.h"
#include "other/surface/surface.hpp"
#include "other/title/title.hpp"
#include "other/pet/pet.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/prestige/prestige.hpp"
#include "other/partner/partner.hpp"
#include "other/train/train.hpp"
#include "other/title/title.hpp"
#include "other/wst_question/wst_question.hpp"
#include "other/skill/roleskill.hpp"
#include "other/event/eventhandler.hpp"
#include "other/workshop/workshop.hpp"
#include "other/baby/baby.hpp"
#include "other/currencyconfig.hpp"
#include "other/systemnotice/systemnotice.hpp"
#include "other/escort/escort.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "config/logicconfigmanager.hpp"
#include "internalcomm.h"
#include "scene/scene.h"
#include "world.h"
#include "config/reloadconfigmanager.hpp"
#include "scene/fbmanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/team/team.hpp"
#include "servercommon/errornum.h"
#include "protocol/msgtrademarket.h"
#include "global/trademarket/trademarketrouter.hpp"
#include "scene/scene.h"
#include "scene/map.h"
#include "protocol/msgfabao.hpp"
#include "other/fabao/fabao.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "other/mapunlock/mapunlock.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "npc/npcmanager.h"
#include "timer/forcekickuserdelay.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/usercache/usercache.hpp"
#include "global/trademarket/trademarketmanager.hpp"
#include "scene/speciallogicimpl/speciallogicbraveground.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/userprotocal/msgcrosscommon.h"
#include "global/team/teammanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/team/teammatch.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/queryhandler/queryhandler.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "other/researchtask/researchtask.h"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "other/salary/salary.hpp"
#include "global/platformbattle/platformbattle.h"
#include "other/medal/medal.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/taskchain/taskchain.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/schooltask/schooltaskconfig.hpp"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "chat/chatmanager.h"
#include "other/roleconvertshop/roleconvertshop.hpp"
#include "global/trademarket/usertradeinfo.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/bountytask/bountytask.hpp"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/activesystem/activesystem.hpp"
#include "global/randactivity/randactivity.hpp"
#include "global/randactivity/randactivityimpl/randactivityexpelmonster.hpp"
#include "global/team/teamconfig.hpp"
#include "other/robot/robotconfig.hpp"
#include "other/facescore/facescore.hpp"
#include "obj/character/robot.h"
#include "global/randactivity/randactivityimpl/randactivityjoyseeksfavors.hpp"
#include "other/partner/partnerconfig.hpp"
#include "scene/speciallogicimpl/speciallogicfantasyfb.hpp"
#include "scene/speciallogic.hpp"
#include "battle/battle_video_manager.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "protocol/msghuanjiezhanchang.h"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "other/treasure_map/treasuremap.h"
#include "servercommon/protobuf/role_battle_data.pb.h"
#include "servercommon/performancecatch.hpp"
#include "scene/speciallogicimpl/speciallogicmiyi.hpp"
#include "servercommon/protobufmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityanimalrace.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/auto_catch_pet/auto_catch_pet.hpp"
#include "other/formation/formation.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityluckyturntable.hpp"
#include "other/roleactivity/impl/roleactivitytruntable.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"
#include "scene/speciallogicimpl/speciallogiccommon.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "global/randactivity/randactivityimpl/randactivitytianjianghaoli.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "gameworld/skill/skillpool.hpp"
#include "other/schoolexercise/schoolexercise.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/msgfuncguide.h"
#include "scene/speciallogicimpl/mazeracermanager.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/impl/roleactivitylaodonghaoli.hpp"
#include "other/roleactivity/impl/roleactivitygoodgiftturn.hpp"
#include "other/roleactivity/impl/roleactivityzhuanshulibao.hpp"
#include "global/randactivity/randactivityimpl/randactivitylaodonghaoli.hpp"
#include "protocol/msgpetmonopoly.h"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "protocol/msgcloudarena.h"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "servercommon/robotdef.hpp"
#include "ai/robotai/robotaimission.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/xunbao/xunbaoconfig.hpp"
#include "protocol/msgformation.h"
#include "other/formation/pethelperformation.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/hongbao/hongbaomanager.hpp"
#include "protocol/msghongbao.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "global/givegift/givegift.h"
#include "other/rolegivegift/rolegivegift.hpp"
#include "global/randactivity/randactivityimpl/randactivityqionglouyuyan.hpp"
#include "global/tiandidaohen/tiandidaohen.h"
#include "other/gm/gmtester/activityhuanjiezhanchangtester.hpp"
#include "other/roleactivity/impl/roleactivitybackreward.hpp"
#include "other/duel/duel.hpp"
#include "other/pet/superskillbookconvert.hpp"
#include "other/tianming/tianming.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "other/capability/capability.hpp"
#include "global/colosseum/gladiator.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "other/dianhuashi/dianhuashiconfig.hpp"
#include "servercommon/redis/redismanager.hpp"

#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "scene/speciallogicimpl/speciallogicweddingfb.hpp"
#include "qingyuan/weddingmanger.hpp"
#include "global/worldarena/worldarena.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "scene/speciallogicimpl/speciallogictuzikuaipao.hpp"
#include "asynctaskimpl/asyncredistest.hpp"
#include "servercommon/asynctask/asynctaskmanager.hpp"
#include "servercommon/asynctask/asynctaskfactorymanager.hpp"

#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "other/minigame/minigame.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "global/randactivity/randactivityimpl/randactivityservercompetition.hpp"
#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"

#include "global/bigcross/utachampionrecordmanager.hpp"
#include "global/cross/crossmanager.h"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "global/bigcross/utaobshadow.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitysaltysweetbattle.hpp"
#include "config/otherconfig/elementelfconfig.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/starchart/starchart.h"
#include "other/funcguide/funcguide.hpp"
#include "other/huanshou/rolehuanshou.hpp"

TesterActivityHuanJieZhanChang* g_HuanJieZhanChangTester = NULL;

Gm & Gm::Instance()
{
	static Gm gm;
	return gm;
}

std::vector<std::string> Gm::SplitString(const std::string& src_str, const std::string& seperator)
{
	std::vector<std::string> ret;
	std::string::size_type  pos1 = 0;
	std::string::size_type  pos2 = src_str.find(seperator);
	while (std::string::npos != pos2)
	{
		ret.push_back(src_str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + seperator.size();
		pos2 = src_str.find(seperator, pos1);
	}
	if (pos1 != src_str.length())
	{
		ret.push_back(src_str.substr(pos1));
	}

	return ret;
}

bool Gm::OnCommand(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *gmc)
{
#ifndef _DEBUG

	if (!role->IsGM() && !role->IsFromDev())
	{
		// dev平台开放gm（不需要是gm)
		return false;
	}

#endif 



	gmc->type[sizeof(gmc->type) - 1] = 0; gmc->command[sizeof(gmc->command) - 1] = 0;

	gamelog::g_log_gm.printf(LL_INFO, "user[%d %s] execute GM command [%s : %s]", UidToInt(role->GetUserId()), role->GetName(), gmc->type, gmc->command);

	CommandFuncItem *command_fun_list = this->GetCommandFuncItem();

	if (NULL != command_fun_list)
	{
		int i = 0;
		bool has_found_command = false;
		while (NULL != command_fun_list[i].pfnCommandHandler)
		{
			if (strlen(gmc->type) == strlen(command_fun_list[i].type) && !strncmp(gmc->type, command_fun_list[i].type, strlen(gmc->type)))
			{
				has_found_command = true;
				bool ret = false;
				char *result[128] = {};
				int arg_num = this->ParseCommandParam(gmc->command, result, " ");

				if (arg_num == command_fun_list[i].param_count)
				{
					if (NULL != command_fun_list[i].pfnCommandHandler)
					{
						ret = (this->*command_fun_list[i].pfnCommandHandler)(netid, scene, role, obj_id, gmc);
					}
				}

				// 错误信息统一在这里发送 具体的命令仅仅处理成功消息 
				if (!ret)
				{
					Protocol::SCGMCommand scgm;
					STRNCPY(scgm.type, gmc->type, sizeof(scgm.type));
					STRNCPY(scgm.result, command_fun_list[i].help, sizeof(scgm.result));
					EngineAdapter::Instance().NetSend(netid, (const char *)&scgm, sizeof(Protocol::SCGMCommand));
				}

				break;
			}

			++i;
		}

		if (!has_found_command)
		{
			//把命令发到bigcross
			this->TransferCommandToBigCross(role, gmc);
	}
	}

	return false;
}

Gm::Gm()
{

}

Gm::~Gm()
{

}

void Gm::Update(unsigned long interval, time_t now_second)
{
	if (NULL != g_HuanJieZhanChangTester)
	{
		g_HuanJieZhanChangTester->Update(interval, now_second);
	}
}

bool Gm::GmTuZiKuaiPao(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (!result[0] || !result[1])
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int op_type = atoi(result[0]);
	int p1 = atoi(result[1]);


	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_TU_ZI_KUAI_PAO)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	SpecialLogicTuZiKuaiPao * spc_qing_yuan_sheng_hui = (SpecialLogicTuZiKuaiPao *)special_lobgic;
	spc_qing_yuan_sheng_hui->GmOp(op_type, p1, 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQingYuanShengHui(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (!result[0] || !result[1])
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int op_type = atoi(result[0]);
	int p1 = atoi(result[1]);


	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_QING_YUAN_SHENG_HUI)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	SpecialLogicQingYuanShengHui * spc_qing_yuan_sheng_hui = (SpecialLogicQingYuanShengHui *)special_lobgic;
	spc_qing_yuan_sheng_hui->GmOp(op_type, p1, 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmLoadAllUserDataCalcSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	UserCacheManager::Instance().GmReadAllRoleData(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GuildHonorBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param1 = atoi(result[1]);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
		return MakeupResult(netid, command->type, false, "");
	}

	GuildHonorBattleManager::Instance().GmOperate(role, type, param1);



	return MakeupResult(netid, command->type, true, "");
}

bool Gm::ResetPos(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Posi target(0, 0);
	scene->GetTownPoint(&target);
	bool ret = role->ResetPos(target.x, target.y);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::ResetPos2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Posi target(atoi(result[0]), atoi(result[1]));

	bool ret = role->ResetPos(target.x, target.y);

	return MakeupResult(netid, command->type, ret, ""); return false;
}

bool Gm::GmBraveGround(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int add_level = atoi(result[1]);

	if (op_type == 0)
	{
		if (scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
		{
			SpecialLogicBraveGround * special_lobgic = (SpecialLogicBraveGround *)scene->GetSpecialLogic();
			special_lobgic->GmTransport(role, add_level);
		}
	}

	if (op_type == 1)
	{
		ActivityBraveGround * activity = (ActivityBraveGround *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND);
		if (activity != NULL)
		{
			activity->OnWeekChange();
			role->GetRoleModuleManager()->OnWeekChange();
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::ProtocolTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	char buff[1024] = { 0 };

	int protocol_num = atoi(result[0]);
	std::vector<std::string> protocol_param = SplitString(result[1], "|");

	union
	{
		unsigned char num_uc;
		unsigned short num_us;
		unsigned int num_ui;
		unsigned long long num_ull;
	} convert_num;

	char * offset = buff;
	*(int *)offset = protocol_num;
	offset += 4;


	for (int i = 0; i < (int)protocol_param.size(); ++i)
	{
		std::string & type_param = protocol_param[i];
		std::vector<std::string> type_param_item = SplitString(type_param, "+");
		if (type_param_item.size() >= 1)
		{
			convert_num.num_ull = 0;
			if (type_param_item.size() >= 2)
			{
				convert_num.num_ull = strtoull(type_param_item[1].c_str(), NULL, 0);
			}

			if (type_param_item[0] == "1" || type_param_item[0] == "c" || type_param_item[0] == "uc")
			{
				*(unsigned char *)offset = convert_num.num_uc;
				offset += sizeof(unsigned char);
			}
			if (type_param_item[0] == "2" || type_param_item[0] == "s" || type_param_item[0] == "us")
			{
				*(unsigned short *)offset = convert_num.num_us;
				offset += sizeof(unsigned short);
			}
			if (type_param_item[0] == "4" || type_param_item[0] == "i" || type_param_item[0] == "ui")
			{
				*(unsigned int *)offset = convert_num.num_ui;
				offset += sizeof(unsigned int);
			}
			if (type_param_item[0] == "8" || type_param_item[0] == "l" || type_param_item[0] == "ul")
			{
				*(unsigned long long *)offset = convert_num.num_ull;
				offset += sizeof(unsigned long long);
			}
		}
	}

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, buff, offset - buff);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::UseItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int column = atoi(result[0]);
	int index = atoi(result[1]);
	int num = atoi(result[2]);
	int p1 = atoi(result[3]);
	int p2 = atoi(result[4]);
	int ret = role->GetRoleModuleManager()->GetKnapsack()->Use(column, index, num, p1, p2, 0);
	return MakeupResult(netid, command->type, ret >= 0, "");
}

bool Gm::AddItem(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int item_id = atoi(result[0]);
	int item_num = atoi(result[1]);
	bool is_bind = (0 != atoi(result[2]));

	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	bool ret = false;
	if (NULL != itembase)
	{
		const short item_type = itembase->GetItemType();
		switch (item_type)
		{
		case I_TYPE_EQUIPMENT:
			{
				static NetValueItemParam tmp_item_param; tmp_item_param.Reset();

				F_STRNCPY(tmp_item_param.equip_param.creator_name, role->GetName(), sizeof(GameName));

				const Equipment* equip = (const Equipment*)itembase;
				equip->RandomEquipmentAttr(&tmp_item_param);
				equip->RandomSpecialEffect(&tmp_item_param);
				tmp_item_param.equip_param.is_treasure = (char)equip->CheckEquipmentIsTreasure(tmp_item_param.equip_param);

				static ItemDataWrapper idw; idw.Reset();
				tmp_item_param.GetData(idw.param_data);

				idw.item_id = item_id;
				idw.num = 1;
				idw.is_bind = is_bind ? 1 : 0;
				idw.has_param = 1;

				ret = role->GetRoleModuleManager()->GetKnapsack()->Put(idw, PUT_REASON_GM);
			}
			break;

		default:
			{
				ItemConfigData item_config_data(item_id, is_bind, item_num);
				ret = role->GetRoleModuleManager()->GetKnapsack()->Put(item_config_data, PUT_REASON_GM);
			}
			break;
		}
	}

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::AddTestEquipment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	bool ret = false;
	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::ClearBag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GetKnapsack()->ClearAll();
	role->GetRoleModuleManager()->GetZaoHuaEquipManager()->ClearAll();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::AddExp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long add_exp = atoll(result[0]);
	role->AddExp(add_exp, ADD_EXP_TYPE_GM, __FUNCTION__);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::AddMoney(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int add_money = atoi(result[0]);

	role->GetRoleModuleManager()->GetMoney()->Add(add_money, add_money, add_money, "Gm");
	for (int type = 0; type < MONEY_TYPE_MAX; ++type)
	{
		if (MONEY_TYPE_GOLD == type || MONEY_TYPE_COIN_BIND == type || MONEY_TYPE_IMMORTAL_COIN == type)
		{
			continue;
		}

		if (MONEY_TYPE_FACE_SCORE == type || MONEY_TYPE_RESERVE == type || MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS == type)
		{
			continue; // 这几个客户端要求屏蔽
		}

		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(add_money, type, "Gm");
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::ClearMoney(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int money_type = atoi(result[0]);
	role->GetRoleModuleManager()->GetMoney()->GmClearMoney(money_type, __FUNCTION__);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::AddPet(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int pet_id = atoi(result[0]);
	int str_level = atoi(result[1]);

	int ret = role->GetRoleModuleManager()->GetPet()->AcquirePet(pet_id, __FUNCTION__, str_level);
	char ret_str[128] = { '\0' };
	sprintf(ret_str, "%d", ret);
	return MakeupResult(netid, command->type, true, ret_str);
}

bool Gm::AddPetExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int index = atoi(result[0]);
	int add_exp = atoi(result[1]);

	int ret = role->GetRoleModuleManager()->GetPet()->AddExp(index, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
	char ret_str[128] = { '\0' };
	sprintf(ret_str, "%d", ret);
	return MakeupResult(netid, command->type, true, ret_str);
}

bool Gm::AddMaxPet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int pet_id = atoi(result[0]);
	int att_type = atoi(result[1]);
	int attr_point_val = atoi(result[2]);

	role->GetRoleModuleManager()->GetPet()->GmAddMaxAttrPet(pet_id, att_type, attr_point_val);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::TestFight(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	bool ret = BattleManagerLocal::GetInstance().StartDemoFightReq(role);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::TestFightMonster(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int monster_group_id = atoi(result[0]);

	bool ret = BattleManagerLocal::GetInstance().StartMonsterFightReq(role, monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::TestTeamFightMonster(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int monster_group_id = atoi(result[0]);

	bool ret = BattleManagerLocal::GetInstance().StartTeamMonsterFightReq(role, monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::TestPrestige(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int a = atoi(result[0]);
	int b = atoi(result[1]);
	int c = atoi(result[2]);

	if (c > 0)
	{
		role->GetRoleModuleManager()->GetPrestige()->OnShoping(a, b);
	}
	else
	{
		role->GetRoleModuleManager()->GetPrestige()->AddPrestige(a, b, __FUNCTION__);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::MakeupResult(const GSNetID &netid, const char *commandType, bool ret, const char *addition_str)
{
	Protocol::SCGMCommand scgm;

	STRNCPY(scgm.type, commandType, sizeof(scgm.type));

	if (!ret)
	{
		STRNCPY(scgm.result, "gm command FAIL!", sizeof(scgm.result));
	}
	else
	{
		STRNCPY(scgm.result, "gm command Succ", sizeof(scgm.result));
	}

	if ('\0' != addition_str[0])
	{
		strcat(scgm.result, " : ");
		strcat(scgm.result, addition_str);
	}

	scgm.result[sizeof(scgm.result) - 1] = 0;

	EngineAdapter::Instance().NetSend(netid, (const char *)&scgm, sizeof(Protocol::SCGMCommand));

	return ret;
}

int Gm::ParseCommandParam(char *src, char **result, const char *spilt)
{
	int arg_num = 0;

	static char temp[Protocol::MAX_GM_COMMAND_LEN] = { 0 };  // 必须是static  

	STRNCPY(temp, src, sizeof(temp));

	char *token = strtok(temp, spilt);

	while (NULL != token)
	{
		result[arg_num++] = token;
		token = strtok(NULL, spilt);
	}

	return arg_num;
}

void Gm::TransferCommandToBigCross(Role *role, Protocol::CSGMCommand *command)
{
	bigchgameprotocol::HGameBigCGmCommand msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	F_STRNCPY(msg.command, command->type, sizeof(msg.command));
	F_STRNCPY(msg.params, command->command, sizeof(msg.params));

	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

bool Gm::PartnerHandle(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int licence_or_task_id = atoi(result[0]);	// 伙伴凭证的ID

	if (!role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(licence_or_task_id, UNLOCK_TYPE_ITEM))
		role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(licence_or_task_id, UNLOCK_TYPE_TASK);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::Test(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	int p3 = atoi(result[3]);

	switch (type)
	{
	case 1:
		role->GetRoleModuleManager()->GetPrestige()->OnShoping(p1, p2);
		break;
	case 2:
		role->GetRoleModuleManager()->GetPrestige()->AddPrestige(p1, p2, __FUNCTION__);
		break;
	case 3:
		role->GetRoleModuleManager()->GetWstQuestion()->OnBuyExp(p1);
		break;
	case 4:
		role->GetRoleModuleManager()->GetWstQuestion()->OnQuestion(p1);
		break;
	case 5:
		role->GetRoleModuleManager()->OnTreviFountainBlessing();
		break;
	case 6:
		role->GetRoleModuleManager()->GetPrestige()->OnDonate(p1, p2, p3);
		break;
	case 7:
		role->GetRoleModuleManager()->GetSurface()->WearSurface(p1, p2);
		break;
	case 8:
		role->GetRoleModuleManager()->GetSurface()->ActivateSurface(p1);
		break;
	case 11:
		role->GetRoleModuleManager()->GetFaceScore()->RequestEquips(p1);
		break;
	case 12:
		role->GetRoleModuleManager()->GetFaceScore()->RequestRemove(p1);
		break;
	case 13:
		{
			RoleActivityLuckyTurnTable * role_activity = static_cast<RoleActivityLuckyTurnTable *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
			if (NULL != role_activity)
			{
				role_activity->OnUseLuckLotteryTicket(p1);
			}
		}
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}


bool Gm::OnDayChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->OnDayChange(1, 2);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::OnWeekChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->OnWeekChange();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::OnResetModuleData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->OnResetModuleData(0, 1);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::OnResetModuleDataWeek(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->OnResetModuleDataWeek();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::ForceFightFinish(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	BattleManagerLocal::GetInstance().ForceFinishFightReq(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::BattleRoleOperate(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int operate_type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	int p3 = atoi(result[3]);
	int p4 = atoi(result[4]);

	BattleManagerLocal::GetInstance().RoleOperateReq(role, operate_type, p1, p2, p3, p4);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::TaskManagerOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	role->GetRoleModuleManager()->GetTaskManager()->GmTaskManagerOrder(op_type, param1, param2);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::SkillOp(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int operate_type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	int p3 = atoi(result[3]);

	role->GetRoleModuleManager()->GetRoleSkill()->OnRoleOperateReq(operate_type, p1, p2, p3);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmOnDig(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	EventHandler::Instance().OnDig(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmOnLumber(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	EventHandler::Instance().OnLumbering(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmOnPickFlower(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	EventHandler::Instance().OnPickFlower(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmOnMakeEquipment(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int equip_id = atoi(result[0]);

	EventHandler::Instance().OnMakeEquipment(role, (ItemID)equip_id, 1);

	return MakeupResult(netid, command->type, true, "");
}


bool Gm::GmAddtitle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int id = atoi(result[0]);

	role->GetRoleModuleManager()->GetTitle()->AddTitle(id, "GM");

	return MakeupResult(netid, command->type, true, "");
}
bool Gm::GmOnWorkshopRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	int param3 = atoi(result[3]);
	int param4 = atoi(result[4]);

	switch (op_type)
	{
	case 0:
		role->GetRoleModuleManager()->GetWorkshop()->CompoundItem(param1, param2, param3);
		break;
	case 1:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemMounting(param1, param2, param3, param4);
		break;
	case 2:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemDismount(param1, param2, param3, param4);
		break;
	case 3:
		role->GetRoleModuleManager()->GetWorkshop()->CraftJewelry(param1);
		break;
	case 4:
		role->GetRoleModuleManager()->GetWorkshop()->ReforgeJewelry(param1, param2);
		break;
	case 5:
		role->GetRoleModuleManager()->GetWorkshop()->ChooseReforgeRetain(param1);
		break;
	case 6:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemUpgrades(param1, param2, param3, param4);
		break;
	case 7:
		role->GetRoleModuleManager()->GetWorkshop()->ConfirmGemUpgrade();
		break;
	case 8:
		role->GetRoleModuleManager()->GetWorkshop()->GmUnlockAllRecipe(0 != param1);
		break;
	case 9:
		role->GetRoleModuleManager()->GetWorkshop()->ExchangeTarget(param1, param2);
		break;
	case 10:
		role->GetRoleModuleManager()->GetWorkshop()->BatchCompoundItem(param1, param2);
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmOnSystemNoticeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	//int seq = atoi(result[0]);

	//role->GetRoleModuleManager()->GetSystemNotice()->GetAward(seq);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmBaby(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	int param3 = atoi(result[3]);

	role->GetRoleModuleManager()->GetBaby()->GmOper(op_type, param1, param2, param3);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAdvanceCareer(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int to_profession = atoi(result[0]);

	bool ret = role->GetRoleModuleManager()->GetAdvanceCareer()->RoleAdvanceCareer(to_profession, 0, true);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GMAddGuildExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exp = atoi(result[0]);

	Guild* guild = GuildManager::Instance().GetGuild(role->GetUserId());
	if (NULL == guild) return MakeupResult(netid, command->type, false, "");

	guild->AddExp(exp);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMAddGuildGongxian(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int value = atoi(result[0]);

	Guild* guild = GuildManager::Instance().GetGuild(role->GetGuildID());
	if (NULL != guild)
	{
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(value, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}

	return MakeupResult(netid, command->type, true, "");
}
bool Gm::GmSetRoleLevel(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int to_level = atoi(result[0]);
	int is_role_level = atoi(result[1]);

	if (0 != is_role_level)
	{
		role->GmSetRoleLevel(to_level);
	}
	else
	{
		role->GmSetRoleTopLevel(to_level);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::AddExpCrystal(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMailTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int param = atoi(result[0]);

	switch (param)
	{
	case 0:
		{
			MailContentParam contentparam; contentparam.Reset();

			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));

			int rand_count = RandomNum(MAX_ATTACHMENT_ITEM_NUM) + 1;
			for (int i = 0; i < rand_count; ++i)
			{
				contentparam.item_list[i].item_id = 26000;
				contentparam.item_list[i].num = 1;
				contentparam.item_list[i].is_bind = 0 == RandomNum(2) ? true : false;
			}

			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = RandomNum(1000);
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = RandomNum(1000);
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = RandomNum(1000);
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = RandomNum(1000);

			MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
		}
		break;
	case 1:
		{
			MailContentParam contentparam; contentparam.Reset();

			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));

			MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
		}
		break;
	case 2:
		{
			MailRoute::Instance().OneKeyFetchAttachment(role);
			MailRoute::Instance().CleanMail(role);
		}
		break;
	case 3:
		{
			MailContentParam contentparam; contentparam.Reset();

			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));

			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = RandomNum(1000);

			//MailRoute::Instance().MailToAllUser(MAIL_REASON_GM_TEST, contentparam);
		}
		break;
	case 4:
		{
			MailContentParam contentparam; contentparam.Reset();
			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));
			contentparam.item_list[0].item_id = 50120;
			contentparam.item_list[0].num = 1;
			contentparam.item_list[0].is_bind = 1;
			contentparam.item_list[0].has_param = 1;
			static NetValueItemParam param;
			param.Reset();
			if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(50120, 2, false, param))
			{
				assert(0);
				return MakeupResult(netid, command->type, false, "");
			}
			param.pet_param.lian_yao_pet_id = 50001;
			param.pet_param.lian_yao_pet_str_lv = 3;
			param.GetData(contentparam.item_list[0].param_data);
			contentparam.item_list[1] = contentparam.item_list[0];
			contentparam.item_list[2] = contentparam.item_list[0];
			contentparam.item_list[3] = contentparam.item_list[0];
			MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
		}
		break;
	
	case 5:
		{
			{
				MailContentParam contentparam; contentparam.Reset();
				MailContentTxt contenttxt = { "GM Test5" };
				memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
				memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));
				contentparam.item_list[0].item_id = 31256;
				contentparam.item_list[0].num = 3;
				contentparam.item_list[0].is_bind = 1;
				contentparam.item_list[1].item_id = 25855;
				contentparam.item_list[1].num = 1;
				contentparam.item_list[1].is_bind = 1;

				MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
			}
			
			{
				MailContentParam contentparam; contentparam.Reset();
				MailContentTxt contenttxt = { "GM Test555" };
				memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
				memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));
				contentparam.item_list[0].item_id = 28677;
				contentparam.item_list[0].num = 3;
				contentparam.item_list[0].is_bind = 1;
				contentparam.item_list[1].item_id = 25542;
				contentparam.item_list[1].num = 3;
				contentparam.item_list[1].is_bind = 1;
				contentparam.item_list[2].item_id = 24326;
				contentparam.item_list[2].num = 20;
				contentparam.item_list[2].is_bind = 1;

				MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
			}

			role->GetRoleModuleManager()->GetRoleCross()->OnBackToOriginServerReq();
		}
		break;

	case 6:
		{
			MailContentParam contentparam; contentparam.Reset();
			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));
			contentparam.item_list[0].item_id = 11206;
			contentparam.item_list[0].num = 1;
			contentparam.item_list[0].is_bind = 1;
			contentparam.item_list[1] = contentparam.item_list[0];
			contentparam.item_list[2] = contentparam.item_list[0];
			contentparam.item_list[3] = contentparam.item_list[0];
			MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
		}
		break;

	case 7:
		{
			MailContentParam contentparam; contentparam.Reset();
			MailContentTxt contenttxt = { "GM Test" };
			memcpy(contentparam.subject, contenttxt, sizeof(MailSubject));
			memcpy(contentparam.contenttxt, contenttxt, sizeof(MailContentTxt));
			contentparam.item_list[0].item_id = 20001;
			contentparam.item_list[0].num = 411700;
			contentparam.item_list[0].is_bind = 1;
			MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_GM_TEST, contentparam, MAIL_TYPE_SYSTEM);
		}
		break;
	}
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGoTo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int scene_id = atoi(result[0]);
	int pos_x = atoi(result[1]);
	int pos_y = atoi(result[2]);

	Posi pos(pos_x, pos_y);
	World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene->GetSceneKey(), pos);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetMineMonsterEncounterFlag(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int flag = atoi(result[0]);
	role->SetEncounterMonsterFlag(flag > 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmReloadConfig(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int config_type = atoi(result[0]);
	ReloadConfigManager::Instance().ReloadConfig(config_type, 0, 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmReloadConfig2(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int config_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	ReloadConfigManager::Instance().ReloadConfig(config_type, param1, param2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmEnterFB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int fb_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	bool ret = FBManager::Instance().EnterFB(role, fb_type, param1, param2);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmLeaveFB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	bool ret = FBManager::Instance().LeaveFB(role);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmSetAttr(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int attr_type = atoi(result[0]);
	int attr_value = atoi(result[1]);

	role->GetRoleModuleManager()->GmSetAttr(attr_type, attr_value);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::ActivityNextState(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int activity_type = atoi(result[0]);

	if (activity_type >= ACTIVITY_TYPE_INVALID && activity_type <= ACTIVITY_TYPE_MAX)
	{
		ActivityManager::Instance().OnForceToNextState(activity_type);
	}
	else if (activity_type >= RAND_ACTIVITY_TYPE_BEGIN && activity_type < RAND_ACTIVITY_TYPE_END)
	{
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
		if (NULL == ramgr)
		{
			return MakeupResult(netid, command->type, false, "");
		}

		ramgr->OnForceToNextState(activity_type);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMAchievement(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	/*int type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	int param3 = atoi(result[3]);
	int param4 = atoi(result[4]);*/


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::AddHonorRollData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int rounds = atoi(result[1]);

	time_t now = EngineAdapter::Instance().Time();

	int count = 0;
	int member_uid[MAX_TEAM_MEMBER_NUM] = { 0 };

	Team * team = role->GetMyTeam();
	if (NULL == team)
	{
		member_uid[count++] = role->GetUID();
	}
	else
	{
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			TeamMember *member = team->GetMemberInfo(i);
			if (NULL != member)
			{
				member_uid[count++] = member->uid;
			}
		}
	}

	EventHandler::Instance().OnClearanceFB(type, count, member_uid, rounds, now);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestSinglePVP(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return MakeupResult(netid, command->type, false, "");
	}

	static ObjID obj_list[256];
	memset(obj_list, 0, sizeof(obj_list));

	int num = scene->GetZoneMatrix()->GetObjByCircle(role->GetPos(), 100, obj_list, 256);

	bool ret = false;
	for (int i = 0; i < num; ++i)
	{
		Obj* obj = scene->GetObj(obj_list[i]);
		if (NULL == obj || Obj::OBJ_TYPE_ROLE != obj->GetObjType()) continue;

		Role* tmp_role = (Role*)obj;
		if (role->GetUID() == tmp_role->GetUID()) continue;
		if (tmp_role->InTeam()) continue;

		ret = BattleManagerLocal::GetInstance().StartSinglePVPPraticeReq(role, tmp_role);

		break;
	}

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmKillMonster(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int monster_id = atoi(result[0]);
	short level = atoi(result[1]);

	EventHandler::Instance().OnKillMonster(role, 1, &monster_id, &level, false);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestSchoolTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	short index = atoi(result[1]);
	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetSchoolTask()->OnTaskReq(index);
		break;
	case 1:
		role->GetRoleModuleManager()->GetSchoolTask()->OnQuickFinish(index);
		break;
	case 2:
		role->GetRoleModuleManager()->GetSchoolTask()->GmSchoolTaskOrder(index);
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestEscort(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);
	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetEscort()->AcceptTask(param);
		break;
	case 1:
		role->GetRoleModuleManager()->GetEscort()->CommitTask();
		break;
	case 2:
		role->GetRoleModuleManager()->GetEscort()->GiveupTask();
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddSaleItem(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int column = atoi(result[0]);
	int index = atoi(result[1]);
	int sale_num = atoi(result[2]);
	int price = atoi(result[3]);

	static Protocol::CSAddItemOnShelve req;
	req.column = column;
	req.index = index;
	req.item_num = sale_num;
	req.price = price;
	req.is_from_pet = 0;
	req.reserve_ch = 0;

	TradeMarketRouter::Instance().OnAddItemOnShelveReq(role, &req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRemoveSaleItem(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int sale_index = atoi(result[0]);

	static Protocol::CSRemoveItemOnShelve req;
	req.sale_index = sale_index;

	TradeMarketRouter::Instance().OnRemoveItemOnShelveReq(role, &req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFindReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int a = atoi(result[0]);
	int b = atoi(result[1]);
	int c = atoi(result[2]);

	role->GetRoleModuleManager()->GetDayReward()->OnFindReward(a, b, c);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintMapBoundry(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int x = atoi(result[0]);
	int y = atoi(result[1]);
	int is_print_boundry = atoi(result[2]);

	scene->GetMap()->PrintExtendBoundry(x, y, 1 == is_print_boundry);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintMapIsNeerBoundry(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int x = atoi(result[0]);
	int y = atoi(result[1]);

	scene->GetMap()->PrintIsNeerBoundry(x, y);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFaBaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int req_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	switch (req_type)
	{
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetFaBao()->SendFaBaoAllInfo();
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_ACTIVE:
		{
			role->GetRoleModuleManager()->GetFaBao()->ActiveFaBao(param1, true);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_UPLEVEL:
		{
			role->GetRoleModuleManager()->GetFaBao()->UpLevelFaBao(param1, param2 == 1);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_ADVANCED:
		{
			role->GetRoleModuleManager()->GetFaBao()->AdvancedFaBao(param1);
		}
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSystemNoticeTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int msg_type = atoi(result[0]);
	int type = atoi(result[1]);
	int param1 = atoi(result[2]);

	int length = 0;
	SysMsgType display_type = GetMsgType(type);
	switch (msg_type)
	{
	case 0:		// 迷宫符文飘字
		display_type = GetMsgType(SYS_MSG_FLOAT);
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_maze_racer_amulet_drop, param1);
		break;
	case 1:		// 当前场景信息
		{
			char * scene_info = new char[2048];
			sprintf(scene_info, "ID: %d | Key: %d | Type: %d", scene->GetSceneID(), scene->GetSceneKey(), scene->GetSceneType());
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), scene_info);
			delete[] scene_info;
		}
		break;
	default:
		return MakeupResult(netid, command->type, false, "");
	}

	if (length <= 0) return MakeupResult(netid, command->type, false, "");

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, display_type))
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCreateWildBoss(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	WildBossManager::Instance().GmCreateWildBoss(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestBuff(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);
	int time = atoi(result[2]);
	UInt32 now_time = (UInt32)EngineAdapter::Instance().Time();
	switch (type)
	{
	case ROLE_BUFF_CHANGE_ATTR:
		{
			BuffChangeAttr *buff = new BuffChangeAttr(role->GetRoleModuleManager(), now_time, now_time + time);
			buff->AddAttr(BATTLE_ATTR_MAX_HP, param);
			buff->SetSave(true);
			role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		}
		break;
	case ROLE_BUFF_EXP_UP:
		{
			BuffExpUp *buff = new BuffExpUp(role->GetRoleModuleManager(), now_time, now_time + time);
			buff->SetExpUpPer(param);
			buff->SetSave(true);
			role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		}
		break;
	case ROLE_BUFF_CHANGE_MOVE_SPEED:
		{
			BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(role->GetRoleModuleManager(), now_time, now_time + time);
			buff->SetMoveSpeed(param);
			buff->SetSave(true);
			role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
		}
		break;

	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSceneOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param = atoi(result[1]);

	switch (op_type)
	{
	case 0:		// 输出可行走点集
		{
			Scene * target_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(param, COMMON_SCENE_KEY);
			if (NULL != target_scene) target_scene->GmPrintfWalkableList();
		}
		break;
	case 1:		// 解锁所有场景
		role->GetRoleModuleManager()->GetMapUnlock()->GmChangeCheckSwitch(param);
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddGatherFbPower(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int value = atoi(result[0]);

	role->GetRoleModuleManager()->GetRoleGatherFb()->AddGatherPower(value);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCreateMineMonsterNPC(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	NpcManager::Instance().CreateNPCMonster(NPC_MINE_MONSTER_NPC_ID, scene, role->GetPos());

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCreateMineMonsterNPCById(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int npc_id = atoi(result[0]);

	NpcManager::Instance().CreateNPCMonster(npc_id, scene, role->GetPos());

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFightMineMonster(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int is_all_lv1 = atoi(result[0]);

	if (!BattleManagerLocal::GetInstance().StartMineMonsterFightReq(role, true, 0, is_all_lv1 > 0))
	{
		if (!scene->HasMineMonster())
		{
			role->NoticeNum(errornum::EN_CUR_SCENE_NO_MINE_MONSTER);
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddTeamRobot(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	Team* team = role->GetMyTeam();

	if (NULL != team)
	{
		if (team->GetMemberCount() < MAX_TEAM_MEMBER_NUM)
		{
			//team->AddRobot();
			Robot* robot = ROBOTMANAGER.CreateRobotToScene(scene, role->GetPos(), role->GetLevel(), true, __FUNCTION__, role->GetUserId().db_index);
			if (NULL != robot)
			{
				team->AddRobot(robot);
			}
		}
		else
		{
			role->NoticeNum(errornum::EN_TEAM_IS_FULL);
		}
	}
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMazeRacerWaitingHallTransImmediately(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (Scene::SCENE_TYPE_MAZE_RACER_WAITING_HALL != scene->GetSceneType()) return MakeupResult(netid, command->type, false, "");

	SpecialLogicMazeRacerWaitingHall * sp_logic = (SpecialLogicMazeRacerWaitingHall *)scene->GetSpecialLogic();
	if (NULL != sp_logic) sp_logic->GmTransportImmidiately();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMazeRacerOperator(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int operator_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	if (Scene::SCENE_TYPE_MAZE_RACER != scene->GetSceneType()) return MakeupResult(netid, command->type, false, "");
	SpecialLogicMazeRacer * sp_mr = (SpecialLogicMazeRacer *)scene->GetSpecialLogic();
	if (NULL == sp_mr) return MakeupResult(netid, command->type, false, "");

	sp_mr->GmMazeRacerOrder(role, operator_type, param1, param2);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmKickRole(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->SetForceKickDelay();
	ForceKickUserDelay *timer = new ForceKickUserDelay(role->GetUserId(), DISCONNECT_NOTICE_TYPE_LOGIN_OTHER_PLACE);  // 定时器启动 一定会T 不能相信客户端
	EngineAdapter::Instance().CreateTimerSecond(1, timer);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddOtherCoin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int p1 = atoi(result[0]);
	int p2 = atoi(result[1]);
	if (MONEY_TYPE_GOLD == p2 || MONEY_TYPE_COIN_BIND == p2 || MONEY_TYPE_IMMORTAL_COIN == p2)
	{
		return MakeupResult(netid, command->type, true, "");
	}
	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(p1, p2, __FUNCTION__);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQuickBuySearchPet(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	ItemID pet_item_id = static_cast<ItemID>(atoi(result[0]));
	short quality = static_cast<short>(atoi(result[1]));
	short order_type = static_cast<short>(atoi(result[2]));
	bool ret = TradeMarketManager::Instance().OnSearchPetForQuickBuyReq(role, order_type, pet_item_id, quality, 0, 0, 0, 0);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmAddChongzhi(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int chongzhi = atoi(result[0]);

	role->GetRoleModuleManager()->GetMoney()->AddGold(chongzhi, "GMaddchongzhi");

	EventHandler::Instance().OnAddChongzhi(role, chongzhi);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartCross(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	static CrossData_Business business_data;
	business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_INVALID;
	role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStopCross(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	static Protocol::SCReturnOriginalServer msg;
	EngineAdapter::Instance().NetSend(netid, (const char*)&msg, sizeof(msg));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintfUID(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int uid = role->GetUID();
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_get_role_uid_float, uid);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartOB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int watch_uid = atoi(result[0]);
	bool ret = BattleManagerLocal::GetInstance().RoleOBReq(role, watch_uid);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmTeamRecruit(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	TeamManager::Instance().OnTeamRecruitReq(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTeamSwitchToCross(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	TeamManager::Instance().OnTeamSwitchToCrossReq(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartTeamMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return MakeupResult(netid, command->type, false, "");
	}

	if (team->GetLeaderUID() != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return MakeupResult(netid, command->type, false, "");
	}

	TeamMatch::Instance().OnTeamStartMatch(team);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartUserMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int team_type = atoi(result[0]);

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return MakeupResult(netid, command->type, false, "");
	}

	TeamMatch::Instance().OnUserStartMatch(role->GetUniqueUserID(), role->GetLevel(), team_type, role->GetGuildID());

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartUserCrossMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int team_type = atoi(result[0]);

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return MakeupResult(netid, command->type, false, "");
	}

	TeamMatch::Instance().OnUserStartCrossMatch(role->GetUniqueUserID(), role->GetLevel(), team_type, role->GetGuildID());
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCOGAddProg(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int seq = atoi(result[0]);
	int num = atoi(result[1]);

	role->GetRoleModuleManager()->GetCourseOfGrowth()->GmAddProgress(seq, num);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetCourseOfGrowthProgress(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int prog = atoi(result[0]);
	int star = atoi(result[1]);

	role->GetRoleModuleManager()->GetCourseOfGrowth()->GmSetProgress(prog, star);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQueryOther(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int target_uid = atoi(result[0]);

	QueryHandler::Instance().OnQueryOther(target_uid, role->GetUID());

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuildInviteOther(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int target_uid = atoi(result[0]);

	Guild * guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return MakeupResult(netid, command->type, false, "");
	}

	GuildManager::Instance().OnGuildInviteOther(role, target_uid);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPartnerStrategy(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int strategy_type = atoi(result[0]);
	int target_grid_idx = atoi(result[1]);

	static battlegameprotocol::GameBattleSetStrategyReq gbssr;
	gbssr.bg_netid = role->GetBGNetID();
	gbssr.uid = role->GetUID();
	gbssr.strategy_type = strategy_type;
	gbssr.target_grid_idx = target_grid_idx;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbssr, sizeof(gbssr));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmColosseumOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);

	ColosseumManager::Instance().GmColosseumHandler(op_type);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmColosseumHandler(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	ColosseumManager::Instance().GmColosseumHandler(role, op_type, param1, param2);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintfAllTitle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetTitle()->GmPrintAllTitle();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSalaryTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);
	int param_3 = atoi(result[3]);
	int param_4 = atoi(result[4]);

	switch (op_type)
	{
	case 0:
		role->GetRoleModuleManager()->GetSalary()->GmAddSalary(param_1);
		break;

	case 1:
		role->GetRoleModuleManager()->GetSalary()->GMChangeSalaryProgress(param_1, param_2, param_3, param_4);
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmClearCmdDisplay(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	system("CLS");
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFlyawayRoleLeaveBattle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int is_leave_team = atoi(result[0]);

	static battlegameprotocol::GameBattleLeaveOBReq gbfrlbq;
	gbfrlbq.bg_netid = role->GetBGNetID();
	gbfrlbq.uid = role->GetUID();
	gbfrlbq.is_leave_team = is_leave_team;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbfrlbq, sizeof(gbfrlbq));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmForceFinishBattleMode(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int battle_mode = atoi(result[0]);

	static battlegameprotocol::GameBattleForceFinishBattleModeReq req;
	req.battle_mode = battle_mode;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddMaxAttrEquipment(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int item_id = atoi(result[0]);
	int attr_times = atoi(result[1]);
	int sp_eff_id = atoi(result[2]);
	int attr_inc = atoi(result[3]);

	bool ret = false;
	const ItemBase * itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase || I_TYPE_EQUIPMENT != itembase->GetItemType()) return MakeupResult(netid, command->type, false, "");

	static NetValueItemParam tmp_item_param; tmp_item_param.Reset();
	EquipmentParam & param_info = tmp_item_param.equip_param;
	param_info.has_random_net_value = 1;
	F_STRNCPY(param_info.creator_name, role->GetName(), sizeof(GameName));

	const Equipment* equip = (const Equipment*)itembase;
	equip->MaxValEquipmentAttr(&tmp_item_param);
	param_info.sp_id = sp_eff_id;

	for (int i = 0; i < param_info.attr_num; ++i)
	{
		param_info.attr_list[i].attr_value *= attr_times;
	}

	if (0 != attr_inc)
	{
		if (attr_inc > MAX_INT16) attr_inc = MAX_INT16;
		param_info.inc_num = ADD_POINT_TYPE_MAX;
		for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
		{
			AttrVocInfo & attr_info = param_info.attr_extra_inc[i];
			attr_info.attr_type = i;
			attr_info.attr_value = attr_inc;
		}
	}

	param_info.is_treasure = (char)equip->CheckEquipmentIsTreasure(param_info);
	static ItemDataWrapper idw; idw.Reset();
	tmp_item_param.GetData(idw.param_data);

	idw.item_id = item_id;
	idw.num = 1;
	idw.is_bind = 1;
	idw.has_param = 1;

	ret = role->GetRoleModuleManager()->GetKnapsack()->Put(idw, PUT_REASON_GM);
	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmResearchTaskHandler(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param = atoi(result[1]);

	role->GetRoleModuleManager()->GetResearchTask()->GmResearchTaskHandle(op_type, param);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuildModuleOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int guild_id = atoi(result[0]);
	int op_type = atoi(result[1]);
	int param = atoi(result[2]);

	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return MakeupResult(netid, command->type, false, "");
	}

	switch (op_type)
	{
	case 0:
		guild->GetMemberManagerRef().GmGuildMemberOrder(param);
		break;
	case 1:
		GuildManager::Instance().GmGuildManagerHandler(param, guild_id);
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuildOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int guild_id = atoi(result[0]);
	int op_type = atoi(result[1]);
	int param = atoi(result[2]);

	Guild * guild = guild_id > 0 ? GuildManager::Instance().GetGuild(guild_id) : role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return MakeupResult(netid, command->type, false, "");
	}

	guild->GmGuildHandler(op_type, param);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPlatformBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param = atoi(result[1]);

	PlatformBattle::Instance().GmReq(role, op_type, param);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmShadowFight(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		int role_num = 0;
		static Role* role_list[SIDE_MAX_ROLE_NUM];
		static RoleBattleData role_data[SIDE_MAX_ROLE_NUM];

		for (int i = 0; i < team->GetMemberCount() && role_num < SIDE_MAX_ROLE_NUM; ++i)
		{
			Role* role = team->GetMemberRoleByIndex(i);
			if (NULL != role)
			{
				role_list[role_num] = role;
				role->GetRoleModuleManager()->CollectRoleBattleData(&role_data[role_num], false);
				++role_num;
			}
		}

		static BattleData_Business business_data;
		business_data.Reset();
		BattleManagerLocal::GetInstance().StartMultiFightShadowReq(role_num, role_list, role_num, role_data, BATTLE_MODE_NORMAL_PRACTICE, business_data);
	}
	else
	{
		static RoleBattleData rbd;
		static BattleData_Business business_data;
		business_data.Reset();
		role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, false);
		BattleManagerLocal::GetInstance().StartSingleFightShadowReq(role, rbd, BATTLE_MODE_NORMAL_PRACTICE, business_data);
	}

	return true;
}

bool Gm::GmRoleHandler(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param = atoi(result[1]);

	role->GmRoleOrder(op_type, param);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartTeamLeaderVote(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (team != NULL)
	{
		team->OnRoleLaunchLeaderVote(role);
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTeamMemberVote(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int is_yes = atoi(result[0]);

	Team* team = role->GetMyTeam();
	if (team != NULL)
	{
		team->OnRoleVote(role, is_yes > 0);
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetChiefElection(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int prof = atoi(result[0]);

	WorldStatus::Instance().GmsetChiefElection(role->GetUID(), prof);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTaskChainOrder(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param = atoi(result[1]);

	role->GetRoleModuleManager()->GetTaskChain()->GmTaskChainOrder(op_type, param);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCreateTeam(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	TeamManager::Instance().CreateTeam(role, 0, 120, 1);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPauseBattle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int is_pause = atoi(result[0]);

	static battlegameprotocol::GameBattleRolePauseBattle gbrpb;
	gbrpb.bg_netid = role->GetBGNetID();
	gbrpb.uid = role->GetUID();
	gbrpb.is_pause = is_pause;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbrpb, sizeof(gbrpb));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GetPos(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	const Posi &pos = role->GetPos();
	char dst_str[128];
	int length = 0;
	length = SNPRINTF(dst_str, sizeof(dst_str), "[%s %d]: Pos = (%d, %d), scene_id = (%d), scene_type = (%d), map[%d,%d]=%c is_near_boundry[%s]",
		role->GetName(), role->GetUID(), pos.x, pos.y, scene->GetSceneID(), scene->GetSceneType(),
		pos.x, pos.y, scene->GetMap()->GetMapGrid(pos.x, pos.y), scene->GetMap()->IsPosNearBoundry(pos.x, pos.y) ? "YES" : "NO");
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), dst_str, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
	return true;
}

bool Gm::GmCreateRobot(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	RobotManager::Instance().CreateRobotToScene(scene, role->GetPos(), role->GetLevel(), true, __FUNCTION__, role->GetUserId().db_index);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestServerSchoolTaskList(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	int today_school_task_list[SCHOOL_TASK_NUM] = { 0 };
	LOGIC_CONFIG->GetSchoolTaskConfig()->RandTaskListForServer(today_school_task_list);

	printf("GmTestServerSchoolTaskList\n");

	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		printf("[%d]\n", today_school_task_list[i]);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::RePos(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int pos_x = atoi(result[0]);
	int pos_y = atoi(result[1]);

	World::GetInstWorld()->GetSceneManager()->GoTo(role, scene->GetSceneID(), scene->GetSceneKey(), Posi(pos_x, pos_y));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPartnerSwitch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int partnerA = atoi(result[0]);
	int partnerB = atoi(result[1]);

	role->GetRoleModuleManager()->GetPartner()->PartnerSwitchFightPost(partnerA, partnerB);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmConvertShopBuy(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int convert_type = atoi(result[0]);
	int seq = atoi(result[1]);

	role->GetRoleModuleManager()->GetRoleConvertShop()->BuyReq(convert_type, seq, 1, 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleTradeMarketStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");


	UserTradeInfo* user = TradeMarketManager::Instance().GetUserTradeInfo(role->GetUID());
	if (user != NULL)
	{
		user->GMSetNextStatusTime();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRandActivityReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	short rand_activity_type = atoi(result[0]);
	short opera_type = atoi(result[1]);
	int param_1 = atoi(result[2]);
	int param_2 = atoi(result[3]);
	int param_3 = atoi(result[4]);
	Protocol::CSRandActivityOperaReq req;
	req.rand_activity_type = rand_activity_type;
	req.opera_type = opera_type;
	req.param_1 = param_1;
	req.param_2 = param_2;
	req.param_3 = param_3;
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityOpReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRandActivityAdd(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int free_num = atoi(result[0]);
	int kill_num = atoi(result[1]);

	RoleActivityTrunTable * role_activity = static_cast<RoleActivityTrunTable * >
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TRUN_TABLE));
	if (NULL != role_activity)
	{
		role_activity->GMAddTimes(free_num, kill_num);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMysteriousTrial(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int day = atoi(result[0]);
	int seq = atoi(result[1]);
	int add_num = atoi(result[2]);

	role->GetRoleModuleManager()->GetMysteriousTrial()->GmAddMysteriousTrial(day, seq, add_num);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetMysteriousTrial(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetMysteriousTrial()->GmSetDayId();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmBossChapterReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);

	switch (type)
	{
	case Protocol::CSBossChapterReq::REQ_TYPE_CLAIM_BOSS_REWARD:
		{
			role->GetRoleModuleManager()->GetBossChapter()->ClaimReward(param);
		}
		break;

	case Protocol::CSBossChapterReq::REQ_TYPE_CLAIM_CHAPTER_REWARD:
		{
			role->GetRoleModuleManager()->GetBossChapter()->ClaimChapterReward(param);
		}
		break;

	case Protocol::CSBossChapterReq::REQ_TYPE_START_BATTLE:
		{
			role->GetRoleModuleManager()->GetBossChapter()->StartBattleReq(param);
		}
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmBountyTaskReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetBountyTask()->GMFastFinishTask();
		break;

	case 1:
		role->GetRoleModuleManager()->GetBountyTask()->OnTurnTableRewardFetchReq();
		break;

	case 2:
		{
			unsigned int exp = 0;
			int coin = 0;
			int jade = 0;
			std::vector<ItemConfigData> rewards;
			role->GetRoleModuleManager()->GetBountyTask()->OneClickFinishTask(exp, coin, jade, rewards);
		}
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmEscapeCurrentPosition(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	Posi pos;
	int scene_id = scene->GetTownPoint(&pos);

	Posi center_pos;
	int center_scene_id = GLOBALCONFIG->GetCenterCityBornPos(center_pos);
	if (center_scene_id == scene_id)
	{
		role->ResetPos(pos.x, pos.y);
	}
	else
	{
		Scene* center_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(center_scene_id, COMMON_SCENE_KEY);
		if (NULL != center_scene)
		{
			center_scene->GetTownPoint(&center_pos);
		}
		World::GetInstWorld()->GetSceneManager()->GoTo(role, center_scene_id, COMMON_SCENE_KEY, center_pos);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSwitchPlatform(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int scene_cfg = atoi(result[0]);

	const PlatformBattleSceneCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetSceneCfg(scene_cfg);
	if (cfg == NULL)
	{
		return false;
	}

	World::GetInstWorld()->GetSceneManager()->GoTo(role, cfg->scene_id, COMMON_FB_KEY, cfg->enter_pos);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTransactionTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int index = atoi(result[1]);
	int refer_num = atoi(result[2]);
	Protocol::CSTransactionTaskReq req;
	req.op_type = op_type;
	req.index = index;
	req.refer_num = refer_num;
	role->GetRoleModuleManager()->GetTransactionTask()->TransactionTaskOp(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmActiveSystem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int index = atoi(result[1]);
	Protocol::CSActiveSystemReq req;
	req.op_type = op_type;
	req.index = index;
	role->GetRoleModuleManager()->GetActiveSystem()->ActiveSystemOp(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTaskChainTaskApt(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int task_type = atoi(result[0]);

	if (task_type < 0)
	{
		role->GetRoleModuleManager()->GetTaskChain()->GMRefreshTaskChain();
	}
	else
	{
		role->GetRoleModuleManager()->GetTaskChain()->GMTaskChainAccept(task_type);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	//World::GetInstWorld()->GetSceneManager()->GetSceneMatchManager()->Test();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRAExpelMonsterTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int type = atoi(result[0]);

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_EXPEL_MONSTER))
	{
		switch (type)
		{
		case 0:
			{
				RandAcitivityExpelMonster * randactivity = ramgr->GetRAExpelMonster();
				if (randactivity != NULL)
				{
					randactivity->GmRefreshMonster();
				}
			}
			break;
		case 1:
			{
				Protocol::CSRandActivityOperaReq req;
				req.rand_activity_type = RAND_ACTIVITY_TYPE_EXPEL_MONSTER;
				req.opera_type = 0;
				req.param_1 = 0;
				req.param_2 = 0;
				role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityOpReq(&req);
			}
			break;
		default:
			break;
		}

	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddActive(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int add_num = atoi(result[0]);
	role->GetRoleModuleManager()->GetActiveSystem()->GmAddActiveDegree(add_num);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAutoResearchTask(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		return false;
	}

	Team* research_team = TeamManager::Instance().GetJoinableTeamByTeamType(TEAM_TYPE_RESEARCH_TASK);
	if (NULL != research_team)
	{
		TeamManager::Instance().JoinTeam(role, research_team->GetTeamIndex(), false);
	}
	else
	{
		const TeamTypeCfg* cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(TEAM_TYPE_RESEARCH_TASK);
		if (NULL == cfg) return false;

		const TeamTypeRobotCfg* team_robot_cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(cfg->team_type);
		if (NULL == team_robot_cfg) return false;

		Team* new_team = TeamManager::Instance().CreateTeam(role, cfg->low_level_limit, cfg->high_level_limit, cfg->team_type);
		if (NULL == new_team) return false;

		int add_robot_num = MAX_TEAM_MEMBER_NUM - new_team->GetMemberCount();
		if (add_robot_num > 0)
		{
			for (int i = 0; i < add_robot_num; ++i)
			{
				int robot_level = ROBOTCONFIG->RandRobotLevelByGroup(team_robot_cfg->level_group_id, role->GetLevel());
				Robot* robot = RobotManager::Instance().CreateRobotToScene(role->GetScene(), role->GetPos(), robot_level, true, __FUNCTION__, role->GetUserId().db_index);
				if (NULL != robot)
				{
					robot->SetBoundTeamType(new_team->GetTeamType());
					robot->SetBoundBattleMode(team_robot_cfg->battle_mode);
					new_team->AddRobot(robot);
				}
			}
		}

		if (!role->GetRoleModuleManager()->GetResearchTask()->IsHaveResearchTask())
		{
			role->GetRoleModuleManager()->GetResearchTask()->OnAcceptResearchTask();
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPetOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	int p3 = atoi(result[3]);
	Protocol::CSPetOp req;
	req.op_type = op_type;
	req.p1 = p1;
	req.p2 = p2;
	req.p3 = p3;
	role->GetRoleModuleManager()->GetPet()->OnPetOp(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSpecialReqMod(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int type = atoi((result[0]));

	RandactivityJoyseeksfavors* act = static_cast<RandactivityJoyseeksfavors*>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
	if (NULL != act)
	{
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS))
		{
			switch (type)
			{
			case Protocol::CSRAJoySeeksFavorsMatchReq::TYPE_MATCH:
				act->OnMatchReq(role);
				break;
			case Protocol::CSRAJoySeeksFavorsMatchReq::TYPE_CANCEL:
				act->OnCancelReq(role);
				break;
			}
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTeamLeaderSummonMember(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int summon_role_id = atoi(result[0]);
	TeamManager::Instance().OnSummonMemberReq(role, summon_role_id);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleCmd(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	//	char *result[128] = {};
	//	this->ParseCommandParam(command->command, result, " ");

	std::string cmd((const char *)command->command);

	role->GetRoleModuleManager()->OnRoleCmd(cmd, EngineAdapter::Instance().Time());
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMPatrolTaskTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);;
	int param_1 = atoi(result[1]);

	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetTaskManager()->GetCoinByLevel(param_1);
		break;

	case 1:
		for (int i = 0; i < param_1; ++i) //杀死数量 = kill_num 触发奖励
		{
			role->GetRoleModuleManager()->GetTaskManager()->AddPatrolTaskProgress();
		}
		break;

	case 2:
		role->GetRoleModuleManager()->GetTaskManager()->CommitPatrolTask();
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMOneKeyFinish(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exe_type = atoi(result[0]);

	switch (exe_type)
	{
	case 0:						// 三界除魔
		role->GetRoleModuleManager()->GetResearchTask()->OnOneKeyFinishZhenZhong();
		break;

	case 1:						// 神算子的考验
		role->GetRoleModuleManager()->GetWstQuestion()->OneKeyFinishWstQuation();
		break;

	case 2:						// 门派任务(赏金任务)
		role->GetRoleModuleManager()->GetBountyTask()->OneKeyFinishBountyTask();
		break;

	case 3:						// 四象幻境
		role->GetRoleModuleManager()->OneKeyFinishFantasyFB();
		break;

	case 4:						// 丝绸之路
		role->GetRoleModuleManager()->GetTransactionTask()->OneKeyFinishTransactionTask();
		break;

	case 5:						// 缉捕盗贼
		role->GetRoleModuleManager()->GetTreasureMap()->OneKeyFinishTreasureMap();
		break;

	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMPartnerTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exe_type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);
	int param_3 = atoi(result[3]);
	int param_4 = atoi(result[4]);

	int partner_list[PARTNER_SYNFIGHT_NUM_MAX];
	memset(partner_list, 0, sizeof(partner_list));
	int fight_num = 0;

	switch (exe_type)
	{
	case 0://伙伴升品
		if (param_1 == NorexItem::I_NOREX_ITEM_PARTNER_QUALITYUP)
		{
			role->GetRoleModuleManager()->GetPartner()->QualityUpgrade(param_2, param_3);
		}
		break;

	case 1://为某个伙伴装备元素水晶
		role->GetRoleModuleManager()->GetPartner()->UseElementCrystal(param_1, param_2);
		break;

	case 2://获取伙伴出战列表
		role->GetRoleModuleManager()->GetPartner()->GetSynFightPartnerIdList(PARTNER_SYNFIGHT_NUM_MAX, partner_list, &fight_num);
		break;

	case 3://快速装备元素水晶到出战伙伴
		role->GetRoleModuleManager()->GetPartner()->UseElementCrystalToFight(param_1, param_2, param_3, param_4);
		break;

	case 4://卸下某个伙伴的元素水晶
		role->GetRoleModuleManager()->GetPartner()->RemoveElementCrystal(param_1);
		break;

	case 5://卸下所有激活伙伴的元素水晶
		role->GetRoleModuleManager()->GetPartner()->RemoveAllCurPartnerElementCrystal();
		break;

	case 6://元素水晶-1
		role->GetRoleModuleManager()->GetPartner()->ElementCrystalCountSubByPartnerId(param_1);
		break;

	case 7://设置元素水晶次数
		role->GetRoleModuleManager()->GetPartner()->GMSetCountSub(param_1, param_2);
		break;
	case 8://设置元素水晶次数
		for (int i = 0; i < 30; ++i)
		{
			role->GetRoleModuleManager()->GetPartner()->OnPartnerActive2(i);
		}
		break;
	}



	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMRandActivityTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	LOGIC_CONFIG->GetRandActivityConfigManager()->GetInvalidTimeActivity(-1);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmForbidRole(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int role_id = atoi(result[0]);
	Role* forbid_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != forbid_role)
	{
		forbid_role->ForbidRole("GM fobid", 2 * 60 * 60);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRemoveVideo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int monster_group_id = atoi(result[0]);
	int is_server_first_kill = atoi(result[1]);

	bool ret = BattleVideoMgr::Instance().RemoveVideo(monster_group_id, is_server_first_kill > 0);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmHuaShenInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	Protocol::CSHuaShenInfoReq req;
	req.op_type = op_type;
	req.param1 = param1;
	role->GetRoleModuleManager()->GetHuaShenManager()->OnInfoReq(req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartNpcDemoBattleReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int npc_demo_seq = atoi(result[0]);
	bool ret = BattleManagerLocal::GetInstance().StartDemoFightReq(role, npc_demo_seq);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmHuanJieZhanChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	Activity * act = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (act != NULL)
	{
		((ActivityHuanJieZhanChang *)act)->GmOp(role, op_type, param1, param2);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTriggerUnknownReasonDisconnect(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	printf("Loop stated\n");
	long long loop_times = atoll(result[0]);
	long long sum = 0;
	for (long long i = 0; i < loop_times; ++i)
	{
		sum += i;
	}
	printf("Loop finish\n");

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHuaShenMedalReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	Protocol::CSHuaShenMedalReq req;
	req.req_type = op_type;
	req.param1 = param1;
	req.param2 = param2;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCheckRoleInBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int battle_id = atoi(result[1]);
	int battle_server_idx = atoi(result[2]);
	switch (type)
	{
	case 0:
		{
			BattleManagerLocal::GetInstance().CheckRoleReallyInBattle(role, __FUNCTION__);
		}
		break;

	case 1:
		{
			battlegameprotocol::GameBattleRoleRejoinBattleScene req;
			req.battle_id = battle_id;
			req.bg_netid = role->GetBGNetID();
			req.uid = role->GetUID();
			F_STRNCPY(req.role_name, role->GetName(), sizeof(req.role_name));

			World::GetInstWorld()->SendMsgToOneBattle(battle_server_idx, (const char*)&req, sizeof(req));
		}
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestProtobuf(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	if (0 == type)
	{
		RoleBattleData rbd;
		role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, true);

		PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_PROTOBUF);
		static char TLV_BUFF[10 * 1024];
		//static char TLV_UNBUFF[10 * 1024];
		static char PB_BUFF[16384];

		PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
		for (int i = 0; i < 1000000; ++i)
		{
			rbd.GetPBData(pb_data);
			TLVSerializer serializer;
			serializer.Reset(TLV_BUFF, sizeof(TLV_BUFF));

			TLVSerializer pb_s;
			int cached_size_before_Serialize = pb_data->GetCachedSize();
			pb_data->SerializeToArray(PB_BUFF, sizeof(PB_BUFF));
			pb_s.Reset(PB_BUFF, sizeof(PB_BUFF));
			pb_s.MoveCurPos(pb_data->GetCachedSize()); // 在Serialize后调用GetCacheSize, 而不调用ByteSize(ByteSize遍历整份数据计算数据大小）
			serializer.Push(pb_s);

			if (i % 1000 == 0)
			{
				printf("i[%d] spaceused[%d]\n", i, pb_data->SpaceUsed());
			}

			/*if (0 == i )
			{
				std::string data = pb_data.SerializeAsString();
				printf("GmTestProtobuf type[%d] data_length[%d]\n", type, serializer.Size());
				printf("PBData cached_size111[%d] cached_size[%d] byte_size[%d] size[%d] length[%d], content[%s]\n", cached_size_before_Serialize, pb_data.GetCachedSize(), pb_data.ByteSize(), data.size(), data.length(), data.c_str());

				TLVUnserializer unserializer;
				unserializer.Reset(serializer.Ptr(), serializer.Size());

				TLVUnserializer pb_uns;
				bool ret = unserializer.Pop(&pb_uns);
				assert(ret);
				PB_RoleBattleData tmp_pb_rbd;
				ret = tmp_pb_rbd.ParseFromArray(pb_uns.Ptr(), pb_uns.Size());
				assert(ret);

				printf("-------------------------------111111\n");
				gamelog::g_log_debug.printf(LL_INFO, "1111111111111\n%s", tmp_pb_rbd.DebugString().c_str());

				printf("-------------------------------\n\n\n");
				gamelog::g_log_debug.printf(LL_INFO, "2222222222222\n%s", pb_data.DebugString().c_str());

				printf("-------------------------------22222\n");

			}*/
		}

		printf("SpaceUsed[%d]\n", pb_data->SpaceUsed());

		long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PROTOBUF);
		printf("GmTestProtobuf type[%d] elapse[%lld(ms), %lld(us)]\n", type, elapse_us / 1000, elapse_us);
	}
	else if (1 == type)
	{
		//RoleBattleData rbd;
		//role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, true);

		//PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_PROTOBUF);
		//static char TLV_BUFF[4 * 1024 * 1024];
		//for (int i = 0; i < 100000; ++i)
		//{
		//	TLVSerializer serializer;
		//	serializer.Reset(TLV_BUFF, sizeof(TLV_BUFF));
		//	rbd.Serialize(serializer);
		//	if (0 == i)
		//	{
		//		printf("GmTestProtobuf type[%d] data_length[%d]\n", type, serializer.Size());
		//	}
		//}
		//long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PROTOBUF);
		//printf("GmTestProtobuf type[%d] elapse[%lld(ms)]\n", type, elapse_us / 1000);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSimulatePokerOper(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);

	// 模拟翻牌操作
	switch (type)
	{
	case 0:
		{
			const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene->GetSceneID());// 判断是不是在场景中
			SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
			if (cfg != NULL && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_MIYI)
			{
				SpecialLogicMiYi * miyi_fb = (SpecialLogicMiYi *)scene->GetSpecialLogic();
				if (miyi_fb != NULL)
				{
					miyi_fb->GetPoker(param_1, param_2);
				}
			}
		}
		break;

	case 1:
		{
			int is_in_scene = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterIdBySceneId(scene->GetSceneID());		// 判断这个怪是不是在场景中
			SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
			if (is_in_scene != 0 && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_COMMON_SCENE)
			{
				SpecialLogicCommon *wild_co = (SpecialLogicCommon *)scene->GetSpecialLogic();
				if (wild_co != NULL)
				{
					wild_co->GetPoker(param_1, param_2);
				}
			}
		}
		break;

	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleSkillTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exe_type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);

	switch (exe_type)
	{
	case 0:// 增加生活技能经验
		role->GetRoleModuleManager()->GetRoleSkill()->AddLifeSkillExp(param_1, param_2);
		break;

	case 1:// 制作装备
		role->GetRoleModuleManager()->GetRoleSkill()->OnMakeEquipment(param_1, 1);
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}


bool Gm::GmRoleResetGodToken(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	RoleActivityGodToken * role_activity = static_cast<RoleActivityGodToken * >
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != role_activity)
	{
		role_activity->OnReSetRAGodToken(true);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGenUniqueID(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int times = atoi(result[0]);
	for (int i = 0; i < times; ++i)
	{
		long long unique_id = 0;
		UniqueIDGenerator::Instance()->GenUniqueID(&unique_id);
		printf("GmGenUniqueID [%lld]\n", unique_id);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCheckDayIndex_6(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	/*
	unsigned int t1 = 1598544000; // 8.28 0点
	unsigned int t2 = 1598565600; // 8.28 6点
	unsigned int t3 = 1598648400; // 8.29 5点
	unsigned int t4 = 1598734800; // 8.30 5点
	unsigned int t5 = 1598752800; // 8.30 10点
	unsigned int t6 = 1598666400; // 8.29 10点

	int day_idx1 = GetDayIndex_6(t1, t2);
	int day_idx2 = GetDayIndex_6(t1, t3);
	int day_idx3 = GetDayIndex_6(t1, t4);
	int day_idx4 = GetDayIndex_6(t1, t5);
	int day_idx5 = GetDayIndex_6(t1, t6);
	*/
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRefreshGuildWeekTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->RefreshGuildWeekTask();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPartnerSkinTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exe_type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);

	switch (exe_type)
	{
	case 0: // 激活伙伴皮肤
		role->GetRoleModuleManager()->GetPartner()->ActiveSkin(param_1, param_2);
		break;

	case 1: // 穿戴皮肤
		role->GetRoleModuleManager()->GetPartner()->PutOnSkin(param_1, param_2);
		break;

	case 2: // 卸下皮肤
		role->GetRoleModuleManager()->GetPartner()->PutOffSkin(param_1, param_2);
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAutoCatchPet(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int monster_id = atoi(result[0]);
	int common_card = atoi(result[1]);
	int silver_card = atoi(result[2]);
	int gold_card = atoi(result[3]);

	Protocol::CSSetAutoCatchPet info;
	info.scene_id = 0;
	info.auto_catch_info_num = 1;
	info.max_seal_card_common = common_card;
	info.max_seal_card_silver = silver_card;
	info.max_seal_card_gold = gold_card;
	info.setting_list[0].monster_id = monster_id;
	info.setting_list[0].max_catch_num = 1;

	role->GetRoleModuleManager()->GetAutoCatchPet()->SetAutoCatchPetInfo(&info);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFacesCoreTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int exe_type = atoi(result[0]);
	int param_1 = atoi(result[1]);

	switch (exe_type)
	{
	case 0:
		role->GetRoleModuleManager()->GetFaceScore()->AddFaceScore(param_1);
		break;

	case 1:
		role->GetRoleModuleManager()->GetFaceScore()->OnGainTitle(param_1);
		break;

	default:
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPetSkin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	//int p3 = atoi(result[3]);

	if (type == 0)
	{
		role->GetRoleModuleManager()->GetPet()->ActivityPetSkin(p1, p2 > 0 ? p2 : 1);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFormationRename(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int seq = atoi(result[0]);
	char* new_name = result[1];

	role->GetRoleModuleManager()->GetRoleFormation()->RenameFormationReq(seq, new_name);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFormationDetailInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int seq = atoi(result[0]);
	role->GetRoleModuleManager()->GetRoleFormation()->FormationDetailInfoReq(seq);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFormationSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Protocol::CSFormationSave req;
	role->GetRoleModuleManager()->GetRoleFormation()->SaveFormationReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestCommonSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op = atoi(result[0]);
	int type = atoi(result[1]);
	int p1 = atoi(result[2]);
	int p2 = atoi(result[3]);
	int p3 = atoi(result[4]);


	CommonSaveManager::Instance().Gm(op, type, p1, p2, p3);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmChiefElectionSeaElection(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	//int type = atoi(result[0]);
	int p1 = atoi(result[1]);
	//int p2 = atoi(result[2]);
	//int p3 = atoi(result[3]);

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity != NULL)
		activity->GmNextMatchState(p1);


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::IsRandActivityOpen(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int rand_activity_type = atoi(result[0]);

	char dst_str[128];
	int length = 0;
	bool ret = false;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(rand_activity_type))
		{
			ret = true;
		}

		length = SNPRINTF(dst_str, sizeof(char) * 128, "Activity %d is %s", rand_activity_type, ret ? "Open" : "Close");
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), dst_str, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
	}

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmSetTianJiangHaoLi(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int begin_time = atoi(result[0]);
	int end_time = atoi(result[1]);

	if (begin_time >= 2400 || end_time >= 2400) return false;
	if (begin_time <= 0 || end_time <= 0) return false;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		RandActivityTianJiangHaoLi * act = (RandActivityTianJiangHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI);
		if (NULL != act)
		{
			act->GmSetTime(begin_time, end_time);
		}
	}

	if (NULL != ramgr)
	{
		RandActivityLaoDongHaoLi * act = (RandActivityLaoDongHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);
		if (NULL != act)
		{
			act->GmSetTime(begin_time, end_time);
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddLuckCoin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Int64 count = atoll(result[0]);
	if (count <= 0) return false;

	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(count, MONEY_TYPE_LUCK_COIN, "GM");

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmBuyToken(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		RandActivityTianJiangHaoLi * act = (RandActivityTianJiangHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN);
		if (NULL != act)
		{
			RoleActivityGodToken * role_activity = static_cast<RoleActivityGodToken *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL != role_activity)
			{
				role_activity->OnRAGodTokenBuyToken(true);
			}
		}
	}
	return false;
}

bool Gm::GmInder(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int start_index = atoi(result[0]);
	int end_index = atoi(result[1]);
	int column = atoi(result[2]);

	bool ret = role->GetRoleModuleManager()->GetKnapsack()->InOrder(column, start_index, end_index, true);

	return MakeupResult(netid, command->type, ret, "");
}

#include "other/rolecommonsave/rolecommonsavemanager.hpp"
bool Gm::GmTestRoleCommonSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op = atoi(result[0]);
	int type = atoi(result[1]);
	int p1 = atoi(result[2]);
	int p2 = atoi(result[3]);

	role->GetRoleModuleManager()->GetRoleCommonSaveManager()->Gm(op, type, p1, p2);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddShanHaiSoulFragment(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int star = atoi(result[0]);
	if (0 > star) return false;

	role->GetRoleModuleManager()->GmShanHaiBossAddSoulFragment(star);

	return MakeupResult(netid, command->type, true, "");
}


bool Gm::GmOnWeekChangeFlexible(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	ActivityManager::Instance().OnWeekChange();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFastforwardOnlyFightEndEnterTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	ActivityOnlyFight* activity = (ActivityOnlyFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
	if (NULL != activity)
	{
		activity->GmEnterEndTimeFastforward();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFastforwardGuildFightEndEnterTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	ActivityGuildFight* activity = (ActivityGuildFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
	if (NULL != activity)
	{
		activity->GmEnterEndTimeFastforward();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRetrieveExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long retrieve_exp = atoll(result[0]);
	role->GmRetrieveExp(retrieve_exp);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuildNoLeader(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Guild* guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->GmSetNoLeader();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleBuyItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	std::istringstream ss;
	ss.str(std::string(command->command));
	role->GetRoleModuleManager()->GmBuyItem(ss);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleBuyItem2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int buy_item_type = 0;
	if (result[0])
	{
		buy_item_type = atoi(result[0]);
	}

	role->GetRoleModuleManager()->GmBuyItem2(buy_item_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMiYiSetPassCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int pass_count = atoi(result[0]);
	if (pass_count >= 0)
	{
		role->GetRoleModuleManager()->GetMiYi()->GmSetDayPassCount(pass_count);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddMatchUser(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	TeamMatch::Instance().GmAddUserToMatch();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSendAttrListReason(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");



	int reason_type = atoi(result[0]);
	int param1 = atoi(result[1]);

	switch (reason_type)
	{
	case Protocol::SCAttrListReason::REASON_TYPE_ROLE:
		{
			role->GetRoleModuleManager()->SendAttrListReason(param1);
		}
		break;
	case Protocol::SCAttrListReason::REASON_TYPE_PET:
		{
			role->GetRoleModuleManager()->GetPet()->SendPetAttrListReason(param1);
		}
		break;
	case Protocol::SCAttrListReason::REASON_TYPE_PARTNER:
		{
			role->GetRoleModuleManager()->GetPartner()->SendPartnerAttrListReason(param1);
		}
		break;
	case Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_ROLE:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->SendRoleAttrReasonList(param1);
		}
		break;
	case Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_PET:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->SendPetAttrReasonList(param1);
		}
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAnecdoteReset(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int anecdote_type = atoi(result[0]);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_type)
	{
		for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->GmResetAnecdote(type);
		}
	}
	else
	{
		role->GetRoleModuleManager()->GetAnecdoteTask()->GmResetAnecdote(anecdote_type);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestNetwork(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
#ifndef _DEBUG
	return false;
#endif

	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	crossgameprotocal::GameHiddenTestNetwork msg;

	int package_count = atoi(result[0]);
	int package_length = atoi(result[1]);
	if (package_length < 0 || package_length > sizeof(msg.buffer))
	{
		return false;
	}

	if (package_count <= 0)
	{
		return false;
	}

	PerformanceCatch::Instance().GetElapseUsBegin(0);

	crossgameprotocal::GameHiddenTestNetwork ghtn;

	for (int i = 0; i < package_count; ++i)
	{
		if (0 == i)
		{
			ghtn.is_start = 1;
		}
		else
		{
			ghtn.is_start = 0;
		}

		if (i == package_count - 1)
		{
			ghtn.is_end = 1;
		}
		else
		{
			ghtn.is_end = 0;
		}

		ghtn.send_length = package_length;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghtn, package_length);
	}

	long long us = PerformanceCatch::Instance().GetElapseUsEnd(0);
	gamelog::g_log_debug.printf(LL_INFO, "Test Network package_count[%d] package_len[%d] elapse[%lld]", package_count, package_length, us);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmResetShanHaiChallengeCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GmResetShanHaiChanllengeCount();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRandActivityMakeUp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	Protocol::CSRandActivityMakeUpReq req;
	req.rand_activity_type = atoi(result[0]);
	req.make_up_type = atoi(result[1]);

	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityMakeUpReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetRandActivityEndTimestamp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int rand_activity_type = atoi(result[0]);
	int end_day = atoi(result[1]);

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr || end_day < 0) return false;

	RandActivity * rand_activity = ramgr->ForceGetRandActivity(rand_activity_type);
	if (NULL == rand_activity) return false;

	if (ACTIVITY_STATUS_OPEN != rand_activity->GetStatus())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int next_zero_timestamp = (unsigned int)EngineAdapter::Instance().NextDayInterval(0, 0, 0) + now_second;
	unsigned int activity_end_timestamp = next_zero_timestamp + end_day * SECOND_PER_DAY;
	if (rand_activity->GetBeginTime() >= activity_end_timestamp)
	{
		return false;
	}

	rand_activity->SetEndTime(activity_end_timestamp, true);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetSpecialLogicActivityEndTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int special_logic_activity_type = atoi(result[0]);
	int end_day = atoi(result[1]);
	WorldStatus::Instance().GmSetFindEndTimestamp(special_logic_activity_type, end_day);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSchoolExercise(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int seq = atoi(result[0]);
	bool ret = role->GetRoleModuleManager()->GetSchoolExercise()->Challenge(seq);

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmTanHe(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	Guild * g = role->GetGuild();
	if (g != NULL)
	{
		g->GmTanHe();
	}

	return MakeupResult(netid, command->type, false, "");
}

bool Gm::GmTempGroupBecomeOfficial(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return MakeupResult(netid, command->type, false, "Role Not In Team");
	}

	GameName group_name;
	F_STRNCPY(group_name, result[0], sizeof(GameName));
	team->TempGroupBecomeOfficialReq(role, group_name);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuideReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int req_type = atoi(result[0]);
	int param = atoi(result[1]);

	Protocol::CSFuncGuideOperator req;
	req.op_type = req_type;
	req.param = param;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintfStack(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	std::string s = PrintStackBackTrace(false);
	printf("Gm::GmPrintfStack : \n%s\n", s.c_str());

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestPutList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);

	switch (type)
	{
	case 0:
		{
			ItemConfigData item_list[50];
			for (int i = 0; i < 50; ++i)
			{
				item_list[i].item_id = 29005;
				item_list[i].num = 1;
				item_list[i].is_bind = 1;
			}
			role->GetRoleModuleManager()->GetKnapsack()->PutList(ARRAY_ITEM_COUNT(item_list), item_list, 0);
		}
		break;

	case 1:
		{
			ItemConfigData item_list[100];
			for (int i = 0; i < 50; ++i)
			{
				item_list[i].item_id = 29005;
				item_list[i].num = 1;
				item_list[i].is_bind = 1;
			}

			for (int i = 50; i < 100; ++i)
			{
				item_list[i].item_id = 4851;
				item_list[i].num = 1;
				item_list[i].is_bind = 1;
			}

			role->GetRoleModuleManager()->GetKnapsack()->PutList(ARRAY_ITEM_COUNT(item_list), item_list, 0);
		}
		break;

	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddCredit(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int credit_num = atoi(result[0]);

	role->GetRoleModuleManager()->GetTradeCredit()->AddCredit(credit_num, __FUNCTION__);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRAbonusReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Protocol::CSRandActivityBonusOperaReq req;
	req.rand_activity_type = atoi(result[0]);
	req.opera_type = atoi(result[1]);
	req.param_1 = atoi(result[2]);

	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityBonusOpReq(&req);

	return MakeupResult(netid, command->type, true, "");
	return false;
}

bool Gm::GmAddGoodGiftTurnTableNum(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int num = atoi(result[0]);

	RoleActivityGoodGiftTurn * good_gift = static_cast<RoleActivityGoodGiftTurn *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN));
	if (NULL != good_gift)
	{
		good_gift->GmAddTurnTableNum(num);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMWorldQuestionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);

	switch (type)
	{
	case 0:
		WorldStatus::Instance().SendWorldAnswerInfo(role);
		break;
	case 1://直接刷新下一题
		{
			role->GetRoleModuleManager()->GetCommonData().world_ans = 0;
			role->GetRoleModuleManager()->GetCommonData().world_ans_tamp = 0;
			role->GetRoleModuleManager()->GetCommonData().world_answer_idx = 0;
		}
	case 2://直接开启活动
	case 3://结束活动
		WorldStatus::Instance().SetWorldQuestionStatus(type);
		break;
	default:
		break;
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGetMerchandiseNum(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int big_type = atoi(result[0]);
	int is_announce = atoi(result[1]);

	TradeMarketManager::Instance().GmGetMerchandiseNum(big_type, (0 != is_announce));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCrossSingleChat(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	std::string content(result[1]);
	int totol_len = sizeof(Protocol::CSSingleChat) + content.size();
	Protocol::CSSingleChat sc;
	sc.content_type = 0;
	sc.msg_length = (int)content.size();
	sc.to_uid = atoi(result[0]);
	char msg_buf[1024 + sizeof(Protocol::CSSingleChat)];
	memcpy(msg_buf, &sc, sizeof(Protocol::CSSingleChat));
	STRNCPY(&msg_buf[sizeof(Protocol::CSSingleChat)], content.c_str(), content.size());
	ChatManager::Instance().OnSingleChat(role, msg_buf, totol_len, true);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRandActivityInfo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int activity_type = atoi(result[0]);
	RandActivity *activity = ramgr->ForceGetRandActivity(activity_type);
	if (NULL == activity)
	{
		return false;
	}

	bool is_open = ramgr->IsRandActivityOpen(activity_type);
	char dst_str[128];
	int length = 0;
	length = SNPRINTF(dst_str, sizeof(dst_str), "activity_%d is %s, open_time:%d, end_time:%d, now_time:%d", activity_type, is_open ? "is open" : "is not open", activity->GetBeginTime(), activity->GetEndTime(), EngineAdapter::Instance().Time());
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), dst_str, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMyDraw(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = 0;;
	int param_1 = 0;;
	int param_2 = 0;;
	DrawManager::Instance().OnReq(role, type, param_1, param_2);

	DrawGroupInfo temp_info;
	temp_info.draw_count = 2;
	temp_info.draw_info_list[0].money_type[0] = 100;
	temp_info.draw_info_list[0].role_uid = role->GetUID();
	temp_info.draw_info_list[0].item_list.push_back(ItemDataWrapper(26006, 1, 1));

	temp_info.draw_info_list[1].money_type[0] = 100;
	temp_info.draw_info_list[1].role_uid = role->GetUID() + 1;
	temp_info.draw_info_list[1].item_list.push_back(ItemDataWrapper(26006, 1, 1));


	DrawManager::Instance().ShuffDrawGroupByUid(temp_info);
	DrawManager::Instance().AddDrawGroup(temp_info, 0);

	return true;
}

bool Gm::GmGetSkillGoBackRetCoin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int skill_id = atoi(result[0]);
	int skill_level = atoi(result[1]);
	int cur_level_exp = atoi(result[2]);
	int go_back_level = atoi(result[3]);
	bool is_judge_prof_skill = (atoi(result[4]) != 0) ? true : false;
	int prof = atoi(result[5]);

	//int back_money = SkillPool::Instance().GetSkillGoBackRetCoin(skill_id, skill_level, cur_level_exp, go_back_level, is_judge_prof_skill, prof);
	//printf("skill_id[%d] skill_level[%d] cur_level_exp[%d] go_back_level[%d] is_judge_prof_skill[%d] prof[%d] back_money[%d]!\n",
	//	skill_id, skill_level, cur_level_exp, go_back_level, is_judge_prof_skill ? 1 : 0, prof, back_money);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetMazeRaceActivityRankTimestamp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	MazeRacerManager::Instance().GmSetActivityRankTimestamp();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestTeamFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Team* team = role->GetMyTeam();
	if (NULL == team || team->GetLeaderUID() != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return MakeupResult(netid, command->type, false, "");
	}

	int pos_id_list[MAX_TEAM_MEMBER_NUM] = { 0 };
	team->GetPosIdList(pos_id_list, MAX_TEAM_MEMBER_NUM, false);

	Protocol::CSFormationSave req;
	req.team_member_num = 0;

	for (int i = 1; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		if (pos_id_list[i] != 0)
		{
			req.member_list[req.team_member_num].member_uid = pos_id_list[i];
			req.member_list[req.team_member_num].member_pos = (i + 1) % MAX_TEAM_MEMBER_NUM;
			req.member_list[req.team_member_num].reserve_sh = 0;
			if (0 == req.member_list[req.team_member_num].member_pos)
			{
				req.member_list[req.team_member_num].member_pos += 1;
			}
			req.team_member_num += 1;
		}
	}

	team->SaveFormationReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmfortunemazeTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int times = atoi(result[0]);

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->GMAddTimes(times);
		return MakeupResult(netid, command->type, true, "");
	}
	return MakeupResult(netid, command->type, false, "");
}

bool Gm::GmChatGroupHandOver(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	long long group_id = atoll(result[0]);
	int new_leader_uid = atoi(result[1]);

	Protocol::CSHandoverChatGroupLeadership req;
	req.group_id = group_id;
	req.new_leader_uid = new_leader_uid;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmChatGroupHandOver2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int new_leader_uid = atoi(result[0]);
	long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[0];

	Protocol::CSHandoverChatGroupLeadership req;
	req.group_id = group_id;
	req.new_leader_uid = new_leader_uid;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFetchActiveSystemExtraReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	Protocol::CSActiveSystemExtraRewardFetch req;
	req.active_type = type;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetColosseumRefreshTampNow(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	unsigned int tamp = (unsigned int)EngineAdapter::Instance().Time();
	WorldStatus::Instance().SetColosseumRefreshTamp(tamp);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetDreamNotesChallengeLevel(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int level = atoi(result[0]);
	if (level < 0)return MakeupResult(netid, command->type, true, "");
	role->GetRoleModuleManager()->GetDreamNotesChallenge()->GmSetPassLevel(level);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetRealmBreak(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	if (param1 < 0)return MakeupResult(netid, command->type, true, "");

	role->GetRoleModuleManager()->GetRealmBreak()->GmSetInfo(op_type, param1);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPetMonopoly(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int opera_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);
	Protocol::CSPetMonopolyOperate req;
	req.operate_type = opera_type;
	req.param1 = param1;
	req.param2 = param2;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRefreshLunHuiWangChuanNow(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	unsigned int tamp = atoi((result[1]));

	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetLunHuiWangChuan()->RefreshNow();
		break;
	case 1:
		{
			if (CrossConfig::Instance().IsHiddenServer())
			{
				WorldStatus::Instance().SetLunHuiWangChuanRefreshTamp(tamp);//跨服轮回忘川刷新时间修改
			}
		}
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCloudArena(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int opera_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	Protocol::CSCloudArenaOperate req;
	req.operate_type = opera_type;
	req.param1 = param1;
	req.param2 = param2;

	MessageHandler handler;
	handler.Init(World::GetInstWorld()->GetSceneManager());
	handler.HandleMessageFromClient(netid, scene, role, obj_id, (const char*)&req, sizeof(req));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRefreshShanHai(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	ShanHaiBossManager::Instance().GmRefreshShanHai();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFixDice(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int fix_dice = atoi(result[0]);
	role->GetRoleModuleManager()->GetPetMonopoly()->GmRollDice(fix_dice);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmJumpToLayer(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int layer = atoi(result[0]);
	role->GetRoleModuleManager()->GetPetMonopoly()->GmJumpToLayer(layer);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddCloudArenaTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int add_times = atoi(result[0]);
	CloudArena::Instance().GmAddChallengeTimes(role, add_times);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddCloudArenaScore(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int add_score = atoi(result[0]);
	CloudArena::Instance().GmAddScore(role, add_score);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRefreshShanHaiChallenge(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GmResetShanHaiDailyChallenge();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmActiveAllFabao(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetFaBao()->GmActiveAllFabao();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMRobottry(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{

	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int robot_type = atoi(result[0]);
	//int x = atoi(result[1]);
	//int y = atoi(result[2]);
	//	return MakeupResult(netid, command->type, true, "");


	RobotParam robot_param;
	robot_param.robot_type = robot_type;

	switch (robot_type)
	{
	case ROBOT_TYPE_MISSION_ROBOT:
		{
			const MissionRobotCreatCfg& cfg_info = ROBOTCONFIG->GetMissionCreatCfg();

			UNSTD_STATIC_CHECK(sizeof(RobotAIMission::InitParam) <= sizeof(robot_param.robot_ai_param));
			RobotAIMission::InitParam * ai_init_param = (RobotAIMission::InitParam *)(&robot_param.robot_ai_param);
			ai_init_param->m_last_task_id = cfg_info.final_task_id;
			ai_init_param->m_cur_task_id = TASKPOOL->GetRobotMissionBeginTask();
			ai_init_param->m_force_next_task_status = EngineAdapter::Instance().Time() + RobotAIMission::TASK_STATUS_OUT_TIME;

			ai_init_param->m_task_status = RobotAIMission::TASK_STATUS_INVALID;
			ai_init_param->m_task_show_action = RobotAIMission::TASK_SHOW_ACTION_NONE;
			ai_init_param->m_task_show_action_param = 0;
			ai_init_param->m_action_target_posi = Posi(-1, -1);
		}
	}
	if (!RobotManager::Instance().ConstructRobotParamByLevel(1, robot_type, true, &robot_param))
	{
		return true;
	}

	Posi creat_posi(role->GetPos());

	Scene * target_scene = World::GetInstWorld()->GetSceneManager()
		->GetSceneById(role->GetScene()->GetSceneID(), role->GetScene()->GetSceneKey());
	if (NULL == target_scene)
	{
		return true;
	}

	Robot* robot = RobotManager::Instance().CreateRobotToScene(target_scene, creat_posi, robot_param, __FUNCTION__);
	////
	return true;
	MoveAssist* move_assist = robot->GetMoveAssist(true);
	Posi targrt(10, 10);
	int ma_type = 0;
	move_assist->SetDestination(targrt, ma_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMResetCloudArenaRefreshCD(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	CloudArena::Instance().GmClearRefreshCD(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMCloudArenaRandomFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	CloudArena::Instance().GmRandomSetFormation(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMRandomPetHelperFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int system_type = atoi(result[0]);
	role->GetRoleModuleManager()->GetPetHelperFormation()->GmRandomFormation(system_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetSmartMountsSkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int smart_mounts_index = atoi(result[0]);
	int skill_index = atoi(result[1]);
	int skill_id = atoi(result[2]);

	role->GetRoleModuleManager()->GetSmartMountsSystem()->GmSetBaseSkill(smart_mounts_index, skill_index, skill_id);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestCloudArenaTitle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int last_season_rank = atoi(result[0]);
	int expire_seconds = atoi(result[1]);
	unsigned int expire_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + expire_seconds);

	role->GetRoleModuleManager()->CalculateCloudArenaTitle(last_season_rank, expire_timestamp);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCheckUniqueID(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long unique_id = atoll(result[0]);
	UniqueIDUnion u;
	u.unique_id = unique_id;

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestChivalrous(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	if (type == 0)
	{
		TeamManager::Instance().QueryUnfullChivalrousTeamList(role);
	}
	else if (type == 1)
	{
		role->GetRoleModuleManager()->GetSalary()->OnFetchHistoryChivalrousReward(1);
	}
	else if (type == 2)
	{
		crossgameprotocal::GameCrossQueryChivalrousRankListTop100Req rank_req;
		rank_req.plat_type = LocalConfig::Instance().GetPlatType();
		rank_req.server_id = LocalConfig::Instance().GetMergeServerId();
		rank_req.uid = role->GetUID();
		InternalComm::Instance().SendToCross((const char*)&rank_req, sizeof(rank_req));
	}
	else if (type == 3)
	{
		RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_CHIVALROUS);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetWildBossRewardTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int times = atoi(result[0]);
	role->SetWildBossRewardTimes(times);
	WildBossManager::Instance().SendInfoToRole(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetFaBaoTranf(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int tranf = atoi(result[0]);
	role->GetRoleModuleManager()->GetFaBao()->GMsetTranf(tranf);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestLoginBroadcast(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (result[0] == NULL)
	{
		return MakeupResult(netid, command->type, true, "");
	}

	int type = atoi(result[0]);

	if (type == 1)
	{
		role->OnLoginBroadCast(true);
	}

	if (type == 2)
	{
		Guild * guild = GuildManager::Instance().GetGuild(role->GetUserId());
		if (guild)
		{
			guild->OnLoginBroadcast(role, true);
		}
	}


	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetSmartMountsEnergy(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char* result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int index = atoi(result[0]);
	int energy = atoi(result[1]);

	role->GetRoleModuleManager()->GetSmartMountsSystem()->GmSetEnergy(index, energy);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCheckItemWrongColumn(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, Protocol::CSGMCommand* command)
{
	char* result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetKnapsack()->GmCheckItemInWrongColumn();

	return MakeupResult(netid, command->type, true, "");
}


bool Gm::GmGetGuildActiveReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	crossgameprotocal::GmGameCrossGuildActiveTest test;
	InternalComm::Instance().SendToCross((const char *)&test, sizeof(test));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGetGuildActivePersonRank(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->SendGuildActivePersonRank(role);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGetXunBaoInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param1 = atoi(result[1]);

	if (type == 1)
	{
		XunBaoDrawCfg * draw_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetXunBaoDrawCfg(param1);
		if (draw_cfg == NULL)
		{
			return false;
		}

		RoleCommonSaveXunBao *  info = role->GetRoleModuleManager()->GetXunBaoInfo(param1);
		if (info)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "xunbao_id:%d  xiyou_bao_di_times:%d free_time:%ud",
				param1, info->xi_you_floor_curr_times, info->free_time);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				if (SceneManager::CreateSystemMsg(&sm, &length, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&sm, length);
				}
			}
		}
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetPartnerTranf(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	char param = atoi(result[1]);

	switch (type)
	{
	case 1:
		role->GetRoleModuleManager()->GetPartner()->GmSetTranf(param);
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}


bool Gm::GmGuildActiveAddDay(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Guild *guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_NO_GUILD);
		return MakeupResult(netid, command->type, false, "");
	}

	guild->OnDayChange(0, 1);
	role->OnDayChange(0, 1);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddGuildActive(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int guild_active_val = atoi(result[0]);

	Guild *guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_NO_GUILD);
		return MakeupResult(netid, command->type, false, "");
	}

	guild->AddActiveVal(role->GetUID(), guild_active_val);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMQueryPetHelperFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int system_type = atoi(result[0]);
	role->GetRoleModuleManager()->GetPetHelperFormation()->OnRoleQueryFormation(system_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCourageChallengeOpenUI(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetCourageChallenge()->OnRoleOpenUI();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHongBaoTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int req_type = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);

	Protocol::CSHongBaoClientReq req;
	req.req_type = req_type;
	req.param1 = param1;
	req.param2 = param2;
	memcpy(req.greeting, "HONGBAO_GREETING_TEST", 22);

	HongBaoManager::Instance().OnClientReq(role, &req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetZhuanShuLiBaoDayGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
	if (NULL != role_activity)
	{
		role_activity->GmSetNextDayTimestamp();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMWeekChangeWorldStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	World::GetInstWorld()->OnWeekChange();
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrestigeTaskTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param1 = atoi(result[1]);

	switch (type)
	{
	case 0:
		{
			int village_id = param1;
			role->GetRoleModuleManager()->GetPrestigeTask()->OnSelectVillageReq(village_id);
		}
		break;

	case 1:
		{
			int cur_task_id = role->GetRoleModuleManager()->GetPrestigeTask()->GetCurTaskID();
			role->GetRoleModuleManager()->GetPrestigeTask()->OnTaskCommit(cur_task_id);
		}
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGiveGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = 0;
	int role_id = 0;
	if (result[0])
	{
		type = atoi(result[0]);
	}

	if (result[1])
	{
		role_id = atoi(result[1]);
	}

	if (type == 0)
	{
		char str[COMMON_SAVE_GIVE_GIFT_STR_NUM] = "aaaaaaaaaa";
		role->GetRoleModuleManager()->GetRoleGiveGift()->GiveItem(role_id, 27713, 1, str);
	}

	if (type == 1)
	{
		GiveGift::Instance().GmFetchItem(role->GetUID());
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQiongLouYuYanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);
	int param_3 = atoi(result[3]);
	int param_4 = atoi(result[4]);
	int param_5 = atoi(result[5]);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN))
		{
			RandActivityQiongLouYuYan * activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
			if (NULL != activity)
			{
				switch (type)
				{
				case 0:
					{
						Protocol::CSRandActivityOperaReq req;
						req.rand_activity_type = param_1;
						req.opera_type = param_2;
						req.param_1 = param_3;
						req.param_2 = param_4;
						req.param_3 = param_5;
						role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityOpReq(&req);
					}
					break;
				case 1:
					activity->GmAddAccumulate(param_1);
					break;
				case 2:
					activity->GmNextTamp();
					break;
				default:
					break;
				}
				return MakeupResult(netid, command->type, true, "");
			}
		}
	}
	return MakeupResult(netid, command->type, false, "");
}

bool Gm::GmSetRandActivityTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_CHAT_GROUP_CROSS_CANNOT_OPERATE);
		return false;
	}

	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (!result[0] || !result[1])
	{
		return false;
	}

	int rand_act_type = atoi(result[0]);
	int param1 = atoi(result[1]);

	if (param1 < 1)
	{
		return false;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr) return false;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return false;
	}

	RandActivity * act = ramgr->ForceGetRandActivity(rand_act_type);
	if (act == NULL)
	{
		return false;
	}

	time_t now = EngineAdapter::Instance().Time();

	time_t begin_time = ::GetZeroTime(now) - (param1 - 1) * SECOND_PER_DAY;
	act->SetRandActivityTime((unsigned int)begin_time, act->GetEndTime(), RAND_ACTIVITY_OPEN_TYPE_NORMAL, __FUNCTION__);
	act->GmNotfiyRandActivityStatus();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestHuanJieZhanChangMatch1(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	if (NULL == g_HuanJieZhanChangTester)
	{
		g_HuanJieZhanChangTester = new TesterActivityHuanJieZhanChang();
		if (NULL == g_HuanJieZhanChangTester)
		{
			return MakeupResult(netid, command->type, false, "");
		}
	}

	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int team_num = atoi(result[0]);

	g_HuanJieZhanChangTester->TestMatch(team_num);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetGodEquipErrUpLevelFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GetCommonData().god_equip_err_flag = 0;
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTianDiDaoHen(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
		return MakeupResult(netid, command->type, false, "");
	}

	TianDiDaoHen::Instance().GmNextState();

	return true;
}

bool Gm::GmAddWeekActiveDegree(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int week_active = atoi(result[0]);

	role->GetRoleModuleManager()->GetActiveSystem()->GmAddWeekActiveDegree(week_active);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddActiveTurntableTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int time = atoi(result[0]);

	role->GetRoleModuleManager()->GetActiveSystem()->GmAddTurntableTime(time);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmChangeKillValue(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int num = atoi(result[0]);

	role->GetRoleModuleManager()->GetDuel()->ChangeKillValue(num, "gm");

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPetTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int pet_index = atoi(result[0]);
	int param1 = atoi(result[1]);
	int param2 = atoi(result[2]);


	role->GetRoleModuleManager()->GetPet()->GmBreakthroughs(pet_index, param1, param2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestHuanJieZhanChangMatch2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	if (NULL == g_HuanJieZhanChangTester)
	{
		g_HuanJieZhanChangTester = new TesterActivityHuanJieZhanChang();
		if (NULL == g_HuanJieZhanChangTester)
		{
			return MakeupResult(netid, command->type, false, "");
		}
	}

	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int total_time_s = atoi(result[0]);
	int internval_s = atoi(result[1]);
	int add_team_num = atoi(result[2]);
	int match_interval_s = atoi(result[3]);

	g_HuanJieZhanChangTester->TestMatch2(total_time_s, internval_s, add_team_num, match_interval_s);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetRABackReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int type = atoi(result[0]);
	int param = atoi(result[1]);

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL == back_reward)
		return MakeupResult(netid, command->type, false, "");

	back_reward->GmSetActivity(type, param);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmConvertSkillBook(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int skill_book1_item_id = atoi(result[0]);
	int skill_book2_item_id = atoi(result[1]);

	ItemConfigData skill_book1, skill_book2;

	skill_book1.item_id = skill_book1_item_id;
	skill_book1.num = 1;

	skill_book2.item_id = skill_book2_item_id;
	skill_book2.num = 1;

	SkillBookConvert::ConvertSkillBook(role, skill_book1, skill_book2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestUseSpecialItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int special_item_id = atoi(result[0]);
	int item_param = atoi(result[1]);

	role->GetRoleModuleManager()->GetPetMonopoly()->GmUseSpecialItem(special_item_id, item_param);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMAddTianMingExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int exp = atoi(result[0]);

	role->GetRoleModuleManager()->GetTianMing()->AddExp(exp, __FUNCTION__);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmResetTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	TaskID task_id = static_cast<TaskID>(atoi(result[0]));

	role->GetRoleModuleManager()->GetTaskManager()->GmResetTask(task_id);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMResetFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int flag_type = atoi(result[0]);
	int flag_param = atoi(result[1]);

	switch (flag_type)
	{
	case 0:
		role->GetRoleModuleManager()->GetCommonData().god_turn_bag_flag = flag_param;
		break;
	case 1:
		role->GetRoleModuleManager()->GetWorkshop()->GmSetGodEquipChangeLevelFlag();
		break;
	case 2:
		role->GetRoleModuleManager()->GetWorkshop()->GmSetGodEquipFixFlag();
		break;
	case 3:
		role->GetRoleModuleManager()->GetPartner()->GmSetReturnFlag();
		break;
	case 4:
		role->GetRoleModuleManager()->GetPet()->GmSetFlag();
		break;
	case 5:
		role->GetRoleModuleManager()->GetEquipmentManager()->GmSetNewVocFlag();
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GMTestAnecdoteBigEvent(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int seq = atoi(result[0]);

	role->GetRoleModuleManager()->GetAnecdoteTask()->OnRoleFetchBigEventReward(seq);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPrintUserStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	UserCacheManager::Instance().PrintUserStatus();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGiveFlower(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int friend_id = atoi(result[0]);
	int item_id = atoi(result[1]);
	int num = atoi(result[2]);
	int is_auto = atoi(result[3]);

	char str[COMMON_SAVE_GIVE_GIFT_STR_NUM] = "give flower test";
	int ret = role->GetRoleModuleManager()->GetRoleGiveGift()->GiveFlower(friend_id, item_id, num, is_auto, str);


	return MakeupResult(netid, command->type, 0 == ret, "");
}

bool Gm::GmJieYi(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	JieYiZuManager::Instance().Gmop(role, type, p1, p2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQingYuan(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);
	QingYuanManager::Instance().Gmop(role, type, p1, p2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmResetGmAttr(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	memset(role->GetRoleModuleManager()->GetCommonData().gm_attr_list, 0, sizeof(role->GetRoleModuleManager()->GetCommonData().gm_attr_list));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestCapability(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetCapability()->CalcRoleCapability();
	int role_capability = role->GetRoleModuleManager()->GetCapability()->GetRoleCapability();

	int plain_role_capability = role->GetRoleModuleManager()->GetCapability()->GmCalcCapability();
	int fight_pet_capability = role->GetRoleModuleManager()->GetPet()->GetFightPetCapability();
	int partner_capability = role->GetRoleModuleManager()->GetPartner()->GetFightParatnerCap();
	int total_role_capability = role_capability + fight_pet_capability + partner_capability;
	int total_plain_role_cap = plain_role_capability + fight_pet_capability + partner_capability;
	gamelog::g_log_world.printf(LL_INFO, "Gm::GmTestCapability_ROLE role_cap[%d] plain_role_cap[%d] pet_cap[%d] partner_cap[%d] total_role_cap[%d] total_plain_role_cap[%d]\n",
		role_capability, plain_role_capability, fight_pet_capability, partner_capability, total_role_capability, total_plain_role_cap);

	Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(role->GetUserId());
	if (NULL == gladiator)
	{
		return MakeupResult(netid, command->type, false, "");
	}

	int gladiator_role_capability = Gladiator::CalcRoleCapability(gladiator->m_battle_data);
	int gladiator_pet_capability = 0;
	if (gladiator->m_battle_data.fight_pet_idx >= 0 && gladiator->m_battle_data.fight_pet_idx < gladiator->m_battle_data.pet_num && gladiator->m_battle_data.fight_pet_idx < ARRAY_LENGTH(gladiator->m_battle_data.pet_character_data))
	{
		// 宠物的战力计算会有所偏差，因为主角的宠物战力计算不是直接用属性来计算的
		gladiator_pet_capability += gladiator->CalcPetCapability(gladiator->m_battle_data.pet_character_data[gladiator->m_battle_data.fight_pet_idx].battle_data);
	}

	int gladiator_partner_capability = 0;
	for (int i = 0; i < gladiator->m_battle_data.partner_num && i < ARRAY_ITEM_COUNT(gladiator->m_battle_data.partner_character_data); ++i)
	{
		gladiator_partner_capability += gladiator->CalcPartnerCapability(gladiator->m_battle_data.partner_character_data[i].battle_data);
	}

	int gladiator_pethelper_capability = 0;
	for (int i = 0; i < gladiator->m_battle_data.pet_helper_num && i < ARRAY_ITEM_COUNT(gladiator->m_battle_data.pet_helper_character_data); ++i)
	{
		gladiator_pethelper_capability += gladiator->CalcPetCapability(gladiator->m_battle_data.pet_helper_character_data[i].pet_data.battle_data);
	}

	int gladiator_total_cap = gladiator_role_capability + gladiator_pet_capability + gladiator_partner_capability + gladiator_pethelper_capability;
	int gladiator_total_cap_without_pethelper = gladiator_role_capability + gladiator_pet_capability + gladiator_partner_capability;

	gamelog::g_log_world.printf(LL_INFO, "Gm::GmTestCapability_GLADIATOR role_cap[%d] pet_cap[%d] partner_cap[%d] pethelper_cap[%d] total_cap[%d] total_cap_without_pethelper[%d]\n",
		gladiator_role_capability, gladiator_pet_capability, gladiator_partner_capability, gladiator_pethelper_capability, gladiator_total_cap, gladiator_total_cap_without_pethelper);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmEndlessRankReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int type = atoi(result[0]);
	int param = atoi(result[1]);
	if (type == 0)
	{
		role->GetRoleModuleManager()->GetEndlessTower()->GMsetBattleSeq(param);
	}
	else if (type == 1)
	{
		WorldStatus::Instance().GmSendEndlessRankReward();
	}
	else if (type == 2)
	{
		WorldStatus::Instance().GmSetEndlessRankClear();
	}
	else if (type == 3)
	{
		role->GetRoleModuleManager()->GetCommonData().endless_refresh_flag = param;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmCowReportGoodNewsResetData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	CowReportGoodNewsManager::Instance().GmCowReportGoodNewsResetData();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTigerBringsBlessingResetData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	TigerBringsBlessingManager::Instance().GmTigerBringsBlessingResetData();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRetieveMoney(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int role_id = atoi(result[0]);
	int money_type = atoi(result[1]);
	int money_num = atoi(result[2]);
	int retiveve_type = atoi(result[3]);
	int retrieve_reason = atoi(result[4]);

	role->GetRoleModuleManager()->GetMoney()->GmRetieveMoney(role_id, money_type, money_num, retiveve_type, retrieve_reason);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAdvanceCareerTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int to_prof = atoi(result[0]);
	int to_color = atoi(result[1]);

	role->GetRoleModuleManager()->GetAdvanceCareer()->RoleAdvanceCareer(to_prof, to_color, false);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddFumoItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int sp_id = atoi(result[0]);
	int level = atoi(result[1]);

	const DianHuaShiOtherCfg & other_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetOtherCfg();
	ItemGridData new_item;
	new_item.Reset();
	new_item.item_id = other_cfg.dianhua_id;
	new_item.num = 1;

	NetValueItemParam new_param;
	new_param.dian_hua_shi_param.has_net_value = 1;
	new_param.dian_hua_shi_param.effect_id = sp_id;
	new_param.dian_hua_shi_param.effect_level = level;
	new_item.param = &new_param;
	if (new_item.param == NULL)return MakeupResult(netid, command->type, false, "");;
	long long item_unique_id = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
	ItemDataWrapper item_wrapper(new_item.item_id, 1, (short)new_item.is_bind, 1,
		new_item.param->data, new_item.invalid_time, 0, item_unique_id, new_item.is_market_buy_flag);

	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(item_wrapper, PUT_REASON_DIAN_HUA_SHI))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return MakeupResult(netid, command->type, false, "");;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmForbidTalk(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int forbid_seconds = atoi(result[0]);
	int talk_type = atoi(result[1]);

	role->ForbidTalk(forbid_seconds, __FUNCTION__, __FUNCTION__, talk_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSoaringSkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int skill_id = atoi(result[0]);
	int skill_lv = atoi(result[1]);

	role->GetRoleModuleManager()->GetCommonData().gm_soaring_skill_id = static_cast<UInt16>(skill_id);
	role->GetRoleModuleManager()->GetCommonData().gm_soaring_skill_lv = static_cast<short>(skill_lv);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPetFeiSheng(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	if (!result[0] || !result[1])
	{
		return false;
	}

	int req_type = atoi(result[0]);
	int p1 = atoi(result[1]);
	int p2 = atoi(result[2]);

	role->GetRoleModuleManager()->GetPet()->GmFeiSheng(req_type, p1, p2);

	return false;
}

bool Gm::GmResetGuildBossTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GetGuildCommonData().guild_boss_play_times = 0;
	Guild* guild = GuildManager::Instance().GetGuild(role->GetUserId());
	if (NULL != guild)
	{
		guild->SendGuildBossInfo(role);
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTaskFinish(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	return false;
}

bool Gm::GmWeddingNextLink(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	bool ret = false;
	if (NULL != scene)
	{
		if (Scene::SCENE_TYPE_WEDDING_FB == scene->GetSceneType())
		{
			SpecialLogicWeddingFB * wedding_special = (SpecialLogicWeddingFB *)scene->GetSpecialLogic();
			if (NULL != wedding_special)
			{
				wedding_special->GMNextLink();
				ret = true;
			}
		}
	}

	return MakeupResult(netid, command->type, ret, "");
}

bool Gm::GmWeddingTry(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = 0;
	int param_1 = 0;
	int param_2 = 0;

	int ret = WeddingManger::Instance().GMOpenWedding(role);
	std::string list_str = STRING_SPRINTF("wedding_id:%d", ret);

	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), list_str.c_str(), list_str.length(), GetMsgType(SYS_MSG_SYSTEM_CHAT));

	return MakeupResult(netid, command->type, true, list_str.c_str());
}

bool Gm::GmXunYouCar(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);

	WeddingManger::Instance().GMCreatXunYouCar(role->GetUID(), type);

	return false;
}

bool Gm::GmWorldArenaOperate(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);

	WorldArena::Instance().OnRoleOperate(role, type, param_1, param_2);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmWorldArenaControl(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	enum COMMAND
	{
		COMMAND_RESTART_SEASON = 0,
		COMMAND_NEXT_STATUS = 1,
		COMMAND_START_DAY_BATTLE = 2,
	};

	int type = atoi(result[0]);
	int param_1 = atoi(result[1]);
	int param_2 = atoi(result[2]);

	switch (type)
	{
	case COMMAND_RESTART_SEASON:
		{
			WorldArena::Instance().GmRestartSeason();
		}
		break;

	case COMMAND_NEXT_STATUS:
		{
			WorldArena::Instance().GmNextStatus();
		}
		break;

	case COMMAND_START_DAY_BATTLE:
		{
			WorldArena::Instance().GmStartBattle(param_1);
		}
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMiniGameClearNextStage(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->GetRoleModuleManager()->GetMiniGame()->OnFinishNextStageReq();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMiniGameFetchPhaseReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int phase_seq = atoi(result[0]);
	role->GetRoleModuleManager()->GetMiniGame()->OnFetchPhaseRewardReq(phase_seq);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmWorldTeamArenaSignUp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	WorldTeamArenaSignUpManager::Instance().OnGmAllTeamSignUp(role);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmWorldTeamArenaOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int p1 = atoi(result[1]);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return MakeupResult(netid, command->type, true, "");
	}
	ActivityWorldTeamArena* activity = (ActivityWorldTeamArena*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity)
	{
		return MakeupResult(netid, command->type, true, "");
	}
	switch (op_type)
	{
		case 0:
		{
			activity->GmEnterEndTimeFastforward();
		}
		break;
		case 1:
		{
			activity->GmSetSignUpTime(p1);
		}
		break;
		default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFallenGodReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);
	
	switch (type)
	{
	case 0:
		role->GetRoleModuleManager()->GetFallenGod()->StarBattle(param);
		break;
	default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFuMoUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int posi = atoi(result[0]);
	int gird = atoi(result[1]);
	int level = atoi(result[2]);

	role->GetRoleModuleManager()->GetWorkshop()->GMRandEnd(posi, gird, level);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmWorldDayChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	World::GetInstWorld()->OnDayChange(0, 1);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGuildQiFuAdd(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);
	bool ret = false;
	Guild * role_guild = role->GetGuild();
	if (NULL != role_guild)
	{
		role_guild->GmSetQiFu(role, type, param);
		ret = true;
	}

	return MakeupResult(netid, command->type, ret, __FUNCTION__);
}

bool Gm::GmStopServer(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	ServerLogic::GetInstServerLogic()->StopServer(__FUNCTION__);
	return true;
}

bool Gm::GmYaoXingSet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int type = atoi(result[0]);
	int param = atoi(result[1]);
	role->GetRoleModuleManager()->GetYaoXingShiLian()->GmSetParam(type, param);

	return MakeupResult(netid, command->type, true, __FUNCTION__);
}

bool Gm::GmTestXinFuBiPinReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr) return false;

	RandActivityServerCompetition* activity = (RandActivityServerCompetition*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	if (NULL != activity)
	{
		activity->GmTestGiveReward(role);
	}

	return MakeupResult(netid, command->type, true, __FUNCTION__);
}

bool Gm::GmAddUTAChampionRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		char dst_str[128];
		int length = 0;
		length = SNPRINTF(dst_str, sizeof(dst_str), "Please operate in hidden server.");
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), dst_str, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));

		return MakeupResult(netid, command->type, false, " Please operate in hidden server.");
	}

	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int index = atoi(result[0]);

	UTAChampionRecordManager::Instance().GmAddUTAChampionRecord(role , index);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmUTAChampionRecordReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	int begin_index = atoi(result[0]);
	int end_index = atoi(result[1]);

	Protocol::CSGetUTAChampionRecord gm_cs;
	gm_cs.begin_index = begin_index;
	gm_cs.end_index = end_index;

	UTAChampionRecordManager::Instance().OnGetUTAChampionRecord(role, &gm_cs);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmGodBeastAdventAddLike(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int add_num = atoi(result[0]);

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->GmAddLike(add_num);
		}
	}

	return MakeupResult(netid, command->type, true, __FUNCTION__);
}

bool Gm::GmSaveData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	role->Save();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmEquipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");
	short column = atoi(result[0]);
	short index = atoi(result[1]);
	short equip_index = atoi(result[2]);
	role->GetRoleModuleManager()->GetEquipmentManager()->PutOn(column, index, equip_index);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHolyEquipClear(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);

	role->GetRoleModuleManager()->GetHolyEquipManager()->GmClearBag(op_type);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::salySet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	bool ret = true;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
		if (NULL != activity)
		{
			activity->GMset();
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		ret = false;
	}

	return MakeupResult(netid, command->type, true, "");
}
bool Gm::GmKeJuExamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);
	int param1 = atoi(result[1]);

	Protocol::CSKeJuExamCSReq req;
	req.op_type = op_type;
	req.param_1 = param1;

	role->GetRoleModuleManager()->GetKeJuExam()->OnClientOpReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHallowStudySkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int skill_id = atoi(result[0]);
	int hole = atoi(result[1]);

	role->GetRoleModuleManager()->GetHolyEquipManager()->GmStudySkill(skill_id, hole);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHallowSetGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int gift_id = atoi(result[0]);
	int hole = atoi(result[1]);

	role->GetRoleModuleManager()->GetHolyEquipManager()->GmSetGift(gift_id, hole);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmKuaFuYingXiongLu(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int op_type = atoi(result[0]);

	switch (op_type)
	{	
		case 0:
		KuaFuYingXiongLuManager::Instance().OnReopen();
		break;
		case 1:
		KuaFuYingXiongLuManager::Instance().OnClose();
		break;
		case 2:
		KuaFuYingXiongLuManager::Instance().OnSetLockFlag(true);
		break;
		case 3:
		KuaFuYingXiongLuManager::Instance().OnSetLockFlag(false);
		break;
		default:
		break;
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmClearFightCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(role->GetUserId());
	if (NULL != gladiator)
	{
		gladiator->GmClearFightCount();
	}

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestElementElfPosibility(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int today_showup_times = atoi(result[0]);
	int rand_times = atoi(result[1]);

	int showup_times = 0;
	int noshow_times = 0;
	for (int i = 0; i < rand_times; ++i)
	{
		if (LOGIC_CONFIG->GetElementElfConfig()->RandElementElfShowUp(2, BATTLE_MODE_NORMAL_MONSTER, today_showup_times, NULL))
		{
			showup_times += 1;
		}
		else
		{
			noshow_times += 1;
		}
	}

	double showup_posibility = 1.0 * showup_times / (showup_times + noshow_times);
	printf("GmTestElementElfPosibility rand_times[%d] showup_times[%d] posibility[%.4f]\n", rand_times, showup_times, showup_posibility);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFuYuManagerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	Protocol::CSFuYuReq req;
	req.req_type = atoi(result[0]);
	req.param1 = atoi(result[1]);
	req.param2 = atoi(result[2]);
	req.param3 = atoi(result[3]);
	req.param4 = atoi(result[4]);
	
	role->GetRoleModuleManager()->GetFuYuManager()->OnClientReq(&req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAddFuYuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	ItemID item_id = atoi(result[0]);
	int sp_group = atoi(result[1]);

	role->GetRoleModuleManager()->GetFuYuManager()->GmPutFuYu(item_id, sp_group);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmRoleFlyUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int fly_up_level = atoi(result[0]);

	role->GetRoleModuleManager()->GetFlyUpSystem()->GmFlyUp(fly_up_level);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmPartnerFlyUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int partner_id = atoi(result[0]);
	int fly_up_level = atoi(result[1]);
	int modify_role_flyup = atoi(result[2]);

	role->GetRoleModuleManager()->GetPartner()->GmPartnerFlyUp(partner_id, fly_up_level, (modify_role_flyup == 0 ? false : true));

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmQueryUserChatRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	ChatManager::Instance().QueryUserChatRecord(role);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmWatchVideo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long file_id = atoll(result[0]);
	BattleVideoMgr::Instance().GmWatchVideo(role, file_id);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetWaiguaMoveFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	bool is_no_check = atoi(result[0]) > 0;
	role->GetRoleModuleManager()->GmSetWaiguaCheckFlag(is_no_check);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmMiniGameBattleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int seq = atoi(result[0]);
	int has_partner = atoi(result[1]);
	int has_pet = atoi(result[2]);
	int has_skill = atoi(result[3]);

	role->GetRoleModuleManager()->GetMiniGame()->OnStartMiniGameBattleReq(seq, has_pet > 0, has_partner > 0, has_skill > 0);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmStartChartReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetStarChart()->GmAllLigthUp();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmResetStartChartReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetStarChart()->GmResetStarChart();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmSetFunctionGuideFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetFuncGuide()->GmSetAllFlag();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmClearAllTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	role->GetRoleModuleManager()->GetTaskManager()->GmClearAllTask();

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmHuanShouActive(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int huanshou_id = atoi(result[0]);

	role->GetRoleModuleManager()->GetRoleHuanShou()->GmActiveHuanShou(huanshou_id);

	return MakeupResult(netid, command->type, true, "");

	return false;
}

bool Gm::GmQueryTradeItemPrice(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char * result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int column = atoi(result[0]);
	int index = atoi(result[1]);

	Protocol::CSQueryItemRecommendPrice req;
	req.is_from_pet = 0;
	req.column = column;
	req.index = index;
	req.reserve_ch = 0;
	req.reserve_sh = 0;

	TradeMarketRouter::Instance().OnQueryItemRecommendPriceReq(role, &req);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestRedisGET(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int test_times = atoi(result[0]);
	int character_num = atoi(result[1]);

	for (int i = 0; i < test_times; ++i)
	{
		std::string key = STRING_SPRINTF("GmTestRedisGET_%d", i);
		std::string value;
		for (int i = 0; i < character_num; ++i)
		{
			value += ('A' + (rand() % 26));
		}
		RedisManager::Instance().SET(key, value);
	}
	
	PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
	for (int i = 0; i < test_times; ++i)
	{
		std::string key = STRING_SPRINTF("GmTestRedisGET_%d", i);
		std::string value;
		RedisManager::Instance().GET(key, value);
	}
	PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);
	
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestRedisSET(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	int test_times = atoi(result[0]);
	int character_num = atoi(result[1]);

	PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
	for (int i = 0; i < test_times; ++i)
	{
		std::string key = STRING_SPRINTF("GmTestRedisSET_%d", i);
		std::string value;
		for (int i = 0; i < character_num; ++i)
		{
			value += ('A' + (rand() % 26));
		}

		RedisManager::Instance().SET(key, value);
	}
	PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmAsyncTaskTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	//AsyncRedisTest a;
	AsyncRedisTest* test = dynamic_cast<AsyncRedisTest*>(AsyncTaskFactoryManager::Instance().Create("AsyncRedisTest"));
	if (NULL == test)
	{
		return MakeupResult(netid, command->type, false, "");
	}
	test->role_id = role->GetUID();
	
	AsyncTaskManager::Instance().AddTask(test);

	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTryEnterUTAScene(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	UTAShadow::Instance().TryEnterBattleScene(role);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmUTAWatchVideo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long video_id = atoll(result[0]);
	UTAShadow::Instance().OnUserWatchVideoReq(role->GetUserId(), video_id);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmUTAStartOB(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	long long schedule_id = atoll(result[0]);
	UTAOBShadow::Instance().StartOB(role, schedule_id);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmFastForwardBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	BattleManagerLocal::GetInstance().RoleFastForwardBattleReq(role);
	return MakeupResult(netid, command->type, true, "");
}

bool Gm::GmTestTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command)
{
	char *result[128] = {};
	this->ParseCommandParam(command->command, result, " ");

	time_t t = static_cast<time_t>(atoll(result[0]));
	tm ttmm;
	GetTimeTM(t, ttmm);
	time_t week_zero_time = GetWeekZeroTime(t);
	tm ttmm2;
	GetTimeTM(week_zero_time, ttmm2);

	char tmp[32] = { NULL };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &ttmm);
	char tmp2[32] = { NULL };
	strftime(tmp2, sizeof(tmp2), "%Y-%m-%d %H:%M:%S", &ttmm2);
	printf("GmTestTime time1[%s] week_zero_time[%s]", tmp, tmp2);

	return MakeupResult(netid, command->type, true, "");
}
