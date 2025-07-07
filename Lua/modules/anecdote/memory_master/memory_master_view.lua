
MemoryMasterView = MemoryMasterView or DeclareView("MemoryMasterView", "anecdote/memory_master", Mod.Anecdote.MemoryMaster)

function MemoryMasterView:MemoryMasterView()
    self.data = MemoryMasterData.Instance
    self.type = NewAnecdoteType.JYDS    -- 新奇闻类型，当触发新奇闻时，走新奇闻逻辑
    local param = AnecdoteData.Instance:GetNewParam(self.type, false)
    self.is_new = not TableIsEmpty(param)
    self.param = param
end

function MemoryMasterView:OnClickReturn()
    if self.is_new then
        ViewMgr:CloseView(MemoryMasterView)
        return
    end

	PublicPopupCtrl.Instance:DialogTips{
        content = Language.Anecdote.MemoryMaster,
        confirm = {
            func = function ()
                ViewMgr:CloseView(MemoryMasterView)
                ViewMgr:CloseView(DialogTipsView)
                AnecdoteCtrl.Instance:StrangerTaskFailTalk(AnecdoteType.MemoryMaster)
            end
        }
    }
end

MemoryMasterViewF = MemoryMasterViewF or DeclareMono("MemoryMasterViewF", UIWFlushPanel)

function MemoryMasterViewF:MemoryMasterViewF()
    self.data = MemoryMasterData.Instance

    self.data_cares = {
		{data = self.data.card_succ, func = self.FlushResult, init_call = false},
		{data = self.data.count_down, func = self.FlushCountDown, init_call = false},
		{data = self.data.card_effect, func = self.FlushCardEffect, init_call = false},
	}

    self.anim_handle_card = self.CardsAnim:ListenEvent("end", function (eve_param)
		if "show" == eve_param then
            for i = 1, self.CardItems:Length() do
                self.CardItems[i]:ToFront(true)
            end
        end
	end)

    self.is_fail = false

end

function MemoryMasterViewF:Awake()
    UIWFlushPanel.Awake(self)

    self.type = NewAnecdoteType.JYDS    -- 新奇闻类型，当触发新奇闻时，走新奇闻逻辑
    local param = AnecdoteData.Instance:GetNewParam(self.type, true)
    self.is_new = not TableIsEmpty(param)
    self.param = param

	self:FlushCards()
end

function MemoryMasterViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.CardsAnim:UnlistenEvent("end", self.anim_handle_card)
    TimeHelper:CancelTimer(self.timer_tt)
end

function MemoryMasterViewF:FlushCards()
    local show_list = self.data:GetShowList()
    for i = 1, self.CardItems:Length() do
        self.CardItems[i]:SetData(show_list[i], i)
    end
end

function MemoryMasterViewF:FlushResult()
    local cs = self.data.card_succ
    if 0 == cs.index then return end
    if cs.succ then
        self.CardItems[cs.index]:HideObj()
    else
        self.CardItems[cs.index]:ToBehind()
    end
    if cs.num == self.CardItems:Length() then
        TimeHelper:CancelTimer(self.timer_tt)
        self:ShowResult(true)
    end
end

function MemoryMasterViewF:FlushCountDown()
    local value = self.data.count_down.value
    if 0 == value then return end
    TimeHelper:CancelTimer(self.timer_tt)
    if value > 0 then
        local play_time = self.data:PlayTime() + 1
        self.timer_tt = TimeHelper:AddCountDownTT(function ()
            play_time = play_time - 1
            self.PlayTime.text = string.format("%sS", play_time)
        end, function ()
            self.PlayTime.text = "0S"
            self:ShowResult(false)
        end, play_time)
        self.PlayTimeObj:SetActive(true)
    else
        local ready_time = self.data:RemTime() + 1
        self.timer_tt = TimeHelper:AddCountDownTT(function ()
            ready_time = ready_time - 1
            self.PlayTime.text = string.format("%sS", ready_time)
        end, function ()
            self.PlayTime.text = "0S"
            for i = 1, self.CardItems:Length() do
                self.CardItems[i]:ToBehind()
            end
        end, ready_time)
        self.PlayTimeObj:SetActive(true)
    end
end

function MemoryMasterViewF:FlushCardEffect(result)
    local ce = self.data.card_effect
    if 0 == ce.index then return end
    self.CardItems[ce.index]:ShowEffect()
end

function MemoryMasterViewF:ShowResult(result)
    if self.is_fail then return end
    self.ResultObj:SetActive(true)
    self.SuccObj:SetActive(result)
    self.FailObj:SetActive(not result)
    if result then
        -- 成功
        if self.is_new then
            local seq = self.param.seq
            AnecdoteData.Instance:NewAnecdoteSucc(seq)
        else
            AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.MemoryMaster)
        end
    else
        self.is_fail = true
    end
end

function MemoryMasterViewF:OnClickStart()
    self.data:StartReset()
    self.BtnStartObj:SetActive(false)
	self.CardsAnim:SetTrigger(APH("show"))
end

function MemoryMasterViewF:OnClickClose()
    ViewMgr:CloseView(MemoryMasterView)
    if self.is_fail then
        -- 失败
        if self.is_new then

        else
            AnecdoteCtrl.Instance:StrangerTaskFailTalk(AnecdoteType.MemoryMaster)
        end
    end
end


MemoryMasterViewItem = DeclareMono("MemoryMasterViewItem", UIWidgetBaseItem)

function MemoryMasterViewItem:MemoryMasterViewItem()
    self.mm_data = MemoryMasterData.Instance
    self.card_index = 1
    self.card_state = 0
    self.block_click = true

    self.anim_handle_card = self.CardAnim:ListenEvent("end", function (eve_param)
		if "showed" == eve_param then
            if 1 == self.card_state then
                self.mm_data:CountDownStart(-1)
            else
                local to_behind, hide = self.mm_data:CardShowed(self.card_index, self.data.value)
                if hide then
                    self:HideObj()
                elseif to_behind then
                    self:ToBehind()
                end
            end
        elseif "show" == eve_param then
            if 1 == self.card_state then return end
            if self.mm_data:CardShow(self.card_index, self.data.value) then
                self:ShowEffect()
            end
		elseif "hide" == eve_param then
            if 1 == self.card_state then
                self.card_state = 2
                self.mm_data:CountDownStart(self.card_index)
            end
            self.block_click = false
        end
	end)
end

function MemoryMasterViewItem:SetData(data, index)
    self.card_index = index
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.CardSp, data.value, ICON_TYPE.ITEM)
end

function MemoryMasterViewItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    self.CardAnim:UnlistenEvent("end", self.anim_handle_card)
end

function MemoryMasterViewItem:ToFront(first_show)
    self.CardAnim:SetTrigger(APH("show"))
    if first_show then
        self.card_state = 1
    end
end

function MemoryMasterViewItem:ToBehind()
    self.CardAnim:SetTrigger(APH("hide"))
end

function MemoryMasterViewItem:HideObj()
    self.ItemObj:SetActive(false)
end

function MemoryMasterViewItem:OnClickCard()
    if self.block_click then return end
    self:ToFront()
end

function MemoryMasterViewItem:ShowEffect()
    self.EffectT:SetActive(true)
end