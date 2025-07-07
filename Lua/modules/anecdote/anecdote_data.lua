--奇闻类型枚举 根据服务端
AnecdoteType = {
    Puzzle = 2,    -- 九宫拼图
    Sudoku = 3,    -- 洛书奇门
    WenXiangLou = 4,--温香楼
    Spy = 7,       -- 铁卫营奸细
    Vision = 8,    -- 眼力训练
    FoxCatch = 10, -- 抓拿狐妖
    StarLife = 11, -- 司命星君
    FishTear= 12,  -- 鱼王眼泪
    Couplet = 13,  -- 天下绝对
    DuanShiFei =14,-- 断是非
	Marriage = 15, -- 姻缘一线牵
    Antique = 16,  -- 古董鉴赏
    Poetry = 17,   -- 游历诗人
    PuzzleStorehouse = 18, -- 神秘宝藏
    Decrypt = 19,  -- 宝箱解密 
    CookSort = 20, -- 寻味美食
    VisionDiscern = 21,--真假画作
    Klotski = 22,  -- 数字华容道
    PackPoetry = 23,-- 诗词歌赋
    QuestionTongBao = 24, -- 彤宝的疑问
    PuzzleMechanical = 25, -- 机关巧解
    MemoryMaster = 26, -- 记忆大师
    ArrayGhost = 27, -- 阵中之魂
    ImmortalRecruitStudent = 28, -- 仙师收徒
    MillenniumPeachBlossom = 29, -- 千年桃花开（阴阳眼）
    SoulBack = 30,               -- 魂归故里（阴阳眼）
    DesertDragon = 31,           -- 沙漠怪龙（阴阳眼）
    PoetrySoul = 32,             -- 诗者之魂（阴阳眼）
    EmperorPortrait = 33,        -- 帝王画像（阴阳眼）
    FishMessenger = 34,          -- 海鱼使者（阴阳眼）
    MusicAnecdote = 35,          -- 高山流水
    FunRiddle = 36,              -- 趣味猜谜
    Poetry2 = 37,                -- 游历诗人（沙漠）
    Decrypt2 = 38,               -- 宝箱解密（沙漠）
    Decrypt3 = 39,               -- 宝箱解密（海底）
    Couplet2 = 40,               -- 天下绝对2
    PackPoetry2 = 41,            -- 诗词歌赋2
    Couplet3 = 42,               -- 天下绝对3

}

-- 新的奇闻玩法类型
NewAnecdoteType = {
    Battle = 0, --挑战类任务
    YLSR = 1,   --游历诗人
    TXJD = 2,   --天下绝对
    QWCM = 3,   --趣味猜谜
    YLXL = 4,   --眼力训练
    SGCF = 5,   --诗歌辞赋  > 不可用，与天下绝对相同，用天下绝对就行
    YY = 6,     --音游
    BXJM = 7,   --宝箱解密
    JGPT = 8,   --九宫拼图
    XWMS = 9,   --寻味美食  > 不可用，与游历诗人相同，用游历诗人就行
    JYDS = 10,  --记忆大师
}

-- 新的奇闻玩法事件类型
NewAnecdoteActionType = {
    GameSucc = 3,   -- 新奇闻挑战成功 p1, task_type, p2 : seq
}

ANECDOTE_TASK_SPECIAL_MAX_NUM = 42

AnecdoteData = AnecdoteData or BaseClass()

