VIEW_REQ("modules/serveractivity/rand_activity/competition_rank_view")

CompetitionRanklCtrl = CompetitionRanklCtrl or BaseClass(BaseCtrl)

function CompetitionRanklCtrl:__init()
    if CompetitionRanklCtrl.Instance ~= nil then
        Debuger.LogError("[CompetitionRanklCtrl] attempt to create singleton twice!")
        return
    end
    CompetitionRanklCtrl.Instance = self
    self.data = CompetitionRanklData.New()
    self:RegisterAllProtocols()
    -- 0下发角色信息
    -- 1请求领取奖励
    -- 2活动信息下发
    -- 3领取每日礼包
    -- 4请求排行榜信息
end

function CompetitionRanklCtrl:__delete()
    CompetitionRanklCtrl.Instance = nil
end

function CompetitionRanklCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAServerCompetitionActivityInfo, "OnSCRAServerCompetitionActivityInfo")
    self:RegisterProtocol(SCRAServerCompetitionOtherInfo, "OnRAServerCompetitionOtherInfo")
    self:RegisterProtocol(SCRAServerCompetitionRoleAllInfo, "OnSCRAServerCompetitionRoleAllInfo")
    self:RegisterProtocol(SCRAServerCompetitionRoleInfo, "OnSCRAServerCompetitionRoleInfo")
    self:RegisterProtocol(SCQueryRoleSimpleInfo, "OnSCQueryRoleSimpleInfo")
    self:RegisterProtocol(SCRAServerCompetitionAllRankInfo, "SCRAServerCompetitionAllRankInfo")

	ActivityRandData.Instance:CustomClickHandle(ACTIVITY_TYPE.KF_COMPETITION_RANK,function()
		ViewMgr:OpenViewByKey(Mod.RandActivity.CompetitionRank)
	end)
    Remind.Instance:Register(Mod.RandActivity.CompetitionRank,self.data.info_data,function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
			return 0
		end
		return self.data:GetCompetitionRankRedNum()
	end)
end

function CompetitionRanklCtrl:SendCompetitionRoleSimpleInfo(role_info)
    if role_info ~= nil then
        local role_id = role_info.role_id or role_info.uid or role_info.role.uid
        local protocol = GetProtocol(CSReferRoleEquipment)
        protocol.role_id = role_id or 0
        protocol.query = 1
        SendProtocol(protocol)
        role_info.role_id = role_id
        self.data.rank_data.role = role_info
    end
end

function CompetitionRanklCtrl:SendCompetitionRankType(type)
    local CompetitionRank = ServeractivityConfig.CompetitionRank
    if type == CompetitionRank.Type.ArenaRank then
        ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_RANK_INFO, CompetitionRank.ArenaRankRankNum)
        ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_RANK_TOP_INFO, 0)
    elseif type == CompetitionRank.Type.CourageChallengeRank then
        CourageChallengeCtrl.Instance:SendCourageChallengeRankListReq(CourageChallengeRankType.Main,CompetitionRank.ArenaRankRankNum)
    else
        SocietyCtrl.Instance:SendCrossGetPersonRankList(CompetitionRank.SendRank[type],{},CompetitionRank.RankNum)
    end
end
function CompetitionRanklCtrl:SendReqAllRankInfo()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.KF_COMPETITION_RANK
    param_t.opera_type = 4
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function CompetitionRanklCtrl:OnSCRAServerCompetitionActivityInfo(protocol)
    --根据数量来判断version
    self.data:SetServerCompetitionActivityInfo(protocol)
end

function CompetitionRanklCtrl:OnRAServerCompetitionOtherInfo(protocol)
    self.data:SetRAServerCompetitionOtherInfo(protocol)
end

function CompetitionRanklCtrl:OnSCRAServerCompetitionRoleAllInfo(protocol)
    self.data:SetServerCompetitionRoleAllInfo(protocol)
end

function CompetitionRanklCtrl:OnSCRAServerCompetitionRoleInfo(protocol)
    self.data:SetCompetitionRoleInfo(protocol)
end

function CompetitionRanklCtrl:OnSCQueryRoleSimpleInfo(protocol)
    self.data:SetCompetitionRoleSimpleInfo(protocol)
