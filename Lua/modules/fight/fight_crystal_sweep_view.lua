
FightCrystalSweepView = FightCrystalSweepView or DeclareView("FightCrystalSweepView", FightConfig.ResPath .. "fight_crystal_sweep")

FightCrystalSweepViewF = FightCrystalSweepViewF or DeclareMono("FightCrystalSweepViewF", UIWFlushPanel)

function FightCrystalSweepViewF:FightCrystalSweepViewF()
	self.data = FightCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Fight.Sweep

	self.data_cares = {
		{data = self.data.fight_crystal_sweep, func = self.FlushShow},
	}

	self.cur_num = 1
	self.stone_num = 0
	self.energy_num = 0
	self.all_num = 0
end

function FightCrystalSweepViewF:FlushShow()
	local cur_sel_dungeon = self.data.cur_sel_dungeon
	local energy_num = self.data:GetFightStrengthNum()
	self.stone_num = BagData.Instance:GetNumByItemId(cur_sel_dungeon.consume.item_id)
	UH.SetIcon(self.IconStone, cur_sel_dungeon.consume:IconId())
	self.energy_num = energy_num
	self.all_num = self.stone_num + math.floor(energy_num / self.data:GetCrystalFbSweepUsePower())
	self:FlushNum()
end

function FightCrystalSweepViewF:FlushNum()
	self.NumVal.text = self.cur_num
	self.StoneVal.text = string.format(self.language.num_f, self.cur_num < self.stone_num and self.cur_num or self.stone_num, self.stone_num)
	self.EnergyVal.text = string.format(self.language.num_f, (self.cur_num < self.stone_num and 0 or self.cur_num - self.stone_num) * self.data:GetCrystalFbSweepUsePower(), self.energy_num)
end

function FightCrystalSweepViewF:OnClickAdd()
	if self.cur_num < self.all_num and self.cur_num < FightConfig.SWEEP_NUM_MAX then
		self.cur_num = self.cur_num + 1
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_max)
	end
end

function FightCrystalSweepViewF:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_min)
	end
end

function FightCrystalSweepViewF:OnClickMax()
	if self.cur_num < self.all_num and self.cur_num < FightConfig.SWEEP_NUM_MAX then
		self.cur_num = self.all_num < FightConfig.SWEEP_NUM_MAX and self.all_num or FightConfig.SWEEP_NUM_MAX
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_max)
	end
end

function FightCrystalSweepViewF:OnClickClose()
	ViewMgr:CloseView(FightCrystalSweepView)
end

function FightCrystalSweepViewF:OnClickConfirm()
	FightCtrl.Instance:SendCrystalFbFastPassInfo(self.data.cur_sel_dungeon.scene_seq, self.cur_num)
	ViewMgr:CloseView(FightCrystalSweepView)
end

function FightCrystalSweepViewF:OnClickCancel()
	ViewMgr:CloseView(FightCrystalSweepView)
end