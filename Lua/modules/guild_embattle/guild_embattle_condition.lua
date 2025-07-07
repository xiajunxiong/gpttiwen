-- 赛况
GuildEmbattleCondition =
    GuildEmbattleCondition or DeclareView('GuildEmbattleCondition', 'guild_embattle/guild_embattle_condition')
function GuildEmbattleCondition:GuildEmbattleCondition()
    -- body
end
function GuildEmbattleCondition:OnClickClose()
    ViewMgr:CloseView(GuildEmbattleCondition)
end

GuildEmbattleConditionPanel = GuildEmbattleConditionPanel or DeclareMono('GuildEmbattleConditionPanel', UIWFlushPanel)
function GuildEmbattleConditionPanel:GuildEmbattleConditionPanel()
    self.embattle_data = GuildEmbattleData.Instance
    self.act_data = ActivityData.Instance
    self.data_cares = {
        {data = self.embattle_data:Base(), func = self.onFlush},
        {
            data = self.embattle_data:ViewData(),
            func = self.onFlush,
            keys = {'flush'}
        }
    }
    self.is_show_last = false
    self.GroupTitle1 = {
        '_LocJueSai',
        '_LocDiJiuMing',
        '_LocDiWuMing',
        '_LocDiShiSanMing'
    }
    self.GroupTitle2 = {'_LocJueSai', '_LocDiWuMing'}
    self.GroupTitle3 = {'_LocJueSai'}
    self.GroupTitle4 = {'_LocJueSai'}
    self.group_id = 1 -- 1胜者2败者123
    self.list1 = {1, 9, 5, 13}
    self.list2 = {1, 5}
end
local function GuildHonorBattleMatchInfoItem(idx)
    local t = {}
    t.idx = idx
    t.server_id = 104
    t.guild_id = idx
    t.name = '测试' .. idx
    t.banner = idx
    t.role_name = '族长' .. idx
    t.rank = idx

    t.match_round = idx % 2 == 0 and 0 or 4
     --到达的回合，和curr_round不同
    t.match_round_2 = 0
     --开始季军赛的家族==1，胜者+1
    t.is_finish = 0
     --结束
    t.is_lose = 0
     --失败
    t.last_match_rank = idx --上次比赛排名

    t.champion_guess_num = 999
    t.guess_num = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        t.guess_num[i] = i
     --每轮支持的人数
    end
    t.battle_id = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
        t.battle_id[i] = i
     --每个区域的观战id
    end
    t.battle_result = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
        t.battle_result[i] = 0
     --每个区域的战斗结果，用来显示某个区域是否可观战
    end
    return t
end
function GuildEmbattleConditionPanel:OnClickBack()
end
function GuildEmbattleConditionPanel:OnClickTest()
    self.group_id = 1
     --这个由逻辑订
    local t = {}
    for i = 1, 16 do
        t[i] = GuildHonorBattleMatchInfoItem(i)
    end
    self.embattle_data:SetRound(4)
    self.embattle_data:SetFighterInfo({fighter_info = t, fighter_num = 16})
    self:FlushGroup()
    self:FlushBattleMsg()
    self:FlushRound()
    self:FlushTarget()
    self.ToggleGroup[1].isOn = true
end
-- 根据参与家族数量 显示组
function GuildEmbattleConditionPanel:FlushGroup()
    local temp = {[1] = 1, [9] = 2, [5] = 3, [13] = 4}
    --[[ local group_type = self.embattle_data:GroupType() ]]
    local info = self.embattle_data.match_info
    if table.nums(info) == 0 then
        info = self.embattle_data.last_match_info
    end
    self.ToggleGroup[1]:SetObjActive(false)
    self.ToggleGroup[2]:SetObjActive(false)
    self.ToggleGroup[3]:SetObjActive(false)
    self.ToggleGroup[4]:SetObjActive(false)
    for key, value in pairs(info) do
        if temp[value.win_rank] then
            self.ToggleGroup[temp[value.win_rank]]:SetObjActive(true)
        end
    end
    --[[ self.ToggleGroup[1]:SetObjActive(group_type <= 2)
    self.ToggleGroup[2]:SetObjActive(group_type <= 2)
    self.ToggleGroup[3]:SetObjActive(group_type == 1)
    self.ToggleGroup[4]:SetObjActive(group_type == 1) ]]
