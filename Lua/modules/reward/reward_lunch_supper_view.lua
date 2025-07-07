RewardLunchSupperView = RewardLunchSupperView or DeclareMono("RewardLunchSupperView", UIWFlushPanel)
function RewardLunchSupperView:RewardLunchSupperView()
    self.data = RewardData.Instance
    self.data_cares = {
        {data = self.data.day_food_info, func = self.OnFlush,  init_call = true},
    }
end

function RewardLunchSupperView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	if self.timer ~= nil then
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end 
end

function RewardLunchSupperView:SetTimerFlush()
	if self.timer == nil then
		self.timer = TimeHelper:AddRunTimer(function ()
				self:FlushOpenOrClose()
		end, 1)	
	end
end

function RewardLunchSupperView:OnFlush()
	self:SetTimerFlush()
	self:FlushTime()
	self:FlushTili()
	self:FlushOpenOrClose()
end

function RewardLunchSupperView:FlushTime()
	local after,night = self.data:GetAfterAndNightTime()
	UH.SetText(self.AfterTime,after)
	UH.SetText(self.NightTime,night)
end

function RewardLunchSupperView:FlushTili()
	local after_info 
	local night_info
	for k,v in pairs(Config.day_reward_auto.day_food) do
		if v.seq == 0 then
			after_info = v
		else
			night_info = v
		end
	end
	UH.SetText(self.AfterTili,string.format(Language.LunchSupper.TiliStr,after_info.food_num))
	UH.SetText(self.NightTili,string.format(Language.LunchSupper.TiliStr,night_info.food_num))
end

function RewardLunchSupperView:FlushOpenOrClose()
	local after_seq = 0
	local night_seq = 1
	local after_info 
	local night_info
	for k,v in pairs(Config.day_reward_auto.day_food) do
		if v.seq == 0 then
			after_info = v
		else
			night_info = v
		end
	end
	local after_flag = self.data:GetTimeFlag(after_seq)
	local after_reward = self.data:GetRewardFlag(after_seq)
	self.AfterButtonTextObj:SetActive(true)
	self.AfterButtonBuLingObj:SetActive(false)
	if after_flag ~= 0 then
		self.AfterOpenObj:SetActive(true)
		self.AfterCloseObj:SetActive(false)
	else
		self.AfterOpenObj:SetActive(false)
		self.AfterCloseObj:SetActive(true)
	end
	if after_reward then
		UH.SetText(self.AfterButtonText,Language.LunchSupper.HaveGetStr)
		self.AfterHandler.Interactable = false
		self.AfterGetObj:SetActive(false)
		self.AfterGetEffetObj:SetActive(false)
	else
		if after_flag == 0 then
			UH.SetText(self.AfterButtonText,Language.LunchSupper.NotTimeStr)
			self.AfterHandler.Interactable = false
			self.AfterGetObj:SetActive(false)
			self.AfterGetEffetObj:SetActive(false)
		elseif after_flag == 1 then
			UH.SetText(self.AfterButtonText,Language.LunchSupper.GetStr)
			self.AfterHandler.Interactable = true
			self.AfterOpenObj:SetActive(false)
			self.AfterCloseObj:SetActive(false)
			self.AfterGetObj:SetActive(true)
			self.AfterGetEffetObj:SetActive(true)
			self.AfterRed:SetNum(1)
			UH.SpriteName(self.AfterSpriteSet,"LvAnNiu_PuTong")
		elseif after_flag == 2 then
			self.AfterButtonTextObj:SetActive(false)
			self.AfterButtonBuLingObj:SetActive(true)
			self.AfterHandler.Interactable = true
			UH.SpriteName(self.AfterIcon,RewardConfig.LunchSupperType[after_info.currency_type])
			UH.SetText(self.AfterNum,tostring(after_info.currency_num))
			self.AfterGetObj:SetActive(false)
			self.AfterGetEffetObj:SetActive(false)
			UH.SpriteName(self.AfterSpriteSet,"HuangAnNiu_PuTong")
		else
			UH.SetText(self.AfterButtonText,Language.LunchSupper.EndTimeStr)
			self.AfterHandler.Interactable = false
			self.AfterGetObj:SetActive(false)
			self.AfterGetEffetObj:SetActive(false)
		end
	end
	
	local night_flag = self.data:GetTimeFlag(night_seq)
	local night_reward = self.data:GetRewardFlag(night_seq)
	self.NightButtonTextObj:SetActive(true)
	self.NightButtonBuLingObj:SetActive(false)
	if night_flag ~= 0 then
		self.NightOpenObj:SetActive(true)
		self.NightCloseObj:SetActive(false)
	else
		self.NightOpenObj:SetActive(false)
		self.NightCloseObj:SetActive(true)
	end
	if night_reward  then
		UH.SetText(self.NightButtonText,Language.LunchSupper.HaveGetStr)
		self.NightHandler.Interactable = false
		self.NightGetObj:SetActive(false)
		self.NightGetEffetObj:SetActive(false)
	else
		if night_flag == 0 then
			UH.SetText(self.NightButtonText,Language.LunchSupper.NotTimeStr)
			self.NightHandler.Interactable = false
			self.NightGetObj:SetActive(false)
			self.NightGetEffetObj:SetActive(false)
		elseif night_flag == 1 then
			UH.SetText(self.NightButtonText,Language.LunchSupper.GetStr)
			self.NightHandler.Interactable = true
			self.NightOpenObj:SetActive(false)
			self.NightCloseObj:SetActive(false)
			self.NightGetObj:SetActive(true)
			self.NightGetEffetObj:SetActive(true)
			self.NightRed:SetNum(1)
			UH.SpriteName(self.NightSpriteSet,"LvAnNiu_PuTong")
		elseif night_flag == 2 then
			self.NightButtonTextObj:SetActive(false)
			self.NightButtonBuLingObj:SetActive(true)
			self.NightHandler.Interactable = true
			UH.SpriteName(self.NightIcon,RewardConfig.LunchSupperType[night_info.currency_type])
			UH.SetText(self.NightNum,tostring(night_info.currency_num))
			self.NightGetObj:SetActive(false)
			self.NightGetEffetObj:SetActive(false)
			UH.SpriteName(self.NightSpriteSet,"HuangAnNiu_PuTong")
		else
			UH.SetText(self.NightButtonText,Language.LunchSupper.EndTimeStr)
			self.NightHandler.Interactable = false
			self.NightGetObj:SetActive(false)
			self.NightGetEffetObj:SetActive(false)
		end
	end
