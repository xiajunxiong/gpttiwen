MSLanternData = MSLanternData or BaseClass()

-- 山海灯会操作请求类型
MSLanternReqType = {
    Info = 0,       --请求下发
    NPCPos = 1,     --请求npc位置下发
    StartAnswer = 2,--开始答题
    Answer = 3,     --答题，p1：seq
    Fight = 4,      --战斗
    UseCard = 5,    --使用卡
    Praise = 6,     --点赞，p1：uid
    Friend = 7,     --加好友，p1：uid
}

-- 山海灯会卡片类型
MSLanternCardType = {
    Free = 1,       --免答卡
    Change = 2,     --换题卡
    ReduceWrong = 3,--除错卡
}

-- 山海灯会飘字协议类型
MSLanternTxtType = {
    Praise = 1,     --点赞
    Friend = 2,     --好友申请
    UseCard = 3,    --使用卡
}

function MSLanternData:__init()
    if MSLanternData.Instance ~= nil then
        Debuger.LogError("[MSLanternData] attempt to create singleton twice!")
        return
    end
    MSLanternData.Instance = self
    self.config = Config.lantern_riddle_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New({base = {}, other = {}, result = {}, timestamp = 0, praise = {}})
    -- 两个npc的info
    self.npc_info = nil
    self.lantren_info = nil
    -- 重上线需要重新找npc
    self.is_need_find = true
    -- 战斗完最终奖励
    self.last_item_list = {}
    -- 用于延迟显示对勾
    self.is_show_right = false

    self.last_data = SmartData.New({info = {}})
    self.use_card_data = SmartData.New({info = {}})

    Cfg.Register("MSLanternQuetion", Cfg.RealTimeSearch(self.config.question, "question_id"))
    Cfg.Register("MSLanternAnswer", Cfg.RealTimeSearch(self.config.answer, "group_id"))
    Cfg.Register("MSLanternReward", Cfg.RealTimeSearch(self.config.reward_show, "satisfaction"))
    Cfg.Register("MSLanternPos", Cfg.RealTimeSearch(self.config.lantern, "lantern"))
    Cfg.Register("MSLanternWord", Cfg.RealTimeSearch(self.config.word, "word_id"))

    self:ResetData()
end

function MSLanternData:__delete()
    MSLanternData.Instance = nil
end

function MSLanternData:ResetData()
    self:ResetBase()
    self:ResetOther()
    self:ResetResult()
    self:ResetPraise()
    self:ResetUseCard()
    self:ResetLastResult()
    self:DisposeScene()
    self.is_need_find = true
end

function MSLanternData:ResetBase()
    local base = {}
    base.day_start_flag = 0
    base.is_get_last_reward = 0
    base.answer_count = 0
    base.satisfy = 0
    -- 0就是没有卡
    base.card = 0
    self.info_data.base = base
end

function MSLanternData:ResetOther(need_check)
    local other = {}
    other.npc_seq = -1
    other.question_id = -1
    self.info_data.other = other
    if need_check then
        MSLanternCtrl.Instance:CheckCreateNPC()
    end
end

function MSLanternData:SetQuestion(id)
    self.info_data.other.question_id = id
end

function MSLanternData:ResetResult()
    local result = {}
    -- 没开始作答是-1，没人作答是0
    result.answerer_id = -1
    -- 这个是回答者的uid
    result.answerer_answer = 0
    result.result = 0
    self.info_data.result = result
    self.info_data.timestamp = 0
end

function MSLanternData:ResetPraise()
    self.info_data.praise = {}
end

function MSLanternData:ResetUseCard()
    local use_card = {}
    use_card.card_type = 0
    use_card.uid = 0
    use_card.param = 0
    self.use_card_data.info = use_card
end

function MSLanternData:ResetLastResult()
    self.last_data.info = {}
end

function MSLanternData:IsNeedFind(value)
    if value ~= nil then
        self.is_need_find = value
    else
        return self.is_need_find
    end
end

function MSLanternData:SetInfoBase(protocol)
    self.info_data.base = protocol.info
    if self.info_data.base.day_start_flag == 0 then
        MSLanternData.Instance:IsNeedFind(true)
        self:ResetData()
    end