end
function CompetitionRanklCtrl:SCRAServerCompetitionAllRankInfo(protocol)
    self.data:SetRankInfo(protocol)
end

--===========================CompetitionRanklData===========================
CompetitionRanklData = CompetitionRanklData or BaseClass()

function CompetitionRanklData:__init()
    if CompetitionRanklData.Instance ~= nil then
        Debuger.LogError("[CompetitionRanklData] attempt to create singleton twice!")
        return
    end
    CompetitionRanklData.Instance = self
    self.rank_data = SmartData.New({select_type = 0,role = {}})
    self.info_data = SmartData.New({info = {},rank_time_list = {},role = {},flush = false})
    self.rank_list = SmartData.New()
    self.select_type_show_model_index = {}
end

function CompetitionRanklData:GetRankConfig(type)
    return self.rank_config[type] or {}
end
function CompetitionRanklData:Version()
    local time_stamp = TimeCtrl.Instance:ServerStartTS()--ActivityData.GetStartStampTime(ACTIVITY_TYPE.KF_COMPETITION_RANK)
    if time_stamp == nil then
        return 0
    end
    local version = 0
    local timestamp_config = Config.server_competition_auto.timestamp
    -- LogDG("Version",time_stamp)
    for k, v in ipairs(timestamp_config) do
        -- LogDG("ver_timestamp", v.ver_timestamp)
        if time_stamp >= v.ver_timestamp then
            version = math.max(version, v.version)
        end
    end
    return version
end
--获取新服比拼配置
function CompetitionRanklData:GetCompetitionRankConfig()
    --新增版本逻辑
    local version = self:Version()
    local version_config = {}
    for i, v in ipairs(Config.server_competition_auto.activity_reward) do
        if v.version == version then
            table.insert(version_config, v)
        end
    end
    -- LogDG("需要处理的活动版本号", version)
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.KF_COMPETITION_RANK, version_config)
end

--============================================================================================================
function CompetitionRanklData:InitCompetitionRankData()
    -- if not self.rank_config then
        self.rank_config = {}
        self.rank_btn_config = {}
        
        for k, v in ipairs(self:GetCompetitionRankConfig()) do
            if self.rank_config[v.rank_type] == nil then
                self.rank_config[v.rank_type] = {}
                self.rank_config[v.rank_type].rank_list = {}
                self.rank_config[v.rank_type].gift_list = {}
            end
            if v.reward_type == 1 then
                table.insert(self.rank_config[v.rank_type].rank_list, v)
            else
                table.insert(self.rank_config[v.rank_type].gift_list, v)
            end
            if self.rank_btn_config[v.rank_type] == nil then
                self.rank_btn_config[v.rank_type] = {}
                self.rank_btn_config[v.rank_type].start_time = v.start_time
                self.rank_btn_config[v.rank_type].rank_type = v.rank_type
                self.rank_btn_config[v.rank_type].version = v.version
            end
        end
    -- end
end
--============================================================================================================

function CompetitionRanklData:SetServerCompetitionActivityInfo(protocol)
    self:InitCompetitionRankData()
    self:InitBtnListData(protocol.rank_time_list)
    self.info_data.rank_time_list = protocol.rank_time_list
    self.info_data.flush = not self.info_data.flush
end

function CompetitionRanklData:SetRAServerCompetitionOtherInfo(protocol)
    self.info_data.is_fetch_day_gift_flag = protocol.is_fetch_day_gift_flag
    self.info_data.flush = not self.info_data.flush
    if next(self.info_data.rank_time_list) then
        self:InitBtnListData(self.info_data.rank_time_list)
    end
end

function CompetitionRanklData:IsFetchDayGiftFlag()
    return self.info_data.is_fetch_day_gift_flag or 0
end

