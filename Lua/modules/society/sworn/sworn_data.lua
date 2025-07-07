SwornData = SwornData or BaseClass()
function SwornData:__init()
    if SwornData.Instance ~= nil then
        Debuger.LogError("[SwornData] attempt to create singleton twice!")
        return
    end
    SwornData.Instance = self

    self.cfg_auto = Config.deep_friendship_auto
    self:InitSkillList()

    self.create_info = SmartData.New({key = -1}) --结义创建信息
    self.sworn_view_tab = SmartData.New({index = 1}) --切换标签页用
    self.sworn_info = SmartData.New() --基础信息
    self.sworn_invite_care = SmartData.New({flush = false}) --邀请刷新用
    self.sworn_role_decl_list = SmartData.New({self_notice = "", list = {}})
    self.sworn_team_decl_list = SmartData.New({self_notice = "", list = {}})

    self.invited_list = {}
    self.in_sworn = false
    self.is_leader = false
end

function SwornData:__delete()
    SwornData.Instance = nil
end

function SwornData:GetOtherCfg()
    return self.cfg_auto.other[1]
end

--需要亲密度，结义NPC
function SwornData:GetSwornNeedIntimacy()
    return self:GetOtherCfg().intimacy_request
end
function SwornData:GetSwornNpcSeq()
    return self:GetOtherCfg().friendship_npc --586
end

function SwornData:RenameTeamCost()
    return {item_id = self:GetOtherCfg().title_revise_consume, num = self:GetOtherCfg().consume_num1}
end
function SwornData:RenameRoleCost()
    return {item_id = self:GetOtherCfg().name_revise_consume, num = self:GetOtherCfg().consume_num2}
end

--是否入伙了
function SwornData:IsHaveSworn()
    return self.in_sworn
end
--是否领导者(队长)
function SwornData:IsSwornLeader()
    return self.is_leader
end

--是否处理弹劾中
function SwornData:IsTanHe()
    local cur_time = TimeCtrl.Instance:GetServerTime()
    local end_time = self.sworn_info.vote_end_time or 0
    if end_time ~= 0 and end_time > cur_time then
        return true
    else
        return false
    end
end

-- 队长信息
function SwornData:LeaderInfo()
    local list = self:GetMemberList()
    if TableIsEmpty(list) then
        return
    end
    for i,v in pairs(list) do
        if v.is_leader == 1 then
            return v
        end
    end
end

-- 自己信息
function SwornData:MyInfo()
    local list = self:GetMemberList()
    if TableIsEmpty(list) then
        return
    end
    for i,v in pairs(list) do
        if v.uid == RoleData.Instance:GetRoleId() then
            return v
        end
    end
end

function SwornData:GetSwornDecl()
    return self.sworn_info.jie_yi_text or ""
end

function SwornData:GetLevelCfg(level)
    return self.cfg_auto.friendship_skill[level]
end

function SwornData:GetLevelList()
    return self.cfg_auto.friendship_skill
end

----------创建结义组
function SwornData:SetCreateInfo(protocol)
    self.create_info.key = protocol.key
    self.create_info.is_name_finish = protocol.is_name_finish
    self.create_info.jie_yi_name = protocol.jie_yi_name
    self.create_info.role_count = protocol.role_count
    self.create_info.role_info = protocol.role_info
    self.create_info.create_time = TimeHelper.GetServerTime()
end

function SwornData:SetCreateName(protocol)
    self.create_info.jie_yi_name = protocol.jie_yi_name
end

function SwornData:SetCreateConfirm(protocol)
    local keys = {}
    for i, v in pairs(protocol.role_id) do
        keys[v] = 1
    end
    for i, v in pairs(self.create_info.role_info) do
        v.is_confirm = keys[v.uid] or 0
    end
    self.create_info:Notify()
end

