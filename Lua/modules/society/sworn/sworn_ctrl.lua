FILE_REQ("modules/society/sworn/sworn_config")
FILE_REQ("modules/society/sworn/sworn_data")

VIEW_REQ("modules/society/sworn/society_view_sworn")
VIEW_REQ("modules/society/sworn/sworn_main_view")
VIEW_REQ("modules/society/sworn/sworn_common")
VIEW_REQ("modules/society/sworn/sworn_watch_team_view")
VIEW_REQ("modules/society/sworn/sworn_notice_view")
VIEW_REQ("modules/society/sworn/sworn_invite_view")
VIEW_REQ("modules/society/sworn/sworn_invited_view")
VIEW_REQ("modules/society/sworn/sworn_invite_team_view")
VIEW_REQ("modules/society/sworn/sworn_affirm_view")
VIEW_REQ("modules/society/sworn/sworn_effect_view")
VIEW_REQ("modules/society/sworn/sworn_rename_view")
VIEW_REQ("modules/society/sworn/sworn_quit_view")
VIEW_REQ("modules/society/sworn/sworn_level_tip")
VIEW_REQ("modules/society/sworn/sworn_kick_out_view")
VIEW_REQ("modules/society/sworn/sworn_edit_decl_view")
VIEW_REQ("modules/society/sworn/sworn_skill_tip")

--结义
SwornCtrl = SwornCtrl or BaseClass(BaseCtrl)
function SwornCtrl:__init()
    if SwornCtrl.Instance ~= nil then
        Debuger.LogError("[SwornCtrl] attempt to create singleton twice!")
        return
    end
    SwornCtrl.Instance = self
    self.data = SwornData.New()

    self:RegisterAllProtocols()
end

function SwornCtrl:__delete()
    if SwornCtrl.Instance == self then
        SwornCtrl.Instance = nil
    end
end

function SwornCtrl:OnInit()
    -- RemindCtrl.Instance:Remind():Register(Mod.Society.Sworn, self.data.sworn_info, function()
    --     return 0
    -- end)
    BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetSwornNpcSeq(), BindTool.Bind(self.SetupBehaviourBtn, self))
end

function SwornCtrl:OnUnloadGameLogic()
    if self.play_succ_timer ~= nil then
        TimeHelper:CancelTimer(self.play_succ_timer)
        self.play_succ_timer = nil
    end
    self.watch_jie_yi_zu_id = nil
    self.data:ClearSwornInfo()
end

function SwornCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSJieYiReq)
    self:RegisterProtocol(CSJieYiCreate)
    self:RegisterProtocol(CSJieYiCreateName)
    self:RegisterProtocol(CSJieYiZuMemberName)
    self:RegisterProtocol(CSJieYiZuText)
    self:RegisterProtocol(CSJieYiZuName)
    self:RegisterProtocol(CSJieYiZuXuanYanReq)
    self:RegisterProtocol(CSJieYiZuXuanYanReqList)
    self:RegisterProtocol(SCJieYiCreateInfo, "OnSCJieYiCreateInfo")
    self:RegisterProtocol(SCJieYiCreateNameUpdate, "OnSCJieYiCreateNameUpdate")
    self:RegisterProtocol(SCJieYiCreateConfirm, "OnSCJieYiCreateConfirm")
    self:RegisterProtocol(SCJieYiCreateCancel, "OnSCJieYiCreateCancel")
    self:RegisterProtocol(SCJieYiZuInfo, "OnSCJieYiZuInfo")
    self:RegisterProtocol(SCJieYiInviteRoute, "OnSCJieYiInviteRoute")
    self:RegisterProtocol(SCJieYiRoleXuanYanList, "OnSCJieYiRoleXuanYanList")
    self:RegisterProtocol(SCJieYiZuXuanYanList, "OnSCJieYiZuXuanYanList")
    self:RegisterProtocol(SCJieYiZuLeaveNotice, "OnSCJieYiZuLeaveNotice")
    self:RegisterProtocol(SCJieYiZuRoleOnlineStatusNotice, "OnSCJieYiZuRoleOnlineStatusNotice")
