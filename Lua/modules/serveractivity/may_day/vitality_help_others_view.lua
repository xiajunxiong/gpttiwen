VitalityHelpOthersView = VitalityHelpOthersView or DeclareView("VitalityHelpOthersView",MayDayConfig.ResPath .. "vitality_help_others_view", Mod.MayDay.VitalityHelpOthers)

function VitalityHelpOthersView:VitalityHelpOthersView()
    self.data = VitalityHelpOthersData.Instance
    self.Board:SetData(self:GetType())
end

function VitalityHelpOthersView:LoadCallback()
    self:FlushTimeView()
    self:FlushTaskView()
    local is_finish = self.data:GetTimes() >= self.data:GetMaxTimes()
    self.Finish:SetActive(is_finish)
    self.BtnGo:SetActive(not is_finish)
    self.RedPiont:SetNum(is_finish and 0 or 1)
end

--刷新活动时间
function VitalityHelpOthersView:FlushTimeView()
	local begin_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS)
    begin_time = begin_time or ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS)
    end_time = end_time or ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS)
    local begin_month,end_month = os.date("%m",begin_time),os.date("%m",end_time)
    local open_day,end_day = os.date("%d",begin_time),os.date("%d",end_time-1)
    begin_month = tonumber(begin_month)
    end_month = tonumber(end_month)
    open_day = tonumber(open_day)
    end_day = tonumber(end_day)
    UH.SetText(self.Time,Format(Language.VitalityHelpOthers.TimeTip,begin_month,open_day,end_month,end_day))    
    
end

--刷新任务描述奖励
function VitalityHelpOthersView:FlushTaskView()
    local config = self.data:GetInfoConfig(true)
    if config ~= nil then
        local reward_list = self.data:GetRewardData(config.item_id)
        local curr_times,max_times = self.data:GetTimes(),self.data:GetMaxTimes()
        self.Grid:SetData(DataHelper.FormatItemList(reward_list))
        UH.SetText(self.Desc, config.progress_desc)
        UH.SetText(self.Times,Format(Language.VitalityHelpOthers.TimesTip,max_times - curr_times,max_times))
    end
end

function VitalityHelpOthersView:OnClickGo()
    local config = self.data:GetInfoConfig()
    if config ~= nil then
        if SceneLogic.Instance:SceneID() == config.scene_id then
            PublicPopupCtrl.Instance:Center(Language.VitalityHelpOthers.SceneTip)
        else
            SceneLogic.Instance:AutoToPos(config.scene_id,nil,function()
                PublicPopupCtrl.Instance:Center(Language.VitalityHelpOthers.SceneTip)
            end,nil,true)
        end
    end
    ViewMgr:CloseView(VitalityHelpOthersView)
    ViewMgr:CloseView(MayDayView)
end