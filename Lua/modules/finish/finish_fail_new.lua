FinishFailNew = FinishFailNew or DeclareView("FinishFailNew", "finish/finish_fail_new")
VIEW_DECLARE_MASK(FinishFailView, ViewMask.BgBlock)

FinishFailNewPanel = FinishFailNewPanel or DeclareMono("FinishFailNewPanel", UIWFlushPanel)
function FinishFailNewPanel:FinishFailNewPanel()
    self.data = FinishData.Instance
    self.lang = Language.Finish
    self.is_only_close = false
end
function FinishFailNewPanel:OnClickClose()
    self.is_only_close = true
    ViewMgr:CloseView(FinishFailNew)
end
function FinishFailNewPanel:onFlush()
    self:FlushFailDesc()
    self:FlushTimeView()

    local list = TableCopy(Config.battle_settlement_auto.list)
    local remind_type = MainViewUpgradeRemindType.Remove
    local temp = {}
    table.sort(list, DataSorter.KeyLowerSorter("order"))
    for k, v in pairs(list) do
        if v.is_hide == 0 then
            remind_type = BianQiangData.Instance:GetRemindFinishBtnType(v.type)
            if remind_type == MainViewUpgradeRemindType.WithRedPoint and FunOpen.Instance:GetFunIsOpened(v.open_panel) then
                if table.nums(temp) < 5 then
                    v.save = true
                    table.insert(temp, v)
                end
            end
        end
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    if table.nums(temp) < 5 then
        for i, v in pairs(list) do
            if not v.save and v.is_hide == 0 then                
                if FunOpen.Instance:GetFunIsOpened(v.open_panel) then
                    -- remind_type = BianQiangData.Instance:GetRemindFinishBtnType(v.type)
                    if table.nums(temp) < 5 and v.is_always_show == 1
                    and role_level >= v.level_min and role_level <= v.level_max
                    and v.type ~= BianQiangData.FinishBtnType.WoYaoBianQiang then
                        table.insert(temp, v)
                    end
                end
            end
        end
    end
    table.remove(temp,5)
    table.insert(temp, list[#list])
    self.IconList:SetData(temp)
    local battle_mode = self.data:GetBattleMode()
    UH.SetText(self.TitleReward, Language.Finish.WuPinJiangLi)
    if battle_mode == BATTLE_MODE.BATTLE_MODE_ARENA then
        UH.SetText(self.TitleReward, Language.Finish.TiaoZhanJiangLi)
        local list = ArenaData.Instance:FinishBattleReward()
        self.cell_list = list
        self.Reward:SetActive(#self.cell_list ~= 0)
        self.RewardList:SetData(list)
        ArenaData.Instance:SetBattleReward(0)
        self.ShowTimer = TimeHelper:AddDelayTimer(function()
            self:FlushShowView()
        end, 0.7)
    end
end
function FinishFailNewPanel:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
		self:SetItem(self.RewardList.item_list[self.count + 1])
		self.count = self.count + 1
		--物品出现的间隔时间是0.2秒
		self.ShowItemTime = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
		end, 0.2)
	end
end

function FinishFailNewPanel:SetItem(item)
	if item then
		item:SetShow()
	end
end
function FinishFailNewPanel:OnDestroy()
    TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
    self.ConfirmTime:CloseCountDownTime()
    UIWFlushPanel.OnDestroy(self)
    if self.is_only_close == true then--or self.data:GetBattleMode() ~= BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA then
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
end
function FinishFailNewPanel:FlushFailDesc()
    local fail_desc = self.data:FailDesc()
    UH.SetText(self.Desc, fail_desc)
end
function FinishFailNewPanel:FlushTimeView()
    local param = self.data:GetCloseFunc()
    if param and param.is_close then
        return
    end
    self.ConfirmTime:SetCallBack(function()
        self.is_only_close = true
        FightData.Instance:SetMarkFantasyCountDown(true)
        ViewMgr:CloseView(FinishFailNew)
    end)

    self.ConfirmTime:TotalTime(self.data:GetCloseTime(false), TimeType.Type_Special_2, Language.Finish.Confirm)
end
FinishFailNewItem = FinishFailNewItem or DeclareMono("FinishFailNewItem", UIWidgetBaseItem)
function FinishFailNewItem:FinishFailNewItem()

end
function FinishFailNewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Desc, data.name)
    UH.SpriteName(self.IconSp, data.icon)
    self.RedPoint:SetNum(data.save == true and 1 or 0)
    if not StringIsEmpty(data.effect) then
        self.handle_effect = self.EffectTool:PlayByEffectId(tonumber(data.effect), self.EffectParent)
    end
end
function FinishFailNewItem:OnDestroy()
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
        self.handle_effect = nil
    end
end
function FinishFailNewItem:OnClickItem()
    ViewMgr:CloseView(FinishFailNew)
    BianQiangData.Instance:FinishButtonClick(self.data.type)
    ViewMgr:OpenViewByKey(self.data.open_panel)
end
