ImperialExamData = ImperialExamData or BaseClass()

-- 科举考试操作请求类型
ImperialExamReqType = {
    Info = 0,       --请求下发
    NPCPos = 1,     --请求npc位置下发（队长）
    StartAnswer = 2,--开始答题（队长）
    Answer = 3,     --答题（指定答题者），p1：答案1~3
    UseCard = 4,    --使用卡
    Praise = 5,     --点赞，p1：uid
    Fight = 6,      --战斗（队长）
    Friend = 7,     --加好友，p1：uid
    Help = 8,       --答题者求助（指定答题者）
    Advise = 9,     --非答题者建议（除答题者以外），p1：建议的答案1~3
}

-- 科举考试卡片类型
ImperialExamCardType = {
    Free = 1,       --免答卡
    Change = 2,     --换题卡
    ReduceWrong = 3,--除错卡
}

-- 科举考试飘字协议类型
ImperialExamTxtType = {
    Praise = 1,     --点赞
    Friend = 2,     --好友申请
    UseCard = 3,    --使用卡
}

function ImperialExamData:__init()
    if ImperialExamData.Instance ~= nil then
        Debuger.LogError("[ImperialExamData] attempt to create singleton twice!")
        return
    end
    ImperialExamData.Instance = self
    self.config = Config.keju_examination_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New({base = {}, other = {}, result = {}, answer_role_id = 0, timestamp = 0, praise = {}})
    -- 两个npc的info
    self.npc_info = nil
    self.answer_npc_info = nil
    -- 重上线需要重新找npc
    self.is_need_find = true
    -- 战斗完最终奖励
    self.last_item_list = {}
    -- 用于延迟显示对勾
    self.is_show_right = false

    self.last_data = SmartData.New({info = {}})
    self.use_card_data = SmartData.New({info = {}})

    Cfg.Register("ImperialExamQuetion", Cfg.RealTimeSearch(self.config.question, "question_id"))
    Cfg.Register("ImperialExamPos", Cfg.RealTimeSearch(self.config.npc, "npc"))

    self:ResetData()
end

function ImperialExamData:__delete()
    ImperialExamData.Instance = nil
end

function ImperialExamData:ResetData()
    self:ResetBase()
    self:ResetOther()
    self:ResetResult()
    self:ResetPraise()
    self:ResetUseCard()
    self:ResetLastResult()
    self:DisposeScene()
    self.is_need_find = true
end

function ImperialExamData:ResetBase()
    local base = {}
    base.day_start_flag = 0
    base.is_get_last_reward = 0
    base.answer_count = 0
    base.satisfy = 0
    -- 0就是没有卡
    base.card = 0
    self.info_data.base = base
end

function ImperialExamData:ResetOther(need_check)
    local other = {}
    other.npc_seq = -1
    other.question_id = -1
    self.info_data.other = other
    if need_check then
        ImperialExamCtrl.Instance:CheckCreateNPC()
    end
end

function ImperialExamData:SetQuestion(id)
    self.info_data.other.question_id = id
end

function ImperialExamData:ResetResult()
    local result = {}
    -- 答题者回答id，-1还在倒计时未作答，0倒计时完未作答，1~3作答id
    result.answer = -1
    result.result = 0
    self.info_data.result = result
    self.info_data.answer_role_id = 0
    self.info_data.timestamp = 0
end

function ImperialExamData:ResetPraise()
    self.info_data.praise = {}
end

function ImperialExamData:ResetUseCard()
    local use_card = {}
    use_card.card_type = 0 --卡
    use_card.uid = 0 --使用者id
    use_card.param = 0 --免答：0，换题：新题目id，排错：错误答案id
    self.use_card_data.info = use_card
end

function ImperialExamData:ResetLastResult()
    self.last_data.info = {}
end

function ImperialExamData:IsNeedFind(value)
    if value ~= nil then
        self.is_need_find = value
    else
        return self.is_need_find
    end
end

function ImperialExamData:SetInfoBase(protocol)
    self.info_data.base = protocol.info
    if self.info_data.base.day_start_flag == 0 then
        ImperialExamData.Instance:IsNeedFind(true)
        self:ResetData()
    end
end

function ImperialExamData:SetInfoOther(protocol)
    self.info_data.other = protocol.info
    ImperialExamCtrl.Instance:CheckCreateNPC()
end

