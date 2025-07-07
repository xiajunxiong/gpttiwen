FinishHurtView = FinishHurtView or DeclareView("FinishHurtView", "finish/finish_hurt")
VIEW_DECLARE_MASK(FinishHurtView, ViewMask.BgBlockClose)

function FinishHurtView:OnClickClose()
	ViewMgr:CloseView(FinishHurtView)
end

FinishHurtPanel = FinishHurtPanel or DeclareMono("FinishHurtPanel", UIWFlushPanel)
function FinishHurtPanel:FinishHurtPanel()
	self.data = FinishData.Instance
	self.lang = Language.Finish
	self.data_cares = {
		{data = BattleData.Instance.battle_statistics, func = self.FlushMemberInfo, init_call = false}
	}
	GuideManager.Instance:RegisterGetGuideUi("DreamNotesFinishBtn", function()
		self.ConfirmTime:CloseCountDownTime()
		self.ConfirmTime:SetTime(Language.TeamSport.Confirm)
		return self.EnterBtn,function ()
            self:OnClickClose()
        end
    end)
end

function FinishHurtPanel:Awake()
	UIWFlushPanel.Awake(self)
	self.finish_info = self.data:GetFinishInfo()
	self.is_win = self.finish_info.is_win or 0
	if self.is_win == 1 then
		self:FlushPassView()
	else
		self:FlushFailView()
	end
	self:FlushMemberInfo()
	self:FlushTimeView()
	self:SetCustomInfo()
end

function FinishHurtPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	FinishCtrl.Instance:CloseFinishView(self.is_win or 0)
	self.ConfirmTime:CloseCountDownTime()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	TimeHelper:CancelTimer(self.ShowMemberTimer)
	BattleData.Instance:ClearBattleStatistic()
end

function FinishHurtPanel:FlushPassView()
	local param = self.data:GetPassData()
	local index = param.show_type or 1
	if self.WinEffect[index] then
		self.WinEffect[index]:SetActive(true)
	end
	self.cell_list = param.reward_list or {}
	local list = FinishData.Instance:GetEleRewards()
	FinishData.Instance:SetEleRewards(nil)
	if list then
		for i, v in ipairs(list) do
			table.insert(self.cell_list, v)
		end
	end
	local show_item = false
	if self.cell_list ~= nil and #self.cell_list > 0 then
		show_item = true
		self.ShowItems:SetActive(true)
		self:FlushCallListView()
	elseif param.reward_desc ~= nil then
		self.ShowItems:SetActive(true)
		UH.SetText(self.RewardDesc, param.reward_desc)
	else
		self.ShowItems:SetActive(false)
	end
	if param.value_list and not param.show_rank then
		self.ShowDescs:SetData(param.value_list)
		UH.LocalPosG(self.ShowDescs.gameObject, Vector3.New(315, show_item and -125 or -180, 0))
	end
	UH.SetText(self.TitleReward, Language.Finish.WuPinJiangLi)
	if param.show_rank == true then
		self.ShowDescs:SetObjActive(false)
		UH.SetText(self.ShowRankObjs[1], param.value_list[1].name)
		UH.SetText(self.ShowRankObjs[2], param.value_list[2].name)
		UH.SetText(self.ShowRankObjs[3], param.value_list[1].num)
		UH.SetText(self.ShowRankObjs[4], param.value_list[2].num)
		UH.SpriteName(self.ShowRankObjs[5], param.value_list[1].sprite)
		self.ShowRank:SetActive(true)
		--246.3
		self.ShowRank:SetLocalPosition(Vector3.New(312.4, -100.2, 0))
		UH.SetText(self.TitleReward, Language.Finish.TiaoZhanJiangLi)
	end
	if BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB == self.data:GetBattleMode() then
		local other_rewards = FightData.Instance.crystal_reward_others
		if nil ~= other_rewards then
			self.CrystalReward:SetData(other_rewards)
			self.CrystalReward:SetObjActive(true)
		end
	end
end

function FinishHurtPanel:FlushFailView()
	local param = self.data:GetFailData()
	local index = param.show_type or (self.data:IsDraw() and 2 or 1)
	if self.FailEffect[index] then
		self.FailEffect[index]:SetActive(true)
	end
	if param.value_list ~= nil then
		self.ShowDescs:SetData(param.value_list)
	end
	self.ShowFailTips:SetActive(true)
	UH.SetText(self.LblFailTip1, param.desc or self.data:FailDesc())
	UH.SetText(self.LblFailTip2, self.data:GetRamdonTip())
end

function FinishHurtPanel:FlushMemberInfo()
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

function FinishHurtPanel:FlushCallListView()
	if #self.cell_list == 0 then
		return
	end
	self.GridList:SetData(self.cell_list)
	--0.7秒之后出现物品
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

