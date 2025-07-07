
FightCrystalSweepItemView = FightCrystalSweepItemView or DeclareView("FightCrystalSweepItemView", FightConfig.ResPath .. "fight_crystal_sweep_item", Mod.FightEnergy.Main)

function FightCrystalSweepItemView:LoadCallback(param_t)
	local num = BagData.Instance:GetNumByItemId(CommonItemId.FightCrystalEnergyItem)
	local show_gold = param_t.show_gold or 0 == num
	if GLOBAL_CONFIG:Flags().block_recharge then
		PublicPopupCtrl.Instance:Center(Language.Fight.NoTiLiItem)
		ViewMgr:CloseView(FightCrystalSweepItemView)
	else
		self.GoldObj:SetActive(show_gold)
		self.MonoObj:SetActive(not show_gold)
	end
end

FightCrystalSweepItemViewFM = FightCrystalSweepItemViewFM or DeclareMono("FightCrystalSweepItemViewFM", UIWFlushPanel)

function FightCrystalSweepItemViewFM:FightCrystalSweepItemViewFM()
	self.data = FightCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Fight.Sweep

	self.data_cares = {
		{data = BagData.Instance.item_grid, func = self.FlushShow},
	}

	self.cur_num = 1
	self.all_num = 0
end

function FightCrystalSweepItemViewFM:FlushShow()
	local use_item = Item.Init(CommonItemId.FightCrystalEnergyItem)
	self.ShowCell:SetData(use_item)
	self:FlushNum()
	local strength_num,max_power = self.data:GetFightStrengthNum()
	self.EnergyValue.text = Format(Language.Fight.Sweep.desc, strength_num, max_power)
end

function FightCrystalSweepItemViewFM:FlushNum()
	local use_item = Item.Init(CommonItemId.FightCrystalEnergyItem)
	local co = Item.GetConfig(CommonItemId.FightCrystalEnergyItem)
	local energy_num = self.data:GetFightStrengthNum()
	local drug_num = BagData.Instance:GetNumByItemId(CommonItemId.FightCrystalEnergyItem)
	local extra_power_limit = self.data:GetCrystalFbSweepExtraPowerLimit()
	self.all_num = math.min(drug_num, math.ceil((extra_power_limit - energy_num) / co.param1))
	self.NumVal.text = self.cur_num
	self.Desc1.text = string.format(self.language.item_desc1, self.cur_num, use_item:Name(), math.min(self.cur_num * co.param1, extra_power_limit - energy_num))
	self.Desc2.text = string.format(self.language.item_desc2, use_item:Name(), drug_num)
end

function FightCrystalSweepItemViewFM:OnClickAdd()
	if self.cur_num < self.all_num then
		self.cur_num = self.cur_num + 1
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_item_max)
	end
end

function FightCrystalSweepItemViewFM:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_item_min)
	end
end

function FightCrystalSweepItemViewFM:OnClickMax()
	if self.cur_num < self.all_num and self.cur_num < FightConfig.SWEEP_ITEM_NUM_MAX then
		self.cur_num = self.all_num < FightConfig.SWEEP_ITEM_NUM_MAX and self.all_num or FightConfig.SWEEP_ITEM_NUM_MAX
		self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips_item_max)
	end
end

function FightCrystalSweepItemViewFM:OnClickClose()
	ViewMgr:CloseView(FightCrystalSweepItemView)
end

function FightCrystalSweepItemViewFM:OnClickConfirm()
	if not self.ViewObj.activeSelf then return end
	ViewMgr:CloseView(FightCrystalSweepItemView)
	local energy_num = self.data:GetFightStrengthNum()
	local extra_power_limit = self.data:GetCrystalFbSweepExtraPowerLimit()
	if energy_num == extra_power_limit then
		PublicPopupCtrl.Instance:Center(self.language.energy_max)
	elseif self.all_num > 0 then
		BagCtrl.Instance:SendUseSpecialLogicItem(UseSpecialLogicItem.CryStalFB, CommonItemId.FightCrystalEnergyItem, self.cur_num)
	else
		ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.FightCrystalEnergyItem})})
	end
