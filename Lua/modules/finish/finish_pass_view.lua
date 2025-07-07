FinishPassView = FinishPassView or DeclareView("FinishPassView", "finish/finish_pass")

VIEW_DECLARE_MASK(FinishPassView,ViewMask.BgBlockClose)

function FinishPassView:OnClickClose()
	ViewMgr:CloseView(FinishPassView)
end

FinishPassPanel = FinishPassPanel or DeclareMono("FinishPassPanel", UIWFlushPanel)

function FinishPassPanel:FinishPassPanel()
	self.data = FinishData.Instance
end

function FinishPassPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FinishPassPanelView()
	self:FlushCallListView()
	if EndlessTowerData.IsEndlessTower() then
		self:FlushAutoTimeView()
	elseif self:IsSanXianGuiDong() then
		self:FlushSanXianTimeView()
	elseif GhostBustData.IsGhostBust() then 
		self:FlushGhostBustTimeView()
	elseif self:IsTaoQuanNaJi() then
		self:FlushTaoQuanNaJiTimeView()
	else
		self:FlushTimeView()
	end
	self:FlushEffectActive()
end

function FinishPassPanel:FinishPassPanelView()
	local param = self.data:GetPassData()
	self:FlushLevelView(param.level)
	UH.SetText(self.Desc,param.desc or self.data:PassDesc())
	self.Title:SetActive(self.Desc.text == "")
	self:FlushValueList(param.value_list)
	self.cell_list = param.reward_list or {}
	local list = self.data:GetEleRewards()
	FinishData.Instance:SetEleRewards(nil)
	if list then
		for i, v in ipairs(list) do
			table.insert(self.cell_list, v)
		end
	end
	local is_show_dowm = next(param) ~= nil
	self.Down:SetActive(is_show_dowm)
	UH.LocalPos(self.Up,Vector3.New(0,is_show_dowm and 175 or 100,0))
	if (self:IsSanXianGuiDong() or self:IsTaoQuanNaJi()) and #self.cell_list == 0 then
		UH.AnchoredPosition(self.DescRect, Vector2.New(0, 50))
	else
		UH.AnchoredPosition(self.DescRect, Vector2.New(0, 134))
	end
end

--刷新倒计时
function FinishPassPanel:FlushTimeView()
	local param = self.data:GetCloseFunc()
	if param and param.is_close then return end
	self.ConfirmTime:SetCallBack(function()
		FightData.Instance:SetMarkFantasyCountDown(true)
		ViewMgr:CloseView(FinishPassView)
	end)
	self.ConfirmTime:TotalTime(self.data:GetCloseTime(#self.cell_list ~= 0 or #self.value_list~=0),nil,Language.Finish.Confirm)
end
----------------------------------------------------------
function FinishPassPanel:FlushAutoTimeView()
	local is_has_config = EndlessTowerData.Instance:GetPassLevelData() ~= nil
	if PrefsInt(PrefKeys.EndlessTowerAuto())==1 then
		self.AutoTime:SetCallBack(function()self:OnClickConfirm()end)
		self.AutoTime:TotalTime(5,TimeType.Type_Special_2,Language.CourageChallenge.FinishAutoTime)
	end
	if is_has_config then
		self.IsAutoChallengeFunc = function()
			EndlessTowerCtrl.Instance:OnEnterEndlessTower()
		end
	end
	self.ConfirmTime:SetTime(Language.CourageChallenge.FinishBtnName[is_has_config and 1 or 2])
end

function FinishPassPanel:OnAutoChallengeFunc()
	if self.IsAutoChallengeFunc then
		self.IsAutoChallengeFunc()
		self.IsAutoChallenge = true
	end
end
----------------------------------------------------------

----------------------------------------------------------
function FinishPassPanel:SanXianGuiDongRestart()
	LookingForRabbitsCtrl.EnterFB()
end
function FinishPassPanel:IsSanXianGuiDong()
	return self.data:GetBattleMode() == BATTLE_MODE.BATTLE_MODE_SANXIANGUIDONG
end
function FinishPassPanel:FlushSanXianTimeView()
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(FinishPassView)
		-- self:SanXianGuiDongRestart()
	end)
	self.BtnCancel:SetActive(false)
	-- self.BtnConfirm:SetActive(not LookingForRabbitsData.Instance:IsPassFinal())
	-- self.ConfirmTime:SetTimeText(Language.LFRabbits.Restart)
	self.ConfirmTime:SetTime(Language.Common.Confirm)
end
---------------------------------------------------------------------
function FinishPassPanel:TaoQuanNaJiRestart()

end
function FinishPassPanel:IsTaoQuanNaJi()
	return self.data:GetBattleMode() == BATTLE_MODE.BATTLE_MODE_TAOQUANNAJI
end 
function FinishPassPanel:FlushTaoQuanNaJiTimeView()
	self.BtnCancel:SetActive(false)
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(FinishPassView)
		self:TaoQuanNaJiRestart()
	end)
	self.ConfirmTime:TotalTime(self.data:GetCloseTime(true),nil,Language.Finish.Confirm)
