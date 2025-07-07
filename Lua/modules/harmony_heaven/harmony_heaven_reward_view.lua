
HarmonyHeavenRewardView = HarmonyHeavenRewardView or DeclareView("HarmonyHeavenRewardView", HarmonyHeavenConfig.ResPath .. "harmony_heaven_reward")
VIEW_DECLARE_MASK(HarmonyHeavenRewardView, ViewMask.BgBlock)
function HarmonyHeavenRewardView:HarmonyHeavenRewardView()
end

function HarmonyHeavenRewardView:LoadCallback()
end

HarmonyHeavenRewardViewF = HarmonyHeavenRewardViewF or DeclareMono("HarmonyHeavenRewardViewF", UIWFlushPanel)
function HarmonyHeavenRewardViewF:HarmonyHeavenRewardViewF()
    self.data = HarmonyHeavenData.Instance
    self.lanuage = Language.HarmonyHeaven.Reward
end

--箱子展示，倒计时自动打开（或手动打开） 播放箱子打开特效 然后播奖励展示
function HarmonyHeavenRewardViewF:onFlush()
    table.sort(self.data.reward_list_all,function(a,b)
        return a:Color() > b:Color()
    end)
    self.List:SetData(self.data.reward_list_all or {})
    self.handle_start = self.EffectTool:Play(3161119)
    UH.SetText(self.TipsText, self.lanuage.OpenBoxTip)
    self.handle_canopen = TimeHelper:AddDelayTimer(function ()
        self.OpenObj:SetActive(true)
    end, 2)
    self.auto_open_handle = TimeHelper:AddDelayTimer(function ()
        self:OnClickOpen()
    end,4)
end

function HarmonyHeavenRewardViewF:OnClickOpen()
    PublicPopupCtrl.Instance:ShowCustom(self.data.reward_list_all or {})
    self.OpenObj:SetActive(false)
    if self.auto_open_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_open_handle)
    end
    self.auto_close_handle = TimeHelper:AddDelayTimer(function ()
        self:OnClickClose()
    end,10)
    if self.handle_start then
        self.EffectTool:Stop(self.handle_start)
    end
    self.EffectTool:Play(3161116)
    UH.SetText(self.TipsText, "")
    self.handle_showreward = TimeHelper:AddDelayTimer(function ()
        -- self.ContentAnim:Play()
        self.ContentObj:SetActive(true)
        -- for k,v in pairs(self.data.reward_list_all) do
        --     ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(v), num = v.num}}
        --     -- local big_type = Item.GetBagType(v.item_id)
        --     -- if big_type and big_type == ITEM_BIG_TYPE.VIRTUAL then
        --         PublicPopupCtrl.Instance:CenterI(Item.Create(v))
        --     -- else

        --     -- end
        -- end
    end, 1.5)
    self.handle_canclose = TimeHelper:AddDelayTimer(function ()
        UH.SetText(self.TipsText, self.lanuage.AnyCloseTip)
        self.CloseObj:SetActive(true)
    end, 2)
end

function HarmonyHeavenRewardViewF:OnClickClose()
    if self.auto_close_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_close_handle)
    end
    ViewMgr:CloseView(HarmonyHeavenRewardView)
end

function HarmonyHeavenRewardViewF:OnDestroy()
    TimeHelper:CancelTimer(self.handle_canopen)
    TimeHelper:CancelTimer(self.handle_canclose)
    TimeHelper:CancelTimer(self.handle_showreward)
    TimeHelper:CancelTimer(self.auto_open_handle)
    TimeHelper:CancelTimer(self.auto_close_handle)
    UIWFlushPanel.OnDestroy(self)
    self.data.reward_list_all = nil
    self.data:AutoToNpc()

    -- TimeHelper:AddDelayTimer(function ()
    --     SceneCtrl.Instance:RequestLeaveFb()
    -- end, 1)
end