function ImperialExamData:SetInfoResult(protocol)
    self.info_data.result = protocol.info
end

function ImperialExamData:SetInfoPraise(protocol)
    self.info_data.praise = {}
    for i = 1, 5 do
        if protocol.info[i].uid > 0 then
            self.info_data.praise[protocol.info[i].uid] = protocol.info[i]
        end
    end
end

function ImperialExamData:GetInfoPraise(uid)
    return self.info_data.praise[uid]
end

function ImperialExamData:SetInfoUseCard(protocol)
    self.use_card_data.info = protocol.info
end

function ImperialExamData:SetLastResultInfo(protocol)
    self.last_data.info = {}
    for i = 1, 5 do
        if protocol.info[i].uid > 0 then
            self.last_data.info[protocol.info[i].uid] = protocol.info[i]
        end
    end
end

function ImperialExamData:GetLastResultInfo(uid)
    return self.last_data.info[uid]
end

function ImperialExamData:SetTimeAndUid(timestamp, answer_role_id)
    self.info_data.timestamp = timestamp
    self.info_data.answer_role_id = answer_role_id
    self.info_data:SetDirty("timestamp")
end

function ImperialExamData:NpcID()
    return self.other.receive_npc
end

function ImperialExamData:AnswerNpcResId()
    local npc_info = NpcHelper.NpcCo(self:AnswerNpcID()) or {}
    return npc_info.res_id or 0
end

function ImperialExamData:AnswerNpcName()
    local npc_info = NpcHelper.NpcCo(self:AnswerNpcID()) or {}
    return npc_info.name or ""
end

function ImperialExamData:AnswerNpcID()
    return self.other.answer_npc
end

function ImperialExamData:GetHelpId()
    return self.other.talk_id
end

function ImperialExamData:GetBattleTalkId()
    return self.other.battle_talk_id
end

-- 是否展示正确答案
function ImperialExamData:IsShowRight(value)
    if value ~= nil then
        self.is_show_right = value
    else
        return self.is_show_right
    end
end

-- 是否展示答题奖励界面
function ImperialExamData:IsShowResult(value)
    if value ~= nil then
        self.is_show_result = value
    else
        return self.is_show_result
    end
end

-- 问题和答案的list
function ImperialExamData:QuestionInfo()
    local list = {}
    local question = ""
    local que_cfg = Cfg.ImperialExamQuetion(self.info_data.other.question_id)
    if TableIsEmpty(que_cfg) then
        return
    end
    question = que_cfg.question
    local index = 1
    while que_cfg["answer"..index] do
        local t = {}
        t.index = index
        t.answer = que_cfg["answer"..index]
        t.right = que_cfg.answer_true
        index = index + 1
        table.insert(list, t)
    end
    return question, list
end

function ImperialExamData:Reward()
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
function ImperialExamData:KardName(m_type)
    for k, v in pairs(self.config.sp_card) do
        if v.type == m_type then
            return v.name
        end
    end
    return ""
end

-- 获取气泡内容
function ImperialExamData:GetWord(word_id)
    local cfg = self.config.word[tonumber(word_id)]
    if not TableIsEmpty(cfg) then
        return cfg.word
    end
    return ""
end

-- 至少要几人组队，不含npc
function ImperialExamData:AtLeastMember()
    return self.other.member
end

-- 队伍类型
function ImperialExamData:TeamType()
    return self.other.target_id
end

-- 场景id
function ImperialExamData:SceneId()
    return self.other.scene
end

-- 人数是否足够
function ImperialExamData:IsMemberEnought()
    if not TeamData.Instance:InTeam() then
        return false
    end
    local team_info = TeamData.Instance:GetTeamInfo()
	return team_info:MemberNum() >= self:AtLeastMember()
end

-- 队伍类型是否正确
function ImperialExamData:IsTeamType()
    if not TeamData.Instance:InTeam() then
        return false
    end
    local team_info = TeamData.Instance:GetTeamInfo()
    return team_info.info.team_type == self:TeamType()
end

-- 返回本题需要显示的满意度
function ImperialExamData:GetQuestionSatisfy(result)
    result = result or self.info_data.result.result
    for k, v in pairs(self.config.result) do
        if v.result == result then
            return v.satisfaction_show
        end
    end
    return 0
end

