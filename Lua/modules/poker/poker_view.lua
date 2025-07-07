PokerView = PokerView or DeclareView("PokerView", "poker/poker_view")
-- VIEW_DECLARE_LEVEL(PokerView, ViewLevel.L1)
VIEW_DECLARE_MASK(PokerView, ViewMask.BgBlock)

function PokerView:CloseCallback()
    PokerData.Instance:RemovePokerInfo()
end

PokerPanel = PokerPanel or DeclareMono("PokerPanel", UIWFlushPanel)
function PokerPanel:PokerPanel()
    self.info = PokerData.Instance:PokerInfo()
    if self.info == nil then
        ViewMgr:CloseView(PokerView)
        return
    end
    if self.info:SmartData() ~= nil then
        self.data_cares = {{data = self.info:SmartData(), func = self.onFlush}}
    end
    self.flush_events = {EventProtocol.ToEventKey(SCRoleDrawInfo)}

    self.handle_runer_list = {}
    self.poker_open_list = {}
    self.poker_wait_list = {}
    self.time_end = false
    self:TimeChoose()
end

function PokerPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for index, value in ipairs(self.handle_runer_list or {}) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_runer_list = {}
    if self.wait_timer ~= nil then
        TimeHelper:CancelTimer(self.wait_timer)
        self.wait_timer = nil
    end
    if self.handle_delay1 ~= nil then
        TimeHelper:CancelTimer(self.handle_delay1)
        self.handle_delay1 = nil
    end
end

function PokerPanel:OnClickSkip()
    ViewMgr:CloseView(PokerView)
end

function PokerPanel:onFlush()
    if self.info == nil then
        return
    end
    local role_poker = false
    self.show_list = self.info:GetShowList()
    local show_list = self.show_list or {}
    local open_num = 0
    for i = 1, SIDE_MAX_ROLE_NUM do
        local info = show_list[i]
        if info ~= nil and info.index ~= -1 then
            if self.poker_open_list[i] == nil then
                if info.role_id == RoleData.Instance:GetRoleId() then
                    self:OpenOnePoker(i)
                    role_poker = true
                else
                    self.poker_wait_list[i] = info
                end
            end
        end
        if info ~= nil then
            if info.role_id == RoleData.Instance:GetRoleId() and info.index ~= -1 then
                open_num = open_num + 1
            end
        end
    end

    local function func_open()
        for i = 1, SIDE_MAX_ROLE_NUM do
            if self.poker_wait_list[i] ~= nil then
                self:OpenOnePoker(i)
            end
        end
        self.handle_delay1 = nil
        if self:IsAllOpen() and self.time_end == false then
            self.time_end = true
            if self.wait_timer ~= nil then
                TimeHelper:CancelTimer(self.wait_timer)
                self.wait_timer = nil
            end
            self:TimeClose()
        end
    end
    --真人玩家点击牌面，等所选牌面先翻过来后，剩余的牌一起翻转
    if self.handle_delay1 == nil then
        if role_poker then
            self.handle_delay1 = TimeHelper:AddDelayTimer(func_open, 0.5)
        else
            func_open()
        end
    end
    self.SkipObj:SetActive(self:IsSelfSelect())
    if self.info.poker_type and self.info.poker_type == POKER_REQ_OPER_TYPE.TYPE_MI_YI then
        self:ConspiracyNum()
    else
        for i = 1, 3, 1 do
            self.ConspTitle[i]:SetActive(false)
        end
    end

    if POKER_REQ_OPER_TYPE.PET_GOD == self.info.poker_type or POKER_REQ_OPER_TYPE.PET_GOD_SWEEP == self.info.poker_type then
        self.TipsShow.text = string.format(Language.Fight.PetGod.PokerNum, open_num)
    end
end
function PokerPanel:ConspiracyNum()
    local total = ConspiracyData.Instance:GetOffList()[1].time
    local num = ConspiracyData.Instance:JoinTimes()
    num = num > total and total or num
    UH.SpriteName(self.ConspSp, "DaoJiShiShuZi"..num)
    local result = ConspiracyData.Instance:BattleResult()
    for i = 1, 3, 1 do
        self.ConspTitle[i]:SetActive(result == i)
    end
end

function PokerPanel:IsAllOpen()
    if self.show_list ~= nil then
        for i = 1, SIDE_MAX_ROLE_NUM do
            local info = self.show_list[i]
            if info ~= nil and info.role_id ~= 0 and info.index == -1 then
                return false
            end
        end
    end
    return true
end

function PokerPanel:ShowEffect(obj, effect_id)
    local function func_effect()
        self.EffectTool:Play(effect_id, obj.gameObject)
        return true
    end
    return Runner.Instance:RunUntilTrue(func_effect)
end

function PokerPanel:OpenAll()
    local show_list = self.show_list or {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        self:OpenOnePoker(i)
    end
    self:TimeClose()
end

function PokerPanel:OpenOnePoker(i)
    if self.show_list ~= nil and self.show_list[i] ~= nil and self.poker_open_list[i] == nil then
        local info = self.show_list[i]
        self.poker_open_list[i] = info
        self.poker_wait_list[i] = nil

        local main_item = Item.Create(info.item)
        self.CellList[i]:SetData(main_item)
        local colorStr = QualityColorStrShallow[main_item:Color() or 1]
        UH.SetText(self.ItemNameList[i], ColorStr(main_item:Name(), colorStr))

        UH.SetText(self.NameList[i], info.role_name)
        if info.other_item ~= nil then
            self.ItemRewardList[i].gameObject:SetActive(true)
            UH.SetText(self.ItemRewardList[i], string.format(Language.Conspiracy.ItemReward, info.other_item.num))
            UH.SpriteName(self.ItemRewardSp[i], info.other_item.item_id)
        else
            self.ItemRewardList[i].gameObject:SetActive(false)
        end
        if self.ResultList[i].eulerAngles.y == 90 then
            self.ItemAnim[i]:Play()
            local effect_id = self.info:EffectId(info.item.item_id)
            if effect_id ~= nil then
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
                table.insert(self.handle_runer_list, self:ShowEffect(self.ResultList[i], effect_id))
            end
        end
    end
end

function PokerPanel:OnClickOpen(index)
    -- index 从1 开始的
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if not self:IsSelfSelect() and self.info:GetGroupId() ~= nil then
        PokerCtrl.Instance:SendPokerReq(self.info:GetGroupId(), index - 1)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[152])
    end
end
--翻牌时间
function PokerPanel:TimeChoose()
    local time = self.info:GetTimeChoose() or 10
    self.Time:TotalTime(time, TimeType.Type_Special_5)
    self.Time:SetCallBack(BindTool.Bind1(self.TimeDelay, self))
end
--等待服务器自动翻牌
function PokerPanel:TimeDelay()
    if self.wait_timer == nil and self.time_end == false then
        local function func_end()
            self.time_end = true
            self:OpenAll()
        end
        self.wait_timer = TimeHelper:AddDelayTimer(func_end, 3)
    end
end
--展示时间
function PokerPanel:TimeClose()
    local time = self.info:GetTimeClose() or 3
    self.Time:TotalTime(time, TimeType.Type_Special_5)
    local function func_end()
        ViewMgr:CloseView(PokerView)
    end
    self.Time:SetCallBack(func_end)
end

function PokerPanel:IsSelfSelect()
    if self.info ~= nil then
        local show_list = self.show_list or {}
        local role_id = RoleData.Instance:GetRoleId()
        local can_open = false
        for i = 1, #show_list do
            if show_list[i] ~= nil and show_list[i].role_id == role_id and show_list[i].index == -1 then
                return false
            end
        end
    end
    return true
end