end

function SwornCtrl:SendSwornReq(req_type, param1, param2)
    local protocol = GetProtocol(CSJieYiReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function SwornCtrl:SendBeginSworn()
    local protocol = GetProtocol(CSJieYiCreate)
    protocol.str1 = ""
    protocol.str2 = ""
    protocol.member_num = TeamData.Instance:MemberNumRole()
    SendProtocol(protocol)
end

function SwornCtrl:SendUpdateSwornName(str1, str2, member_num)
    if self.data.create_info.key == -1 then
        return
    end
    local protocol = GetProtocol(CSJieYiCreateName)
    protocol.key = self.data.create_info.key
    protocol.str1 = str1 or ""
    protocol.str2 = str2 or ""
    protocol.member_num = member_num or 0
    SendProtocol(protocol)
end

function SwornCtrl:SendLeaderNameFinishReq()
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_NAME_FINISH)
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_CONFIRM)
end

function SwornCtrl:OnSCJieYiCreateInfo(protocol)
    self.data:SetCreateInfo(protocol)
    if not ViewMgr:IsOpen(SwornAffirmView) then
        ViewMgr:OpenView(SwornAffirmView)
    end
end

function SwornCtrl:OnSCJieYiCreateNameUpdate(protocol)
    self.data:SetCreateName(protocol)
end

function SwornCtrl:SendMemberIsAffirm(is_affirm)
    if is_affirm then
        self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_CONFIRM)
    else
        self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_CANCEL)
    end
end

--确认
function SwornCtrl:OnSCJieYiCreateConfirm(protocol)
    self.data:SetCreateConfirm(protocol)
    local count = self.data.create_info.role_count
    local num = 0
    for i, v in pairs(protocol.role_id) do
        if v > 0 then
            num = num + 1
        end
    end
    if num >= count then
        local function func_play()
            self.play_succ_timer = nil
            ViewMgr:CloseView(SwornAffirmView)
            ViewMgr:OpenView(SwornEffectView)
            if SwornData.Instance:IsSwornLeader() then
                self:SendChangeSwornDeclReq(Language.Sworn.SwornDefaultDecl)
            end
        end
        if self.play_succ_timer == nil then
            self.play_succ_timer = Invoke(func_play, 1)
        end
    end
end

--取消
function SwornCtrl:OnSCJieYiCreateCancel(protocol)
    ViewMgr:CloseView(SwornAffirmView)
    local role_list = self.data.create_info.role_info
    if role_list ~= nil then
        for i, v in pairs(role_list) do
            if v.uid == protocol.role_id and Language.Sworn.SwornAffirmError[protocol.type] ~= nil then
                PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornAffirmError[protocol.type], v.name))
                return
            end
        end
    end
end

function SwornCtrl:OnSCJieYiZuInfo(protocol)
    if protocol.info_type == 1 then
        if self.add_friend_zu_id ~= nil then
            self:SendAddFriendInfo(protocol)
            self.add_friend_zu_id = nil
        end
        if self.watch_jie_yi_zu_id ~= nil then
            ViewMgr:OpenView(SwornWatchTeamView, {member_list = protocol.role_info})
            self.watch_jie_yi_zu_id = nil
        end
        return
    end
    self.data:SetSwornInfo(protocol)
    for i, v in pairs(protocol.role_info) do
        if v.uid == RoleData.Instance:GetRoleId() then
            if v.member_name ~= "" and v.has_first_member_name ~= 0 then
                RoleData.Instance:Set({"sworn_info"}, {sworn_info = {empty = false, zu_name = protocol.jie_yi_name, member_name = v.member_name}})
            else
                RoleData.Instance:Set({"sworn_info"}, {sworn_info = {empty = true}})
            end
            break
        end
    end
end