----------结义组信息
function SwornData:SetSwornInfo(protocol)
    self.sworn_info.jie_yi_zu_id = protocol.jie_yi_zu_id
    self.sworn_info.create_time = protocol.create_time
    self.sworn_info.jie_yi_name = protocol.jie_yi_name
    self.sworn_info.jie_yi_text = protocol.jie_yi_text
    self.sworn_info.role_info_count = protocol.role_info_count
    self.sworn_info.role_info = protocol.role_info
    self.sworn_info.vote_end_time = protocol.vote_end_time -- 投票结束时间

    self.in_sworn = self.sworn_info.create_time ~= 0
    for i, v in pairs(self.sworn_info.role_info) do
        if v.uid == RoleData.Instance:GetRoleId() then
            self.is_leader = v.is_leader == 1
            break
        end
    end
    self.sworn_info:Notify()
end
function SwornData:GetSwornInfo()
    return self.sworn_info
end

function SwornData:OnRoleOnlineStateChange(protocol)
    for i, v in pairs(self:GetMemberList()) do
        if v.uid == protocol.uid then
            v.last_login_time = protocol.last_login_time
            v.online_state = protocol.online_state
            self.sworn_info:Notify()
            break
        end
    end
end

function SwornData:OnSCJieYiZuLeaveNotice(protocol)
    self:ClearSwornInfo()
end

function SwornData:ClearSwornInfo()
    self.in_sworn = false
    self.is_leader = false
    self.sworn_info:Clear()
    self.sworn_info:Notify()
end

--计算结义天数
function SwornData:GetSwornDay()
    local sworn_time = TimeCtrl.Instance:ChangDataTime(self.sworn_info.create_time)
    local cur_time = TimeCtrl.Instance:ChangDataTime(TimeHelper.GetServerTime())
    return (cur_time - sworn_time) / TIME_DAY_SECOND_NUM + 1
end

--计算结义最低亲密度
function SwornData:GetMinIntimacy()
    local value = nil
    for i, v in pairs(self:GetMemberList()) do
        if v.uid ~= RoleData.Instance:GetRoleId() then
            local intimacy = SocietyData.Instance:GetFriendIntimacyVal(v.uid)
            if value == nil or value > intimacy then
                value = intimacy
            end
        end
    end
    return value or 0
end

--计算结义等级。
function SwornData:GetSwornLevel()
    local day = self:GetSwornDay()
    local intimacy = self:GetMinIntimacy()
    for i, v in ipairs(self.cfg_auto.friendship_skill) do
        if v.friendship_intimacy > intimacy or v.friendship_time > day then
            return i - 1
        end
    end
    return #self.cfg_auto.friendship_skill
end

function SwornData:GetMemberList()
    return self.sworn_info.role_info or {}
end
function SwornData:GetMemberNum()
    return self.sworn_info.role_info_count or 0
end

function SwornData:GetSelfInfo()
    for i, v in pairs(self:GetMemberList()) do
        if v.uid == RoleData.Instance:GetRoleId() then
            return v
        end
    end
end

function SwornData:MemberInTeam(uid)
    if self:IsHaveSworn() then
        for i, v in pairs(self:GetMemberList()) do
            if v.uid == uid then
                return true
            end
        end
    end
    return false
end

--可踢的组员  判断离线时间大于等于两天时间
function SwornData:GetCanKickOutList()
    local show_list = {}
    for i, v in pairs(self:GetMemberList()) do
        if v.uid ~= RoleData.Instance:GetRoleId() then
            table.insert(show_list, v)
        end
    end
    return show_list
end

function SwornData:GetSwornName()
    if self.sworn_info.jie_yi_name ~= nil then
        return self.sworn_info.jie_yi_name.sworn_name or ""
    end
    return ""
end

----------邀请信息
function SwornData:SetInvitedIndex(index)
    self.invite_index = index
end
function SwornData:IsInInvitedList(uid)
    return self.invited_list[uid]
end

function SwornData:AddToInvitedList(uid)
    self.invited_list[uid] = self.invite_index
end

function SwornData:CancelInvited(uid)
    self.invited_list[uid] = nil
end

function SwornData:ResetInvitedList()
    self.invited_list = {}
end