end

function MSLanternData:SetInfoOther(protocol)
    self.info_data.other = protocol.info
    MSLanternCtrl.Instance:CheckCreateNPC()
end

function MSLanternData:SetInfoResult(protocol)
    self.info_data.result = protocol.info
end

function MSLanternData:SetInfoPraise(protocol)
    self.info_data.praise = {}
    for i = 1, 5 do
        if protocol.info[i].uid > 0 then
            self.info_data.praise[protocol.info[i].uid] = protocol.info[i]
        end
    end
end

function MSLanternData:GetInfoPraise(uid)
    return self.info_data.praise[uid]
end

function MSLanternData:SetInfoUseCard(protocol)
    self.use_card_data.info = protocol.info
end

function MSLanternData:SetLastResultInfo(protocol)
    self.last_data.info = {}
    for i = 1, 5 do
        if protocol.info[i].uid > 0 then
            self.last_data.info[protocol.info[i].uid] = protocol.info[i]
        end
    end
end

function MSLanternData:GetLastResultInfo(uid)
    return self.last_data.info[uid]
end

function MSLanternData:SetTimestamp(timestamp)
    self.info_data.timestamp = timestamp
    self.info_data:SetDirty("timestamp")
end

function MSLanternData:NpcID()
    return self.other.receive_npc
end

function MSLanternData:LanternNpcID()
    return self.other.answer_npc
end

function MSLanternData:GetHelpId()
    return self.other.talk_id
end

function MSLanternData:GetBattleTalkId()
    return self.other.battle_talk_id
end

function MSLanternData:IsShowRight(value)
    if value ~= nil then
        self.is_show_right = value
    else
        return self.is_show_right
    end
end

function MSLanternData:QuestionInfo()
    local list = {}
    local question = ""
    local que_cfg = Cfg.MSLanternQuetion(self.info_data.other.question_id)
    if TableIsEmpty(que_cfg) then
        return
    end
    question = que_cfg.question
    local ans_cfg = Cfg.MSLanternAnswer(que_cfg.answer)
    local index = 1
    while ans_cfg["answer"..index] do
        local t = {}
        t.index = index
        t.answer = ans_cfg["answer"..index]
        t.right = ans_cfg.answer_true
        index = index + 1
        table.insert(list, t)
    end
    return question, list
end

function MSLanternData:Reward()
    local list = {}
    local item_ids = {}
    if not StringIsEmpty(self.other.reward_show) then
        item_ids = Split(self.other.reward_show, ":")
    end
    for k, v in pairs(item_ids) do
        table.insert(list, Item.Init(tonumber(v), 1, true))
    end
    return list
end

-- 返回卡的名字
function MSLanternData:KardName(m_type)
    for k, v in pairs(self.config.sp_card) do
        if v.type == m_type then
            return v.name
        end
    end
    return ""
end

-- 获取气泡内容
function MSLanternData:GetWord(word_id)
    local cfg = Cfg.MSLanternWord(word_id)
    if not TableIsEmpty(cfg) then
        return cfg.word
    end
    return ""
end

-- 至少要几人组队，不含npc
function MSLanternData:AtLeastMember()
    return self.other.member
end

-- 队伍类型
function MSLanternData:TeamType()
    return self.other.target_id
end

-- 场景id
function MSLanternData:SceneId()
    return self.other.scene
end

-- 人数是否足够
function MSLanternData:IsMemberEnought()
    if not TeamData.Instance:InTeam() then
        return false
    end
    local team_info = TeamData.Instance:GetTeamInfo()
	return team_info:MemberNum() >= self:AtLeastMember()
end

-- 队伍类型是否正确
function MSLanternData:IsTeamType()
    if not TeamData.Instance:InTeam() then
        return false
    end
    local team_info = TeamData.Instance:GetTeamInfo()
    return team_info.info.team_type == self:TeamType()
end

-- 返回本题需要显示的满意度
function MSLanternData:GetQuestionSatisfy(result)
    result = result or self.info_data.result.result
    for k, v in pairs(self.config.result) do
        if v.result == result then
            return v.satisfaction_show
        end
    end
    return 0
