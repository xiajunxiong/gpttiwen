GuildMember = GuildMember or DeclareMono("GuildMember", UIWFlushPanel)
function GuildMember:GuildMember()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.member_info, func = self.FlushMemberList}
    }
    self.lang = Language.Guild
    self.select_data = nil

    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self
        })
    end
end
function GuildMember:OnEnable()
    UIWFlushPanel.OnEnable(self)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
end
function GuildMember:onFlush()
    
end

function GuildMember:FlushMemberList()
    self.mVo = self.data:GetMemberVo()
    --申请列表的显示
    if self.mVo then
        local job_info = self.data:GetJobInfo(self.mVo.post)
        -- self.BtnKick:SetActive(job_info.kick == 1)
        self.BtnApply:SetActive(job_info.approval == 1)
        -- self.BtnSet:SetActive(job_info.appoint == 1)
    end
    local info = self.data:GetMemberInfo()
    --list加工
    --1分钟
    local list = info.member_list
    --[[ table.sort(list, function (a,b)
        return a.level > b.level
    end) ]]
    --[[ table.sort(list, function (a,b)
        return a.post < b.post
    end) ]]
    table.sort(list, function (a,b)
        local s_a = 1
        local s_b = 1
        if a.online > b.online then
            s_a = s_a + 10000
            if a.post < b.post then
                s_a = s_a + 1000
            else
                s_b = s_b + 1000
            end
        elseif a.online == b.online then
            s_a = s_a + 10000
            s_b = s_b + 10000
			if a.online == 1 then
				if a.post < b.post then
					s_a = s_a + 1000
				else
					s_b = s_b + 1000
				end
			else
				if a.timestamp > b.timestamp then
					s_a = s_a + 1000
				else
					s_b = s_b + 1000
				end
			end
			
        else
            s_b = s_b + 10000
            if a.post < b.post then
                s_a = s_a + 1000
            else
                s_b = s_b + 1000
            end
        end
        return s_a > s_b
    end)
    for i,v in ipairs(list) do
        v.index = i
    end
    self.MemberList:SetData(list)
    if self.mVo then
        if self.select_index then
            self.MemberList:ClickItem(self.select_index)
        else
            self.MemberList:ClickItem(1)
        end
    end
    if self.mVo and (self.mVo.post == 0 or self.mVo.post == 1) then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.BtnApply, Vector3.New(77,20,0))
        end
        self.red_point:SetNum(self.data.apply_count.count)
    end
end

function GuildMember:OnClickList()
    ViewMgr:OpenView(GuildList)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
end
function GuildMember:OnClickOut()
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
    if status == ActStatusType.Close then
    else
        PublicPopupCtrl.Instance:Center(ErrorText[136])
        return
    end
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildMelee)
    if status == ActStatusType.Close then
    else
        PublicPopupCtrl.Instance:Center(ErrorText[138])
        return
    end
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildAnswer)
    if status == ActStatusType.Close then
    else
        PublicPopupCtrl.Instance:Center(ErrorText[144])
        return
    end
    -- 族长需要先转让
    local member_count = self.data:GetMemberInfo().member_count
    if self.mVo ~= nil and self.mVo.post == 0 and member_count > 1 then
        PublicPopupCtrl.Instance:Center(ErrorText[30])
        return
    end
    if self.data:HasPetOnTraining() then
        PublicPopupCtrl.Instance:Center(ErrorText[163])
        return
    end
    --弹提示窗
    local info = {
        content = string.format(self.lang.OutTip,self.data:GetGuildName()),
        confirm = {
            func = function()
                GuildCtrl.SendQuitGuild(self.data:GetGuildID())
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end
function GuildMember:OnClickListItem(flag, data)
    if self.mVo == nil then return end 
    self.select_data = data
    self.select_index = self.MemberList:GetByData(data)
    UH.SetText(self.Name, self.select_data.name)
    --如果我是副族长并且选中族长
    UH.SetText(self.LblSet, self.lang.SetStr[self.select_data.post == 0 and self.mVo.post == 1 and 2 or 1])
    --[[ if self.select_data.post == 0 or self.select_data.post == 1 then
        self.BtnSet:SetActive(self.mVo.post ~= self.select_data.post)
        self.BtnKick:SetActive(self.mVo.post ~= self.select_data.post)
    end ]]
    self.ui_obj:SetData(data.appearance)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function GuildMember:OnClickFriend()
    SocietyCtrl.Instance:SendFriendAdd(self.select_data.uid)
end
function GuildMember:OnClickSend()
    --判断是否在线和是否是自己
    if self.select_data.online == 1 then
        SocietyCtrl.Instance:SendAddRecentChat(self.select_data.uid)
        SocietyData.Instance:SetRecentlyChatRole(self.select_data.uid)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[64])
    end
end
function GuildMember:OnClickCheck()
    -- ViewMgr:OpenView(MemberInfo)
    if self.select_data.online == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[64])
        return
    end
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.select_data.uid)
    --[[ TimeHelper:AddDelayFrameTimer(function()
    end, 1) ]]
    ViewMgr:OpenView(RoleInfoDetailsView)