end

function RewardLunchSupperView:OnClickGet(seq)
	local flag = self.data:GetTimeFlag(seq)
	local reward = self.data:GetRewardFlag(seq)
	if not reward  and (flag == 1 or flag == 2) then
		if flag == 2 then
			local info 
			for k,v in pairs(Config.day_reward_auto.day_food) do
				if v.seq == seq then
					info = v
					break
				end
			end
			if info then
				local have_num = 0
				if info.currency_type == 1 then
					have_num = RoleData.Instance:GetCurrency(CurrencyType.Gold)
					if have_num < info.currency_num then
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
						BehaviorData.Instance.auto_behavior = 0
						if GLOBAL_CONFIG:Flags().block_recharge then
							info.no_confirm = true
						end
						PublicPopupCtrl.Instance:DialogTips(info)
						return
					end
				elseif info.currency_type == 0 then
					have_num = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
					if have_num < info.currency_num then
						PublicPopupCtrl.Instance:Error(Language.LunchSupper.TIP3)
						return
					end
				elseif info.currency_type == 2 then
					have_num = RoleData.Instance:GetCurrency(CurrencyType.ImmortalCoin)
					if have_num < info.currency_num then
						PublicPopupCtrl.Instance:Error(Language.LunchSupper.TIP4)
						return
					end
				end
			end
		end
		RewardCtrl.Instance:SendLunchSupperRewardReq(seq)
	else
		if reward then
			PublicPopupCtrl.Instance:Error(Language.LunchSupper.TIP1)
		else
			if flag == 0 then
				PublicPopupCtrl.Instance:Error(Language.LunchSupper.TIP0)
			end
			if flag == 3 then
				PublicPopupCtrl.Instance:Error(Language.LunchSupper.TIP2)
			end
		end
	end
end