end

-- 返回本题获得的道具
function MSLanternData:GetQuestionItem(result)
    result = result or self.info_data.result.result
    for k, v in pairs(self.config.result) do
        if v.result == result then
            return DataHelper.FormatItemList(v.answer_reward)
        end
    end
end

-- 今日总次数
function MSLanternData:TotalTimes()
    return self.other.carrot_times
end

-- 判断今日答题次数是否足够，是否活动开启，是否已经领取最终奖励
function MSLanternData:IsTimesEnought()
    return self:IsOpen() and self.info_data.base.is_get_last_reward == 0 and self.info_data.base.answer_count >= self:TotalTimes()
end

function MSLanternData:Remind()
    if self:IsOpen() then
        if self.info_data.base.is_get_last_reward == 0 or self:TotalTimes() > self.info_data.base.answer_count then
            return 1
        end
    end
    return 0
end

-- 判断是否已经生成题目和npc
function MSLanternData:HasNpcAndQuestion()
    return self.info_data.other.npc_seq ~= -1 and self.info_data.other.question_id ~= -1
end

-- 是否开启
function MSLanternData:IsOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.MSLantern)
    -- return true
end

-- 判断是否显示在任务栏
function MSLanternData:IsShowTask()
    return self.info_data.base.day_start_flag > 0 and self.info_data.base.is_get_last_reward == 0
end

-- 返回当前npc位置
function MSLanternData:NowPos()
    if self.info_data.other.npc_seq ~= -1 then
        local cfg = Cfg.MSLanternPos(self.info_data.other.npc_seq)
        return cfg.pos_x, cfg.pos_y
    end
    return 0, 0
end

-- 获得回答者的名字
function MSLanternData:GetAnswerName()
    if self.info_data.result.answerer_id >= 0 then
        return self:GetNameByUid(self.info_data.result.answerer_answer)
    end
    return ""
end

-- 根据uid获取队伍里人的名字
function MSLanternData:GetNameByUid(uid)
    local team_info = TeamData.Instance:GetTeamInfo()
    local member_list = team_info:MemberListSort()
    for k, v in pairs(member_list or {}) do
        if v.info.uid == uid then
            return v.info.name
        end
    end
    return ""
end

function MSLanternData:SetLastItemList(value)
    self.last_item_list = value
end

function MSLanternData:GetLastItemList()
    return self.last_item_list
end

function MSLanternData:SetNpcInfo(info)
    if info then
        self.npc_info = info
    else
        return self.npc_info
    end
end

function MSLanternData:LanternInfo(info)
    if info then
        self.lantren_info = info
    else
        return self.lantren_info
    end
end

function MSLanternData:InitScene()
    if self:IsOpen() then
        local dy = VoMgr:NpcVoByNpcId(self:NpcID())
        local vo1 = CommonStruct.NpcVo(dy[1])
        local npc_co1 = NpcHelper.NpcCo(self:NpcID())
        vo1.obj_id = CUSTOM_OBJID.MS_LANTERN_NPC
        vo1.res_id = npc_co1.res_id
        vo1.name = npc_co1.name
        vo1.is_server = false
        self.npc_info = vo1
        VoMgr:AddGCObj(vo1)
    end

    if self:IsOpen() and self:HasNpcAndQuestion() then
        local vo2 = CommonStruct.NpcVo({})
        local npc_co2 = NpcHelper.NpcCo(self:LanternNpcID())
        vo2.id = self:LanternNpcID()
        vo2.obj_id = CUSTOM_OBJID.MS_LANTERN
        vo2.res_id = npc_co2.res_id
        vo2.name = npc_co2.name
        vo2.scene_id = self:SceneId()
        vo2.is_server = false
        vo2.x, vo2.y = self:NowPos()
        vo2.alway_show = true
        self.lantren_info = vo2
        VoMgr:AddGCObj(vo2)
    end
end

function MSLanternData:DisposeScene()
    VoMgr:DelGCObj(CUSTOM_OBJID.MS_LANTERN_NPC, true)
    VoMgr:DelGCObj(CUSTOM_OBJID.MS_LANTERN, true)
end