end
function GuildMember:OnClickTeam()
    TeamCtrl.Instance:SendTeamInvitationReq(self.select_data.uid)
end
function GuildMember:OnClickSet()
    --打开一个职务列表
    --如果是族长那么显示0-5
    --如果是副族长显示1-5
    -- 1或者2 时显示弹劾
    if self.mVo == nil then return end 
    if self.select_data.post == 0 and (self.mVo.post == 1 or self.mVo.post == 2) then
        self.BtnParent:SetActive(false)
        local param_t = {
            guild_id = self.data:GetGuildID(),
            uid = self.select_data.uid,
            post = 0
        }
        GuildCtrl.SendGuildAppointmentOp(param_t)
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
    else
        self.BtnList[1]:SetActive(self.mVo.post == 0)
        self.BtnList[2]:SetActive(self.mVo.post ~= 1)
        self.BtnParent:SetActive(not self.BtnParent.activeSelf)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
    end
end
function GuildMember:OnClickHide()
    self.BtnParent:SetActive(false)
end
function GuildMember:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end
function GuildMember:OnClickSetJob(post)
    local param_t = {
        guild_id = self.data:GetGuildID(),
        uid = self.select_data.uid,
        post = post
    }
    if post == 0 then
        PublicPopupCtrl.Instance:DialogTips({
            content = Language.Guild.ZhuanRang,
            confirm = {
                func = function ()
                    GuildCtrl.SendGuildAbdicationOp(self.select_data.uid)
                    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        })
        self:OnClickHide()
        return
    else
        GuildCtrl.SendGuildAppointmentOp(param_t)
    end
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
    self:OnClickHide()
end

function GuildMember:OnClickKickOut()
    if self.select_data.uid == RoleData.Instance:GetRoleId() then
        PublicPopupCtrl.Instance:Center(Language.Guild.GuildMemberOutNot)
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Guild.GuildMemberOutTip,self.select_data.name),function()
        local param_t = {
            guild_id = self.data:GetGuildID(),
            expel_uid = self.select_data.uid
        }
        GuildCtrl.SendGuildExpelOutReq(param_t)
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
    end)
end

function GuildMember:OnClickApply()
    -- GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_APPLY_LIST})
    ViewMgr:OpenView(GuildApply)
end

