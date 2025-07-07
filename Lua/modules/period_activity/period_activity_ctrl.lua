FILE_REQ("modules/period_activity/period_activity_config")
FILE_REQ("modules/period_activity/period_activity_data")
FILE_REQ("modules/period_activity/fairy_where_ctrl")
FILE_REQ("modules/period_activity_act/store/period_activity_store_ctrl")
VIEW_REQ("modules/period_activity/period_activity_view")
VIEW_REQ("modules/period_activity/period_activity_help_view")
VIEW_REQ("modules/period_activity/period_activity_poster_view")
VIEW_REQ("modules/period_activity/period_activity_info_view")

PeriodActivityCtrl = PeriodActivityCtrl or BaseClass(BaseCtrl)

function PeriodActivityCtrl:__init()
	if PeriodActivityCtrl.Instance ~= nil then
		Debuger.LogError("[PeriodActivityCtrl] attempt to create singleton twice!")
		return
	end
	PeriodActivityCtrl.Instance = self
	self.data = PeriodActivityData.New()
    FairyWhereCtrl.New()
    PeriodActivityStoreCtrl.New()
    self:RegisterAllProtocols()
end

function PeriodActivityCtrl:__delete()
    self:UnRegisterAllProtocols()
    PeriodActivityCtrl.Instance = nil
end

function PeriodActivityCtrl:OnUnloadGameLogic()
    self:DeleteRepeat()
    self.data:ClearData()
end

function PeriodActivityCtrl:OnInit()
	ActivityRandData.Instance:Register(Mod.PeriodActivity.Main,BindTool.Bind(self.GetPeriodActivityEnd, self),self.data.info_data)
    ActivityRandData.Instance:RegisterActCountDown(Mod.PeriodActivity.Main, BindTool.Bind(self.OnActCountDown, self))
    ActivityRandData.Instance:CustomBaseItemData(Mod.PeriodActivity.Main,function(data)
        local text_list = Split(data.text,"|")
        local type = self.data:GetPeriodType()
        return {sprite = data.sprite .. type,text = text_list[type]}
	end)
    FairyWhereCtrl.Instance:OnRemindHandle()
	TimeCtrl.Instance.server_data:KeysCare({"server_open_day"}, function()
        self.IsFirstEnter = nil
    end)
end

function PeriodActivityCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCPeriodActivityInfo, "OnPeriodActivityInfo")
end

function PeriodActivityCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCPeriodActivityInfo)
end

--周期活动协议下发
function PeriodActivityCtrl:OnPeriodActivityInfo(protocol)
    self.data:SetPeriodActivityInfo(protocol)
    FairyWhereCtrl.Instance:OnSendAllProtocols()
end

--海报提醒开启条件
function PeriodActivityCtrl:GetPosterOpen()
    if self.data:GetPeriodActivityEnd() == true then
        if PrefsInt(PrefKeys.PeriodActivityPoster()) ~= self.data:GetPeriodType() then
            PrefsInt(PrefKeys.PeriodActivityPoster(),self.data:GetPeriodType())
            ActivityRandData.Instance:CheckRandOpenData()
            self.IsFirstEnter = nil
            return true
        end
    end
    return false
end

function PeriodActivityCtrl:GetFirstOpen(num)
    if self.IsFirstEnter == nil then
        return num
    else
        return 0
    end
end

--主界面倒计时刷新
function PeriodActivityCtrl:OnActCountDown(Time)
    local stamp_time,is_show = self.data:GetActivityStampTime()
    if is_show then
        Time:StampTime(stamp_time,TimeType.Type_Special_9, "")
    else
        Time:SetTime()
    end
end

--主界面图标开启
function PeriodActivityCtrl:GetPeriodActivityEnd()
    local is_open = self.data:GetPeriodActivityEnd()
    if self.is_open == true and is_open == false then
        self:DeleteRepeat()
    else
        if is_open and self.IntervalRepeatTimer == nil then
            self.IntervalRepeatTimer = TimeHelper:AddRunTimer(BindTool.Bind(self.OnIntervalRepeat, self), 10)
            PosterHelper.OpenPoster()
        end
    end
    self.is_open = is_open
    return is_open
end

function PeriodActivityCtrl:OnIntervalRepeat()
    self.data:OnCreateHandle()
end

function PeriodActivityCtrl:DeleteRepeat()
    TimeHelper:CancelTimer(self.IntervalRepeatTimer)
    self.IntervalRepeatTimer = nil
    self.data:OnDeleteHandle()
end

function PeriodActivityCtrl:OpenTipView(param1)
    local config = self.data:GetConfig(tonumber(param1))
    if config ~= nil then
        ViewMgr:OpenView(PeriodActivityTipView,{config = config,is_open = true})
    else
        ViewMgr:OpenViewByKey(tonumber(param1))
    end
end

--玩法说明 规则说明 help_tip是游戏说明id 则读取对应的配置说明
--mod_key 对应活动界面，找对应图标
--close_func 可选关闭回调
--[[param_t = {
    custom_close_func = func 自己操作关闭界面
} 
]]
function PeriodActivityCtrl:HelpTip(help_tip, mod_key, close_func, param_t)
    help_tip = tonumber(help_tip) and Config.language_cfg_auto.textdesc[help_tip].desc or help_tip
	self:OpenTipsPopup({tips_str = help_tip, mod_key = mod_key, close_func = close_func, param_t = param_t or {}})
end

function PeriodActivityCtrl:OpenTipsPopup(data, mask)
	ViewMgr:OpenView(PeriodActivityHelpView, data)
end
function PeriodActivityCtrl:CloseTipsPopup()
    ViewMgr:CloseView(PeriodActivityHelpView)
end

--注册周期玩法红点 废弃
function PeriodActivityCtrl:RegisterRemind(modkey, origin, check_func)
    Remind.Instance:Register(modkey,origin,check_func)
    Remind.Instance:Register(modkey,self.data.remind_data,check_func)
end