end
-- 根据家族数和 组别显示 对战信息
function GuildEmbattleConditionPanel:FlushBattleMsg()
    local show_type = self.embattle_data:GroupTargetType()
    if show_type == 1 then
        if self.group_id == 1 then
            for i = 1, self.Group1:Length(), 1 do
                self.Group1[i]:SetActive(true)
            end
            for i = 1, self.Group2:Length(), 1 do
                self.Group2[i]:SetActive(true)
            end
            for i = 1, self.Group3:Length(), 1 do
                self.Group3[i]:SetActive(true)
            end
        elseif self.group_id == 2 then
            for i = 1, self.Group1:Length(), 1 do
                self.Group1[i]:SetActive(false)
            end
            for i = 1, self.Group2:Length(), 1 do
                self.Group2[i]:SetActive(true)
            end
            for i = 1, self.Group3:Length(), 1 do
                self.Group3[i]:SetActive(true)
            end
        elseif self.group_id == 3 or self.group_id == 4 then
            for i = 1, self.Group1:Length(), 1 do
                self.Group1[i]:SetActive(false)
            end
            for i = 1, self.Group2:Length(), 1 do
                self.Group2[i]:SetActive(false)
            end
            for i = 1, self.Group3:Length(), 1 do
                self.Group3[i]:SetActive(true)
            end
        end
    elseif show_type == 2 then
        if self.group_id == 1 then
            for i = 1, self.Group1:Length(), 1 do
                self.Group1[i]:SetActive(false)
            end
            for i = 1, self.Group2:Length(), 1 do
                self.Group2[i]:SetActive(true)
            end
            for i = 1, self.Group3:Length(), 1 do
                self.Group3[i]:SetActive(true)
            end
        elseif self.group_id == 2 or self.group_id == 3 then
            for i = 1, self.Group1:Length(), 1 do
                self.Group1[i]:SetActive(false)
            end
            for i = 1, self.Group2:Length(), 1 do
                self.Group2[i]:SetActive(false)
            end
            for i = 1, self.Group3:Length(), 1 do
                self.Group3[i]:SetActive(true)
            end
        end
    elseif show_type == 3 then
        for i = 1, self.Group1:Length(), 1 do
            self.Group1[i]:SetActive(false)
        end
        for i = 1, self.Group2:Length(), 1 do
            self.Group2[i]:SetActive(false)
        end
        for i = 1, self.Group3:Length(), 1 do
            self.Group3[i]:SetActive(true)
        end
    elseif show_type == 4 then
        for i = 1, self.Group1:Length(), 1 do
            self.Group1[i]:SetActive(false)
        end
        for i = 1, self.Group2:Length(), 1 do
            self.Group2[i]:SetActive(false)
        end
        for i = 1, self.Group3:Length(), 1 do
            self.Group3[i]:SetActive(false)
        end
    end
    -- 1 1 全显示
    -- 1 2 组1 不显示
    -- 1 3 组1 2不显示
    -- 1 4 组1 2不显示
    -- 2 1 组1 不显示
    -- 2 2 组1 2 不显示
    -- 3 1 组1 2 不显示
    -- 4 1 组1 2 3不显示
end
-- 根据家族数 显示轮数
function GuildEmbattleConditionPanel:FlushRound()
    local num = self.embattle_data:MatchNum()
    local offset = 4 - num
    for i = 1, self.LeftRound:Length(), 1 do
        UH.SetText(self.LeftRound[i], string.format(Language.GuildEmbattle.RoundTitle, DataHelper.GetDaXie(i - offset)))
    end
    for i = 1, self.RightRound:Length(), 1 do
        UH.SetText(
            self.RightRound[i],
            string.format(Language.GuildEmbattle.RoundTitle, DataHelper.GetDaXie(i - offset))
        )
    end
    UH.SetText(self.FinalRound, string.format(Language.GuildEmbattle.RoundTitle, DataHelper.GetDaXie(num)))
end
-- 显示争夺目标 固定争夺显示，第1名 第9名 第5名 第13名
function GuildEmbattleConditionPanel:FlushTarget()
    local target_type = self.embattle_data:GroupTargetType()
    UH.SpriteName(self.FinalTitle, self['GroupTitle' .. target_type][self.group_id])
end
function GuildEmbattleConditionPanel:OnClickView()
    local base = self.embattle_data:Base()
    local battle_process = self.embattle_data:BattleProcess()
    local final_stage = self.embattle_data:FinalStage()
    if base.curr_round == final_stage then --or battle_process.process ~= 2 then
        ViewMgr:OpenView(GuildEmbattleFinal)
    else
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.RoundTip)
    end
end
function GuildEmbattleConditionPanel:OnClickGroup(group)
    self.group_id = group

    -- self.Anim:SetTrigger("ShowCondition")
    -- UH.SpriteName(self.FinalTitle, self.GroupTitle[group])
    --[[ for i = 1, self.Group1:Length(), 1 do
        self.Group1[i]:SetActive(group == 1)
    end
    for i = 1, self.Group2:Length(), 1 do
        self.Group2[i]:SetActive(group <= 2)
    end
    for i = 1, self.Group3:Length(), 1 do
        self.Group3[i]:SetActive(group <= 4)
    end ]]
    self:onFlush()
    --[[ self:FlushGroup()
    self:FlushBattleMsg()
    self:FlushRound()
    self:FlushTarget() ]]
