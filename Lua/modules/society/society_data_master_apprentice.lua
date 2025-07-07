
function SocietyData:MasterApprenticeConfig()
    self.config_ma = Config.mentor_cfg_auto
    self.config_ma_other = self.config_ma.other[1]
    self.maq_list = {}
    for k, v in ipairs(self.config_ma.mentor_question) do
        self.maq_list[v.button_group] = self.maq_list[v.button_group] or {question = v.question, buttons = {}}
        table.insert(self.maq_list[v.button_group].buttons, v)
    end
    self.maq_avatars = {}
end

function SocietyData:MasterApprenticeData()
    self.is_master = false
    self.master_apprentice_num = 0
    self.master_apprentice_num1 = 0
    self.master_apprentice_num2 = 0
    self:MasterApprenticeList()
    self:MasterApprenticeAddList()
    self:MasterApprenticeApplyList()
end

function SocietyData:MasterApprenticeList()
    self.master_apprentice_list_m = {}
    self.master_apprentice_list_a = {}
    for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
        self.master_apprentice_list_m[i] = self:RoleInfoGet()
        self.master_apprentice_list_a[i] = self:RoleInfoGet()
    end
end

function SocietyData:MasterApprenticeAddList()
    self.master_apprentice_add_list = {
        [0] = {},
        [1] = {},
    }
end

function SocietyData:MasterApprenticeApplyList()
    self.master_apprentice_apply_list = {}
    for _, type in pairs(SocietyConfig.ShiTuSeekingType) do
        self.master_apprentice_apply_list[type] = {}
    end
end

function SocietyData:IsMaster()
    return self.is_master
end

function SocietyData:GetGraduateLevel()
    return self.config_ma_other.graduate_level
end

function SocietyData:SetShiTuMakeRelationRoute(protocol)
    local type = protocol.type
    local uid = protocol.role_info.uid
    if nil == self.master_apprentice_apply_list[type][uid] then
        self.master_apprentice_apply_list[type][uid] = self:RoleInfoGet()
    end
    self.master_apprentice_apply_list[type][uid]:RoleInfo(protocol.role_info)
    self:NewMasterRedPoint(true)
end

function SocietyData:MasterApprenticeApplyRemoveByUid(uid, type)
    -- local type = self.is_master and SocietyConfig.ShiTuSeekingType.seek_master or SocietyConfig.ShiTuSeekingType.seek_apprentice
    self:RoleInfoRecycle(self.master_apprentice_apply_list[type][uid])
    self.master_apprentice_apply_list[type][uid] = nil
    self:NewMasterRedPoint()
end

function SocietyData:MasterApprenticeApplyEmpty()
    for type, list in pairs(self.master_apprentice_apply_list) do
        for _, item in pairs(list) do
            self:RoleInfoRecycle(item)
        end
        self.master_apprentice_apply_list[type] = {}
    end
    self:NewMasterRedPoint()
end

function SocietyData:GetMasterApprenticeApplyList()
    -- local type = self.is_master and SocietyConfig.ShiTuSeekingType.seek_master or SocietyConfig.ShiTuSeekingType.seek_apprentice
    -- return self.master_apprentice_apply_list[type]
    local list = {}
    for k, info in pairs(self.master_apprentice_apply_list[0]) do
        table.insert(list, info)
    end
    for k, info in pairs(self.master_apprentice_apply_list[1]) do
        table.insert(list, info)
    end
    return list
end

function SocietyData:SetShiTuRelationInfo(protocol)
    self.is_master = RoleData.Instance:GetBaseData().level >= self.config_ma_other.graduate_level
    local master_num = 0
    for i = 1, SocietyConfig.MASTER_COUNT_MAX do
        local uid = protocol.master_info_list[i].uid
        -- self.is_master = self.is_master and (0 == uid)
        if 0 == uid then
            protocol.master_info_list[i] = nil
        else
            master_num = master_num + 1
        end
    end
    local max_num = self.is_master and SocietyConfig.MasterApprenticeMax.apprentice or SocietyConfig.MasterApprenticeMax.master
    self.master_apprentice_num = self.is_master and protocol.apprentice_count or master_num
    self.master_apprentice_num1 = master_num
    self.master_apprentice_num2 = protocol.apprentice_count
    local list = self.is_master and protocol.apprentice_info_list or protocol.master_info_list
    for i = 1, SocietyConfig.MasterApprenticeMax.master do
        self.master_apprentice_list_m[i]:RoleInfo(protocol.master_info_list[i])
    end
    for i = 1, SocietyConfig.MasterApprenticeMax.apprentice do
        self.master_apprentice_list_a[i]:RoleInfo(protocol.apprentice_info_list[i])
    end

    self:SocietyShiTuAchieveRedPoint()
end