--加载奖励列表
function FinishHurtPanel:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
		self:SetItem(self.GridList.item_list[self.count + 1])
		self.count = self.count + 1
		--物品出现的间隔时间是0.2秒
		self.ShowItemTime = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
		end, 0.2)
	end
end

function FinishHurtPanel:SetItem(item)
	if item then
		item:SetShow()
	end
end

--刷新倒计时
function FinishHurtPanel:FlushTimeView()
	local param = self.data:GetCloseFunc()
	if param and param.is_close then return end
	self.ConfirmTime:SetCallBack(function()
		FightData.Instance:SetMarkFantasyCountDown(true)
		ViewMgr:CloseView(FinishHurtView)
	end)
	local flag = (self.cell_list ~= nil and #self.cell_list ~= 0) or (self.value_list ~= nil and #self.value_list ~= 0)
	self.ConfirmTime:TotalTime(self.data:GetCloseTime(flag),
	TimeType.Type_Special_2,Language.Finish.Confirm)
end

--加动态效果
function FinishHurtPanel:FlushMemberView()
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

function FinishHurtPanel:SetCustomInfo()
	if FinishConfig.HurtCustom[self.data:GetBattleMode()] ~= nil then
		self.CustomInfoArray[FinishConfig.HurtCustom[self.data:GetBattleMode()]]:SetActive(true)
	end
end

FinishHurtItem = FinishHurtItem or DeclareMono("FinishHurtItem", UIWidgetBaseItem)
function FinishHurtItem:FinishHurtItem()
	self.lang = Language.Finish
end

function FinishHurtItem:SetData(data)
	if data == nil then
		return
	end
	UIWidgetBaseItem.SetData(self, data)
	self:FlushView()
end

function FinishHurtItem:FlushView()
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

function FinishHurtItem:ShowProg()
	local stat_info = BattleData.Instance:GetBattleStatisticResult()
	local t_list = {"total_damage_to_other", "total_heal_num", "total_suffer_injure"}
	for i, v in pairs(t_list) do
		local value, max_value = self.data[v] or 0, math.max(stat_info[v] or 1, 1)
		local function func_set()
			self.ProgList[i]:SetText(value)
		end
		self.ProgList[i]:AddToProgress(value / max_value, func_set)
	end
end

function FinishHurtItem:SetShow()
	if self.Root then
		self.Root:SetActive(true)
	end
	self.timer_handle = TimeHelper:AddDelayTimer(function()
		self:ShowProg()
	end, 0.4)
end

function FinishHurtItem:OnDestroy()
	if self.timer_handle ~= nil then
		TimeHelper:CancelTimer(self.timer_handle)
		self.timer_handle = nil
	end
end

----------------------------FinishHurtCellItem----------------------------
FinishHurtCellItem = FinishHurtCellItem or DeclareMono("FinishHurtCellItem", UIWidgetBaseItem)
function FinishHurtCellItem:SetData(data)
	if data == nil then
		return
	end
	data.item = Item.Create(data)
	self.Call:SetData(data.item)
	self.Label:SetActive(data.first_kill and data.first_kill == true)
	self.Ele:SetActive(data.ele_reward and data.ele_reward == true)
	UIWidgetBaseItem.SetData(self, data)
end

function FinishHurtCellItem:SetShow()
	self.Root:SetActive(true)
	if self.Effect.isActiveAndEnabled then
		local effect_id = self:GetEffectId()
		if effect_id ~= 0 then
			self.Effect:Play(tostring(effect_id))
		end
	end
end

function FinishHurtCellItem:GetEffectId()
	if self.data.item:IsEquip() then
		return 10010335
	elseif Item.GetColor(self.data.item_id) >= 4 or BagData.IsRare(self.data.item_id) then
		return 10010336
	end
	return 0
end

----------------------------FinishHurtDescItem----------------------------
FinishHurtDescItem = FinishHurtDescItem or DeclareMono("FinishHurtDescItem", UIWidgetBaseItem)
function FinishHurtDescItem:SetData(data)
	UH.SetText(self.Value, data.name .. data.num)
	if data.item_id and data.item_id ~= 0 then
		UH.SpriteName(self.Icon, tostring(data.item_id))
		self.Icon:SetObjActive(true)
	end
end

FinishHurtCrystalItem = FinishHurtCrystalItem or DeclareMono("FinishHurtCrystalItem")

function FinishHurtCrystalItem:SetData(data)
	self.ExpNum.text = data.exp_num or 0
	self.ItemSNum.text = data.item_s1_num or 0
	self.ExpNum:SetObjActive(data.exp_num > 0)
	self.ItemSNum:SetObjActive(data.item_s1_num ~= nil and data.item_s1_num > 0)
end
