ActivityRemindView = ActivityRemindView or DeclareMono("ActivityRemindView", UIWFlushPanel)

function ActivityRemindView:ActivityRemindView()
    self.data_cares = {
        {data = GuideData.Instance.notice_info, func = self.FlushRemindView,init_call = false},
        {data = RoleData.Instance.base_data,func = self.FlushRemindView,keys = {"level"},init_call = false},
        {data = AnecdoteHintData.Instance:NetData(), func = self.FlushRemindView,keys = {"is_open","end_timestamp"},init_call = false},
        {data = ServerActivityData.Instance.login_reward_data, func = self.FlushRemindView,keys = {"reward_flag"}},
        {data = LunJianData.Instance:SeasonInfo(), func = self.FlushRemindView,keys = {"cur_status","index"},init_call = false},
        {data = RewardOnlineGiftData.Instance.check_info, func = self.FlushRemindView,init_call = false},
        {data = BicycleData.Instance:GetBicycleList(), func = self.FlushRemindView, init_call = false}
    }
end

function ActivityRemindView:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.RootRect, STANDALONE_UI_VECTOR3)
    end
end

function ActivityRemindView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushRemindView()
end

function ActivityRemindView:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.RootRect, STANDALONE_UI_VECTOR3)
    end
end

function ActivityRemindView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushRemindView()
end

function ActivityRemindView:FlushRemindView()
    local list,list_type = FunOpen.Instance:GetNoticeList()
    local is_list = not TableIsEmpty(list)
    local is_totel = RewardOnlineGiftData.Instance:ShouldShowGift() and RewardOnlineGiftData.Instance:ShouldForeShow()

    -- LogError("?fucke?!",list_type," list ",list)
    -- LogError("?is_totel?!",is_totel)

    -- LogError("?asd checke act",is_list and list_type == 0 and (not is_totel) )
    -- LogError("?asd checke model",is_list and list_type == 1 and (not is_totel) )
    self.ShowActObj:SetActive(is_list and list_type == 0 and (not is_totel) )
    self.ShowModelObj:SetActive(is_list and list_type == 1 and (not is_totel ) )
    self.RedPoint:SetNum(0, true)
    if is_list then
        if list_type == 0 then
            self:FlushAct(list)
        elseif list_type == 1 then
            self.RedPoint:SetNum(FunOpen.Instance.show_notice_red_point.show and 1 or 0, true)
            self:FlushModel(list)
        end
    end
    self.ShowTotalObj:SetActive(is_totel)
end

function ActivityRemindView:FlushAct(list)
    local page_list = DataHelper.GetPageData(list)
    self.PageList:SetData(page_list)
    self.ScrollPage.TotalePageCount = #page_list > 1 and #page_list or 0
    self.ScrollPage.enabled = #page_list > 1
end

function ActivityRemindView:FlushModel(list)
    self.ShowPetItem:SetData(list[1] or {})
end

----------------------------ActivityRemindPage----------------------------
ActivityRemindPage = ActivityRemindPage or DeclareMono("ActivityRemindPage", UIWidgetBaseItem)

function ActivityRemindPage:SetData(data)
    self.List:SetData(data)
end

----------------------------ActivityRemindItem----------------------------
ActivityRemindItem = ActivityRemindItem or DeclareMono("ActivityRemindItem", UIWidgetBaseItem)
function ActivityRemindItem:ActivityRemindItem()
    self.rp_handle = nil
    self.hint_data_care = nil
    self.hint_countdown_timer = nil
