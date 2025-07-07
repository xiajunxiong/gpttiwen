VitalityFlop = VitalityFlop or DeclareView('VitalityFlop', 'vitality_flop/vitality_flop', Mod.MayDay.VitalityFlop)
function VitalityFlop:VitalityFlop()
end
VitalityFlopPanel = VitalityFlopPanel or DeclareMono('VitalityFlopPanel', UIWFlushPanel)
function VitalityFlopPanel:VitalityFlopPanel()
    self.v2Pos = Vector2.zero
    self.data = VitalityFlopData.Instance
    self.data_cares = {
        {data = self.data:Info(), func = self.FlushCount, keys = {"times"}},
        {data = self.data:Info(), func = self.StateChange, keys = {"state"}},
        {data = self.data:Info(), func = self.FlushExItem, keys = {"exchange"}},
        {data = BagData.Instance.item_grid, func = self.ItemChange, init_call = false}
    }
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
end

function VitalityFlopPanel:ItemChange()
    self:FlushExItem()
end

function VitalityFlopPanel:StateChange()
    local state = self.data:Info().state
    local items = self.CardList.item_list
    local select_idx = self.data.select_idx
    local item = self.data:CardReward()
    if state == 1 and select_idx ~= nil and items and select_idx > 0 and item then
        --对选中item进行翻转
        for k, v in pairs(items) do
            if k ~= select_idx then
                v:HideEffect()
            end
        end
        items[select_idx]:SetData(nil, item)
        items[select_idx]:Open()
        self.handle_wait = TimeHelper:AddDelayTimer(function ()
            for k, v in pairs(items) do
                if k ~= select_idx then
                    v:Open()
                end
            end
        end, 1)
        
        self.handle_close = TimeHelper:AddDelayTimer(function ()
            for k, v in pairs(items) do
                v:Close()
            end
        end, 5)
        self.handle_flush = TimeHelper:AddDelayTimer(function ()
            local data = self.data:RamdonList()
            self.CardList:SetData(data)
            self.CardList:SetCompleteCallBack(function ()
                self.data:SetCardReward(nil)
                self.data:Info().state = 0
                self.data.select_idx = 0
            end)
        end,6)
    end
    self:FlushExItem()
end
function VitalityFlopPanel:onFlush()
	UH.SetText(self.Time, string.format(Language.VitalityFlop.ActTime, self.data:GetActTime()))
    
    self:FlushExItem()
	self:FlushCard()
    self:FlushPreview()
	-- self:FlushCount()
end
function VitalityFlopPanel:FlushExItem()
    local item = self.data:ExItem()
    UH.SetIcon(self.Icon, item:IconId())
    UH.SetText(self.ItemNum, Item.GetNum(item.item_id))
    self.ExRedPoint:SetNum(self.data:CanExchange() and 1 or 0)
end
function VitalityFlopPanel:OnClickIcon()
    local item = self.data:ExItem()
    CellClicks.BagGridNoButton(item)
end
function VitalityFlopPanel:FlushPreview()
    local data = self.data:PreviewReward()
    for i, v in ipairs(data) do
        v.index = 0
        v.count = table.nums(data)
        v.order = i
    end
	self.RollView.alpha = 0
    self.RollList:SetData(data)
    self.RollList:SetCompleteCallBack(
        function()
            self.v2Pos:Set(0, self.RollContent.sizeDelta.y)
            self.RollLayoutGroup.enabled = false
            self.RollContent.anchoredPosition = self.v2Pos
			self.RollView.alpha = 1
            self:ShowRewardMove(data)
        end
    )
end
function VitalityFlopPanel:FlushCount()
	local data = self.data:CountReward()
	self.CountList:SetData(data)
end
function VitalityFlopPanel:FlushCard()
	local data = self.data:RamdonList()--GetActReward()
    self.data:Info().state = -1
    self.CardGroup.alpha = 0
	self.CardList:SetData(data)
    self.CardList:SetCompleteCallBack(function ()
        local items = self.CardList.item_list
        for k, v in pairs(items) do
            v.gameObject:SetActive(false)
        end
        self.CardGroup.alpha = 1
        local total = table.nums(items)
        local index = 1
        local temp
        self.handle_runtime2 = TimeHelper:AddRunTimer(function ()
            temp = items[index]
            temp.gameObject:SetActive(true)
            temp:Close()
            index = index + 1
            if index > total then
                self.data:Info().state = 0
                TimeHelper:CancelTimer(self.handle_runtime2)
            end
        end, 0.1)
    end)
end

function VitalityFlopPanel:OnClickExchange()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
	ViewMgr:OpenView(VitalityFlopExchange)
end
function VitalityFlopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:Info().state = -1
    TimeHelper:CancelTimer(self.handle_runtime2)
    TimeHelper:CancelTimer(self.handle_runtime)
    TimeHelper:CancelTimer(self.handle_wait)
    TimeHelper:CancelTimer(self.handle_close)
    TimeHelper:CancelTimer(self.handle_flush)