function AnecdoteData:__init()
    if AnecdoteData.Instance ~= nil then
        Debuger.LogError("[AnecdoteData] attempt to create singleton twice!")
        return
    end
    AnecdoteData.Instance = self
    self.task_cache_list = SmartData.New()
    self.task_config = Config.anecdotes_tasklist_auto.task_list
    self.npc_bubble = Config.anecdotes_tasklist_auto.npc_config
    self.challenge_anecdote = Config.anecdotes_tasklist_auto.challenge_anecdote


    self.task_select = {}
    self.special_list = {}
    self.auto_func_list = {}
    self.special_list2 = {}
    self.add_once_npc = {}
    self.new_anecdote_param = {}    -- 寄存新奇闻参数
    --[[ self.challenge_anecdote_scene_list = {}
    for i, v in ipairs(self.challenge_anecdote) do
        self.challenge_anecdote_scene_list[v.scene_id] = self.challenge_anecdote_scene_list[v.scene_id] or {}
        table.insert(self.challenge_anecdote_scene_list[v.scene_id], v)
    end ]]

    Cfg.Register("AnecdoteChallenge", Cfg.RealTimeSearch2Key(Config.anecdotes_tasklist_auto.challenge_anecdote,"special_type","monster_seq"))
    Cfg.Register("AnecdoteChallengeCfg", Cfg.RealTimeSearch(Config.anecdotes_tasklist_auto.challenge_anecdote,"seq"))
end

function AnecdoteData:ClearData()
    self.add_once_npc = {}
    self.task_cache_list:Clear()
end

function AnecdoteData:TaskInfo(task_id)
    local task = self:GetTaskInfoById(task_id)
    if task == nil then
        task = SmartData.New()
        task:Set(TaskInfoAnecdote.New():TaskInfo())
    end
    return task
end

--处理任务
function AnecdoteData:CustomTask(vo)
    local param = CommonStruct.CustomTask()
    param.task_id = vo.task_id
    param.progress = vo.progress
    param.has_finish = vo.has_finish
    param.is_all_info = vo.is_all_info
    param.stranger_task_type = vo.stranger_task_type
    param.list_type = 1 - vo.accept_status
    return param
end

function AnecdoteData:SetAnecdoteTaskSpecialInfo(protocol)
    self.special_list = protocol.special_list
end

function AnecdoteData:GetSpecialInfo(anecdote_type)
    return self.special_list[anecdote_type] or {}
end

function AnecdoteData:SetSpecialData(anecdote_type,data)
    self.special_list[anecdote_type] = data
end

--缓存当前任务id
function AnecdoteData:SetTaskCacheList(type,task_id)
    self.task_cache_list[type] = task_id
end

--获取当前任务id
function AnecdoteData:GetTaskCacheId(type)
    return self.task_cache_list[type]
end

--根据任务id拿到task_info
function AnecdoteData:GetTaskInfoById(task_id)
    return TaskData.Instance:GetTaskInfoByTaskId(task_id)
end

--根据奇闻类型拿到当前task_info stranger_task_type
function AnecdoteData:GetTaskInfoByType(type)
    return self:GetTaskInfoById(self:GetTaskCacheId(type) or 0)
end

--是否完成某个的奇闻
function AnecdoteData:IsHasFinish(stranger_task_type)
    local task_info = self:GetTaskInfoByType(stranger_task_type)
    if task_info then
        return task_info.has_finish == 1
    end
    return true
end

--根据任务id拿对应的任务类型
function AnecdoteData:GetType(task_id)
    return self.task_config[task_id].stranger_task_type
end

--根据奇闻类型拿任务的其他配置统一写在这里
function AnecdoteData.GetConfig(type)
    return Config.anecdotes_tasklist_auto.task_config[type]
end

--获取npc气泡框
function AnecdoteData:GetNpcBubble(npc)
    if npc == nil then return end
    return self.npc_bubble[npc.seq] or self.npc_bubble[npc.id]
end

function AnecdoteData:IsAutoTalk(task_id)
    return self.task_config[task_id].is_auto_talk == 1
end

--是否是最后一个任务
function AnecdoteData.IsLastTask(type,task_id)
    return AnecdoteData.GetConfig(type).last_task_id == task_id
end

--获取奇闻任务NPC对话任务
function AnecdoteData:GetAnecdote(obj_vo)
    local config = self:GetNpcBubble(obj_vo)
    if config ~= nil and config.type ~= -1 then
        return self:GetTaskInfoByType(config.type)
    end
    return nil
end

