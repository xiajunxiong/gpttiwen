ActiveTimeLimitView = ActiveTimeLimitView or DeclareMono("ActiveTimeLimitView", UIWFlushPanel)

function ActiveTimeLimitView:ActiveTimeLimitView()
    self.data = ActiveData.Instance
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,init_call = true,keys = {"act_type", "status"}},
        {data = ActivityData.Instance.sign_up_data,func = self.OnFlush,init_call = false},
        {data = ActiveData.Instance.time_limit_data,func = self.OnFlush,init_call = false},
    }
end

function ActiveTimeLimitView:OnFlush()
    self.List:SetData(self.data:GetTimeLimitList())
    self:CheckAnim()
end

function ActiveTimeLimitView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
end

function ActiveTimeLimitView:OnClickPush()
    ViewMgr:OpenView(ActivityPushView)
end

function ActiveTimeLimitView:OnClickWeek()
    ViewMgr:OpenView(ActivityWeekView)
end

function ActiveTimeLimitView:CheckAnim()
    if self.ShowTimer == nil and ActiveData.IsListAnim == nil then
        self.ShowTimer = Invoke(function()
            self.ListAnim:Play()
            ActiveData.IsListAnim = true
        end,0.1)
    end
end

----------------------------ActiveTimeLimitItem----------------------------
ActiveTimeLimitItem = ActiveTimeLimitItem or DeclareMono("ActiveTimeLimitItem", UIWidgetBaseItem)

function ActiveTimeLimitItem:SetData(data)
    UH.RenameGame(self,Format([[Item_%s]],data.index))
    UH.SetText(self.Name,data.name_text)
    UH.SetText(self.Desc,data.time_text)
    UH.SetIcon(self.Icon,data.icon_id,ICON_TYPE.ACT)
    UH.SetText(self.LabelName,Language.Activity.LabelName[data.title])
    UH.SpriteName(self.Label,ActivityConfig.LabelName[data.title])

    self.LabelList:SetData(ActiveData.Instance:GetLabelList(data.act_type))
    local status = ActivityData.Instance:GetActivityStatus(data.act_type)
    local act_sp_status = ActivityData.Instance:GetActivitySpStatus(data.act_type)
    local act_sp_btn_name = ActiveData.Instance:GetActivitySpBtnName(data.act_type)
    local is_open = RoleData.Instance:GetRoleLevel() >= data.level_restrict
    UH.SetText(self.BtnName,is_open and (act_sp_btn_name ~= nil and act_sp_btn_name or Language.Active.BtnStatus[status]) or Format(Language.Common.LevelLimit,data.level_restrict))
    if act_sp_status == nil then 
        self.BtnInter.Interactable = is_open and status ~= ActStatusType.Close
    else
        self.BtnInter.Interactable = act_sp_status ~= ActStatusType.Close
    end 
    
    local act_info = ActivityData.Instance:GetActivityInfo(data.act_type)
    local is_today = act_info and act_info:IsToday() or false
    self.Effect:SetActive(is_open and ActiveData.Instance:GetSignUpFlag(data.act_type))
    self.BtnGo:SetActive(not self.Partake.activeSelf and not self.BtnEffect.activeSelf and not self.Effect.activeSelf)

    self.BtnEffect:SetActive(false)
    self.Partake:SetActive(false)
    
    if is_today == true and ActiveConfig.GloryType[data.act_type]~=true then
        UH.SpriteName(self.Label,ActivityConfig.LabelOtherName)
		UH.SetText(self.LabelName,Language.Activity.LabelOtherName)
    end
    if is_today == true or (is_open and status ~= ActStatusType.Close) or (act_sp_status ~= ActStatusType.Close) or self.Effect.activeSelf then
        self.Grid:SetData(ActiveData.Instance:GetSignUpRewardData(data.act_type))
    end
    if ActiveData.Instance:IsHadSignUpFlag(data.act_type)then
        UH.SetText(self.BtnName,Language.Notice.HadForReward)
    end
    self.BtnHonor:SetActive(ActiveConfig.HonorType[data.act_type]==true)
    self.RedPoint:SetNum(ActiveData.Instance:CheckLimitActIsOpen(data))
    UIWidgetBaseItem.SetData(self,data)
end

function ActiveTimeLimitItem:OnClickGo()
    local act_sp_status = ActivityData.Instance:GetActivitySpStatus(self.data.act_type)
    local str = ActiveData.Instance:GetActivitySpBtnUnAct(self.data.act_type)
    if not ActivityData.IsOpen(self.data.act_type) and (act_sp_status == ActStatusType.Close and str == nil) then
        return
    end
    if not self.BtnInter.Interactable then
        local role_level = RoleData.Instance:GetRoleLevel()
        if role_level >= self.data.level_restrict then
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        else
            PublicPopupCtrl.Instance:Center(str ~= nil and str or string.format(ErrorText[90],self.data.level_restrict))
        end
		return
    end
    if ActiveData.Instance:CheckLimitActIsOpen(self.data) == 1 then
        ActiveData.Instance:SetLimitActClickGoto(self.data.act_type)
        self.RedPoint:SetNum(ActiveData.Instance:CheckLimitActIsOpen(self.data))
    end
    local info = ActivityData.Instance:GetActivityInfo(self.data.act_type)
    if info.click_handle then
        if info.click_handle() then
            return
        end
    end
    ViewMgr:CloseView(ActiveView)
    ActivityData.Instance:SetOpenRemind(self.data.act_type)
end

function ActiveTimeLimitItem:OnClickItem()
    ItemInfoCtrl.Instance:ActivityInfoView{info = ActivityData.Instance:GetActivityInfo(self.data.act_type)}
end

function ActiveTimeLimitItem:OnClickReward()
    ActiveCtrl.Instance:SendSignUpReq(1,self.data.act_type)
end

function ActiveTimeLimitItem:OnClickHelp()
	local reward_func = ActivityData.Instance:GetPreviewRewardFunc(self.data.act_type)
	if reward_func ~= nil then
		ActivityCtrl.Instance:OpenTimeLimitReward(reward_func())
	else
        local config = ActivityData.GetConfig(self.data.act_type)
        if config and config.content_tip then
            if Config.language_cfg_auto.textdesc[config.content_tip] then
                PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[config.content_tip].desc)
            elseif IS_EDITOR then
                LogError("限时活动奖励界面获取语言描述配置报空 act_type="..self.data.act_type.." content_tip ="..config.content_tip)
            else
                BuglyReport("限时活动奖励界面获取语言描述配置报空 act_type="..self.data.act_type.." content_tip ="..config.content_tip)
            end
        elseif IS_EDITOR then
            LogError("限时活动奖励界面获取活动配置为空 act_type="..self.data.act_type)
        else
            BuglyReport("限时活动奖励界面获取活动配置为空 act_type="..self.data.act_type)
        end
    end
end