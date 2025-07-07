TigerMoraView = TigerMoraView or DeclareView("TigerMoraView", "serveractivity/tiger_year/tiger_mora", Mod.TigerYear.TigerMora)
VIEW_DECLARE_LEVEL(TigerMoraView, ViewLevel.L1)

function TigerMoraView:CloseCallback()
    BagData.Instance:SetHideCurrencyCenterFlag(false)
end

function TigerMoraView:OnClickClose()
    ViewMgr:CloseView(TigerMoraView)
end

function TigerMoraView:OnClickHelp()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    PublicPopupCtrl.Instance:HelpTip(220)
end

TigerMoraPanel = TigerMoraPanel or DeclareMono("TigerMoraPanel", UIWFlushPanel)
function TigerMoraPanel:TigerMoraPanel()
    self.data_cares = {
        {data = TigerMoraData.Instance.act_info, func = self.FlushView},
        {data = TigerMoraData.Instance.pk_result, func = self.OnResult, init_call = false}
    }
    self.left_pic, self.right_pic = -1, -1
end

function TigerMoraPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:InitView()
    self:FlushReward()
    self.ActTimer:CreateActTime(ACTIVITY_TYPE.RAND_TIGER_MORA, TimeType.Type_Special_4, Language.TigerMora.Txt2)
    BagData.Instance:SetHideCurrencyCenterFlag(true)
end

function TigerMoraPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.pk_timer)
    self.pk_timer = nil
    TimeHelper:CancelTimer(self.send_timer)
    self.send_timer = nil
    TimeHelper:CancelTimer(self.invoke_timer)
    self.invoke_timer = nil
end

function TigerMoraPanel:InitView()
    local last_info = TigerMoraData.Instance:GetLastGameInfo()
    self.PkItemL:SetStateActive(last_info.is_win ~= 0)
    self.PkItemR:SetStateActive(last_info.is_win ~= 0)
    if last_info.is_win ~= 0 then
        self.PkItemL:SetStateImage(last_info.is_win == 2)
        self.PkItemR:SetStateImage(last_info.is_win == 1)
        self.left_pic = last_info.left_pic
        self.right_pic = last_info.right_pic
    else
        self.left_pic = 3
        self.right_pic = 1
    end
    self.AnimL[self.left_pic]:SetActive(true)
    self.AnimR[self.right_pic]:SetActive(true)

    self.PkItemL:SetData(nil)
    self.PkItemR:SetData(nil)
end

function TigerMoraPanel:FlushView()
    if self.pk_timer ~= nil then
        return
    end
    local max_times = TigerMoraData.Instance:GetMaxTimes()
    local remain_times = TigerMoraData.Instance:GetRemainTimes()
    UH.SetText(self.RemainTimes, Format(Language.TigerMora.RemainTimes, remain_times, max_times))
end

function TigerMoraPanel:FlushReward()
    local reward_cfg = TigerMoraData.Instance:GetRewardCfg()

    local win_show_list = {}
    for i, v in ipairs(TigerMoraData.Instance:GetRewardList(reward_cfg.win_rate_group)) do
        self:LinkItems(win_show_list, v.win_reward_item, true)
    end
    self:LinkItems(win_show_list, reward_cfg.win_reward_item, false)
    self.WinItemList:SetData(win_show_list)

    local fail_show_list = {}
    for i, v in ipairs(TigerMoraData.Instance:GetRewardList(reward_cfg.fail_rate_group)) do
        self:LinkItems(fail_show_list, v.win_reward_item, true)
    end
    self:LinkItems(fail_show_list, reward_cfg.fail_reward_item, false)
    self.FailItemList:SetData(fail_show_list)
end
function TigerMoraPanel:LinkItems(show_list, item_list, is_rate)
    for i = 0, #item_list do
        table.insert(show_list, {is_rate = is_rate, item = Item.Create(item_list[i])})
    end
end

function TigerMoraPanel:OnClickPK()
    if self.pk_timer ~= nil or self.invoke_timer ~= nil then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if TigerMoraData.Instance:GetRemainTimes() > 0 then
        self:DoPKAnim()
    else
        PublicPopupCtrl.Instance:Center(Language.TigerMora.NoTimes)
    end
end