function AnecdoteData:GetTaskBubble(obj_vo)
    local config = self:GetNpcBubble(obj_vo)
    local task_info = AnecdoteData.Instance:GetAnecdote(obj_vo)
    if config and task_info and config.is_gather == 0 and task_info:IsTaskNpc(obj_vo)then
        return config
    elseif config and task_info and config.is_gather == 1 then
        return config
    elseif config and config.type == -1 then
        return config
    end
end

--获取场景话id
function AnecdoteData:GetNormalTalkId(type)
    return AnecdoteData.GetConfig(type).normal_talk
end

--获取失败id
function AnecdoteData:GetFailTalkId(type)
    return AnecdoteData.GetConfig(type).fail_talk
end

--奇闻任务监听
function AnecdoteData:Care(func)
    return self.task_cache_list:Care(func)
end

function AnecdoteData:UnCare(ht)
    self.task_cache_list:Uncare(ht)
end

function AnecdoteData:SetSelectIndex(type, index)
    self.task_select[type] = index
end

function AnecdoteData:GetSelectIndex(type)
    return self.task_select[type] or 1
end

--获取奇闻离开玩法提示
function AnecdoteData:GetCloseTip()
	for k,v in pairs(AnecdoteConfig.CloseViewList) do
        if ViewMgr:IsOpen(ModToView[v]) then
            return Language.Anecdote.TeamInviteTip,function()
                ViewMgr:CloseView(ModToView[v])
			end
		end
    end
    return "",nil
end

--获取奇闻名字
function AnecdoteData.GetName(type)
    return Config.anecdotes_tasklist_auto.task_config[type].name
end

--注册奇闻自动玩法
function AnecdoteData:CustomAutoHandle(anecdote_type, func)
	self.auto_func_list[anecdote_type] = func
end

function AnecdoteData:OnAutoHandle(anecdote_type)
	if self.auto_func_list[anecdote_type] then
		Call(self.auto_func_list[anecdote_type])
	end
end

function AnecdoteData:SetSpecialAnecdoteList2(task_type, info_list)
    -- LogDG(task_type, info_list)
    if task_type ~= nil then
        self.special_list2[task_type] = info_list
    else
        self.special_list2 = info_list
    end
    for k, v in pairs(self.challenge_anecdote) do
        if self:TaskIsCompelete(v.special_type, v.monster_seq) == 0 then
            self.add_once_npc[v.special_type] = self.add_once_npc[v.special_type] or {}
            if self.add_once_npc[v.special_type][v.monster_seq] == nil then
                -- LogDG(v.special_type, v.monster_seq)
                VoMgr:AddNpc(v.npc_seq)
                self.add_once_npc[v.special_type][v.monster_seq] = true
            end
        else
            VoMgr:DeleteNpc(v.npc_seq)
        end
    end
end
function AnecdoteData:TaskIsCompelete(task_type, task_seq)
    if self.special_list2[task_type] then
        return (self.special_list2[task_type][task_seq] or 0)
    else
        return 0
    end
end



function AnecdoteData:SetNewParam(key, param)
    self.new_anecdote_param[key] = param
end
function AnecdoteData:GetNewParam(key, is_clear)
    -- 测试用
    -- if key == NewAnecdoteType.JYDS then
    --     return {seq = 143}
    -- end
    local t = self.new_anecdote_param[key]
    if is_clear then
        self.new_anecdote_param[key] = nil
    end
    return t
end
function AnecdoteData:GetAnecdoteChallengeCfg(seq)
    return Cfg.AnecdoteChallengeCfg(seq)
end

-- 新奇闻完成处理
function AnecdoteData:NewAnecdoteSucc(seq)
    local task_cfg = self:GetAnecdoteChallengeCfg(seq)
    AnecdoteCtrl.Instance:NewAnecdoteAction(task_cfg.special_type, task_cfg.monster_seq)
    if task_cfg.task_id ~= "" then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = task_cfg.task_id})
    end
end