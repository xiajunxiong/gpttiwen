
GiftOpenShiTuCGView = GiftOpenShiTuCGView or DeclareView("GiftOpenShiTuCGView", "main_other/gift_open_shitu_chuangong")

VIEW_DECLARE_MASK(GiftOpenShiTuCGView,ViewMask.None)
VIEW_DECLARE_LEVEL(GiftOpenShiTuCGView, ViewLevel.Lt)

function GiftOpenShiTuCGView:GiftOpenShiTuCGView()
    self.language = Language.Society.ShiTuCG
end

function GiftOpenShiTuCGView:LoadCallback(param_t)
    self.param_t = param_t

    local item_list = param_t.item_list or {}
    local the_score = param_t.the_score or 0
    local score_record = math.max(the_score, param_t.score_record or 0)
    local is_first = 0 == (param_t.score_record or 0)
    local is_empty = 0 == #item_list
    local the_score_show = string.format(self.language.TheScoreShow, the_score)
    local score_record_show = string.format(self.language.ScoreRecordShow, score_record)

    self.ScoreShow1.text = is_first and the_score_show or score_record_show
    self.ScoreShow2.text = is_first and "" or the_score_show
    self.FirstScore:SetActive(is_first)

    self.GiftItem:SetData(item_list)
    self.GiftItem.gameObject:SetActive(true)

    self.EmptyObj:SetActive(is_empty)
    self.RewardTitle:SetActive(not is_empty)

    if not is_empty then
        self:FlushPopupView(item_list)
    end
end

function GiftOpenShiTuCGView:OpenCallback()
    TimeHelper:CancelTimer(self.timer_dt_ac)
    if self.param_t.auto_close then
        self.timer_dt_ac = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickBlock, self), self.param_t.auto_close)
    end
end

--物品飘字
function GiftOpenShiTuCGView:FlushPopupView(item_list)
    self.ItemTimer = TimeHelper:AddDelayTimer(function()
        PublicPopupCtrl.Instance:ShowCustom(item_list)
        self.ItemTimer = nil
	end, 1)
end

function GiftOpenShiTuCGView:CloseCallback()
    TimeHelper:CancelTimer(self.ItemTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.timer_dt_ac)

    -- if self.param_t.close_func then
    --     Call(self.param_t.close_func)
    -- end
end

function GiftOpenShiTuCGView:IsHasClose()
    -- self.GiftAnim:Play("gift_close")
    -- return true
    return false
end

function GiftOpenShiTuCGView:OnClickBlock()
    if self.ItemTimer then
        return
    end
    self.ShowTimer = Invoke(function()
        ViewMgr:CloseView(GiftOpenShiTuCGView)
    end,self:IsHasClose() and 1 or 0)
end