end
function GuildEmbattleConditionPanel:onFlush()
    self:FlushGroup()
    self:FlushBattleMsg()
    self:FlushRound()
    self:FlushTarget()
    self:ResetShow()
    local is_open = self.embattle_data:IsOpen()
    local base = self.embattle_data:Base()

    --胜者组显示内容
    local final_stage = self.embattle_data:FinalStage()
    local stage = self.embattle_data:Stage()
    if is_open and base.round_state ~= nil then
        UH.SetText(
            self.Desc,
            string.format(Language.GuildEmbattle.RoundDesc[base.round_state], Language.GuildEmbattle.RoundName[stage])
        )
    else
        UH.SetText(self.Desc, '')
    end
    --格式化
    self.match_list = {}
    local list = {8, 4, 2}
    for i = 1, 3 do
        self.match_list[i] = {}
        for j = 1, list[i] do
            self.match_list[i][j] = {}
        end
    end
    --根据抢夺目标类型显示对战组
    --类型3 4 直接进决赛
    self.fighter_info = nil
    LogDG("赛况显示=========================================")
    --这里老赛况也要设置 基础信息
    local target_type = self.embattle_data:GroupTargetType()
    local win_rank = 1
    if target_type < 3 then
        win_rank = self['list' .. target_type][self.group_id]
    end
    local fighter_num = 0
    local init_stage = 1
    for _, value in ipairs(self.embattle_data.match_info) do
        if value.win_rank == win_rank then
            init_stage = value:InitStage()
            self.fighter_info = value.fighter_info
            fighter_num = value.fighter_num
        end
    end
    if self.fighter_info == nil then
        for _, value in ipairs(self.embattle_data.last_match_info) do
            if value.win_rank == win_rank then
                init_stage = value:InitStage()
                self.fighter_info = value.fighter_info
                fighter_num = value.fighter_num
            end
        end
    end
    -- local info = self.act_data:GetActivityStatusInfo(ActType.GuildEmbattle)
    -- local process = self.embattle_data:BattleProcess().process
    -- self.is_show_last = false
    --[[ if
        info.status == ActStatusType.Close and
            (process == GUILD_HONOR_BATTLE_PROCESS.WEEK_START or process == GUILD_HONOR_BATTLE_PROCESS.FINISH)
     then
        self.fighter_info = self.embattle_data:LastInfo().list
        local new_table = {}
        for i, v in ipairs(self.fighter_info) do
            if v.guild_id > 0 then
                table.insert(new_table, v)
            end
        end
        self.fighter_info = new_table
        self.is_show_last = true
    end ]]
    if self.fighter_info == nil then
        LogDG('未获取到要显示的组别win_rank = ', win_rank)
        return
    end
    --[[ LogDG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
        for index, value in ipairs(self.fighter_info) do
            LogDG(value.name)
        end
    LogDG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") ]]
    self.fighter_num = fighter_num
    if fighter_num > 8 then
        self:Grouping(1)
    elseif fighter_num <= 8 and fighter_num > 4 then
        self:Grouping(2)
    elseif fighter_num >= 2 then
        self:Grouping(3)
    end
    
    -- local init_stage = self.embattle_data:InitStage()
    LogDG("当前选组",self.group_id, "争夺目标", win_rank, "参与家族数",fighter_num, "计算得初始阶段", init_stage)
    --这个地方可能要改
    for i = 1, 8 do
        self.BattleItems1[i]:FlushTop(self.match_list[1][i][1], 2 - init_stage)
        self.BattleItems1[i]:FlushDown(self.match_list[1][i][2], 2 - init_stage)
    end
    for i = 1, 4 do
        self.BattleItems2[i]:FlushTop(self.match_list[2][i][1], 3 - init_stage)
        self.BattleItems2[i]:FlushDown(self.match_list[2][i][2], 3 - init_stage)
    end
    for i = 1, 2 do
        self.BattleItems3[i]:FlushTop(self.match_list[3][i][1], 4 - init_stage)
        self.BattleItems3[i]:FlushDown(self.match_list[3][i][2], 4 - init_stage)
    end
    -- 设置老赛况时需设置基础信息，curr_round此时等 final_stage
    -- match_round 相邻两个比较 大的晋级
    self.BtnInter.Interactable = base.curr_round == final_stage
end
--重置赛况
function GuildEmbattleConditionPanel:ResetShow()
    for i = 1, 8 do
        self.BattleItems1[i]:FlushTop()
        self.BattleItems1[i]:FlushDown()
    end
    for i = 1, 4 do
        self.BattleItems2[i]:FlushTop()
        self.BattleItems2[i]:FlushDown()
    end
    for i = 1, 2 do
        self.BattleItems3[i]:FlushTop()
        self.BattleItems3[i]:FlushDown()
    end
end
function GuildEmbattleConditionPanel:Grouping(start)
    -- local curr_round = self.embattle_data:Base().curr_round
    local group
    --[[ if self.fighter_num == 2 then
        for i, value in ipairs(self.fighter_info) do
            group = i
            self.match_list[start][group] = self.match_list[start][group] or {}
            table.insert(self.match_list[start][group], value)
        end
    else ]]
        for i, value in ipairs(self.fighter_info) do
            group = math.ceil(value.number / 2)
            self.match_list[start][group] = self.match_list[start][group] or {}
            table.insert(self.match_list[start][group], value)
        end
    --[[ end ]]

    local temp
    for round = start + 1, 3 do
        self.match_list[round] = {}
        for i, v in ipairs(self.match_list[round - 1]) do
            group = math.ceil(i / 2)
            temp = nil
            if v[1] and v[2] and v[1].match_round > v[2].match_round then
                temp = v[1]
            elseif v[1] and v[2] and v[1].match_round < v[2].match_round then
                temp = v[2]
            elseif v[1] and v[2] and v[1].match_round == v[2].match_round then
                -- temp = v[1]--临时
                LogDG("赛况出现到达回合相同的情况，这是不可以的",v[1].name, v[2].name)
            elseif v[2] == nil then
                --[[ if self.is_show_last == false then
                    if round <= curr_round then
                        temp = v[1]
                    end
                else ]]
                    temp = v[1]
                --[[ end ]]
            end
            self.match_list[round][group] = self.match_list[round][group] or {}
            table.insert(self.match_list[round][group], temp)
        end
    end
end

GuildEmbattleConditionItem = GuildEmbattleConditionItem or DeclareMono('GuildEmbattleConditionItem', UIWFlushPanel)
function GuildEmbattleConditionItem:FlushTop(data, round)
    if data == nil and round == nil then
        self.JoinObjs[1]:SetActive(false)
        self.NoJoinObjs[1]:SetActive(true)
        self.Fail1:SetActive(false)
        return
    end
    self.JoinObjs[1]:SetActive(data ~= nil)
    self.NoJoinObjs[1]:SetActive(data == nil)

    local base = GuildEmbattleData.Instance:Base()
    local is_open = GuildEmbattleData.Instance:IsOpen()
    if not is_open then
        self.Watch:SetActive(false)
    else
        if data and data.battle_result and base.round_state == 1 then
            local is_show = false
            for _, value in ipairs(data.battle_result) do
                if value == 0 then
                    is_show = true
                end
            end
            if is_show == true and data.is_finish == 1 then
                is_show = false
            end
            self.Watch:SetActive(is_show)
        else
            self.Watch:SetActive(false)
        end
    end
    if data ~= nil then
        UH.SetText(self.Names[1], data.name)
        UH.SetText(self.Banner[1], data.banner)
        local is_lose
        if data.is_lose then
            is_lose = data.is_lose == 1 and data.match_round <= round
        else
            is_lose = not (data.is_lose == 0 or data.match_round > round)
        end
        self.Fail1:SetActive(is_lose)
        self.Gray1[1].enabled = is_lose
        self.Gray1[2].enabled = is_lose
        self.Gray1[3].enabled = is_lose
    else
        self.Fail1:SetActive(false)
        self.Gray1[1].enabled = true
        self.Gray1[2].enabled = true
        self.Gray1[3].enabled = true
    end
end
function GuildEmbattleConditionItem:FlushDown(data, round)
    if data == nil and round == nil then
        self.JoinObjs[2]:SetActive(false)
        self.NoJoinObjs[2]:SetActive(true)
        self.Fail2:SetActive(false)
        return
    end
    self.JoinObjs[2]:SetActive(data ~= nil)
    self.NoJoinObjs[2]:SetActive(data == nil)
    if data ~= nil then
        UH.SetText(self.Names[2], data.name)
        UH.SetText(self.Banner[2], data.banner)
        local is_lose
        if data.is_lose then
            is_lose = data.is_lose == 1 and data.match_round <= round
        else
            is_lose = not (data.is_lose == 0 or data.match_round > round)
        end
        self.Fail2:SetActive(is_lose)
        self.Gray2[1].enabled = is_lose
        self.Gray2[2].enabled = is_lose
        self.Gray2[3].enabled = is_lose
    else
        self.Fail2:SetActive(false)
        self.Gray2[1].enabled = true
        self.Gray2[2].enabled = true
        self.Gray2[3].enabled = true
    end
end
