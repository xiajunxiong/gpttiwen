YunZeView = YunZeView or DeclareView("YunZeView","serveractivity/yun_ze/yun_ze_view", Mod.YunZe.Main)
VIEW_DECLARE_LEVEL(YunZeView, ViewLevel.L0)
function YunZeView:OnCloseClick()
    ViewMgr:CloseView(YunZeView)
end

YunZePanel = YunZePanel or DeclareMono("YunZePanel", UIWFlushPanel)
function YunZePanel:YunZePanel()
    self.data = YunZeData.Instance
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushPanel, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushBoos, init_call = false},
        {data = self.data.remind_data, func = self.FlushPanel, init_call = false},
    }
end

function YunZePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.anim_ht = TimeHelper:AddRunTimer(function ()
        self.BubbleAnim:Play()
    end,5,-1,false)
    self:FlushPanel()
end

function YunZePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
    TimeHelper:CancelTimer(self.anim_ht)
end

function YunZePanel:FlushPanel()
    for i = 1, self.ActItems:Length() do
        local data = {plan_type = i}
        local open_act, not_open_act = self.data:GetPlanActInfo(i)
        data.cur_act = open_act[1]
        data.next_act = not_open_act[1]
        data.act_list = open_act
        data.not_act_list = not_open_act
        self.ActItems[i]:SetData(data)
    end
    self:FlushBoos()
end

function YunZePanel:FlushBoos()
    local cfg = self.data:GetBoosActCfg()
    if TableIsEmpty(cfg) then
        return
    end
    self.Bubble:SetActive(false)
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.Bubble:SetActive(true)
        self.BubbleAnim:Play()
    end)
    local icon = Item.GetIconId(cfg.icon)
    local num = Item.GetNum(cfg.icon)
    UH.SetIcon(self.CurrIcon, icon)
    UH.SetText(self.CurrNum, num)
    
    UH.SetText(self.BoosActName, cfg.text)
    if cfg.act_type == ACTIVITY_TYPE.RAND_CLOUD_SWAMP then
        local section_num = TimeCtrl.Instance:GetActOpenDay(cfg.act_type)
        local num = 0
        for i,v in ipairs(Config.zaizhanyunze_auto.basics) do
            if section_num >= v.section_start and section_num <= v.section_end then
                num = v.seq + 1
                break
            end
        end
        UH.SetText(self.BubbleTip, string.format(Language.YunZe.Txt1, num))
    else
        UH.SetText(self.BubbleTip, "")
    end
end

function YunZePanel:OnBoosClick()
    local cfg = self.data:GetBoosActCfg()
    if ActivityData.IsOpen(cfg.act_type) then
        ViewMgr:OpenViewByKey(cfg.mod_key)
    else
        PublicPopupCtrl.Instance:Error(Language.YunZe.act_status_2)
    end
end

YunZeActItem = YunZeActItem or DeclareMono("YunZeActItem", UIWidgetBaseItem)
-- data : {plan_type, cur_act, next_act, act_list, not_act_list} cur_act : Config.activity_main_auto.yunzemengce[index]
function YunZeActItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if not data.cur_act then
        return
    end
    local info = data.cur_act
    local act_type = info.act_type
    local act_mod = info.mod_key or 0
    local act_nmae = ActivityRandData.GetName(act_type)
    UH.SetText(self.Name, act_nmae or "")--Language.YunZe.PlanName[data.plan_type] or "")
    UH.SetText(self.TabText, info.label)
    local is_open = ActivityData.IsOpen(act_type)
    local status_tip = is_open and Language.YunZe.act_status_1 or Language.YunZe.act_status_2
    --local is_new = ActivityData.Instance:IsHasSign(act_type) and is_open
    --self.NewFlag:SetActive(is_new == true)
    self.NewPoint:SetNum(0)--(is_new == true and 1 or 0)
    UH.SetText(self.StatusText, status_tip)
    for i,v in pairs(data.act_list) do
        self:FlushRemindView(v.mod_key)
    end
    self:FlushRedNum()
end

function YunZeActItem:FlushRemindView(mod)
    if not self.handle_list then
        self.handle_list = {}
    end
    local mod_t = TabMod(mod)
    if self.handle_list[mod] == nil then
        local handle_info = {}
        handle_info.mod_t = mod_t
        handle_info.handle = Remind.Instance:RegisterGroup(mod_t, function()
            self:FlushRedNum()
        end, false)
        self.handle_list[mod] = handle_info
    end
end

function YunZeActItem:FlushRedNum()
    for i,v in pairs(self.data.act_list) do
        local act_type = v.act_type
        local is_open = ActivityData.IsOpen(act_type)
        local is_new = ActivityData.Instance:IsHasSign(act_type) and is_open
        if is_new then
            self.RedPoint:SetNum(1)
            return
        end

        local mod_t = TabMod(v.mod_key)
        local num = Remind.Instance:GetGroupNum(mod_t)
        if num > 0 then
            self.RedPoint:SetNum(1)
            return
        end
    end
    self.RedPoint:SetNum(0)
