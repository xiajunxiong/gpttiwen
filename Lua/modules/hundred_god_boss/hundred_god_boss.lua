HundredGodBoss = HundredGodBoss or DeclareView("HundredGodBoss", "hundred_god_boss/hundred_god_boss",Mod.HundredGodBoss.Main)

function HundredGodBoss:HundredGodBoss()
    self.data = HundredGodBossData.Instance
end
function HundredGodBoss:LoadCallback()
    -- self.total_frame = 24
    local start = self.data:StartPos()
    if start then
        local endPos = Vector2.zero
        local frame = 0
        local value = 0
        start = Vector2.New(start.x, start.y)
        self.Anima:Play("open")
        self.hand_runer = TimeHelper:AddRunFrameTimer(function ()
            value = Mathf.Clamp((frame/ self.TotalFrame), 0, 1)
            UH.AnchoredPosition(self.RootRect, Vector2.Lerp(start, endPos, value))
            frame = frame + 1
        end, 1, self.TotalFrame)
    else
        self.RootGroup.alpha = 1
        self.RootGroup.interactable = true
        self.RootGroup.blocksRaycasts = true
    end
end
function HundredGodBoss:CloseCallback()
    TimeHelper:CancelTimer(self.hand_runer)
end


HundredGodBossPanel = HundredGodBossPanel or DeclareMono("HundredGodBossPanel", UIWFlushPanel)
function HundredGodBossPanel:HundredGodBossPanel()
    self.data = HundredGodBossData.Instance
    self.data_cares = {
        {data = self.data:Info(), func = self.FlushReward},
    }
    self.effect_ids = {4165102,4165103,4165104,4165105}
    self.all_finish_effect = 4165106
end

function HundredGodBossPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.hand_runertrue =  Runner.Instance:RunUntilTrue(function ()
        if self.CanvasGroup.alpha == 1 and self.Anima.isPlaying == false then
            self:OnStartEnd()
            return true
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("HundredGodBossClose",function ()
        return self.BtnClose
    end)
end
function HundredGodBossPanel:OnStartEnd()
    self.LuaClick.enabled = false
    local data = self.data:TargetConfig()
    self.List:SetData(data)
    for i, v in ipairs(data) do
        --假设有已完成
        if self.data:IsFinish(v) then
            --播放盖章过程
            if not self.data:CacheEffectPlayed(v.id) then
                self.EffectTool:Play(self.effect_ids[v.id])
                self.data:CacheEffectPlayed(v.id, true)
            end
        end
    end
    self.hand_delay = TimeHelper:AddDelayTimer(function ()
        for i, v in ipairs(data) do
            if self.data:CacheEffectPlayed(v.id) then 
                self.TargetOpens[i]:SetLocalScale(Vector3.one)
            end
        end
        if self.data:AllFinish() and not self.data:CacheEffectPlayed(5) then
            self.EffectTool:Play(self.all_finish_effect)
            self.data:CacheEffectPlayed(5, true)
            for i = 1, 4 do
                self.TargetOpens[i]:SetActive(false)
                self.TargetLocks[i]:SetActive(false)
                self.DescObjs[i]:SetActive(false)
            end
            self.Finish:SetActive(true)
        end
    end, 1)
end
function HundredGodBossPanel:FlushReward()
    local data = self.data:TargetConfig()
    self.List:SetData(data)
    for i, v in ipairs(data) do
        UH.SetText(self.TargetDescs[i], v.task_dec)
        --假设有已完成
        self.TargetLocks[i]:SetActive(true)--not self.data:IsFinish(v))
        if self.data:CacheEffectPlayed(v.id) then
           self.TargetOpens[i]:SetLocalScale(Vector3.one)
        end
        -- self.TargetOpens[i]:SetActive(self.data:IsFinish(v.cond_type, v.param))
    end
    if self.data:AllFinish() and self.data:CacheEffectPlayed(5) then
        for i = 1, 4 do
            self.TargetOpens[i]:SetActive(false)
            self.TargetLocks[i]:SetActive(false)
            self.DescObjs[i]:SetActive(false)
        end
        self.Finish:SetActive(true)
    end
    self.Finish:SetActive(self.data:AllFinish())
    UH.SetText(self.Tip, Language.HundredGodBoss.FinishTip)
    self.TimeMeter:StampTime(self.data:Info().time, TimeType.Type_Time_8)
    -- local info = PartnerData.Instance:GetPartnerInfoById(self.data:ParnerId())
    self.Cell:SetData(self.data:FinalItem())
    self.List:SetCompleteCallBack(function ()
        GuideManager.Instance:RegisterGetGuideUi("HundredGodReward1",function ()
            return self.List.item_list[1]
        end)
    end)
    UH.SetText(self.BtnPreTex, self.data:AllFinish() and Language.HundredGodBoss.LingQu or Language.HundredGodBoss.Preview)
    self.RedPoint:SetNum(self.data:AllFinish() and 1 or 0)
