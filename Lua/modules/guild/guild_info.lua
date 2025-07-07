GuildInfo = GuildInfo or DeclareMono("GuildInfo", UIWFlushPanel)
function GuildInfo:GuildInfo()
    self.data = GuildData.Instance
    self.lang = Language.Guild
    self.data_cares = {
        {data = self.data.guild_info, func = self.FlushBaseInfo},
		{data = self.data.guild_active_info_flush, func = self.FlushActiveInfo, keys = {"flag"}},
		{data = self.data.guild_active_box_flag, func = self.FlushBoxFlag, keys = {"flag"}},
        {data = self.data.guild_trial_point, func = self.onFlush, init_call = false},
        {data = self.data.cooperate_data, func = self.onFlush, init_call = false},
        {data = ActivityData.Instance:GetActivityEvent(), func = self.onFlush, init_call = false},
        {data = GuildHofData.Instance.info_data, func = self.onFlush, keys = {"info"}, init_call = false},
        {data = GuildHofData.Instance.remind_data, func = self.onFlush, init_call = false},
        {data = GuildBuildingData.Instance.remind_data, func = self.onFlush, init_call = false},
    }
    GuildHofCtrl.Instance:SendGuildHofRank(GuildHofData.Instance:GetSingleRank())
end

function GuildInfo:onFlush()
    self.ActList:SetData(self.data:GetActListWithSort(1))
    self.List2:SetData(self.data:GetActListWithSort(2))
    self.List3:SetData(self.data:GetActListWithSort(3))
end

function GuildInfo:FlushActiveInfo()
	local info = self.data.guild_active_info
	UH.SetText(self.ActiveWeekNum, info.week_active_val)
    UH.SetText(self.ActiveDayNum, info.day_active_val)
	local progress_1 = 0
	local progress_2 = 0
	local progress_3 = 0
	local active_limit_1 = 0
	local active_limit_2 = 0
	local active_limit_3 = 0
	local active_limit_4 = 0
	for k, v in pairs(Config.family_auto.active_gift) do
		if v.level_min == 1 then
			if v.seq == 0 then
				active_limit_1 = v.active_limit	
			end
			if v.seq == 1 then
				active_limit_2 = v.active_limit
			end
			if v.seq == 2 then
				active_limit_3 = v.active_limit
			end
			if v.seq == 3 then
				active_limit_4 = v.active_limit	
			end
		end
	end
	if info.day_active_val and active_limit_1 and info.day_active_val >= active_limit_1 then
		progress_1 = (info.day_active_val-active_limit_1)/(active_limit_2 - active_limit_1)
	end
	if info.day_active_val and active_limit_2 and info.day_active_val >= active_limit_2 then
		progress_2 = (info.day_active_val-active_limit_2)/(active_limit_3 - active_limit_2)
	end
	if info.day_active_val and active_limit_3 and info.day_active_val >= active_limit_3 then
		progress_3 = (info.day_active_val-active_limit_3)/(active_limit_4 - active_limit_3)
	end
	UH.FillAmount(self.ActiveProgress1,progress_1)
	UH.FillAmount(self.ActiveProgress2,progress_2)
	UH.FillAmount(self.ActiveProgress3,progress_3)
	self:FlushBoxFlag()
end

function GuildInfo:FlushBoxFlag()
	self.ActiveBoxList:SetData(self.data:GetGuildActiveBoxList())
end

function GuildInfo:OnHelpClick()
    local capital_desc_id = Config.family_auto.other[1].capital_desc_id or 1
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[capital_desc_id].desc)
end

