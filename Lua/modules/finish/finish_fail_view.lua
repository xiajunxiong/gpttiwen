FinishFailView = FinishFailView or DeclareView("FinishFailView", "finish/finish_fail")

VIEW_DECLARE_MASK(FinishFailView,ViewMask.BgBlockClose)

FinishFailPanel = FinishFailPanel or DeclareMono("FinishFailPanel", UIWFlushPanel)

function FinishFailPanel:FinishFailPanel()
	self.data = FinishData.Instance
	self.cell_list = {}
end

function FinishFailPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FinishPassPanelView()
	if self:IsTaoQuanNaJi() then 
		self:FlushTaoQuanNaJiTimeView()
	else 
		self:FlushTimeView()
	end 
	self:FlushCallListView()
end

function FinishFailPanel:FinishPassPanelView()
	local param = self.data:GetFailData()
	-- 对于同属服务器boss类别的特殊处理的
	if FinishConfig.ServerBoss[self.data:GetBattleMode()] then 
		self:ShowServerBoss(param)
		return 
	end 

    UH.SetText(self.Desc,param.desc or self.data:FailDesc())
	self.value_list = param.value_list or {}
	self.cell_list = param.reward_list or {}
	self.ValueList:SetData(self.value_list)
	self:SetEffectActive(param.show_type or (self.data:IsDraw() and 2 or 1))
	--是否展示随机的提示


	if param.desc == nil and param.value_list == nil then
		UH.SetText(self.Ramdon,self.data:GetRamdonTip())
		self.Ramdon:SetObjActive(true)
	else
		self.Ramdon:SetObjActive(false)
	end 
end

function FinishFailPanel:ShowServerBoss(param)
	self:SetEffectActive(3)
	self.worldboss_part.gameObject:SetActive(true)
	self.worldboss_part:SetData(param.value_list[1])
	UH.SetText(self.Desc,"")
	UH.SetText(self.Ramdon,Language.WorldBoss.FinishTips)
	self.Ramdon:SetObjActive(true)
	self.value_list = {}
	self.cell_list = {}
end

function FinishFailPanel:FlushTimeView()
	local param = self.data:GetCloseFunc()
	if param and param.is_close then return end
	self.ConfirmTime:SetCallBack(function()ViewMgr:CloseView(FinishFailView)end)
	self.ConfirmTime:TotalTime(self.data:GetCloseTime(#self.value_list~=0),
	TimeType.Type_Special_2,Language.Finish.Confirm)
	self.BtnConfirm:SetActive(#self.value_list~=0 or #self.cell_list ~= 0)
	self.GridList:SetObjActive(#self.cell_list ~= 0)
	self.Ramdon:SetObjActive(#self.cell_list == 0)
end

function FinishFailPanel:FlushCallListView()
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
function FinishFailPanel:FlushShowView()
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

function FinishFailPanel:SetItem(item)
	if item then
		item:SetShow()
	end
end

function FinishFailPanel:SetEffectActive(index)
	if self.Effect[index] then
		self.Effect[index]:SetActive(true)
	end
end
----------------------------------------------
function FinishFailPanel:IsTaoQuanNaJi()
	return self.data:GetBattleMode() == BATTLE_MODE.BATTLE_MODE_TAOQUANNAJI
end 
function FinishFailPanel:FlushTaoQuanNaJiTimeView()
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(FinishFailView)
	end)
	self.ConfirmTime:TotalTime(self.data:GetCloseTime(true),
	TimeType.Type_Special_2,Language.Finish.Confirm)
	self.BtnConfirm:SetActive(true)
	self.GridList:SetObjActive(false)
	self.Ramdon:SetObjActive(false)
	UH.AnchoredPosition(self.DescRect, Vector2.New(0, 50))
end 

function FinishFailPanel:OnClickClose()
	ViewMgr:CloseView(FinishFailView)
end

AUTO_TIME_FINISH_FUNC = {
	[BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE] = function()
		return CourageChallengeData.Instance:GetRewardDataList()
	end,
	[BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER] = function()
		return EndlessTowerData.Instance:GetRewardDataList()
	end,
}

function FinishFailPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	self.ConfirmTime:CloseCountDownTime()
	local auto_func = AUTO_TIME_FINISH_FUNC[self.data:GetBattleMode()]
	if auto_func and #auto_func() ~= 0 then
		AllSoulsStorybookCtrl.Instance:OpenRewardView(auto_func(),AllSoulsRewardType.Mult,AllSoulsRewardTitleType.ChallengeReward, true)
		EndlessTowerData.Instance:ResetRewardData()
		CourageChallengeCtrl.Instance:OnPetHelperFunc()
		AllSoulsStorybookCtrl.finish_close_func = function()
			FinishCtrl.Instance:CloseFinishView(0)
		end
	else
		FinishCtrl.Instance:CloseFinishView(0)
	end
end



----------------------------FinishItem----------------------------
FinishItem = FinishItem or DeclareMono("FinishItem", UIWidgetBaseItem)

function FinishItem:SetData(data)
	UH.SetText(self.Value,data.num)
	if data.item_id and data.item_id ~= 0 then
		UH.SpriteName(self.Icon,tostring(data.item_id))
		self.Icon:SetObjActive(true)
		if data.name then
			UH.SetText(self.Name,data.name)
			self.Name:SetObjActive(true)
		else
			self.Name:SetObjActive(false)
		end
	else
		UH.SetText(self.Name,data.name)
		self.Icon:SetObjActive(false)
		self.Name:SetObjActive(true)
	end
end

--------------------FinishFailWorldBossPanel
FinishFailWorldBossPanel = FinishFailWorldBossPanel or DeclareMono("FinishFailWorldBossPanel", UIWFlushPanel)
function FinishFailWorldBossPanel:FinishFailWorldBossPanel() end
function FinishFailWorldBossPanel:SetData(data)
	UH.SetText(self.Damage,data.damage)
	UH.SetText(self.Heal,data.heal)
end 