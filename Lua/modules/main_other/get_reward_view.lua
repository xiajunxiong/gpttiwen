GetRewardView = GetRewardView or DeclareView("GetRewardView", "main_other/get_reward")

VIEW_DECLARE_MASK(GetRewardView,ViewMask.None)
VIEW_DECLARE_LEVEL(GetRewardView, ViewLevel.Lt)

function GetRewardView:LoadCallback(param_t)
    local item_list = param_t.item_list or {}
    if #item_list > 4 then
		self.Content.pivot = Vector2.New(0,0.5)
	end
    if param_t.exp ~= nil then
        self.ExpGame:SetActive(true)
        UH.SetText(self.Exp,param_t.exp or 0)
        UH.SetGold(self.ExpIcon, CurrencyType.Exp)
    elseif param_t.other_reward ~= nil then
        self.ExpGame:SetActive(true)
        UH.SetText(self.Exp, param_t.other_reward.num)
        if param_t.other_reward.gold_type ~= nil then
            UH.SetGold(self.ExpIcon, param_t.other_reward.type)
        elseif param_t.other_reward.item_id ~= nil then
            UH.SetIcon(self.ExpIcon, Item.GetIconId(param_t.other_reward.item_id))
        elseif param_t.other_reward.icon_id ~= nil then
            UH.SpriteName(self.ExpIcon, param_t.other_reward.icon_id)
        end
    else
        self.ExpGame:SetActive(false)
    end
    self.CheckBtnObj:SetActive(param_t and param_t.show_check_btn)

    UH.SpriteName(self.Title,param_t.title_name or "_LocHuoDeJiangLi")
    UH.SetText(self.Desc,param_t.desc or Language.MainOther.GetRewardTip)

    self.param_t = param_t
    self.Grid:SetData(item_list)
    self:FlushPopupView(item_list)
end

function GetRewardView:FlushPopupView(item_list)
    self.ItemTimer = Invoke(function()
        self.ItemTimer = nil
        if not self.param_t.is_not_show then
            PublicPopupCtrl.Instance:ShowCustom(item_list)
        end
    end,0.5)
end

function GetRewardView:OnClickBlock()
    if self.ItemTimer == nil then 
        ViewMgr:CloseView(GetRewardView)
    end
end

function GetRewardView:CloseCallback()
    TimeHelper:CancelTimer(self.ItemTimer)
    MainOtherCtrl.Instance:WaitViewFunc()

    if self.param_t.start_guide == 1 then 
        GuideManager.Instance:Start(138)
    end 

    local task_id = self.param_t.task_id or 0
    if task_id == Config.shengqi_auto.other[1].task_id then
        ShengQiCtrl.Instance:OpenGuide()
    end
end

function GetRewardView:OnClickGotoCheck()
    ViewMgr:CloseView(GetRewardView)
    ViewMgr:OpenView(BagView)
end