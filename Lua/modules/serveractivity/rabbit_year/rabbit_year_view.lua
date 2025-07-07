-- 兔年春节活动汇总
RabbitYearView = RabbitYearView or DeclareView("RabbitYearView","serveractivity/rabbit_year/rabbit_year_view", Mod.RabbitYear.Main)
function RabbitYearView:OnCloseClick()
    ViewMgr:CloseView(RabbitYearView)
end


RabbitYearPanel = RabbitYearPanel or DeclareMono("RabbitYearPanel", UIWFlushPanel)
function RabbitYearPanel:RabbitYearPanel()
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushPanel, init_call = false},
    }
    self.act_list = Config.activity_main_auto.tiger_spring_festival
end

function RabbitYearPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function RabbitYearPanel:FlushPanel()
    for i = 1, self.ActList:Length() do
        local mono = self.ActList[i]
        local data = self.act_list[i]
        mono:SetObjActive(data ~= nil)
        if data then
            mono:SetData(data)
        end
    end
end


RabbitYearActItem = RabbitYearActItem or DeclareMono("RabbitYearActItem", UIWidgetBaseItem)
function RabbitYearActItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local act_type = data.act_type
    local is_open = ActivityData.IsOpen(act_type)
    local act_mod = data.mod_key or 0
    local desc = data.desc or ""
    local name_img = self:GetNameImg(data.seq)
    local time_desc = self:GetTimeDesc(act_type)

    UH.SpriteName(self.NameImg, name_img)
    UH.SetText(self.Desc, desc)
    UH.SetText(self.TimeDesc, time_desc)
    UH.SetInter(self.Inter, is_open)
    self.TitleEffect:SetActive(is_open)

    self:FlushRedNum()
end

function RabbitYearActItem:GetNameImg(seq)
    local t = {
        [1] = "_LocYingChunNaFu",
        [2] = "_LocFuYunShangCheng",
        [3] = "_LocHongYunLeiChong",
        [4] = "_LocXingXingDianDeng",
    }
    return t[seq] or ""
end

-- 获取活动时间描述
function RabbitYearActItem:GetTimeDesc(act_type)
    local is_open = ActivityData.IsOpen(act_type)
    local open_time, end_time
    if is_open then
        open_time = ActivityData.GetStartStampTime(act_type)
        end_time = ActivityData.GetSwitchStampTime(act_type)
    else
        open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
    end
    if open_time and end_time then
        end_time = end_time - 1
        local begin_month,end_month = os.date("%m",open_time),os.date("%m",end_time)
        local open_day,end_day = os.date("%d",open_time),os.date("%d",end_time)
        return Format(Language.TigerYearTotal.ActTimeDesc,begin_month,open_day,end_month,end_day)
    end
    return ""
end

function RabbitYearActItem:FlushRedNum()
    local is_open = ActivityData.IsOpen(self.data.act_type)
    self:UnRegisterGroup()
    self.RedPoint:SetNum(0)
    if is_open and self.handle == nil then
        local mod_t = TabMod(self.data.mod_key)
        self.handle = Remind.Instance:RegisterGroup(mod_t, function()
            self.RedPoint:SetNum(Remind.Instance:GetGroupNum(mod_t) > 0 and 1 or 0)
        end, true)
    end
end

function RabbitYearActItem:OnClick()
    if not self.data.mod_key then
        return
    end
    local is_open = ActivityData.IsOpen(self.data.act_type)
    if is_open then
        ViewMgr:OpenViewByKey(self.data.mod_key)
    else
        PublicPopupCtrl.Instance:Error(Language.Activity.NotOpenTip)
    end
    
end

function RabbitYearActItem:OnDestroy()
    self:UnRegisterGroup()
end

function RabbitYearActItem:UnRegisterGroup()
    local mod_t = TabMod(self.data.mod_key)
    Remind.Instance:UnRegisterGroup(mod_t, self.handle)
    self.handle = nil
end