end
--RP====UIWSubstite[UIWRedPoint2]
function ActivityRemindItem:SetData(data)
    self:UnregisterRPHandle()
    self:UnregisterRPCare()
    self:CancelHintTimer()
    self.RP:SetNum(0)
    if data.act_type == nil then
        UH.SetText(self.Name,data.name)
        UH.SetIcon(self.Icon,data.icon,ICON_TYPE.ACT)
        if data.modkey == Mod.Anecdote.Hint then
            
            -- if self.rp_handle == nil then
            --     LogError("REGISTER GROUP")
            --     self.rp_handle = Remind.Instance:RegisterGroup(Mod.Anecdote.Hint,function()
            --         LogError("!!!!!!!!!!!!===",Remind.Instance:GetRemindNum(Mod.Anecdote.Hint))
            --         self.RP:SetNum(Remind.Instance:GetRemindNum(Mod.Anecdote.Hint))
            --     end, true)
            -- end
            local set_rp_func = function () 
                    self.RP:SetNum(AnecdoteHintData.Instance:OnRemind())
                end
            set_rp_func()
            self.hint_data_care = AnecdoteHintData.Instance:NetData():Care(
                set_rp_func,"task_finish_num","reward_fetch_status","reward_version")
            self.hint_countdown_timer = TimeHelper:AddCountDownCT(function() 
                    local left_t = AnecdoteHintData.Instance:EndTimeStamp() - TimeHelper:GetServerTime()  
                    left_t = (left_t / 3600) + 1
                    local hint_txt = string.format(Language.Anecdote.Hint.ActRemindTime,left_t)
                    UH.SetText(self.Level,hint_txt)
                end,function() end,
                AnecdoteHintData.Instance:EndTimeStamp(),1800,true)
        elseif data.modkey == Mod.LunJian.Signup then
            UH.SetText(self.Level,string.format(Language.LunJian.Season,DataHelper.GetDaXie(LunJianData.Instance:CurSeason(),false)))
        -- elseif data.modkey == Mod.RewardOnlineGift.Main then
        --     UH.SetText(self.Level,RewardOnlineGiftData.Instance:CompleteText())
        else
            local flag = GuideData.Instance:NoticeInfo(data.notice_id)
            UH.SetText(self.Level,flag == FunPreNoticeEnum.UnLock and Language.Notice.CanRewardTip or 
            Format(Language.Notice.OpenLevel, data.end_level))
            self.Effect:SetActive(flag == FunPreNoticeEnum.UnLock)
        end
    else
        local config = ActivityData.GetConfig(data.act_type)
        UH.SetText(self.Name,config.name_text)
        UH.SetIcon(self.Icon,config.icon_id,ICON_TYPE.ACT)
        self.Effect:SetActive(ActivityData.IsOpen(data.act_type))
        if data.act_type == ActType.Battlefield then
            UH.SetText(self.Level,BattlefieldData.Instance:GetConditionRemindTip())
        elseif data.act_type == ActType.GuildEmbattle then
            self:GuildEmbattleDisplay()
        end
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ActivityRemindItem:UnregisterRPHandle()
    if self.rp_handle ~= nil then
        Remind.Instance:UnRegisterGroup(Mod.Anecdote.Hint,self.rp_handle)
        self.rp_handle = nil
    end
end

function ActivityRemindItem:UnregisterRPCare()
    if self.hint_data_care ~= nil then
        AnecdoteHintData.Instance:NetData():Uncare(self.hint_data_care)
        self.hint_data_care = nil
    end
end

function ActivityRemindItem:CancelHintTimer()
   if self.hint_countdown_timer ~= nil then
        TimeHelper:CancelTimer(self.hint_countdown_timer)
        self.hint_countdown_timer = nil
   end
end

function ActivityRemindItem:OnEnable()
    if self.data ~= nil and self.data.act_type == nil and self.data.modkey == Mod.Anecdote.Hint then
        if self.hint_data_care == nil then
            local set_rp_func = function () 
                self.RP:SetNum(AnecdoteHintData.Instance:OnRemind())
            end
            set_rp_func()
            self.hint_data_care = AnecdoteHintData.Instance:NetData():Care(
                set_rp_func,"task_finish_num","reward_fetch_status","reward_version")
        end
        if self.hint_countdown_timer == nil then
            self.hint_countdown_timer = TimeHelper:AddCountDownCT(function() 
                    local left_t = AnecdoteHintData.Instance:EndTimeStamp() - TimeHelper:GetServerTime()  
                    left_t = (left_t / 3600) + 1
                    local hint_txt = string.format(Language.Anecdote.Hint.ActRemindTime,left_t)
                    UH.SetText(self.Level,hint_txt)
                end,function() end,
                AnecdoteHintData.Instance:EndTimeStamp(),1800,true)
        end
    end
