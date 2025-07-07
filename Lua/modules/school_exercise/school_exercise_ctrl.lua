VIEW_REQ("modules/school_exercise/school_exercise_view")

SchoolExerciseCtrl = SchoolExerciseCtrl or BaseClass(BaseCtrl)
function SchoolExerciseCtrl:__init()
    if SchoolExerciseCtrl.Instance ~= nil then
		Debuger.LogError("[SchoolExerciseCtrl] attempt to create singleton twice!")
		return
	end	
	SchoolExerciseCtrl.Instance = self
    self.data = SchoolExerciseData.New()
    self:RegisterAllProtocol()
    RemindCtrl.Instance:Remind():Register(Mod.Experience.SchoolExercise,self.data.school_data,function()
		return self.data:GetRemindNum()
	end)
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_SCHOOL_EXERCISE,{reward_func = function()
		return self.data:GetCurRewardData()
	end})
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_SCHOOL_EXERCISE, function(is_win)
		ViewMgr:OpenView(SchoolExerciseView)
    end)
end

function SchoolExerciseCtrl:__delete()
    self.data = nil
end

function SchoolExerciseCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.SchoolExercise,function(desc_val,left_time_title,left_time_val)
        local num,total = self.data:GetTotalNum()
        return total-num
        -- ChallengeNaviCtrl.SetFormatDescNum(desc_val,num,total)
    end)
end

function SchoolExerciseCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCSchoolExerciseInfo,"OnSCSchoolExerciseInfo")
    self:RegisterProtocol(SCSchoolExerciseChallengeSucc,"OnSCSchoolExerciseChallengeSucc")
    self:RegisterProtocol(CSSchoolExerciseChallenge)
    self:RegisterProtocol(CSSchoolExerciseFirstOpenUI)
end

function SchoolExerciseCtrl:OnSCSchoolExerciseInfo(protocol)
   -- LogError("校场信息【3590】协议",protocol)
    self.data:SetSchoolData(protocol)
end

function SchoolExerciseCtrl:OnSCSchoolExerciseChallengeSucc(protocol)
    --LogError("【校场演武】挑战成功【3592】协议",protocol)
    self.data:SetCurSeq(protocol.seq,protocol.has_reward)
end

function SchoolExerciseCtrl:SendExerciseReq(seq)
    local protocol = ProtocolPool.Instance:GetProtocol(CSSchoolExerciseChallenge)
	protocol.seq = seq or -1
	SendProtocol(protocol)
end

function SchoolExerciseCtrl:SendOpenFirstUIReq()
    local protocol = ProtocolPool.Instance:GetProtocol(CSSchoolExerciseFirstOpenUI)
    SendProtocol(protocol)
end

SchoolExerciseData = SchoolExerciseData or BaseClass()

function SchoolExerciseData:__init()
    if SchoolExerciseData.Instance ~= nil then
		Debuger.LogError("[SchoolExerciseData] attempt to create singleton twice!")
		return
	end	
	SchoolExerciseData.Instance = self
    self.cfg = Config.school_exercise_auto
    self.school_data = SmartData.New({})
    self.first_open_flag = 0
    self:InitData()
end

function SchoolExerciseData:__delete()
    self.school_data:Set({})
    self.school_data = nil
end

function SchoolExerciseData:InitData()
    self.list_data = {}
    for i = 1,#self.cfg.challenge do
        local v = self.cfg.challenge[i]
        if self.list_data[v.group] == nil then
            self.list_data[v.group] = {}
        end
        if self.list_data[v.group].children == nil then
            self.list_data[v.group].children = {}
        end
        if self.list_data[v.group].id == nil then
            self.list_data[v.group].id = v.group
            self.list_data[v.group].name = v.name
        end
        table.insert(self.list_data[v.group].children,v)
    end
end

function SchoolExerciseData:SetSchoolData(protocol)
    self.school_data.clear_flag = protocol.clear_flag
    self.first_open_flag = protocol.first_flag