end

function FightCrystalSweepItemViewFM:OnClickCancel()
	if not self.ViewObj.activeSelf then return end
	ViewMgr:CloseView(FightCrystalSweepItemView)
end


FightCrystalSweepItemViewFG = FightCrystalSweepItemViewFG or DeclareMono("FightCrystalSweepItemViewFG", UIWFlushPanel)

function FightCrystalSweepItemViewFG:FightCrystalSweepItemViewFG()
	self.data = FightCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Fight.Sweep

	-- self.data_cares = {
	-- 	{data = RoleData.Instance.currencys, func = self.FlushGoldValue, init_call = false},
	-- }
	self.cost_gold = 0
end

function FightCrystalSweepItemViewFG:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushShow()
end

function FightCrystalSweepItemViewFG:FlushShow()
	local buy_times = self.data.fight_crystal_sweep.buy_times
	local buy_times_max = self.data:GetSweepBuyTimes()
	local limit_type = self.data:GetSweepLimitType()
	local desc_times = string.format(self.language.limit_type[limit_type] or self.language.limit_type[1], buy_times_max - buy_times)
	
	-- local energy_value = string.format(self.language.desc, self.data.fight_crystal_sweep.role_enter_energy, self.data.crystal_config_other.max_power)
	-- energy_value = energy_value .. string.format(self.language.limit_type[limit_type] or self.language.limit_type[1], buy_times_max - buy_times)
	-- self.EnergyValue.text = energy_value
	
	buy_times = buy_times + 1
	buy_times = math.min(buy_times, buy_times_max)
	local co = Cfg.CrystalBuyPowerByBuyTimes(buy_times)
	self.cost_gold = co.price
	self.Content.text = string.format(self.language.buy_energy_tips, co.price, co.get_power) .. desc_times

	-- self:FlushGoldValue()
end

-- function FightCrystalSweepItemViewFG:FlushGoldValue()
	-- self.GoldValue.text = RoleData.Instance:GetCurrency(CurrencyType.Gold)
-- end

function FightCrystalSweepItemViewFG:OnClickConfirm()
	if not self.ViewObj.activeSelf then return end
	local buy_times = self.data.fight_crystal_sweep.buy_times
	local buy_times_max = self.data:GetSweepBuyTimes()
	if buy_times_max - buy_times <= 0 then
		PublicPopupCtrl.Instance:Center(self.language.num_not_enough)
		return
	end
	ViewMgr:CloseView(FightCrystalSweepItemView)
	local energy_num = self.data:GetFightStrengthNum()
	local extra_power_limit = self.data:GetCrystalFbSweepExtraPowerLimit()
	if energy_num == extra_power_limit then
		PublicPopupCtrl.Instance:Center(self.language.energy_max)
	elseif RoleData.Instance:GetCurrency(CurrencyType.Gold) >= self.cost_gold then
		FightCtrl.Instance:SendCrystalFbReq(2)
	else
		local info = {
			content = Language.Mall.NotCurrency[3],
			close = true,
			cancel = {
				name = Language.Mall.NotCurrency[4],
				func = function()
					TradeData.Instance:SetJumpItemData(Item.Init())
					ViewMgr:OpenViewByKey(Mod.Mall.Trade)
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			},
			confirm = {
				name = Language.Mall.NotCurrency[5],
				func = function()
					ViewMgr:OpenViewByKey(Mod.Store.Recharge)
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			},
		}
		if GLOBAL_CONFIG:Flags().block_recharge then
			info.no_confirm = true
		end
		PublicPopupCtrl.Instance:DialogTips(info)
	end
end

function FightCrystalSweepItemViewFG:OnClickCancel()
	if not self.ViewObj.activeSelf then return end
	ViewMgr:CloseView(FightCrystalSweepItemView)
end