function GuildInfo:FlushBaseInfo()
    local info = self.data:GetGuildInfo()
    local level_info = self.data:GetLevelInfo()
    local member_info = self.data:GetMemberInfo()
    if self.last_level and self.last_level ~= info.level then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    end
    self.last_level = info.level
    -- UH.SetText(self.LblLevel, Format(self.lang.BaseInfo[1], info.level))
    UH.SetText(self.ArmsTex, info.arms)
    UH.SetText(self.LblLevel, info.level)
    UH.SetText(self.LblWeiHu, Format(self.lang.WeiHu, self.data:GetServeMoney()))
    -- UH.SetText(self.LblID, Format(self.lang.BaseInfo[2], info.guild_id))
    -- UH.SetText(self.LblName, Format(self.lang.BaseInfo[3], info.patriarch_name))
    UH.SetText(self.LblNum, Format(Language.Common.Slash, member_info.member_count or 1, self.data:GetMaxPerson()))
    -- UH.SetText(self.LblCreate, Format(self.lang.BaseInfo[5], os.date("%Y-%m-%d", info.create_time)))
    -- UH.SetText(self.LblJoin, Format(self.lang.BaseInfo[6], os.date("%Y-%m-%d", info.join_time)))
    local is_full, now, full = self.data:GetIsFullMoney()
    UH.SetText(self.LblZiJin, is_full and Format(Language.Guild.FullMoney, now) or now)
    UH.SetText(self.LblGuildName, info.guild_name)
    UH.SetText(self.LblNotice, info.guild_notice)
    local job_info = self.data:GetJobInfo()
    local is_control = job_info.member_type == 0 or job_info.member_type == 1
    self.BtnChange:SetActive(job_info.notice == 1)
    -- self.Exp:SetProgress(info.exp / self.data.guild_other.max_coin)
    -- self.Exp:SetText(info.exp)--, level_info.exp))
    if info.level == self.data.guild_other.max_level then
        -- self.Exp:SetProgress(1)
        -- self.Exp:SetText(Language.Common.MaxLevel)
        self.BtnLevelUp.Interactable = false
        self.MaxLevel:SetActive(true)
        self.BtnLevelUp.gameObject:SetActive(false)
    else
        self.BtnLevelUp.Interactable = is_control
        self.MaxLevel:SetActive(false)
        self.BtnLevelUp.gameObject:SetActive(true)
    end

    self.BtnNameReset:SetActive(job_info.member_type == 0) -- 族长可以控制改命
    -- self.GuildMasterObj:SetActive(is_control)
    -- self.GuildMemberObj:SetActive(not is_control)
    local master_info = self.data:GetGuildMasterInfo()
    if master_info then
        UH.SetText(self.MasterName, master_info.name)
    end

    --self.EventList:SetData(info.eventhandler_list)
    UH.SetText(self.LblJJTip, self.lang.JJTip[info.level == 1 and 1 or 2])
    --[[ self.BtnRongYao:SetActive(info.guild_buff_rank ~= nil and info.guild_buff_rank > 0) ]]
    self.BtnRongYao.enabled = not (info.guild_buff_rank ~= nil and info.guild_buff_rank > 0)
    self:FreshGuildActive()
end

function GuildInfo:FreshGuildActive()
	local Str = RichTextHelper.LinkStr(Language.Guild.GuildActiveRank, "_ul",COLORSTR.Green4)
	UH.SetText(self.ActiveRank, Str)
	local Str1 = RichTextHelper.LinkStr(Language.Guild.GuildActivePersonalRank, "_ul",COLORSTR.Green4)
	UH.SetText(self.ActivePersonal, Str1)
end

function GuildInfo:OnClickNotice()
    if RoleCtrl.NationalDayCheck() then return end
    ViewMgr:OpenView(GuildNotice)
end

function GuildInfo:OnClickLevelUp()
    local job_info = self.data:GetJobInfo()
    local is_control = job_info.member_type == 0 or job_info.member_type == 1

    if not is_control then
        PublicPopupCtrl.Instance:Center(ErrorText[120])
        return
    end

    ViewMgr:OpenView(GuildLevelUp)
end

function GuildInfo:OnClickDetail()
    ViewMgr:OpenView(GuildDetail)
end