-- 返回本题获得的道具
function ImperialExamData:GetQuestionItem(result)
    result = result or self.info_data.result.result
    for k, v in pairs(self.config.result) do
        if v.result == result then
            return DataHelper.FormatItemList(v.answer_reward)
        end
    end
end

-- 今日总次数
function ImperialExamData:TotalTimes()
    return self.other.carrot_times
end

-- 判断今日答题次数是否足够，是否活动开启，是否已经领取最终奖励
function ImperialExamData:IsTimesEnought()
    return self:IsOpen() and self.info_data.base.is_get_last_reward == 0 and self.info_data.base.answer_count >= self:TotalTimes()
end

function ImperialExamData:Remind()
    if self:IsOpen() then
        if self.info_data.base.is_get_last_reward == 0 or self:TotalTimes() > self.info_data.base.answer_count then
            return 1
        end
    end
    return 0
end

-- 判断是否已经生成题目和npc
function ImperialExamData:HasNpcAndQuestion()
    return self.info_data.other.npc_seq ~= -1 and self.info_data.other.question_id ~= -1
end

-- 是否开启
function ImperialExamData:IsOpen()
    -- return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.ImperialExam)
    return false
end

-- 判断是否显示在任务栏
function ImperialExamData:IsShowTask()
    return self.info_data.base.day_start_flag > 0 and self.info_data.base.is_get_last_reward == 0
end

-- 返回当前npc位置
function ImperialExamData:NowPos()
    if self.info_data.other.npc_seq ~= -1 then
        local cfg = Cfg.ImperialExamPos(self.info_data.other.npc_seq)
        return cfg.pos_x, cfg.pos_y
    end
    return 0, 0
end

-- 判断自己是否回答者
function ImperialExamData:IsCanAnswer()
    if self.info_data.answer_role_id >= 0 then
        return RoleId() == self.info_data.answer_role_id
    end
    return false
end

-- 获得回答者的名字
function ImperialExamData:GetAnswerName()
    if self.info_data.answer_role_id >= 0 then
        return self:GetNameByUid(self.info_data.answer_role_id)
    end
    return ""
end

-- 根据uid获取队伍里人的名字
function ImperialExamData:GetNameByUid(uid)
    local team_info = TeamData.Instance:GetTeamInfo()
    local member_list = team_info:MemberListSort()
    for k, v in pairs(member_list or {}) do
        if v.info.uid == uid then
            return v.info.name
        end
    end
    return ""
end

function ImperialExamData:SetLastItemList(value)
    self.last_item_list = value
end

function ImperialExamData:GetLastItemList()
    return self.last_item_list
end

function ImperialExamData:NpcInfo(info)
    if info then
        self.npc_info = info
    else
        return self.npc_info
    end
end

function ImperialExamData:AnswerNpcInfo(info)
    if info then
        self.answer_npc_info = info
    else
        return self.answer_npc_info
    end
end

function ImperialExamData:InitScene()
    if self:IsOpen() then
        local dy = VoMgr:NpcVoByNpcId(self:NpcID())
        local vo1 = CommonStruct.NpcVo(dy[1])
        local npc_co1 = NpcHelper.NpcCo(self:NpcID())
        vo1.obj_id = CUSTOM_OBJID.MS_LANTERN_NPC
        vo1.res_id = npc_co1.res_id
        vo1.name = npc_co1.name
        vo1.is_server = false
        self:NpcInfo(vo1)
        VoMgr:AddGCObj(vo1)
    end

    if self:IsOpen() and self:HasNpcAndQuestion() then
        local vo2 = CommonStruct.NpcVo({})
        local npc_co2 = NpcHelper.NpcCo(self:AnswerNpcID())
        vo2.id = self:AnswerNpcID()
        vo2.obj_id = CUSTOM_OBJID.MS_LANTERN
        vo2.res_id = npc_co2.res_id
        vo2.name = npc_co2.name
        vo2.scene_id = self:SceneId()
        vo2.is_server = false
        vo2.x, vo2.y = self:NowPos()
        vo2.alway_show = true
        self:AnswerNpcInfo(vo2)
        VoMgr:AddGCObj(vo2)
    end
end

function ImperialExamData:DisposeScene()
    VoMgr:DelGCObj(CUSTOM_OBJID.MS_LANTERN_NPC, true)
    VoMgr:DelGCObj(CUSTOM_OBJID.MS_LANTERN, true)
end