----------宣言信息
function SwornData:OnSCJieYiRoleXuanYanList(protocol)
    self.sworn_role_decl_list.list = protocol.xuan_yan_items
    self.sworn_role_decl_list.self_notice = ""
    for i, v in pairs(protocol.xuan_yan_items) do
        if v.uid == RoleData.Instance:GetRoleId() then
            self.sworn_role_decl_list.self_notice = v.text
            break
        end
    end
end

function SwornData:OnSCJieYiZuXuanYanList(protocol)
    self.sworn_team_decl_list.list = protocol.xuan_yan_items
    self.sworn_team_decl_list.self_notice = ""
    for i, v in pairs(protocol.xuan_yan_items) do
        if SwornData.SameZuId(v.jie_yi_zu_id, self.sworn_info.jie_yi_zu_id) then
            self.sworn_team_decl_list.self_notice = v.text
        end
    end
end

function SwornData:GetSelfDecl()
    return self.sworn_role_decl_list.self_notice or ""
end

function SwornData:GetTeamDecl()
    return self.sworn_team_decl_list.self_notice or ""
end

--判断两个结义组是否一样
function SwornData.SameZuId(zu_id1, zu_id2)
    if type(zu_id1) == "table" and type(zu_id2) == "table" then
        for i = 1, 2 do
            if zu_id1[i] ~= zu_id2[i] then
                return false
            end
        end
        return true
    end
    return zu_id1 == zu_id2
end

----------技能信息
function SwornData:InitSkillList()
    local list = {}
    for i, v in ipairs(self.cfg_auto.friendship_skill) do
        if list[v.skill_sort] == nil then
            list[v.skill_sort] = {}
        end
        table.insert(list[v.skill_sort], v)
    end
    self.skill_cfg_list = list
end

function SwornData:GetSkillShowList()
    local show_list = {}
    local swown_level = SwornData.Instance:GetSwornLevel()
    for i, v in ipairs(self.skill_cfg_list) do
        for level, cfg in ipairs(v) do
            if show_list[i] == nil or cfg.friendship_level <= swown_level then
                show_list[i] = cfg
            end
        end
    end
    return show_list
end

----------结义名字合理化
function SwornData.ByteToCount(iByte)
    if iByte > 0 and iByte < 128 then
        return 1
    elseif iByte >= 128 and iByte < 224 then
        return 2
    elseif iByte >= 224 and iByte < 240 then
        return 3
    elseif iByte >= 240 and iByte <= 247 then
        return 4
    end
    return 0
end
local illegal_list = {}
function SwornData.IsIllegalWord(str)
    for i, v in pairs(illegal_list) do
        if v == str then
            return true
        end
    end
    return false
end

--不符合字转*
function SwornData.NameFilter(str)
    local new_str = ""
    local lenInByte = #str
    local i = 1
    while true do
        if i > lenInByte then
            break
        end
        local curByte = string.byte(str, i)
        local byteCount = SwornData.ByteToCount(curByte)
        if byteCount == 0 then
            break
        end
        local temp_str = string.sub(str, i, i + byteCount - 1)
        if curByte <= 227 or curByte == 239 or SwornData.IsIllegalWord(temp_str) then
            new_str = new_str .. "*"
        else
            new_str = new_str .. temp_str
        end
        i = i + byteCount
    end
    return new_str
end

--是否含有不符合文字
function SwornData.IsIllegal(str)
    local lenInByte = #str
    local i = 1
    while true do
        if i > lenInByte then
            break
        end
        local curByte = string.byte(str, i)
        local byteCount = SwornData.ByteToCount(curByte)
        if byteCount == 0 then
            break
        end
        local temp_str = string.sub(str, i, i + byteCount - 1)
        if curByte <= 227 or curByte == 239 or SwornData.IsIllegalWord(temp_str) then
            return true
        end
        i = i + byteCount
    end
    return false
end

function SwornData.GetSwornRoleNameDesc(role_info)
    local desc = ""
    for i,v in ipairs(role_info) do
        if v ~= "" then
            desc = desc .. v .. "\n"
        end
    end
    return desc
end