function GuildInfo:OnClickRestName()
    if RoleCtrl.NationalDayCheck() then return end
    local info = {
        content = Language.Guild.GuildInfoReset,
        cancel = {
            name = Language.Guild.ChangeArms,
            func = function()
                ViewMgr:OpenView(GuildArms)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        },
        confirm = {
            name = Language.Guild.ChangeGuildName,
            func = function()
                if BagData.Instance:GetNumByItemId(CommonItemId.ChangeGuildName) > 0 then
                    PublicPopupCtrl.Instance:AlertTip(
                        Language.Guild.GuildNameResetTips,
                        function()
                            ViewMgr:OpenView(GuildResetNameView)
                        end
                    )
                else
                    PublicPopupCtrl.Instance:AlertTip(
                        Language.Guild.GuildNameResetGoToBuyTips,
                        function()
                            MallCtrl.Instance:Jump(CommonItemId.ChangeGuildName)
                        end
                    )
                end
            end
        },
        close = true
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function GuildInfo:OnClickGuildRank()
	ViewMgr:OpenView(GuildActiveRankView)
end

function GuildInfo:OnClickPersonal()
	ViewMgr:OpenView(GuildPersonRankView)
end

function GuildInfo:OnClickRongYao()
    ViewMgr:OpenView(RongYaoView)
end

---------------GuildActItem---------------
GuildActItem = GuildActItem or DeclareMono("GuildActItem", UIWidgetBaseItem)
function GuildActItem:GuildActItem()
    -- body
end

function GuildActItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.cfg = ActivityData.GetConfig(self.data.act_type)
    local limit_level = data.open_level
    -- 开启等级，如果活动有就拿活动的
    if not TableIsEmpty(self.cfg) then
        limit_level = self.cfg.level_restrict
    end
    UH.SetText(self.Name, data.activity_name)
    UH.SetText(self.Tip, data.open_time)
    UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ACT)

    local role_level = RoleData.Instance:GetRoleLevel()
    self.ObjCount:SetActive(data.act_type == ActType.GuidlTrial or data.mod == Mod.Guild.GuildCooperateTask)
    UH.SetText(self.LblLevel, Format(Language.Workshop.LimitLv, limit_level))
    -- 次数
	local count = GuildData.Instance:GetActCount(data.act_type)
    if data.act_type == ActType.GuidlTrial then
        -- 家族试炼
        self.Remind:SetNum(GuildData.Instance:GuildBossInfo().boss_participate_reward_fetch_flag ~= 1 and 1 or 0) --GuildData.Instance.guild_trial_point.count)
    elseif data.mod == Mod.Guild.GuildHof then
        -- 名人堂
        self.Remind:SetNum(GuildHofData.Instance:AllRemind())
    elseif data.mod == Mod.Guild.GuildCooperateTask then
        self.Remind:SetNum(GuildData.Instance:GetCooperateRemindNum())
    else
        self.Remind:SetNum(0)
    end
    UH.SetText(self.LblCount, Format(Language.Common.Slash, count, data.activity_num))
    self.GoObj:SetActive(role_level >= limit_level)
    if data.mod == Mod.Guild.GuildCooperateTask then
        UH.SetText(self.LblCount,Format(Language.Common.Slash,GuildData.Instance.cooperate_data.finish_times,Config.family_daily_mission_auto.other[1].tasknum_every_day))
    end 
    -- 家族大乱斗活动未开启置灰
    -- if data.act_type == ActType.GuildMelee then
    --     local status = ActivityData.Instance:GetActivityStatus(ActType.GuildMelee)
    --     if status == ActStatusType.Standy then
    --         UH.SetText(self.Tip, Language.Activity.ActItem.Status[status])
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[2])
    --     elseif status == ActStatusType.Close then
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[status])
    --     else
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[status])
    --     end
    --     self.GoInter.Interactable = status == ActStatusType.Open
    -- end
    -- if data.act_type == ActType.GuildAnswer then
    --     local status = ActivityData.Instance:GetActivityStatus(ActType.GuildAnswer)
    --     if status == ActStatusType.Standy then
    --         UH.SetText(self.Tip, Language.Activity.ActItem.Status[status])
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[2])
    --     elseif status == ActStatusType.Close then
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[status])
    --     else
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[status])
    --     end
    --     self.GoInter.Interactable = (status == ActStatusType.Standy or status == ActStatusType.Open)
    -- end
    -- if data.act_type == ActType.GuildEmbattle then
    --     local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
    --     if status == ActStatusType.Close then
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[status])
    --         self.GoInter.Interactable = false
    --     else
    --         local info = GuildData.Instance:GuildHonorInfo()
    --         -- 获取具体状态
    --         UH.SetText(self.Tip, Language.Guild.EmbattleState[status])--info.state])
    --         UH.SetText(self.BtnTex, Language.Activity.ActItem.Status[2])
    --         self.GoInter.Interactable = status == ActStatusType.Open
    --             -- (info.state == GUILD_HONOR_BATTLE_STATE.SET or info.state == GUILD_HONOR_BATTLE_STATE.BATTLE)
    --     end
    -- end
    local show_list = {}
    if data.mod == Mod.Guild.GuildRedbag then
        -- 家族红包标签
        show_list = {
            Config.daily_activity_auto.activity_label_show[30]
        }
    elseif data.mod == Mod.Guild.GuildHof then
        -- 名人堂标签
        show_list = {
            Config.daily_activity_auto.activity_label_show[5],
            Config.daily_activity_auto.activity_label_show[8],
            Config.daily_activity_auto.activity_label_show[9],
        }
        self.ObjRankShow:SetActive(true)
        UH.SizeDelta(self.ObjRoot, Vector2.New(475, 293))
        local rank_title, rank_info = GuildHofData.Instance:GetGuildInfoShow()
        UH.SetText(self.TxtTitle, rank_title)
        self.RankList:SetData(rank_info)
    elseif data.mod == Mod.Guild.GuildCooperateTask then
        self.ObjRankShow:SetActive(false)
        show_list = {
            Config.daily_activity_auto.activity_label_show[23],
            Config.daily_activity_auto.activity_label_show[5],
            Config.daily_activity_auto.activity_label_show[4],
        }
        UH.SizeDelta(self.ObjRoot, Vector2.New(229, 293))
    else
        self.ObjRankShow:SetActive(false)
        UH.SizeDelta(self.ObjRoot, Vector2.New(229, 293))
        local act_config = ActivityData.Instance:GetActRewardData(data.act_type) or {}
        self.cfg.reward_id = act_config.reward_id
        local label_shows = string.split(act_config.label_show, ",")
        for i = 1, #label_shows do
            if i <= 3 then
                table.insert(show_list, ActivityData.Instance.config_activity_label_show[tonumber(label_shows[i])])
            else
                LogError("该活动的label_show大于3个，请修改配置",act_config)
            end
        end
    end
    self.TypeList:SetData(show_list)
