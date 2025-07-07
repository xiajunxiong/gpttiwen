FILE_REQ("modules/guild_embattle/guild_embattle_data")
VIEW_REQ("modules/guild_embattle/guild_embattle_namelist")
VIEW_REQ("modules/guild_embattle/guild_embattle_enter")
VIEW_REQ("modules/guild_embattle/guild_embattle_condition")
VIEW_REQ("modules/guild_embattle/guild_embattle_msg")
VIEW_REQ("modules/guild_embattle/guild_embattle_zone")
VIEW_REQ("modules/guild_embattle/guild_embattle_preview")
VIEW_REQ("modules/guild_embattle/guild_embattle_final")
GuildEmbattleCtrl = GuildEmbattleCtrl or BaseClass(BaseCtrl)
GUILD_HONOR_BATTLE_PROCESS = {
    WEEK_START = 0,--一周开始(客户端理解是获取到的是上周赛果和上周赛况)
    GUILD_FIGHT = 1,--本周开始第一次大乱斗(客户端理解是第一次大乱斗后发，这样才能拿到名单)
    READY = 2,--准备开始荣誉战(客户端理解是第二次大乱斗后发，这样就能拿到2次积分情况)
    FINISH = 3,--完成(客户端理解是家族荣誉战后发，这样才能获取到本周赛果和本周赛况)
}
GUILD_HONOR_REQ_TYPE_2 = {
    SCORE_INFO = 0,--请求某个家族的大乱斗一星期的分数 p1 guild_id
    SCORE_RANK = 1,--大乱斗星期的分数排行榜
    GUESSING = 2,--竞猜 p1 guild_id p2 win_rank
    CANCEL_GUESSING = 3,--取消竞猜 p1 guild_id p2 win_rank
    MATCH_RECORD_INFO = 4,--上次比赛信息
    GUESSING_INFO = 5,  --竞猜信息
    GUESSING_CHAMPION = 6, --冠军竞猜 p1 guild_id
    CANCEL_GUESSING_CHAMPION = 7,--取消冠军竞猜 p1 guild_id
}
GUILD_HONOR_STATE = {
    GUESSING = 0,
    FIGHTING = 1,
    FIGHTEND = 2
}
function GuildEmbattleCtrl:__init()
    if GuildEmbattleCtrl.Instance ~= nil then
        Debuger.LogError("[GuildEmbattleCtrl] attempt to create singleton twice!")
        return
    end
    GuildEmbattleCtrl.Instance = self
    self.data = GuildEmbattleData.New()

    self:RegisterProtocol(SCGuildHonorBattleProcessInfo, "SCGuildHonorBattleProcessInfo")
    self:RegisterProtocol(SCGuildHonorBattleScoreInfo, "SCGuildHonorBattleScoreInfo")
    self:RegisterProtocol(SCGuildHonorScoreRankInfo, "SCGuildHonorScoreRankInfo")
    self:RegisterProtocol(SCGuildHonorBattleGuessInfo, "SCGuildHonorBattleGuessInfo")
    self:RegisterProtocol(SCGuildHonorBattleMatchInfo, "SCGuildHonorBattleMatchInfo")
    self:RegisterProtocol(SCGuildHonorBattleMatchRecord, "SCGuildHonorBattleMatchRecord")
    self:RegisterProtocol(SCGuildHonorBattleTopInfo, "SCGuildHonorBattleTopInfo")
    self:RegisterProtocol(SCGuildHonorRoleBattleState, "SCGuildHonorRoleBattleState")
    self:RegisterProtocol(CSGuildHonorReq2)
end
function GuildEmbattleCtrl:__delete()
    
end
function GuildEmbattleCtrl:TestBecomeRecord()
    for index, value in ipairs(self.data.match_info) do
        value:BecomeRecord()
        if value:IsWinGroup() then
            --胜者组里有一些基础信息
            self.data:SetBase(value)
        end
    end
    self.data.last_match_info:Set(self.data.match_info)
