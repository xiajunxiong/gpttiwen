CourageChallengeFinishAutoView = CourageChallengeFinishAutoView or DeclareView("CourageChallengeFinishAutoView", "courage_challenge/courage_challenge_finish_auto")

VIEW_DECLARE_MASK(CourageChallengeFinishAutoView,ViewMask.BgBlockClose)

function CourageChallengeFinishAutoView:CourageChallengeFinishAutoView()
	self.data = CourageChallengeData.Instance
end

function CourageChallengeFinishAutoView:LoadCallback(param_t)
	local info = param_t.layer_info or {}
	local config
	self.pagoda_type = param_t.type or CourageChallengeData.PagodaType.GuMo
	if param_t.type == CourageChallengeData.PagodaType.GuMo then
		config = self.data:GetLayerBossConfig(info.layer_level,info.level_seq)
	else
		config = self.data:GetLayerBossConfig2(info.layer_level,info.level_seq)
	end
    self.Condition:SetData({
		flag = info.flag_list or {},
		star_num = info.flag_num or 0,
		desc = self.data:GetConditionList(config),
    })
	local pass_tips = Language.CourageChallenge.PassTips[info.flag_num or 1]
	UH.SetText(self.Desc, Format(Language.CourageChallenge.FinishTips, pass_tips))

	local monster_config = CourageChallengeData.Instance:GetMonsterConfig(config.monster_group)

	self.cell_list = self.data:GetFinishRewardData(config,param_t) or {}
	local list = FinishData.Instance:GetEleRewards()
	FinishData.Instance:SetEleRewards(nil)
	if list then
		for i, v in ipairs(list) do
			table.insert(self.cell_list, v)
		end
	end
	self.data:SetRewardDataList(self.cell_list,monster_config.name)
	self.Right:SetActive(#self.cell_list ~= 0)
	self:FlushCallListView()
	self:FlushAutoTimeView(config)
end
----------------------------------------------------------
function CourageChallengeFinishAutoView:FlushAutoTimeView(config)
	local next_config
	if self.pagoda_type == CourageChallengeData.PagodaType.GuMo then
		next_config = self.data:GetChallengeConfig(config.seq + 1)
	else
		next_config = self.data:GetChallengeConfig2(config.seq + 1)
	end
	if next_config ~= nil and next_config.level <= RoleData.Instance:GetRoleLevel() then
		if PrefsInt(PrefKeys.CourageChallengeAuto())==1 then
			self.AutoTime:SetCallBack(function()self:OnClickConfirm()end)
			self.AutoTime:TotalTime(5,TimeType.Type_Special_2,Language.CourageChallenge.FinishAutoTime)
		end
		self.IsAutoChallengeFunc = function()
			if self.pagoda_type == CourageChallengeData.PagodaType.GuMo then
				CourageChallengeCtrl.Instance:SendBattleReq(next_config.layer_level,next_config.level_seq)
			else
				CourageChallengeCtrl.Instance:SendBattleReq2(next_config.layer_level,next_config.level_seq)
			end
		end
	elseif next_config ~= nil and next_config.level > RoleData.Instance:GetRoleLevel() then 
		self.IsAutoChallenge = nil
		-- 如果勾选了自动战斗则直接关闭本界面即可
		if PrefsInt(PrefKeys.CourageChallengeAuto())==1 then
			self:OnClickClose()
		end 
		self.IsAutoChallengeFunc = function()
			PublicPopupCtrl.Instance:Center(string.format(Language.CourageChallenge.LevelFail,next_config.level))
		end
	elseif next_config == nil then
		self.IsAutoChallengeFunc = function ()
			ViewMgr:CloseView(CourageChallengeFinishAutoView)
		end
	end	

	UH.SetText(self.BtnName,Language.CourageChallenge.FinishBtnName[next_config and 1 or 2])
end
function CourageChallengeFinishAutoView:OnAutoChallengeFunc()
	if self.IsAutoChallengeFunc then
		self.IsAutoChallengeFunc()
		self.IsAutoChallenge = true
	end
end
----------------------------------------------------------

function CourageChallengeFinishAutoView:OnClickConfirm()
	self:OnAutoChallengeFunc()
	ViewMgr:CloseView(CourageChallengeView)
end

--物品动画逻辑
function CourageChallengeFinishAutoView:FlushCallListView()
	if #self.cell_list == 0 then return end
	self.grid_with = 90 + 110 * (#self.cell_list-1)
	self.GridRect.sizeDelta = Vector2.New(self.grid_with,100)
	self.GridList:SetData(self.cell_list)
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

function CourageChallengeFinishAutoView:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
		self:SetItem(self.GridList.item_list[self.count + 1])
		self.count = self.count + 1
		self.ShowItemTime = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
        end, 0.2)
	end
end

function CourageChallengeFinishAutoView:SetItem(item)
	if item then
		item:SetPosition(Vector3.New(90 / 2 + self.count * 110,0,0))
	end
end

function CourageChallengeFinishAutoView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	BattleData.Instance:ClearBattleStatistic()
	self.AutoTime:CloseCountDownTime()
	if self.IsAutoChallenge == nil then
		local reward_list = self.data:GetRewardDataList()
		if #reward_list == 0 then
			ViewMgr:OpenView(CourageChallengeView)
			CourageChallengeCtrl.Instance:OnPetHelperFunc()
		else
			AllSoulsStorybookCtrl.Instance:OpenRewardView(reward_list,AllSoulsRewardType.Mult,AllSoulsRewardTitleType.ChallengeReward, true)
			self.data:ResetRewardData()
			AllSoulsStorybookCtrl.finish_close_func = function()
				ViewMgr:OpenView(CourageChallengeView)
			end
		end
	end
end

function CourageChallengeFinishAutoView:OnClickClose()
	ViewMgr:CloseView(CourageChallengeFinishAutoView)
end

CourageChallengeFinishItem = CourageChallengeFinishItem or DeclareMono("CourageChallengeFinishItem", UIWidgetBaseItem)
function CourageChallengeFinishItem:CourageChallengeFinishItem()
	self.lang = Language.Finish
end

function CourageChallengeFinishItem:SetData(data)
	if data == nil then
		return
	end
	UIWidgetBaseItem.SetData(self, data)
	self:FlushView()
end

function CourageChallengeFinishItem:FlushView()
	local index = self.data.seq % 2 == 0 and 2 or 1
	UH.SpriteName(self.BgSp, "RoleHeadBtm" .. index)
	UH.SizeDelta(self.BgSp, Vector3.New(800, 86))

	if self.data.obj_type == 0 then
		if self.data.special_appearance_type == 1 then
			local co = PartnerData.Instance:GetPartnerInfoById(self.data.special_appearance_param)
			UH.SetIcon(self.IconSp, co.info.icon_id, ICON_TYPE.ITEM)
			UH.SetText(self.Name, self.data.role_name)
		else
			UH.SetAvatar(self.IconSp, self.data.avatar_type, self.data.avatar_id,self.data.avatar_quality)
			UH.SetText(self.Name, self.data.role_name)
		end
	elseif self.data.obj_type == 1 then
		local partner_cfg = Cfg.PartnerById(self.data.partner_id)
		if partner_cfg ~= nil then
			UH.SetText(self.Name, partner_cfg.name)
			local icon_id = partner_cfg.icon_id
			if self.data.skin_id ~= 0 then
				local co = Cfg.PartnerSkinById(self.data.skin_id)
				if co ~= nil then
					icon_id = co.head_portrait
				end
			end
			UH.SetIcon(self.IconSp, icon_id, ICON_TYPE.ITEM)
		end
	elseif self.data.obj_type == 2 then
		self.PetNode:SetActive(true)
		self.IconNode:SetActive(false)
		local pet = Pet.New(self.data)
		self.PetCell:SetData(pet)
		UH.SetText(self.Name, pet:Name())
		UH.SetText(self.PetLevel, pet:Level())
		PetData.SetStrLevel(nil, self.PetStrLevel, pet:StrLevel())
	end
end

function CourageChallengeFinishItem:ShowProg()
	local stat_info = BattleData.Instance:GetBattleStatisticResult()
	local t_list = {"total_damage_to_other", "total_heal_num", "total_suffer_injure"}
	for i, v in pairs(t_list) do
		local value, max_value = self.data[v] or 0, math.max(stat_info[v] or 1, 1)
		self.ProgList[i]:AddToProgress(value / max_value, function() 
			self.ProgList[i]:SetText(value)
		end)
	end
end

function CourageChallengeFinishItem:SetShow()
	self.Root:SetActive(true)
	self.timer_handle = TimeHelper:AddDelayTimer(function()
		self:ShowProg()
	end, 0.4)
end

function CourageChallengeFinishItem:OnDestroy()
	if self.timer_handle ~= nil then
		TimeHelper:CancelTimer(self.timer_handle)
		self.timer_handle = nil
	end
end

----------------------------CourageChallengeFinishCall----------------------------
CourageChallengeFinishCall = CourageChallengeFinishCall or DeclareMono("CourageChallengeFinishCall", UIWidgetBaseItem)

function CourageChallengeFinishCall:SetData(data)
	if data == nil then
		return
	end
	data.item = Item.New(data)
	self.Call:SetData(data.item)
	self.Label:SetActive(data.is_extra_reward == true)
	self.Ele:SetActive(data.ele_reward and data.ele_reward == true)
	UIWidgetBaseItem.SetData(self, data)
end

function CourageChallengeFinishCall:SetPosition(vect)
	UH.LocalPos(self.Rect,vect)
	self.Root:SetActive(true)
	if self.Effect.isActiveAndEnabled then
		local effect_id = self:GetEffectId()
		if effect_id ~= 0 then
			self.Effect:Play(tostring(effect_id))
		end
	end
end

function CourageChallengeFinishCall:GetEffectId()
	if self.data.item:IsEquip() then
		return 10010335
	elseif Item.GetColor(self.data.item_id) >= 4 or BagData.IsRare(self.data.item_id) then
		return 10010336
	end
	return 0
end