function CompetitionRanklData:InitBtnListData(rank_time_list)
    local list = {}
    for k,v in pairs(self.rank_btn_config) do
        local start_time = v.start_time - TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.KF_COMPETITION_RANK)
        local end_time = TimeCtrl.Instance:GetDataDayNum(rank_time_list[v.rank_type], TimeHelper.GetServerTime()) - 1
        if TimeHelper.GetServerTime() < rank_time_list[v.rank_type] then
            table.insert(list,{type = v.rank_type,time = rank_time_list[v.rank_type],day = start_time,end_day = end_time, weight = start_time, version = v.version})
        else
            table.insert(list,{type = v.rank_type,time = 0,day = 0, end_day = 100, weight = 100 - v.start_time, version = v.version})
        end
    end
    if self:Version() == 1 then
        table.sort(list,DataSorter.KeyLowerSorter([[end_day]]))
    else
        table.sort(list,DataHelper.SortFunc([[weight]]))
    end
    for i,v in ipairs(list) do
        v.index = i
    end
    self.rank_data.btn_list = list
    -- ActivityRandData.Instance:RegisterActCountDown(Mod.RandActivity.CompetitionRank, BindTool.Bind(self.Time, self))
end

function CompetitionRanklData:Time(timeMeter)
    local param_t = {
        stamp_time = self.rank_data.btn_list[1].time,
        show_type = TimeType.Type_Time_8,
        none_zero = false,
    }
    -- if timeMeter then
        timeMeter:SetData(param_t)
        timeMeter:SetCallBack(BindTool.Bind(self.Time, self))
    -- end
end

function CompetitionRanklData:GetIsOpenEnd(rank_type)
    if self.rank_btn_config == nil then return false end
    if ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) == false then return false end
    local config = self.rank_btn_config[rank_type] or {}
    local day_num = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.KF_COMPETITION_RANK)
    local time = self.info_data.rank_time_list[rank_type] or 0
    return ((config.start_time or 0) - day_num) <= 0 and (time > 0 and TimeHelper.GetServerTime() < time)
end

function CompetitionRanklData:GetCompetitionRankRedNum()
    if self:IsFetchDayGiftFlag() == 0 then
        return 1
    end
    for k,v in pairs(self.info_data.info) do
        if self.rank_config and self.rank_config[k] then
            if self:GetIsOpenEnd(k) and self:GetCompetitionRankNum(k,v.cur_value) then
                return 1
            end
        end
    end
    return 0
end

function CompetitionRanklData:GetCompetitionRankNum(type,cur_value)
    local config = self:GetRankConfig(type) or {}
    local gift_list =  config.gift_list or {}
    for k,v in pairs(gift_list) do
        if type == ServeractivityConfig.CompetitionRank.Type.ArenaRank then
            if self:GetRankRewardFlag(type,v.seq) == 0 and cur_value <= v.parameter_1 then
                return true
            end
        else
            if self:GetRankRewardFlag(type,v.seq) == 0 and cur_value >= v.parameter_1 then
                return true
            end
        end
    end
end

--===========================新修改===========================
function CompetitionRanklData:SetCompetitionRoleSimpleInfo(protocol)
    self.info_data.role = protocol.info
end

function CompetitionRanklData:SetServerCompetitionRoleAllInfo(protocol)
    self:InitCompetitionRankData()
    self.info_data.info = protocol.rank_info
    self.info_data.is_fetch_day_gift_flag = protocol.is_fetch_day_gift_flag
end

function CompetitionRanklData:SetCompetitionRoleInfo(protocol)
    self:InitCompetitionRankData()
    --[[ local old = self.info_data.info[protocol.info.rank_type]
    if old and old.cur_value ~= protocol.info.cur_value then
        CompetitionRanklCtrl.Instance:SendReqAllRankInfo()
    end ]]
    self.info_data.info[protocol.info.rank_type] = protocol.info
    self.info_data.flush = not self.info_data.flush
end

function CompetitionRanklData:SetSelectData(data)
    self.rank_data.select_type = data.type
end

function CompetitionRanklData:GetSelectType()
    return self.rank_data.select_type
end

function CompetitionRanklData:GetBtnList()
    return self.rank_data.btn_list
end

function CompetitionRanklData:GetShowRoleData()
    local role = self.info_data.role
    if role and role.role_name == "" then
        role.role_name = self:GetSelectRoleName()
    end
    return role
end

function CompetitionRanklData:GetRankInfo(rank_type)
    return self.info_data.info[rank_type]
end