end
function HundredGodBossPanel:OnClickPre()
    if self.data:AllFinish() then
        HundredGodBossCtrl.FinalReward()
        ViewMgr:CloseView(HundredGodBoss)
    else
        PartnerData.Instance:SetNewPartner(self.data:ParnerId(), true)
        local info = PartnerData.Instance:GetPartnerInfoById(self.data:ParnerId())
        ViewMgr:OpenView(PartnerViewLock, {partner_name = info.info.name})
    end
end
function HundredGodBossPanel:OnClickClose()
    self.LuaClick.enabled = true
    local start = self.data:StartPos()
    if start then
        start = Vector2.New(start.x, start.y)
        local endPos = Vector2.zero
        self.Anima:Play("close")
        local frame = 0
        self.hand_runer = TimeHelper:AddRunFrameTimer(function ()
            UH.AnchoredPosition(self.RootRect, Vector2.Lerp(endPos, start, frame/ self.TotalFrame))
            frame = frame + 1
        end, 1, self.TotalFrame)
    else
        ViewMgr:CloseView(HundredGodBoss)
    end
end
function HundredGodBossPanel:OnDestroy()
    GuideManager.Instance:UnRegisterGetGuideUi("HundredGodReward1")
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.hand_runer)
    TimeHelper:CancelTimer(self.hand_delay)
    Runner.Instance:RemoveRunUntilTrue(self.hand_runertrue)
    PartnerData.Instance.force_unlock = nil
end
function HundredGodBossPanel:OnAnimEnd(value)
    TimeHelper:CancelTimer(self.hand_runer)
    ViewMgr:CloseView(HundredGodBoss)
end
function HundredGodBossPanel:OnClickTarget(id)
    local data = self.data:TargetConfig()
    local cfg = data[id]
    --还要接各种功能
    if self.data:IsFinish(cfg) then
        PublicPopupCtrl.Instance:Center(Language.HundredGodBoss.TargetFinish)
    elseif id == 2 then
        ViewMgr:CloseView(HundredGodBoss)
        TaskCtrl.Instance:AutoTaskImmediately()
    elseif id == 3 then
        if RoleData.Instance:GetRoleLevel() < ActivityData.GetConfig(ActType.GuildTask).level_restrict then
            PublicPopupCtrl.Instance:Center(string.format(Language.HundredGodBoss.LevelTip, ActivityData.GetConfig(ActType.GuildTask).level_restrict))
            return
        end
        ViewMgr:CloseView(HundredGodBoss)
        ViewMgr:OpenViewByKey(Mod.Activity.Daily)
    else
        PublicPopupCtrl.Instance:Center(Language.HundredGodBoss.GotoTask)
    end
end
HundredGodBossItem = HundredGodBossItem or DeclareMono("HundredGodBossItem", UIWidgetBaseItem)
function HundredGodBossItem:HundredGodBossItem()
    self.god_data = HundredGodBossData.Instance
end
function HundredGodBossItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.finish_reward))
    UH.SetText(self.Desc, data.target or data.id)
    self.GetObj:SetActive(self.god_data:IsGet(data.id))
    self.RedPoint:SetNum(self.god_data:CanGet(data) and 1 or 0)
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
        self.handle_effect = nil
    end
    if self.god_data:CanGet(data) then
        self.handle_effect = self.EffectTool:Play(4165109)
    end
end
function HundredGodBossItem:OnClickGet()
    if self.god_data:IsGet(self.data.id) then
        CellClicks.BagGridNoButton(self.Cell)
    else
        if self.god_data:IsFinish(self.data) then
            HundredGodBossCtrl.TargetReward(self.data.id)
        else
            CellClicks.BagGridNoButton(self.Cell)
        end
    end
end
