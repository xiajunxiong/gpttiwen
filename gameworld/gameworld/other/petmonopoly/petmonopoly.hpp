#ifndef __PET_MONOPOLY_HPP__
#define __PET_MONOPOLY_HPP__

#include "servercommon/petmonopolydef.hpp"


class RoleModuleManager;
struct PetMonopolyLayerGridCfg;
class PetMonopoly
{
public:
	PetMonopoly();
	~PetMonopoly();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const NewPetMonopolyParam& new_param, const PetMonopolyParam& param);
	void GetInitParam(ARG_OUT NewPetMonopolyParam* param);

	void OnOperateReq(int operate_type, int param1, int param2);
	void OnBattleFinish();
	void AddPetToRecord(const ItemDataWrapper& item_wrapper);
	void AddRewardToRecord(const ItemConfigData& item);
	int GetCurLevel() { return m_param.cur_level; }
	bool DoublePetReward();

	void GmRollDice(int fix_dice);
	void GmJumpToLayer(int layer);
	void GmUseSpecialItem(int special_item_id, int item_param);

	void SendInfo();
	void SendSpecialInfo( );
private:
	void SendRewardRecordInfo();
	void SendExpectPetChangeNotice();
	void SendCurGridChangeNotice();
	void SendCurGridChangeNotice(int cur_grid_idx);
	void SendEnterNextLevelNotice(int next_level);
	void SendTurnTableRewards();
	void SendTurnTableResult();
	void SendSpecialItemNum(int item_type);
	void SendFastMoveReward(int grid_index);

	void SetExpectPet(int pet_id);
	void ClearExpectPet(bool is_gold);
	void RollDice(int fix_dice = 0);
	bool TryFixDiceForCatchPetGuide(ARG_OUT int* fix_dice);
	void FetchGridReward();
	void StartBattle();
	void GenerateGridData(int level);
	bool FillGridData(const PetMonopolyLayerGridCfg& cfg, PetMonopolyGrid& grid, int expect_silver_pet_id, int expect_gold_pet_id);
	void CheckGridVersion();
		
	void EnterNextLevel();
	bool GenerateTurnTableRewards();
	void ShowTurnTableRewards();
	void ClearTurnTableData();
	void ShowTurnTableResult();
	void TurnTableEnd();
	void AddExp();

	void UseSpecialItem(int special_item_id,int item_param = 0);
	void SpecialItemConsume(int special_item_type);
	void UseLubanDice(int fix_dice);
	void UseFastMovePaper();
	void UseWishCard(int wish_item_seq);
	void UsePortal();
	void UseDoubleCard();
	void DoubleReward(int& item_num);
	bool CheckFastMove(int grid_index);
	void FastMoveConsume(int grid_index);


	NewPetMonopolyParam m_param;
	RoleModuleManager* m_mgr;
	
};

#endif