end

function GuildActItem:OnClickInfo()
    if self.cfg then
        ItemInfoCtrl.Instance:ActivityInfoView{info = ActivityData.Instance:GetActivityInfo(self.data.act_type)}
    end
end

function GuildActItem:OnClickToGo(data)
    if data.act_type == ActType.GuidlTrial then
        -- 家族试炼
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
        GuildCtrl.SendGuildBossReq()
        ViewMgr:OpenView(GuildTrialNew)
        self.Remind:SetNum(0)
        -- self.Remind:SetNum(GuildData.Instance.guild_trial_point.count)
    else
        if not StringIsEmpty(self.data.mod) then
            ViewMgr:OpenViewByKey(self.data.mod)
        end
    end
end

---------------GuildInfoLeftBtn---------------
GuildInfoLeftBtn = GuildInfoLeftBtn or DeclareMono("GuildInfoLeftBtn", UIWidgetBaseItem)
function GuildInfoLeftBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.ImgSetIcon, data.icon_id, ICON_TYPE.ACT)
    UH.SetText(self.TxtName, data.activity_name)
    self.Interactor.Interactable = true
    if data.mod == Mod.Guild.GuildRedbag then
        -- 家族红包
        self.Remind:SetNum(GuildRedbagData.Instance:AllRemind())
    elseif data.mod == Mod.Guild.GuildBuilding then
        -- 家族内政建设
        self.Remind:SetNum(GuildBuildingData.Instance:AllRemind())
        self.Interactor.Interactable = GuildBuildingData.Instance:BuildingIsOpen()
    end
end

function GuildInfoLeftBtn:OnClick()
    if not StringIsEmpty(self.data.mod) then
        ViewMgr:OpenViewByKey(self.data.mod)
    end
end

---------------GuildInfoRankItem---------------
GuildInfoRankItem = GuildInfoRankItem or DeclareMono("GuildInfoRankItem", UIWidgetBaseItem)
function GuildInfoRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ImgSetRank:SetObjActive(data.index <= 3)
    if data.index <= 3 then
        UH.SpriteName(self.ImgSetRank, "Rank_" .. data.index)
    end
    self.ObjLine:SetActive(data.index ~= GuildHofData.Instance:GetOtherNumber())
    UH.SetText(self.TxtRankNum, data.index)
    UH.SetText(self.TxtName, data.name or Language.GuildHof.NoPerson)