function SocietyData:GetMasterApprenticeList(seek_type)
    local list, sel_role = {}
    if SocietyConfig.ShiTuSeekingType.seek_master == seek_type then
        local num = SocietyConfig.MasterApprenticeMax.master
        for i = 1, num do
            list[i] = self.master_apprentice_list_m[i]
        end
    else
        local num = SocietyConfig.MasterApprenticeMax.apprentice
        for i = 1, num do
            list[i] = self.master_apprentice_list_a[i]
        end
    end
    if self.cur_sel_role then
        for _, role in ipairs(list) do
            if self.cur_sel_role == role then
                sel_role = role
            end
        end
    end
    return list, sel_role
end


function SocietyData:SetShiTuBreakRelationRoute(protocol)
    local role_info = protocol.role_info
    local reply_type = Language.PublicPopup.ReplyType
    local info = {
        content = string.format(Language.Society.GraduadeDesc[protocol.type], role_info.name),
        confirm = {
            name = reply_type.agree,
            func = function()
				SocietyCtrl.Instance:SendShiTuBreakRelationAck(protocol.type, role_info.uid, true)
                ViewMgr:CloseView(DialogTipsView)
            end
		},
        cancel = {
            name = reply_type.refuse,
            func = function()
                SocietyCtrl.Instance:SendShiTuBreakRelationAck(protocol.type, role_info.uid, false)
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function SocietyData:SetShiTuSeekingInfo(protocol)
    local seeking_type = protocol.seeking_type
    -- local role_id = RoleData.Instance:GetRoleId()
    self.ma_seeking_notice = protocol.seeking_notice

    for _, item in pairs(self.master_apprentice_add_list[seeking_type]) do
        self:RoleInfoRecycle(item)
    end
    self.master_apprentice_add_list[seeking_type] = {}
    for _, info in pairs(protocol.role_info_list) do
        local uid = info.uid
        if uid ~= 0 then
            table.insert(self.master_apprentice_add_list[seeking_type], self:RoleInfoGet():RoleInfo(info))
        end
    end
    self:SortMasterApprenticeAddList(self.master_apprentice_add_list[seeking_type])
end

function SocietyData:SortMasterApprenticeAddList(list)
    table.sort(list, function (a, b)
		if not a or not b then return a end
        local reco_a = self.maq_avatars[a.info.avatar_type] and (a:IsSelf() and 1 or 2) or 0
        local reco_b = self.maq_avatars[b.info.avatar_type] and (b:IsSelf() and 1 or 2) or 0
        return reco_a > reco_b
    end)
end

function SocietyData:GetMasterApprenticeAddList(seeking_type)
    return self.master_apprentice_add_list[seeking_type]
end

function SocietyData:NewMasterRedPoint(new_ma)
    -- if new_ma then
    --     self.new_master_red_point.val = true
    --     return
    -- end
    -- local type = self.is_master and SocietyConfig.ShiTuSeekingType.seek_master or SocietyConfig.ShiTuSeekingType.seek_apprentice
    local mrp = false
	for _, friend in pairs(self.master_apprentice_apply_list[0]) do
		mrp = true
	end
    for _, friend in pairs(self.master_apprentice_apply_list[1]) do
		mrp = true
	end
	self.new_master_red_point.val = mrp
end

function SocietyData:IsNeedOpenMaqView()
    local value = self.maq_info[1]
    self.maq_options:Set{cur_page = 1, jobs = {}}
    return nil == value or 0 == value
end

function SocietyData:SetShiTuPreferenceInfo(protocol)
    self.maq_avatars = {}
    for i = 1, SocietyConfig.PREFERENCE_NUM do
        local avatar = protocol.maq_info[i]
        if avatar > 0 then
            self.maq_avatars[avatar] = true
        end
    end
    self:SortMasterApprenticeAddList(self.master_apprentice_add_list[0])
    self:SortMasterApprenticeAddList(self.master_apprentice_add_list[1])
    self.maq_info:Set(protocol.maq_info)
end

function SocietyData:IsMasterOrApprentice(uid)
    local value, type = false, 0
    for i = 1, SocietyConfig.MasterApprenticeMax.master do
        local info = self.master_apprentice_list_m[i].info
        if info and uid == info.uid then
            value = true
            type = 0
            break
        end
    end
    for i = 1, SocietyConfig.MasterApprenticeMax.apprentice do
        local info = self.master_apprentice_list_a[i].info
        if info and uid == info.uid then
            value = true
            type = 1
            break
        end
    end
    return value, type
end
function SocietyData:MasterOrApprenticeInfo(uid)
    for i = 1, SocietyConfig.MasterApprenticeMax.master do
        local info = self.master_apprentice_list_m[i].info
        if info and uid == info.uid then
            return info
        end
    end
    for i = 1, SocietyConfig.MasterApprenticeMax.apprentice do
        local info = self.master_apprentice_list_a[i].info
        if info and uid == info.uid then
            return info
        end
    end
    return nil
end