end

function SchoolExerciseData:GetSchoolData()
    return self.school_data
end

function SchoolExerciseData:GetListData()
    return self.list_data
end

function SchoolExerciseData:GetJumpData()
    local list_data = self:GetListData()
    for i,v in pairs(list_data) do
        for j = 1, #v.children do
            if self:GetItemFlag(v.children[j]) == 0 then
                return v.children[j]
            end
        end
    end
    return list_data[0].children[1]
end

-- 1 = 已完成，0 等于未完成，-1 等于未解锁
function SchoolExerciseData:GetItemFlag(data)
    if not self.school_data.clear_flag then
        return -1
    end
    local role_lv = RoleData.Instance:GetRoleLevel()
    if role_lv < data.level then
        return -1
    end
    -- 判断上一位
    local group_data = self.list_data[data.group]
    local up_seq
    for i,v in pairs(group_data.children) do
        if v.seq < data.seq then
            up_seq = v.seq
        end
    end
    if up_seq then
        if self.school_data.clear_flag[up_seq] == 0 then
            return -2
        end
    end
    if self.school_data.clear_flag == nil or self.school_data.clear_flag[data.seq] == 1 then
        return 1
    else
        return 0
    end
end

-- 1 是表示全部完成
function SchoolExerciseData:GetGroupFlag(group_data)
    local len = #group_data.children
    for i,v in pairs(group_data.children) do
        local flag = self:GetItemFlag(v)
        if flag == 0 then
            return 0
        end
        if flag == 1 and #group_data.children > 0 then
            len = len - 1
        end
    end
    if len == 0 then
        return 1
    else
        return -1
    end

end

function SchoolExerciseData:SetCurSeq(seq,flag)
    self.cur_seq_status = {seq = seq, has_reward = flag}
end

function SchoolExerciseData:GetCurSeq()
    return self.cur_seq_status
end

-- 获取当前战斗奖励
function SchoolExerciseData:GetCurRewardData(is_clear)
    is_clear = is_clear == nil and true or is_clear
    local cur_seq_status = self:GetCurSeq()
    if cur_seq_status == nil or cur_seq_status.has_reward ~= 1 then
        return
    end
    local cur_seq = cur_seq_status.seq
    local vo = self.cfg.challenge[cur_seq + 1]
    if is_clear then
        self:SetCurSeq(nil)
    end
    if vo then
        local reward_list_data = {}
        if vo.coin > 0 then
            table.insert(reward_list_data,Item.Create({item_id = CommonItemId.Coin,num = vo.coin}))
        end
        AndList(reward_list_data,DataHelper.FormatItemList(vo.clear_reward))
        return reward_list_data
    end
    return
end

-- 红点逻辑
function SchoolExerciseData:GetRemindNum()
    local item_flag
    for i,v in pairs(self.cfg.challenge) do
        item_flag = self:GetItemFlag(v)
        if item_flag == 0 then
            return 1
        end
    end
    return 0
end

-- 是否是首次打开界面
function SchoolExerciseData:IsFirstOpen()
    return self.first_open_flag == 0
end

function SchoolExerciseData:SetPSelectData(data)
    self.select_parent_data = data
end

function SchoolExerciseData:GetPSelectData(data)
    return self.select_parent_data
end
function SchoolExerciseData:IsSchoolBattle(group_id)
    if group_id > 0 then
        for index, value in ipairs(Config.school_exercise_auto.challenge) do
            if value.monster_group == group_id then
                return true
            end
        end
    end
    return false
end

-- 获取通关数量和全部关卡数量
function SchoolExerciseData:GetTotalNum()
    local group_list = self:GetListData()
    local num = 0
    local total = 0
    for i,group in pairs(group_list) do
        for j,v in pairs(group.children) do
            local flag = self:GetItemFlag(v)
            if flag == 1 then
                num = num + 1
            end
            if flag ~= -1 then
                total = total + 1
            end
        end
    end
    return num,total
end