end

function YunZeActItem:OnClick()
    if not self.data.cur_act then
        return
    end

    for i,v in pairs(self.data.act_list) do
        local act_type = v.act_type
        local is_open = ActivityData.IsOpen(act_type)
        if is_open then
            ActivityData.Instance:SetHasSign(act_type)
        end
    end

    self:FlushRedNum()

    local is_open = ActivityData.IsOpen(self.data.cur_act.act_type)
    if self.data.plan_type == 3 then
        ViewMgr:OpenView(YunZeActInfoView, {data = self.data})
    elseif is_open then
        ViewMgr:OpenViewByKey(self.data.cur_act.mod_key)
    else
        PublicPopupCtrl.Instance:Error(Language.YunZe.act_status_2)
    end
    
end

function YunZeActItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i,v in pairs(self.handle_list or {}) do
        local mod_t = v.mod_t
        local handle = v.handle
        Remind.Instance:UnRegisterGroup(mod_t, handle)
    end
end

YunZeActInfoView = YunZeActInfoView or DeclareView("YunZeActInfoView","serveractivity/yun_ze/yun_ze_act_info")
function YunZeActInfoView:YunZeActInfoView()
    self.act_index = 1
end
-- param_t : YunZeActItem:GetData()
function YunZeActInfoView:LoadCallback(param_t)
    param_t = param_t or {}
    local data = param_t.data
    self.act_list = data.act_list or {}
    self.not_act_list = data.not_act_list or {}
    self.NextBtnObj:SetActive(#self.act_list > 1)
    if not data then
        LogError("打开 YunZeActInfoView 必须传入数据")
        return
    end
    self:FlushShowAct()
end

function YunZeActInfoView:FlushShowAct()
    local act_info = self.act_list[self.act_index]
    if not act_info then
        return
    end
    self.CurActInfo:SetData(act_info)
    UH.SetText(self.CurName, act_info.text)
    local next_info = self.not_act_list[self.act_index]
    self.NextInfoObj:SetActive(next_info ~= nil)
    if next_info then
        UH.SetText(self.NextName, next_info.text)
        local is_open = ActivityData.IsOpen(next_info.act_type)
        local status_tip = is_open and Language.YunZe.act_status_1 or ColorStr(Language.YunZe.act_status_2, COLORSTR.Yellow11)
        UH.SetText(self.NextStatus, status_tip)
        UH.SetText(self.NextTime, YunZeData.Instance:GetTimeDesc(next_info.act_type))
    end
end

function YunZeActInfoView:OnCloseClick()
    ViewMgr:CloseView(YunZeActInfoView)
end

function YunZeActInfoView:OnNextClick()
    self.act_index = self.act_index + 1
    if self.act_index > #self.act_list then
        self.act_index = 1
    end
    self:FlushShowAct()
end

YunZeActInfoItem = YunZeActInfoItem or DeclareMono("YunZeActInfoItem", UIWidgetBaseItem)
-- data: Config.activity_main_auto.yunzemengce[index]
function YunZeActInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local act_type = data.act_type
    local act_mod = data.mod_key or 0
    local act_name = ActivityRandData.GetName(act_type)
    UH.SetText(self.Name, act_name)
    UH.SetText(self.Desc, data.describe)
    local is_open = ActivityData.IsOpen(act_type)
    local status_tip = is_open and Language.YunZe.act_status_1 or ColorStr(Language.YunZe.act_status_2, COLORSTR.Yellow11)
    UH.SetText(self.StatusText, status_tip)
    self.Interactor.Interactable = is_open
    self.EnterEffect:SetActive(is_open)
    UH.SetText(self.Time, YunZeData.Instance:GetTimeDesc(act_type))
    if data.reward_item_multi == 1 then
        local rewards_data = YunZeData.Instance:GetRewardPreview(act_type)
        local rewards_item = DataHelper.FormatItemList(rewards_data)
        self.Rewards:SetData(rewards_item)
    end
    self:FlushRemindView(data.mod_key)
end

function YunZeActInfoItem:OnEnterClick()
    local is_open = ActivityData.IsOpen(self.data.act_type)
    if not is_open then
        PublicPopupCtrl.Instance:Error(Language.YunZe.act_status_2)
        return
    end
    local mod_key = self.data.mod_key or 0
    ViewMgr:OpenViewByKey(mod_key)
    ViewMgr:CloseView(YunZeActInfoView)
end

function YunZeActInfoItem:FlushRemindView(mod)
    Remind:UnRegisterGroup(group_id, handle)
    local mod_t = TabMod(mod)
    local num = Remind.Instance:GetGroupNum(mod_t)
    self.RedPoint:SetNum(num)
end

YunZeActInfoRewardItemCell = YunZeActInfoRewardItemCell or DeclareMono("YunZeActInfoRewardItemCell", UIWidgetBaseItem)
function YunZeActInfoRewardItemCell:SetData(item)
    UIWidgetBaseItem.SetData(self, item)
    self.Cell:SetData(item)
end