end 
---
function FinishPassPanel:FlushGhostBustTimeView()
	local next_config = GhostBustData.Instance:GetNextData()
	local is_has_config = GhostBustData.Instance:GetNextData() ~= nil

	if is_has_config and next_config.level_limit > RoleData.Instance:GetRoleLevel() then 
		self.IsAutoChallenge = nil
		self.IsAutoChallengeFunc = function() end

		ViewMgr:CloseView(FinishPassView)
		return 
	end

	if PrefsInt(PrefKeys.GhostBustAuto())==1 then
		self.AutoTime:SetCallBack(function()self:OnClickConfirm()end)
		self.AutoTime:TotalTime(5,TimeType.Type_Special_2,Language.CourageChallenge.FinishAutoTime)
	end	
	-- if is_has_config then
		self.IsAutoChallengeFunc = function()
			if is_has_config then 
			    GhostBustCtrl.Instance:GhostChallenge()
			else 
				ViewMgr:OpenViewByKey(Mod.GhostBust.Main)
			end 
		end
	-- end
	self.ConfirmTime:SetTime(Language.CourageChallenge.FinishBtnName[is_has_config and 1 or 2])
end
-----------------------------------------------------
function FinishPassPanel:OnClickConfirm()
	self:OnAutoChallengeFunc()
	ViewMgr:CloseView(FinishPassView)
	--[[ if self:IsSanXianGuiDong() then
		self:SanXianGuiDongRestart()
	end ]]
end

--刷新其他奖励配置
function FinishPassPanel:FlushValueList(list)
	self.value_list = list or {}
	self.ValueList:SetData(self.value_list)
end

--通过关数
function FinishPassPanel:FlushLevelView(level)
	if level ~= nil then
		UH.SpriteName(self.Num1,level % 10)
		UH.SpriteName(self.Num2,math.floor(level / 10) % 10)
		UH.SpriteName(self.Num3,math.floor(level / 100))
		self.Num2:SetObjActive(level >= 10)
		self.Num3:SetObjActive(level >= 100)
		UH.SetText(self.Detail,Format(Language.Finish.PassDetail, level))
	end
	self.PassView:SetActive(level ~= nil)
end

--设置特效
function FinishPassPanel:FlushEffectActive()
	local param = self.data:GetPassData()
	local index = param.show_type or 1
	if self.Effect[index] then
		self.Effect[index]:SetActive(true)
	end
end

function FinishPassPanel:FlushCallListView()
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
function FinishPassPanel:FlushShowView()
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

function FinishPassPanel:SetItem(item)
	if item then
		item:SetShow()
	end
end

function FinishPassPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	self.ConfirmTime:CloseCountDownTime()
	self.AutoTime:CloseCountDownTime()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	if self.IsAutoChallenge == nil then
		if self.IsAutoChallengeFunc ~= nil then
		    if EndlessTowerData.IsEndlessTower() and #EndlessTowerData.Instance:GetRewardDataList() ~= 0 then
			    local reward_list = EndlessTowerData.Instance:GetRewardDataList()
			    AllSoulsStorybookCtrl.Instance:OpenRewardView(reward_list,AllSoulsRewardType.Mult,AllSoulsRewardTitleType.ChallengeReward, true)
			    EndlessTowerData.Instance:ResetRewardData()
			    AllSoulsStorybookCtrl.finish_close_func = function()
				    FinishCtrl.Instance:CloseFinishView(1)
			    end
			elseif GhostBustData.IsGhostBust() and #GhostBustData.Instance:GetRewardDataList() ~= 0 then
				local reward_list = GhostBustData.Instance:GetRewardDataList()
				AllSoulsStorybookCtrl.Instance:OpenRewardView(reward_list,AllSoulsRewardType.Mult,AllSoulsRewardTitleType.ChallengeReward, true)
				GhostBustData.Instance:ResetRewardData()
				AllSoulsStorybookCtrl.finish_close_func = function()
					FinishCtrl.Instance:CloseFinishView(1)
			    end
		    end 
		else
			FinishCtrl.Instance:CloseFinishView(1)
		end
	end
end