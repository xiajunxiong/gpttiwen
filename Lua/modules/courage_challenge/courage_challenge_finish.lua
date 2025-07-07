CourageChallengeFinishView = CourageChallengeFinishView or DeclareView("CourageChallengeFinishView", "courage_challenge/courage_challenge_finish")

VIEW_DECLARE_MASK(CourageChallengeFinishView,ViewMask.BgBlockClose)

function CourageChallengeFinishView:CourageChallengeFinishView()
	self.data = CourageChallengeData.Instance
end

function CourageChallengeFinishView:LoadCallback(param_t)
	local info = param_t.layer_info or {}
	local config
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
	self.cell_list = {}
	if param_t.first_kill == 0 and info.first_kill == 1 then
		self.cell_list = self.data:GetRewardList(config,param_t)
		if (param_t.get_extra or 0) == 0 and (RoleData.Instance:GetRoleLevel() > self.data:GetExtraRewardLevel() + config.level) then
			table.remove(self.cell_list,1)
		end
		local list = FinishData.Instance:GetEleRewards()
		FinishData.Instance:SetEleRewards(nil)
		if list then
			for i, v in ipairs(list) do
				table.insert(self.cell_list, v)
			end
		end
		self.Rewards:SetActive(#self.cell_list ~= 0)
	end
	self:FlushCallListView()
	self:FlushMemberInfo()
end

function CourageChallengeFinishView:OnClickConfirm()
	ViewMgr:CloseView(CourageChallengeFinishView)
end

--物品动画逻辑
function CourageChallengeFinishView:FlushCallListView()
	if #self.cell_list == 0 then 
		self.Rewards:SetActive(#self.cell_list ~= 0)
		UH.LocalPos(self.Left,Vector3.New(-356,-130,0))
		return
	end
	self.GridList:SetData(self.cell_list)
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

function CourageChallengeFinishView:FlushShowView()
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

function CourageChallengeFinishView:SetItem(item)
	if item then
		item:SetPosition(Vector3.New(45 + self.count * 110, -50, 0))
	end
end

function CourageChallengeFinishView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	TimeHelper:CancelTimer(self.ShowMemberTimer)
	ViewMgr:OpenView(CourageChallengeView)
	CourageChallengeCtrl.Instance:OnPetHelperFunc()
	BattleData.Instance:ClearBattleStatistic()
end

function CourageChallengeFinishView:FlushMemberInfo()
	local member_data = BattleData.Instance:GetBattleStatisticResult()
	if member_data.stat_list ~= nil then
		for i, v in pairs(member_data.stat_list) do
			v.seq = i
		end
		self.member_list = member_data.stat_list
		self.MemberList:SetData(member_data.stat_list)
		self.ShowMemberTimer = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushMemberView, self), 0.7)
	end
end

--加动态效果
function CourageChallengeFinishView:FlushMemberView()
	if self.member_count == nil then
		self.member_count = 0
	end
	if self.member_count < #self.member_list then
		local item = self.MemberList.item_list[self.member_count + 1]
		if item ~= nil then
			item:SetShow()
		end
		self.member_count = self.member_count + 1
		self.ShowMemberTimer = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushMemberView, self), 0.2)
	end
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