end


function ActivityRemindItem:OnDisable()
    self:UnregisterRPCare()
    self:UnregisterRPHandle()
    self:CancelHintTimer()

end

function ActivityRemindItem:GuildEmbattleDisplay()
    --如果活动开启了那么监听活动状态并显示 几进几
    --如果活动没开启显示查看赛果
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
    if status == ActStatusType.Open then
        local stage = GuildEmbattleData.Instance:Stage()
        UH.SetText(self.Level, Language.GuildEmbattle.RoundName[stage])
        if self.handle_care_round then
            GuildEmbattleData.Instance:Base():Uncare(self.handle_care_round)
            self.handle_care_round = nil
        end
        self.handle_care_round = GuildEmbattleData.Instance:Base():Care(function ()
            stage = GuildEmbattleData.Instance:Stage()
            UH.SetText(self.Level, Language.GuildEmbattle.RoundName[stage])
        end,"curr_round")
    else
        if self.handle_care_round then
            GuildEmbattleData.Instance:Base():Uncare(self.handle_care_round)
            self.handle_care_round = nil
        end
        if self.handle_care_process then
            GuildEmbattleData.Instance.battle_process:Uncare(self.handle_care_process)
            self.handle_care_process = nil
        end
        self.handle_care_process = GuildEmbattleData.Instance.battle_process:Care(function()
            if status == ActStatusType.Standy then
                UH.SetText(self.Level, Language.GuildEmbattle.ActStatusDesc[1])
            else
                local is_ready = GuildEmbattleData.Instance:IsReady()
                if is_ready then
                    UH.SetText(self.Level, Language.GuildEmbattle.CheckNameList)
                else
                    UH.SetText(self.Level, Language.GuildEmbattle.CheckInfo)
                end
            end
        end)
        if status == ActStatusType.Standy then
            UH.SetText(self.Level, Language.GuildEmbattle.ActStatusDesc[1])
        else
            local is_ready = GuildEmbattleData.Instance:IsReady()
            if is_ready then
                UH.SetText(self.Level, Language.GuildEmbattle.CheckNameList)
            else
                UH.SetText(self.Level, Language.GuildEmbattle.CheckInfo)
            end
        end
    end
end

