
FightSweepRewardView = FightSweepRewardView or DeclareView("FightSweepRewardView", FightConfig.ResPath .. "fight_sweep_reward")
VIEW_DECLARE_MASK(FightSweepRewardView,ViewMask.BgBlockCloseNoAnim)

function FightSweepRewardView:FightSweepRewardView()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight.Sweep
end

function FightSweepRewardView:LoadCallback()
	local reward_info = self.data:GetFightSweepRewardInfo()
	self.SweepNum.text = string.format(self.language.sweep_num, reward_info.sweep_num)
	self.ExpNum.text = reward_info.exp_num
	self.ItemSNum.text = reward_info.item_s1_num
	self.Rewards:SetData(reward_info.rewards)
end

FightCrystalRewardShowItem = FightCrystalRewardShowItem or DeclareMono("FightCrystalRewardShowItem")

function FightCrystalRewardShowItem:FightCrystalRewardShowItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightCrystalRewardShowItem:SetDataBySeq(data)
	local exp_num = 0
	local item_s1_num = 0
	local dungeon_info = self.fight_data:GetDungeonInfoBySceneSeq(data.seq)
	if dungeon_info then
		item_s1_num = dungeon_info.item_s1_num
		-- exp_num = dungeon_info.exp_num
	end
	local exp_num = self.fight_data:GetFightCrystalExpNum()
	self.ExpNum.text = exp_num
	self.ItemSNum.text = item_s1_num
	self.ExpNum:SetObjActive(exp_num > 0)
end