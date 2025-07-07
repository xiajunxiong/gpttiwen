
FightCrystalMenuView = FightCrystalMenuView or DeclareMono("FightCrystalMenuView", UIWFlushPanel)

function FightCrystalMenuView:FightCrystalMenuView()
	self.data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetFightCrystalInfo(), func = self.FlushMenu},
	}
	self.language = Language.Fight.CrystalDungen
end

function FightCrystalMenuView:FlushMenu()
	local fight_crystal_info = self.data:GetFightCrystalInfo()
	local cur_seq_info = self.data:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
	self.Layer.text = string.format(self.language.progress, fight_crystal_info.layer, cur_seq_info.layer)

	self.MonsterNum.text = fight_crystal_info.monster_num
	self.TreasureBoxNum.text = fight_crystal_info.box_num

	local npc_c = Cfg.NpcById(cur_seq_info.boss_id)
	if self:CheckConfig(npc_c) then return end
	local group_c = Cfg.MonsterGroupById(npc_c.param1)
	if self:CheckConfig(group_c) then return end
	local monster_c = Cfg.MonsterById(group_c.show_monster)
	if self:CheckConfig(monster_c) then return end
	self.Goal.text = string.format(self.language.goal, monster_c.name)
end

function FightCrystalMenuView:OnClickLeave()
	PublicPopupCtrl.Instance:DialogTips{
		content = Language.Chat.BackTips,
		confirm = {
			func = function()
				SceneCtrl.Instance:RequestLeaveFb()
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end

function FightCrystalMenuView:CheckConfig(config)
	if nil == config then
		self.Goal.text = self.language.goal2
		Debuger.LogError("配置出错找兆杰")
		return true
	end
	return false
end

function FightCrystalMenuView:OnClickItem(item_id)
	local info = {
		item_list = {Item.Init(item_id)},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end