--退出结义组，操作
function SwornCtrl:OnSCJieYiZuLeaveNotice(protocol)
    self.data:OnSCJieYiZuLeaveNotice(protocol)
    if ViewMgr:IsOpen(SocietyView) then
        SwornData.Instance.sworn_view_tab.index = 1
        SwornData.Instance.sworn_view_tab:Notify()
    end
    RoleData.Instance:Set({"sworn_info"}, {sworn_info = {empty = true}})
end

function SwornCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    return self:SetupBehaviourBtn2(btn_list, obj_vo)
end
function SwornCtrl:SetupBehaviourBtn2(btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
    if obj_vo.seq == self.data:GetSwornNpcSeq() and FunOpen.Instance:GetFunIsOpened(Mod.Society.Sworn) then
        local function func_tips()
            PublicPopupCtrl.Instance:HelpTip(174)
        end
        table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.Sworn.SwornBtn3, func_tips, true, nil))

        if not SwornData.Instance:IsHaveSworn() then
            local function func_sworn()
                if not TeamData.Instance:InTeam() or TeamData.Instance:MemberNumRole() < 2 then
                    PublicPopupCtrl.Instance:Center(Language.Sworn.SwornError1)
                    return
                end
                -- 不在线，不是好友，好友度不够，每个人都要显示出来。(有其它结义组服务端判断)
                local limit_list = self:MemberFilter()
                -- if limit_list.has_partner then
                --     PublicPopupCtrl.Instance:Center(Language.Sworn.SwornError9)
                --     return
                -- end
                if #limit_list.off_line_list > 0 then
                    PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornError4, self:MemberToStr(limit_list.off_line_list)))
                    return
                end
                if #limit_list.no_level_list > 0 then
                    PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornError8, self:MemberToStr(limit_list.no_level_list)))
                    return
                end
                if #limit_list.no_friend_list > 0 then
                    PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornError5, self:MemberToStr(limit_list.no_friend_list)))
                    return
                end
                if #limit_list.no_intimacy_list > 0 then
                    PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornError2, self:MemberToStr(limit_list.no_intimacy_list), self.data:GetSwornNeedIntimacy()))
                    return
                end
                -- ViewMgr:OpenView(SwornAffirmView)
                self:SendBeginSworn()
            end
            local function func_friend()
                self:OpenInviteFriendView()
            end
            table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.Sworn.SwornBtn2, func_friend, true, nil))
            table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.Sworn.SwornBtn1, func_sworn, true, nil))
        end
    end
end

function SwornCtrl:MemberFilter()
    local limit_list = {
        off_line_list = {}, --不在线列表
        no_intimacy_list = {}, --亲密度不够列表
        no_friend_list = {}, --非好友列表
        no_level_list = {}, --等级不够
        has_partner = false,
    }
    local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Society.Sworn)
    for i, v in ipairs(TeamData.Instance:GetMemberRoleList()) do
        if not v:IsSelf() then
            if not v:IsOnline() then
                table.insert(limit_list.off_line_list, v)
            end
            if v.info.level < open_level then
                table.insert(limit_list.no_level_list, v)
            end
            local friend_info = SocietyData.Instance:GetFriendInfoByUid(v.info.uid)
            if friend_info == nil then
                table.insert(limit_list.no_friend_list, v)
            else
                if SocietyData.Instance:GetFriendIntimacyVal(v.info.uid) < self.data:GetSwornNeedIntimacy() then
                    table.insert(limit_list.no_intimacy_list, v)
                end
            end
        end
    end
    return limit_list
end

function SwornCtrl:MemberToStr(member_list)
    local name_str = ""
    for i, v in ipairs(member_list) do
        if i == 1 then
            name_str = name_str .. v.info.name
        else
            name_str = name_str .. "、" .. v.info.name
        end
    end
    return name_str
end

function SwornCtrl:SendQuitReq(state)
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_LEAVE, state)
end

function SwornCtrl:SendKickOut(uid)
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_KICK, uid)
end

function SwornCtrl:SendInviteSwron(uid)
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_INVITE, uid)
end

function SwornCtrl:SendInviteApply(zu_id, is_apply)
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_INVITE_APPLY, zu_id, is_apply)
end