function GuildMember:OnClickMail(data)
    local flag_complete = data.role_guild_task_finish_times == Config.family_daily_mission_auto.other[1].tasknum_every_day
    if flag_complete then
        PublicPopupCtrl.Instance:Center(Language.GuildCooperateTask.MailDoneError)
        return 
    end

    if data.role_guild_task_is_mail_notice == 1 then 
        PublicPopupCtrl.Instance:Center(Language.GuildCooperateTask.MailNoticeDoneError)
        return 
    end

    if GuildData.Instance:CheckCooMemberIsOnly(data.uid) then 

        local param_t = {
            content = Language.GuildCooperateTask.MailOnlySingle,
            confirm = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                    GuildCtrl.SendGuildCooperateReq({
                        req_type = 6,
                        param1 = 0,
                        param2 = data.uid,
                    })
                end 
            },
            cancel = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                end 
            }
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return 
    end 

    local param_t = {
        content = Language.GuildCooperateTask.MailTips,
        confirm = {
            name = Language.GuildCooperateTask.MailOneKey,
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                GuildCtrl.SendGuildCooperateReq({
                    req_type = 6,
                    param1 = 1,
                })
            end 
        },
        cancel = {
            name = Language.GuildCooperateTask.MailSingle,
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                GuildCtrl.SendGuildCooperateReq({
                    req_type = 6,
                    param1 = 0,
                    param2 = data.uid,
                })
            end 
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)

end

GuildMemberItem = GuildMemberItem or DeclareMono("GuildMemberItem", UIWidgetBaseItem)
function GuildMemberItem:GuildMemberItem()
    self.guild_data = GuildData.Instance
    self.lang = Language.Guild
end
function GuildMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local job_info = self.guild_data:GetJobInfo(data.post)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Job, job_info.member_name)
    if data.top_level and data.top_level > 0 then
        UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, data.top_level))
    else
        UH.SetText(self.Level, data.level)
    end
    UH.SetText(self.GongXian, data.total_gongxian)
	UH.SetText(self.SevenActive, data.seven_active)
    UH.SpriteName(self.ProfSp, RoleData.GetProfSp(data.appearance.prof_base, false))
    UH.Alpha(self.Bg, (data.index ~= nil and data.index % 2 == 0) and 0.65 or 1)
    self.SelectInter.Interactable = (data.online == 1)
    UH.Color(self.Name, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
    UH.Color(self.Job, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
    UH.Color(self.Level, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
    UH.Color(self.GongXian, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
    UH.Color(self.Online, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
	UH.Color(self.SevenActive, data.online == 1 and COLORS.Yellow10 or COLORS.Gray3)
	if data.uid == RoleData.Instance:GetRoleId() then
		UH.Color(self.Name,COLORS.Green2)
	end
    self.EditorGray.enabled = data.online == 0
    self.ProfGray.enabled = data.online == 0
    self.EditorObj:SetActive(data.uid ~= RoleData.Instance:GetRoleId())
    if data.online == 0 then
        local time = os.time()
        local delta = time - data.timestamp
        delta = delta < 0 and 1 or delta
        if delta < 3600 then
            UH.SetText(self.Online, string.format(self.lang.Online2[1], math.ceil(delta / 60)))
        elseif delta < 86400 then
            UH.SetText(self.Online, string.format(self.lang.Online2[2], math.ceil(delta / 3600)))
        elseif delta < 604800 then
            UH.SetText(self.Online, string.format(self.lang.Online2[3], math.ceil(delta / 86400)))
        else
            UH.SetText(self.Online, self.lang.Online2[4])
        end
    else
        UH.SetText(self.Online, self.lang.Online[data.online])
    end
    UH.SetText(self.GuildTaskProgress,data.role_guild_task_finish_times.."/"..Config.family_daily_mission_auto.other[1].tasknum_every_day)
    self.GuildTaskBtn.Interactable = data.role_guild_task_is_mail_notice == 0
    local flag_complete = data.role_guild_task_finish_times == Config.family_daily_mission_auto.other[1].tasknum_every_day

    local role_id = RoleData.Instance:GetRoleId()
    local mVo = self.guild_data:GetMemberVo()
    local m_job_info =  self.guild_data:GetJobInfo(mVo.post)
    self.GuildTaskBtn.gameObject:SetActive(role_id ~= data.uid and not flag_complete and m_job_info.mission_remind == 1)
    self.GuildTaskDone:SetActive(flag_complete)
end

function GuildMemberItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end

--[[ function GuildMemberItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected and self.ItemClick then
        self.ItemClick:Trigger()
    end
end ]]
function GuildMemberItem:OnClickDetail()
    self.data.guild_name = GuildData.Instance:GetGuildName()
    self.data.open_in_member_view = true
    SocietyCtrl.Instance:ClickToRoleVo(self.data)
end