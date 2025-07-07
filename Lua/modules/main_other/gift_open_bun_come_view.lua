
GiftOpenBunComeView = GiftOpenBunComeView or DeclareView("GiftOpenBunComeView", "main_other/gift_open_bun_come")

VIEW_DECLARE_MASK(GiftOpenBunComeView,ViewMask.None)
VIEW_DECLARE_LEVEL(GiftOpenBunComeView, ViewLevel.Lt)

function GiftOpenBunComeView:GiftOpenBunComeView()
    self.data = MayDayData.Instance
    self.language = Language.MayDay.BunCome
end

function GiftOpenBunComeView:LoadCallback(param_t)
    self.param_t = param_t

    local item_list = param_t.item_list or {}
    local show_type = param_t.is_win and 1 or 2
    local is_empty = 0 == #item_list

    self.GiftItems[show_type]:SetData(item_list)
    self.GiftItems[show_type].gameObject:SetActive(true)
    self.GiftDescs[show_type].text = string.format(self.language.DescShow, self.data.bun_come_sm.num)

    -- if not is_empty then
    --     self:FlushPopupView(item_list)
    -- end
end

function GiftOpenBunComeView:OpenCallback()
    TimeHelper:CancelTimer(self.timer_dt_ac)
    if self.param_t.auto_close then
        self.timer_dt_ac = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickBlock, self), self.param_t.auto_close)
    end
end

--物品飘字
function GiftOpenBunComeView:FlushPopupView(item_list)
    self.ItemTimer = TimeHelper:AddDelayTimer(function()
        PublicPopupCtrl.Instance:ShowCustom(item_list)
        self.ItemTimer = nil
	end, 1)
end

function GiftOpenBunComeView:CloseCallback()
    TimeHelper:CancelTimer(self.ItemTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.timer_dt_ac)

    -- if self.param_t.close_func then
    --     Call(self.param_t.close_func)
    -- end
    self.data:ResetBunComeInfo()
end

function GiftOpenBunComeView:IsHasClose()
    -- self.GiftAnim:Play("gift_close")
    -- return true
    return false
end

function GiftOpenBunComeView:OnClickBlock()
    if self.ItemTimer then
        return
    end
    self.ShowTimer = Invoke(function()
        ViewMgr:CloseView(GiftOpenBunComeView)
    end,self:IsHasClose() and 1 or 0)
end