end
function VitalityFlopPanel:ShowRewardMove(reward)
    -- 从上往下方向反了
    self.move_record = {}
    local index = 0
    for k, v in pairs(reward) do
        index = index + 1
        table.insert(self.move_record, index, -1)
    end
    if self.handle_runtime == nil then
        self.handle_runtime = TimeHelper:AddRunTimer(BindTool.Bind(self.Rolling, self, reward), 0)
    end
end

function VitalityFlopPanel:Rolling(reward)
    local y = self.RollContent.anchoredPosition.y - 90 * Time.deltaTime
    local reward_count = table.nums(reward) or 0
    local have_move = math.floor((reward_count - 3) * 140 - y)
     -- 一轮 -420 超过3个后开始改变
    local move_count = math.floor(have_move / 140)
     --item height 140
    local yu_index = math.fmod(move_count, reward_count)
    local zheng_index = math.modf(move_count, reward_count)
    if move_count > 0 then
        local add_index = yu_index
        if add_index == 0 then
            add_index = reward_count
        end

        add_index = reward_count - add_index + 1 --方向向下了

        for k, v in pairs(reward) do
            if v.order == add_index then
                if self.move_record[add_index] ~= move_count then
                    self.move_record[add_index] = move_count
                    v.index = v.index + 1
                    self.RollList:SetItemDataByKey(k, v)
                    break
                end
            end
        end
    end
    self.v2Pos:Set(0, y)
    self.RollContent.anchoredPosition = self.v2Pos
end
function VitalityFlopPanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(VitalityFlop)
end
VitalityFlopRollItem = VitalityFlopRollItem or DeclareMono('VitalityFlopRollItem', UIWidgetBaseItem)
function VitalityFlopRollItem:SetData(data)
    UH.SetText(self.Count, string.format(Language.VitalityFlop.Rate, (data.reward_rate / 10000) .. '%'))
    self.Cell:SetData(Item.Create(data.reward_item))
    if data.index > 0 then
        local tempY = self.ItemRect.anchoredPosition.y
        local y = tempY + data.count * 140
        self.ItemRect.anchoredPosition = Vector2.New(71.42, y)
    end
end

VitalityFlopCountItem = VitalityFlopCountItem or DeclareMono('VitalityFlopCountItem', UIWidgetBaseItem)
function VitalityFlopCountItem:VitalityFlopCountItem()
    self.flop_data = VitalityFlopData.Instance
end
function VitalityFlopCountItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.Cell:SetData(Item.Create(data.reward_item))
	UH.SetText(self.Count, string.format(Language.VitalityFlop.Count,self.flop_data:Info().count > data.task and data.task or self.flop_data:Info().count, data.task))
    local is_get = self.flop_data:IsGet(data.seq)
	self.IsGet:SetActive(is_get)
    local can_get = self.flop_data:CanGet(data.task)
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
        self.handle_effect = nil
    end
    self.RedPoint:SetNum(0)
    if not is_get and can_get then
        self.handle_effect = self.EffectTool:Play(6165155)
        self.RedPoint:SetNum(1)
    end
end
function VitalityFlopCountItem:OnClickGet()
    if self.flop_data:IsGet(self.data.seq) then
        CellClicks.BagGridNoButton(self.Cell)
    else
        if self.flop_data:Info().count < self.data.task then
            PublicPopupCtrl.Instance:Center(Language.VitalityFlop.CountTip)
            CellClicks.BagGridNoButton(self.Cell)
        else
            VitalityFlopCtrl.ReceiveTimes(self.data.seq)
        end
    end
end

VitalityFlopCardItem = VitalityFlopCardItem or DeclareMono('VitalityFlopCardItem', UIWidgetBaseItem)
function VitalityFlopCardItem:VitalityFlopCardItem()
    self.flop_data = VitalityFlopData.Instance
end
function VitalityFlopCardItem:SetData(data, reward)
    if data then
        UIWidgetBaseItem.SetData(self, data)
    end
    if reward then
        self.Cell:SetData(reward)
    else
        self.Cell:SetData(Item.Create(data.reward_item))
    end
	-- UH.SetText(self.Count, string.format(Language.VitalityFlop.Count, data.task))
	-- self.IsGet:SetActive(false)
end

function VitalityFlopCardItem:OnClickCard()
    if self.Anim.isPlaying then
        return
    end
    if self.flop_data:Info().state == 0 then
        local item = self.flop_data:ExItem()
        if Item.GetNum(item.item_id) == 0 then
            ViewMgr:OpenView(VitalityFlopExchange)
            return
        end
        self.flop_data:Info().state = 0.5
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.TreasureHuntOpen)
        self.EffectObj:SetActive(true)
        self.flop_data.select_idx = self.data.fuc_seq
        VitalityFlopCtrl.Lottery()
        return
    end
end
function VitalityFlopCardItem:HideEffect()
    self.EffectObj:SetActive(false)
end
function VitalityFlopCardItem:Open()
    if self.Anim.isPlaying then
        return
    end
    self.Anim:Play("open")
end
function VitalityFlopCardItem:Close()
    self.Anim:Play("close")
end