function CompetitionRanklData:GetRankRewardFlag(rank_type,seq)
    local info = self:GetRankInfo(rank_type)
    return info and (info.reward_flag[seq] or 0) or 0
end

function CompetitionRanklData:GetCompetitionBtnData()
    local data = ServerActivityData.Instance:GetOpenServerConfig(ACTIVITY_TYPE.KF_COMPETITION_RANK)
    data.mod = TabMod(data.mod_key)
    return data
end

function CompetitionRanklData:GetSelectRoleName()
    local role = self.rank_data.role
    return role.name or role.role_name or ""
end
function CompetitionRanklData:GetSelectRoleId()
    local role = self.rank_data.role
    return role.role_id or role.uid or 0
end

function CompetitionRanklData:CheckRankListFlush()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
        return
    end
    if not FunOpen.Instance:IsFunOpen(Mod.RandActivity.CompetitionRank) then
        return
    end
    if not ViewMgr:IsOpen(CompetitionRankView) then
        return
    end
    self.info_data.flush = not self.info_data.flush
end

function CompetitionRanklData:GetGiftList(type)
    local gift_list,list = self:GetRankConfig(type).gift_list or {},{0}
    table.sort(gift_list,DataHelper.SortFunc([[parameter_1]]))
    if type == ServeractivityConfig.CompetitionRank.Type.ArenaRank then
        gift_list = DataHelper.ReverseList(gift_list)
        list = {(gift_list[1].parameter_1 + gift_list[#gift_list].parameter_1) * -1}
        for i,v in ipairs(gift_list) do
            list[i + 1] = v.parameter_1 * -1
        end
    else
        for i=1,#gift_list do
            list[i + 1] = gift_list[i].parameter_1
        end
    end
    return gift_list,list
end

function CompetitionRanklData:GetRankRewardData(rank)
    local reward_list = {}
    local rank_list = self:GetRankConfig(self:GetSelectType()).rank_list or {}
    for k,v in pairs(rank_list) do
        if v.parameter_1 == v.parameter_2 and v.parameter_1 == rank then
            reward_list = v.reward_item
        elseif rank >= v.parameter_1 and rank <= v.parameter_2 then
            reward_list = v.reward_item
        end
    end
    return DataHelper.FormatItemList(reward_list)
end

function CompetitionRanklData:GetTitleName(type)
    local index = self.select_type_show_model_index[self:GetSelectType()] or 1
    local rank_info = self:GetRankConfig(type).rank_list[index] or {}
    if rank_info.reward_item and rank_info.reward_item[0] then
        local item_config = Item.GetConfig(rank_info.reward_item[0].item_id or 0) or {}
        local title_cfg = Config.face_cfg_auto.resource[item_config.param1]
        return title_cfg and AppearanceConfig.IconPre[2] .. title_cfg.res_id, AppearanceConfig.IconType[2] or nil
    end
end


function CompetitionRanklData:GetRankList(type)
    -- if ServeractivityConfig.CompetitionRank.Type.ArenaRank == type then
    -- return DataHelper.FormatList(ArenaData.Instance:GetRankList(),1,ServeractivityConfig.CompetitionRank.ArenaRankRankNum)
   --[[  elseif ServeractivityConfig.CompetitionRank.Type.CourageChallengeRank == type then
        return CourageChallengeData.Instance:GetRankList(CourageChallengeRankType.Main)
    else
        local rank_type = ServeractivityConfig.CompetitionRank.SendRank[type] or 0
        return SocietyData.Instance:GetRankInfo(rank_type) ]]
    -- end
    return self.rank_list:Val()[type] or {}
end

function CompetitionRanklData:SetRankInfo(protocol)
   self.rank_list:Set(protocol.rank_info)
end
function CompetitionRanklData:GetNeedValue(type, rank)
    local rank_list = self:GetRankConfig(type).rank_list or {}
    local need_value = 0
    for k,v in pairs(rank_list) do
        if v.parameter_1 == v.parameter_2 and v.parameter_1 == rank then
            need_value = v.parameter_3
        elseif rank >= v.parameter_1 and rank <= v.parameter_2 then
            need_value = v.parameter_3
        end
    end
    return need_value
end