function ActivityRemindItem:OnClickFunOpen()
    if self.data.modkey == Mod.Anecdote.Hint then
        ViewMgr:OpenView(AnecdoteHintView)
    elseif self.data.modkey == Mod.LunJian.Signup then
        local season_status = LunJianData.Instance:SeasonStatus()
        if season_status == LunJianData.Status.Signup or LunJianData.Instance:HasSignuped() == false then 
            ViewMgr:OpenView(LunJianSignupView)
        elseif season_status == LunJianData.Status.Battle then
            ViewMgr:OpenView(LunJianMainView)
        else
            if LunJianData.Instance:HasReportData() then
                ViewMgr:OpenView(LunJianReportView)
            else
                ViewMgr:OpenView(LunJianSignupView)
            end
        end
    -- elseif self.data.modkey ==  Mod.RewardOnlineGift.Main then
    --     ViewMgr:OpenViewByKey(self.data.modkey)
    elseif self.data.modkey then
        ViewMgr:OpenView(FunOpenNoticeView,{mod= self.data.modkey})
        -- ViewMgr:OpenViewByKey(self.data.modkey, true)
    elseif self.data.act_type and self.data.act_type ~= ActType.GuildEmbattle then
        if AnotherData.IsGuideOpened() then
            local config = ActivityData.GetConfig(ActType.BattlefieldAgain)
            PublicPopupCtrl.Instance:Center(Format(Language.Battlefield.GuideOpenedTips,config.time_text))
            return
        end
        BattlefieldCtrl.Instance:OnBattlefield()
    elseif self.data.act_type and self.data.act_type == ActType.GuildEmbattle then
        local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
        local embattle_data = GuildEmbattleData.Instance
        if status == ActStatusType.Open then
            if embattle_data:Round() == 0 then
                PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.OpenError)
            else
                ViewMgr:OpenView(GuildEmbattleMsg)
            end
        elseif status == ActStatusType.Standy then
            -- PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.ActStatusDesc[1])
            ViewMgr:OpenView(GuildEmbattleMsg)
        else
            local is_ready = GuildEmbattleData.Instance:IsReady()
            if is_ready then
                ViewMgr:OpenView(GuildEmbattleNameList)
            else
                if embattle_data:Base().fighter_num == 0 then
                    PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.NotLastInfo)
                else
                    ViewMgr:OpenView(GuildEmbattleMsg)
                end
            end
            -- ViewMgr:OpenView(GuildEmbattleMsg)
        end
    end
end

--===========================ActivityRemindTip===========================
ActivityRemindTip = ActivityRemindTip or DeclareView("ActivityRemindTip", "main_other/activity_remind_tip")

function ActivityRemindTip:ActivityRemindTip()
    self.Time:SetCallBack(function()
        ViewMgr:CloseView(ActivityRemindTip)
    end)
end

function ActivityRemindTip:LoadCallback(param_t)
    self.act_info = param_t.act_info or {}
    self:FlushActInfoView(param_t.info or self.act_info.info)
    self:FlushActTimeView(param_t.stamp_time)
end

function ActivityRemindTip:FlushActInfoView(data)
    UH.SetText(self.Title,data.name_text)
    UH.SetText(self.Desc,data.content_text)
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.ACT)
    local content = Config.language_cfg_auto.textdesc[data.content_tip]
    UH.SetText(self.Content,content and content.desc or "")
end

function ActivityRemindTip:FlushActTimeView(stamp_time)
    local status = ActivityData.Instance:GetActivityStatus(self.act_info.act_type)
    local remind_tip = Language.MainOther.RemindTips[status]
    stamp_time = stamp_time or self.act_info.next_status_switch_time or 0
    if stamp_time > 0 then
        if status ~= 0 then
            self.Time:StampTime(stamp_time,TimeType.Type_Time_1,remind_tip,"")
        else
            self.Time:SetTime("")
        end
    end
    local reward_list = self.act_info.reward_list or {}
    if #reward_list >= 5 then
		self.GridRect.pivot = Vector2.New(0,0.5)
	end
    self.Grid:SetData(reward_list)
    self.BtnInter.Interactable = status ~= 0
    UH.SetText(self.BtnState,status == 0 and Language.MainOther.EndOpenTips[self.act_info.act_type] or Language.Activity.ActItem.Status[status])
end

function ActivityRemindTip:OnClickGoTo()
    if self.act_info == nil then
        return
    end
    if self.act_info.status == ActStatusType.Standy then
        PublicPopupCtrl.Instance:Center(Language.Activity.StandyRemindTip)
        return
    end
    if ActivityData.Instance:IsNoEnoughLevel(self.act_info.act_type) then
        return
    end
    if self.act_info.click_handle() then
        return
    end
	ViewMgr:CloseView(ActivityRemindTip)
end

function ActivityRemindTip:OnClickClose()
    ViewMgr:CloseView(ActivityRemindTip)
end

local sprite_config = {
    [ActType.ChiefElection] = "ShouXiJingXuan",
    [ActType.GuildMelee] = "JiaZuDaLuanDou",
    [ActType.GuildEmbattle] = "JiaZuRongYuZhan",
}