function TigerMoraPanel:DoPKAnim()
    self.PkItemL:SetStateActive(false)
    self.PkItemR:SetStateActive(false)
    --直接SetActive有点坑，按钮动画没播完隐藏会导致按钮一直处于放大状态。
    self.FreeNode:SetLocalPosition(Vector3.New(0, -20000, 0))
    self.pk_timer = TimeHelper:AddRunTimer(BindTool.Bind1(self.UpdateCall, self), 0.2, nil, true)
    self.send_timer = Invoke(BindTool.Bind1(self.DelaySendPK, self), 3)
end

function TigerMoraPanel:DelaySendPK()
    TigerMoraCtrl.Instance:SendPK()
end

function TigerMoraPanel:UpdateCall()
    self.AnimL[self.left_pic]:SetActive(false)
    self.left_pic = self:GetNextPic(self.left_pic)
    self.AnimL[self.left_pic]:SetActive(true)

    self.AnimR[self.right_pic]:SetActive(false)
    self.right_pic = self:GetNextPic(self.right_pic)
    self.AnimR[self.right_pic]:SetActive(true)
end

function TigerMoraPanel:OnResult()
    TimeHelper:CancelTimer(self.pk_timer)
    self.pk_timer = nil
    local last_info = TigerMoraData.Instance:GetLastGameInfo()
    if last_info.is_win == 0 then
        return
    end
    self.AnimL[self.left_pic]:SetActive(false)
    self.left_pic = last_info.left_pic
    self.AnimL[self.left_pic]:SetActive(true)

    self.AnimR[self.right_pic]:SetActive(false)
    self.right_pic = last_info.right_pic
    self.AnimR[self.right_pic]:SetActive(true)

    self.PkItemL:SetStateActive(true)
    self.PkItemL:SetStateImage(last_info.is_win == 2)
    self.PkItemR:SetStateActive(true)
    self.PkItemR:SetStateImage(last_info.is_win == 1)
    self.PKAnim:Play("show_result")
    self.WinEffect:SetActive(last_info.is_win == 1)

    self.invoke_timer = Invoke(BindTool.Bind1(self.ToNomal, self), 1.5)
end

function TigerMoraPanel:ToNomal()
    self.invoke_timer = nil
    self:FlushView()
    self.FreeNode:SetLocalPosition(Vector3.New(0, 0, 0))
    self.WinEffect:SetActive(false)
end

function TigerMoraPanel:GetNextPic(cur_pic)
    local list = {}
    for i = 1, 3 do
        if i ~= cur_pic then
            table.insert(list, i)
        end
    end
    local index = GameMath.Ramdon(1, #list)
    return list[index]
end

TigerMoraBaseItem = TigerMoraBaseItem or DeclareMono("TigerMoraBaseItem", UIWidgetBaseItem)
function TigerMoraBaseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function TigerMoraBaseItem:SetStateActive(is_active)
    self.State:SetActive(is_active)
end

function TigerMoraBaseItem:SetStateImage(is_win)
    local text = is_win and "Win" or "Fail"
    UH.SpriteName(self.StateBG, text .. "BG")
    UH.SpriteName(self.StateText, "_Loc_" .. text)
end

TigerMoraLeftItem = TigerMoraLeftItem or DeclareMono("TigerMoraLeftItem", TigerMoraBaseItem)
function TigerMoraLeftItem:SetData(data)
    TigerMoraBaseItem.SetData(self, data)

    local pet_config = PetData.Instance:GetPetCfg(TigerMoraData.Instance:GetPetId())
    UH.SetIcon(self.ImgSetPetIcon, pet_config.icon_id)
    UH.SpriteName(self.ImgSetFrameQua, PetData.PinZhi[pet_config.quality])
end

TigerMoraRightItem = TigerMoraRightItem or DeclareMono("TigerMoraRightItem", TigerMoraBaseItem)
function TigerMoraRightItem:SetData(data)
    TigerMoraBaseItem.SetData(self, data)
    self.Avatar:SetAvatar(RoleData.Instance:GetRoleAvatarType(), RoleData.Instance:GetRoleAvatarId())
end

TigerMoraRewardItem = TigerMoraRewardItem or DeclareMono("TigerMoraRewardItem", UIWidgetBaseItem)
function TigerMoraRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Rate:SetActive(data.is_rate)
    self.ItemCell:SetData(Item.Create(data.item))
end
