
AnimalRaceRuleView = AnimalRaceRuleView or DeclareView("AnimalRaceRuleView", AnimalRaceConfig.ResPath .. "animal_race_rule")
VIEW_DECLARE_MASK(AnimalRaceRuleView, ViewMask.BgBlockCloseNoAnim)

function AnimalRaceRuleView:LoadCallback(param_t)
	self.Content.text = param_t.desc or Config.language_cfg_auto.textdesc[61].desc
end

AnimalRaceSupportView = AnimalRaceSupportView or DeclareView("AnimalRaceSupportView", AnimalRaceConfig.ResPath .. "animal_race_support")
VIEW_DECLARE_MASK(AnimalRaceSupportView, ViewMask.BgBlockNoAnim)

function AnimalRaceSupportView:AnimalRaceSupportView()
	self.data = AnimalRaceCtrl.Instance:Data()
	self.language = Language.AnimalRace.Support
end

function AnimalRaceSupportView:LoadCallback()
	local player = self.data.cur_sel_player

	self.max_num = Item.GetNum(AnimalRaceConfig.AnimalCoin)
	self.cur_num = 1

	UH.SetIcon(self.IconSp, player.picture_id)
	self.HaveNum.text = self.max_num
end

function AnimalRaceSupportView:OnFlush()
	self.CurNum.text = self.cur_num
end

function AnimalRaceSupportView:OnClickClose()
	ViewMgr:CloseView(AnimalRaceSupportView)
end

function AnimalRaceSupportView:OnClickConfirm()
	AnimalRaceCtrl.Instance:SendAnimalRaceOperBet(self.data.cur_sel_player.id, self.cur_num)
	ViewMgr:CloseView(AnimalRaceSupportView)
end

function AnimalRaceSupportView:OnClickCancel()
	ViewMgr:CloseView(AnimalRaceSupportView)
end

function AnimalRaceSupportView:OnClickAdd()
	if self.max_num > self.cur_num then
		self.cur_num = self.cur_num + 1
		ViewMgr:FlushView(AnimalRaceSupportView)
	else
		PublicPopupCtrl.Instance:Center(self.language.MaxNum)
	end
end

function AnimalRaceSupportView:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		ViewMgr:FlushView(AnimalRaceSupportView)
	else
		PublicPopupCtrl.Instance:Center(self.language.MinNum)
	end
end

AnimalRaceSupportViewF = AnimalRaceSupportViewF or DeclareMono("AnimalRaceSupportViewF", UIWFlushPanel)

function AnimalRaceSupportViewF:AnimalRaceSupportViewF()
	self.data = AnimalRaceCtrl.Instance:Data()
	self.language = Language.AnimalRace.Support

	self.data_cares = {
		{data = self.data.detail_info, func = self.FlushSupport},
	}
end

function AnimalRaceSupportViewF:FlushSupport()
	local detail_info = self.data.detail_info
	local player = self.data.cur_sel_player
	local num = self.data:GetBetTypeCountById(player.id)
	self.Tips.text = string.format(self.language.Tips, player.name, num)
end