end

---------------GuildInfoButtomAct---------------
GuildInfoButtomAct = GuildInfoButtomAct or DeclareMono("GuildInfoButtomAct", UIWidgetBaseItem)
function GuildInfoButtomAct:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.act_info = ActivityData.Instance:GetActivityInfo(data.act_type)
    UH.SetIcon(self.ImgSetIcon, data.icon_id, ICON_TYPE.ACT)
    UH.SetText(self.TxtName, data.activity_name)
    if ActivityData.IsOpen(data.act_type) then
        UH.SetText(self.TxtTip, Language.Guild.Opening)
    else
        UH.SetText(self.TxtTip, data.open_time)
    end
    self.ObjToday:SetActive(self.act_info:IsToday())
    if GuildData.Instance.guild_info_is_active_remind and ActivityData.IsOpen(data.act_type) then
        self.Remind:SetNum(1)
    else
        self.Remind:SetNum(0)
    end
end

function GuildInfoButtomAct:OnClickInfo(data)
    if ActivityData.IsOpen(data.act_type) then
        GuildData.Instance.guild_info_is_active_remind = false
        GuildData.Instance.guild_trial_point:Notify()
        -- 家族大乱斗
        if data.act_type == ActType.GuildMelee then
            --限制一下等级
            local flag = ActivityData.Instance:IsNoEnoughLevel(data.act_type)
            if flag then
                return
            end
            local co = Cfg.TeamTypeTargetAct(ActType.GuildMelee)
            TeamCtrl.Instance:SendChangeTeamLimit(co.low_level, co.high_level, co.target_id)
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GUILD_FIGHT)
            GuildData.Instance:SetDefeatedTimes(0)
            ViewMgr:CloseView(GuildView)
        -- 家族荣誉战
        elseif data.act_type == ActType.GuildEmbattle then
            if GuildData.Instance:IsInGuildHonorFB(true) then
                PublicPopupCtrl.Instance:Center(ErrorText[81])
            else
                --[[ local info = GuildData.Instance:GuildHonorInfo()
                if info.state == GUILD_HONOR_BATTLE_STATE.BATTLE then
                    PublicPopupCtrl.Instance:Center(ErrorText[137])
                    return
                end
                GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
                ViewMgr:CloseView(GuildView) ]]
                if GuildEmbattleData.Instance:EntryQualify() == false then
                    ViewMgr:OpenView(GuildEmbattleMsg)
                else
                    GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
                end
            end
        -- 家族答题
        elseif data.act_type == ActType.GuildAnswer then
            if GuildData.Instance:IsInAnswerScene() then
                return
            end
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GUILD_ANSWER)
        end
    else
        ItemInfoCtrl.Instance:ActivityInfoView{info = self.act_info}
    end
end

---------------GuildActType---------------
GuildActType = GuildActType or DeclareMono("GuildActType", UIWidgetBaseItem)
function GuildActType:GuildActType()
    self.config = ActivityConfig.ActivityActType
end

function GuildActType:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.label_name
    self.TypeName.color = self.config.label[data.label_color]
    self.TypeNameOutLine.effectColor = self.config.outline[data.label_color]
    self.TypeBg.SpriteName = self.config.bg_sp[data.label_color]
end

---------------GuildEventItem---------------
GuildEventItem = GuildEventItem or DeclareMono("GuildEventItem", RichTextItem)
function GuildEventItem:GuildEventItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
    RichTextHelper.Bind(self.Content, BindTool.Bind(self.OnClickLink, self))
end

function GuildEventItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.event_type = math.floor(data.eventhandler_type / 10000)
    local event_param = math.fmod(data.eventhandler_type, 10000)
    if data.eventhandler_type >= 100000 then
        self.event_type = math.floor(data.eventhandler_type / 100000)
        event_param = math.fmod(data.eventhandler_type, 100000)
    end
    local date = os.date("%Y-%m-%d", data.eventhandler_time)
    local time = os.date("%H:%M:%S", data.eventhandler_time)
    local param_str = nil
    -- if self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_ADD_MEMBER
    --     or self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_DEC_MEMBER then
    --     content = Format(self.lang.Event[self.event_type], date, time, data.eventhandler_name)
    -- else
    if self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_APPOINTMENT then --任命
        local job_info = self.guild_data:GetJobInfo(event_param)
        param_str = job_info.member_name
    elseif self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_XUNBAO 
        or self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_ADD_PET then --获得宠物
        
        local pet_cfg = PetData.Instance:GetPetCfg(event_param)
        if pet_cfg ~= nil then
            param_str =
                RichTextHelper.LinkStr(
                RichTextHelper.ColorStr(pet_cfg.name, QualityColorStr[pet_cfg.quality]),
                "_ul",
                QualityColorStr[pet_cfg.quality]
            )
        else
            param_str = tostring(event_param)
        end
    elseif self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH then --成就达到那一阶段的1星
        param_str = AchieveNewData.GetStageName(event_param, 1)
    elseif
        self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND or
            self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY
     then --工坊锻造
        local item_cfg = Item.GetConfig(event_param)
        param_str =
            RichTextHelper.LinkStr(
            RichTextHelper.ColorStr(item_cfg.name, QualityColorStr[item_cfg.color]),
            "_ul",
            QualityColorStr[item_cfg.color]
        )
    elseif
        self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_MITI or --通过试炼之地第几层
            self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE
     then --通过锢魔之塔第几层
        param_str = tostring(event_param)
	elseif self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP then --家族升级
        param_str =  tostring(event_param)
    end
    if self.lang.Event[self.event_type] ~= nil then
        local content = Format(self.lang.Event[self.event_type], date, time, data.eventhandler_name, param_str)
        UH.SetText(self.Content, content)
    end
end

function GuildEventItem:OnClickLink(dict)
    if dict == nil then
        return
    end
    local event_param
    if
        self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_XUNBAO or
            self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_ADD_PET
     then
        event_param = math.fmod(self.data.eventhandler_type, 100000)
        local pet = self.data.info_type.param and Pet.New(self.data.info_type.param) or Pet.New({item_id = event_param})
        ViewMgr:OpenView(PetDetailView, pet)
    elseif
        self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND or
            self.event_type == GUILD_EVENTHANDLER_TYPE.GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY
     then
        event_param = math.fmod(self.data.eventhandler_type, 100000)
        local item = self.data.info_type.param and Equip.New(self.data.info_type) or Item.New({item_id = event_param})
        ItemInfoCtrl.Instance:ItemInfoView {item_list = {item}}
    end
end

---------------GuildActiveItem---------------
GuildActiveItem = GuildActiveItem or DeclareMono("GuildActiveItem", UIWidgetBaseItem)
function GuildActiveItem:GuildActiveItem()
   self.lang = Language.Guild
   self.guild_data = GuildData.Instance
   self.is_can_get = false
end

function GuildActiveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.ActiveText, Format(self.lang.GuildActive,tostring(data.active_limit)))
	local image_name = "guildbox"
	self.RedPoint:SetNum(0)
	self.is_can_get = false
	if data.flag == 1 then
		image_name = "guildboxopen"
	else
		if self.guild_data.guild_active_info.day_active_val and data.active_limit and self.guild_data.guild_active_info.day_active_val >= data.active_limit then
			self.is_can_get = true
			self.RedPoint:SetNum(1)
		end
	end
	self.ImageObj:SetActive(not self.is_can_get)
	self.ShakeImageObj:SetActive(self.is_can_get)
	UH.SpriteName(self.BoxImage,image_name)
end

function GuildActiveItem:OnClickInfo()
	self.guild_data.guild_active_box_seq.seq = self.data.seq
	if self.is_can_get then
		GuildCtrl.SendGuildActiveGetGift(self.data.seq)
	else
		ViewMgr:OpenView(GuildActiveBoxView)
	end
end

---------------GuildAccuse---------------
GuildAccuse = GuildAccuse or DeclareView("GuildAccuse", "guild/guild_accuse")
VIEW_DECLARE_MASK(GuildAccuse,ViewMask.Block)
function GuildAccuse:GuildAccuse()
end
function GuildAccuse:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.AccuseTitle,Vector2.New(550,340))
end
function GuildAccuse:OnFlush()
    UH.SetText(self.Text, Language.Guild.AccuseTipDialogTxt)
end

function GuildAccuse:OnClickConfirm()
    ViewMgr:CloseView(GuildAccuse)
end