end
function GuildEmbattleCtrl:SCGuildHonorBattleProcessInfo(protocol)
    -- LogDG("荣誉战过程信息", protocol)
    self.data:Clear()
    -- GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_RANK)
    self.data:SetBattleProcess(protocol)
    local act_info = ActivityData.Instance:GetActivityStatusInfo(ActType.GuildEmbattle)
    if act_info.status == ActStatusType.Open then
        GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING_INFO)
    end
    if protocol.process == GUILD_HONOR_BATTLE_PROCESS.WEEK_START or 
    protocol.process == GUILD_HONOR_BATTLE_PROCESS.FINISH then
        --请求上届名单
        GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.MATCH_RECORD_INFO)
        -- GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_RANK)
    elseif protocol.process == GUILD_HONOR_BATTLE_PROCESS.GUILD_FIGHT 
    or protocol.process == GUILD_HONOR_BATTLE_PROCESS.READY  then
        --请求分数排行榜
        GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_RANK)
        if GuildData.Instance:GetGuildID() > 0 then
            GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_INFO, GuildData.Instance:GetGuildID())
        end
    else
        -- 未开始荣誉战前
        -- GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_RANK)
        -- if GuildData.Instance:GetGuildID() > 0 then
        --     GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_INFO, GuildData.Instance:GetGuildID())
        -- end
        -- GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING_INFO)
    end
end
function GuildEmbattleCtrl:SCGuildHonorBattleScoreInfo(protocol)
    if protocol.guild_id == GuildData.Instance:GetGuildID() then
        self.data:SetMyBattleScoreInfo(protocol)
    end
end
function GuildEmbattleCtrl:SCGuildHonorScoreRankInfo(protocol)
    self.data:SetScoreRankInfo(protocol)
end
function GuildEmbattleCtrl:SCGuildHonorBattleGuessInfo(protocol)
    self.data:SetGuessInfo(protocol)
end
function GuildEmbattleCtrl:Test()
    -- match_info
    local info = {}
    info[1] = GuildHonorBattleMatchInfo.New()
    info[1]:TestData(1, 15, 15)
    info[2] = GuildHonorBattleMatchInfo.New()
    info[2]:TestData(9, 8, 7)
    self.data:SetMatchInfo(info)
end
function GuildEmbattleCtrl:SCGuildHonorBattleMatchInfo(protocol)
--    LogDG(protocol)
    self.data:SetMatchInfo(protocol.match_info)
    --获取胜者组进度
    for index, value in ipairs(protocol.match_info) do
        if value:IsWinGroup() then
            --胜者组里有一些基础信息
            self.data:SetBase(value)
        end
    end
    local is_active = false
    for index, value in ipairs(protocol.match_info) do
        if value:WillBattle() then
            is_active = true
            break
        end
    end
    MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = is_active})
    if self.data:Base().is_finish == 1 then
        MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = false})
        GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.MATCH_RECORD_INFO)
    end
    -- self.data:SetFighterInfo(protocol)
    -- self.data:SetRound(protocol.curr_round)
    -- self.data:SetState(protocol.round_state)
    --[[ if protocol.round_state == 0 and self.data:GuildJoin() then
        local info = self.data:OpponetInfo()
        if info == nil then
            MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = false})
        else
        --如果自己已经输了也不显示
            MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = true})
        end
    else
        MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = false})
    end ]]
    --[[ self.data:SetBase(protocol)
    if protocol.is_finish == 1 then
        MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = false})
        GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.MATCH_RECORD_INFO)
    end ]]
    self.data:ViewData().flush = not self.data:ViewData().flush
end
function GuildEmbattleCtrl:SCGuildHonorBattleMatchRecord(protocol)
    --上次赛程信息
    --这个也要设置一些基础信息
    for index, value in pairs(protocol.match_info_record) do
        value.curr_round = self.data:FinalRound(value.fighter_num)
        if value:IsWinGroup() then
            --curr_round=能进行的轮数
            --胜者组里有一些基础信息
            self.data:SetBase(value)
        end
    end
    self.data:SetMathcRecord(protocol)

end
function GuildEmbattleCtrl:SCGuildHonorBattleTopInfo(protocol)
    GuildData.Instance:SetTopInfo(protocol.guild_top_info)
end
function GuildEmbattleCtrl:SCGuildHonorRoleBattleState(procotol)
    BattleData.Instance:BaseData().embattle_state = procotol.state
end
function GuildEmbattleCtrl.CSGuildHonorReq2(req_type, p1, p2)
    local protocol = GetProtocol(CSGuildHonorReq2)
    protocol.req_type = req_type or 0
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    SendProtocol(protocol)
end