--===============================ActivityRemindSub===============================
ActivityRemindSub = ActivityRemindSub or DeclareMono("ActivityRemindSub",UIWFlushPanel)
function ActivityRemindSub:ActivityRemindSub()
    self.Time:SetCallBack(function()
        ActivityRandData.Instance:CheckRandOpenData()
    end)
end

function ActivityRemindSub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:OnFlush()
end

function ActivityRemindSub:OnFlush()
    local act_type = tonumber(self.ActType)
    local config = ActivityData.GetConfig(act_type)
    self.Time:CloseCountDownTime()
    if config ~= nil then
        UH.SetText(self.Name,config.name_text)
        UH.SpriteName(self.IconImg,sprite_config[act_type])
        self.Time:SetShowCallBack(function(t)return ActivityData.GetConditionTimeTip(t)end)
        self.Time:StampTime(ActivityData.GetRemindTimeStamp(act_type))
    end
end

function ActivityRemindSub:OnClickItem()
    local act_type = tonumber(self.ActType)
    if act_type == ActType.GuildEmbattle then
        ViewMgr:OpenView(GuildEmbattlePreview)
        return
    end
    local reward_func = ActivityData.Instance:GetPreviewRewardFunc(act_type)
	if reward_func ~= nil then
		ActivityCtrl.Instance:OpenTimeLimitReward(reward_func(),act_type)
	end
end

ActivityRemindFuncPetNotice = ActivityRemindFuncPetNotice or DeclareMono("ActivityRemindFuncPetNotice", UIWidgetBaseItem)
function ActivityRemindFuncPetNotice:OnEnable()
    self.ModelShow.block_default_layer = true
end
function ActivityRemindFuncPetNotice:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end
    self.NameObj:SetActive(data.notice_id ~= FunOpen.ConstNotice.XingTian)
    self.XingTianName:SetActive(data.notice_id == FunOpen.ConstNotice.XingTian)
    local res_path = DrawerHelper.GetNpcPath(data.res_id)
    if data.notice_id == FunOpen.ConstNotice.LingLu then
        res_path = DrawerHelper.GetRidePath(data.res_id)
    end
    self.ui_obj:SetData(res_path)
    UH.SetText(self.Name, data.name)
    if data.notice_id == FunOpen.ConstNotice.XingTian then
        local flag = ServerActivityData.Instance:GetLoginRewardFlag(2)
        if flag == 1 then
            UH.SetText(self.Tip, Language.GivePetNotice.TodayCanGet)
        else
            UH.SetText(self.Tip, string.format(Language.GivePetNotice.MainNoticeTip[data.notice_id], data.end_level))
        end
    else
        UH.SetText(self.Tip, string.format(Language.GivePetNotice.MainNoticeTip[data.notice_id], data.end_level))
    end
    self.Time:SetObjActive(data.notice_id == FunOpen.ConstNotice.XingTian)
    if data.notice_id == FunOpen.ConstNotice.XingTian then
        local flag = ServerActivityData.Instance:GetLoginRewardFlag(2)
        local time = (flag == 0) and TimeManager.DayEnd() or 0
        self.Time:SetObjActive(time > 0)
        self.Time:StampTime(time,TimeType.Type_Time_8)
    end
end

function ActivityRemindFuncPetNotice:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    self.Time:CloseCountDownTime()
end

function ActivityRemindFuncPetNotice:OnClick()
    if self.data.notice_id == FunOpen.ConstNotice.XingTian then
        local flag = ServerActivityData.Instance:GetLoginRewardFlag(2)
        if flag == 1 then
            ViewMgr:OpenView(LoginRewardView)
            return
        end
    end
    ViewMgr:OpenViewByKey(self.data.modkey)
    FunOpen.Instance.cur_open_func_notice_data = self.data
end