function SwornCtrl:OnSCJieYiInviteRoute(protocol)
    if ViewMgr:IsOpen(SwornInvitedView) then
        return
    end
    local friend_info = SocietyData.Instance:GetFriendInfoByUid(protocol.uid)
    if friend_info then
        ViewMgr:OpenView(SwornInvitedView, protocol)
    end
end

function SwornCtrl:SendChangeName(member_name)
    local protocol = GetProtocol(CSJieYiZuMemberName)
    protocol.member_name = member_name
    SendProtocol(protocol)
end

function SwornCtrl:SendChangeSwornDeclReq(text)
    local protocol = GetProtocol(CSJieYiZuText)
    protocol.text = text
    SendProtocol(protocol)
end

function SwornCtrl:SendChangeSwornNameReq(str1, str2)
    local protocol = GetProtocol(CSJieYiZuName)
    protocol.str1 = str1
    protocol.str2 = str2
    protocol.member_num = self.data:GetMemberNum()
    SendProtocol(protocol)
end

function SwornCtrl:OpenInviteFriendView()
    if SwornData.Instance:IsHaveSworn() then
        if SwornData.Instance:IsSwornLeader() then
            ViewMgr:OpenView(SwornInviteView)
        else
            PublicPopupCtrl.Instance:Center(Language.Sworn.SwornAdminError1)
        end
    else
        if TeamData.Instance:InTeam() then
            if not TeamData.Instance:IsLeader() then
                PublicPopupCtrl.Instance:Center(Language.Sworn.InviteError4)
                return
            end
        else
            TeamCtrl.Instance:SendCreateTeam()
        end
        ViewMgr:OpenView(SwornInviteView)
    end
end

function SwornCtrl:OnSCJieYiRoleXuanYanList(protocol)
    self.data:OnSCJieYiRoleXuanYanList(protocol)
end

function SwornCtrl:OnSCJieYiZuXuanYanList(protocol)
    self.data:OnSCJieYiZuXuanYanList(protocol)
end

function SwornCtrl:SendSeekingInfo()
    self:SendJieYiZuXuanYanReqList(0)
    self:SendJieYiZuXuanYanReqList(1)
end

function SwornCtrl:SendSeekingInsert(seeking_type, is_delete, seeking_notice)
    local protocol = GetProtocol(CSJieYiZuXuanYanReq)
    protocol.type = seeking_type
    protocol.is_delete = is_delete
    protocol.text = seeking_notice
    SendProtocol(protocol)
end

function SwornCtrl:SendJieYiZuXuanYanReqList(type)
    local protocol = GetProtocol(CSJieYiZuXuanYanReqList)
    protocol.type = type
    SendProtocol(protocol)
end

function SwornCtrl:SendWatchTeam(watch_jie_yi_zu_id)
    self.watch_jie_yi_zu_id = watch_jie_yi_zu_id
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_ZU_INFO, watch_jie_yi_zu_id)
end
function SwornCtrl:SendAddFriend(watch_jie_yi_zu_id)
    self.add_friend_zu_id = watch_jie_yi_zu_id
    self:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_ZU_INFO, watch_jie_yi_zu_id)
end

function SwornCtrl:SendAddFriendInfo(protocol)
    if SwornData.SameZuId(self.add_friend_zu_id, protocol.jie_yi_zu_id) then
        for i, v in ipairs(protocol.role_info) do
            if v.is_leader == 1 then
                local is_friend = SocietyData.Instance:IsFriend(v.uid)
                if is_friend then
                    PublicPopupCtrl.Instance:Center(Language.Sworn.AddFriendError2)
                    return
                end
                SocietyCtrl.Instance:SendFriendAdd(v.uid)
                break
            end
        end
    end
end

function SwornCtrl:OnSCJieYiZuRoleOnlineStatusNotice(protocol)
    if not self.data:IsHaveSworn() then
        return
    end
    self.data:OnRoleOnlineStateChange(protocol)
end
