ACHIEVEMENT_TYPE_NUM = 1024         --成就类型数量
ACHIEVEMENT_NUM = 128             --成就数量
WORLD_HONOR_ROLL_TEAM_COUNT = 3
MAX_TEAM_MEMBER_NUM  = 5


AchieveData = AchieveData or BaseClass()

function AchieveData:__init()
    if AchieveData.Instance ~= nil then
        Debuger.LogError("[AchieveData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    AchieveData.Instance = self

    -----------数据成员--------------
    --成就类型枚举
    self.P = {
        g = 1,  --成长
        w = 2,  --工坊
        s = 3,  --社交
        t = 4,  --任务
        Total = 5,  --总和
    }

    --完成枚举
    self.RA = {
        r = 1,  --最近完成
        a = 2,  --即将完成
    }

    self.base_data = SmartData.New({
        recv_fetch = SmartData.New(),
        complete_info = {
            [self.RA.r] = {},      --最近完成                                     
            [self.RA.a] = {},      --即将完成
        }
    })
    self.recv_progress = SmartData.New()         --每种成就类型的成就进度
    --self.recv_fetch = SmartData.New()            --每种成就的奖励领取情况

    self.cur_select_data = {group_id = 1,type_id = 1}           --当前选中的列表数据,默认值
    self.all_achievements = {}                                  --所有成就信息
    
    self.group_data = self:SetGroupData()                       --获取手风琴结构

    self.progress = SmartData.New({
        [self.P.g] = {progress = 0},                         --记录成长的总进度
        [self.P.w] = {progress = 0},                         --记录工坊的总进度
        [self.P.s] = {progress = 0},                        --记录社交的总进度
        [self.P.t] = {progress = 0},                         --记录任务活动的总进度
        [self.P.Total] = {progress = 0},                     --记录所有成就的总进度
    })

    self.honor_relation = {{1,2,3,4,5},{6,7,8},{9,10,11}}

    self.select_data = {}       --用于保存已经选择的成就数据

    self.recv_data = SmartData.New({
        recv_fb_type = 0,
        recv_first_data = {},
        recv_roll_data = {},
    })
    -- self.recv_fb_type = 0
    -- self.recv_first_data = SmartData.New()
    -- self.recv_roll_data = SmartData.New()
    self.cur_fb_type = 0        --当前选择的fb_type，用于给服务器传送

end

function AchieveData:__delete()
    if AchieveData.Instance == self then
        AchieveData.Instance = nil
    end
end

function AchieveData:GetSelectHonor(seq,select_relation)
    local pet_list = Config.achievement_auto.pet_list
    local select_data = {}
    for k, v in pairs(pet_list)do
        if seq == v.hn_type then
            table.insert(select_data,v)
        end
        -- for x, y in pairs(select_relation)do
        --     if v.hn_type == y then
        --         table.insert(select_data,v)
        --     end
        -- end
    end
    return select_data
end

function AchieveData:GetSelectData()
    return self.select_data
end

function AchieveData:SetSelectData(data)
    self.select_data = data
end

function AchieveData:GetGroupData()
    return self.group_data
end

--获取适合手风琴控件数据类型的数据
function AchieveData:SetGroupData()
    
    local config_group = Config.achievement_auto.group

    local group_count = 0
    local pre_group_id = 0
    local group_data = {}
    --获取组别数量
    for k,v in pairs(config_group) do
        if v.group_id ~= pre_group_id then 
            group_count = group_count + 1
            pre_group_id = v.group_id
            if group_data[group_count] == nil then 
                group_data[group_count] = {}
                group_data[group_count].group_id = v.group_id
                group_data[group_count].name = v.group_name
                
                --Debuger.LogError("xxxxxx"..tostring(v.group_id))
            end
        end
    end

    for i=1, group_count do
        for k, v in pairs(config_group) do
            if i == v.group_id then 
                local child = {}
                 child.name = v.type_name
                 child.type_id = v.type_id
                 child.group_id = v.group_id
                if group_data[i].children == nil then group_data[i].children = {} end
                table.insert(group_data[i].children,child)
            end
        end
    end

    return group_data
end

function AchieveData:FindAchieveByGroupIdAndTypeId(group_id,type_id)
    local pre_type_id = 0
    local achievement = Config.achievement_auto.achievement
    local finded_data = {}
    for k, v in pairs(achievement) do
        if group_id == v.group_id and type_id == v.type_id then
            --Debuger.LogError(tostring(v.group_id).."::::"..tostring(v.type_id))
            table.insert(finded_data,v)
        end
    end
    
    return finded_data
end

function AchieveData:GetProfSelectData(select_data)
    local prof = RoleData.Instance:GetBaseData().profession_id
    local prof_select_data = {}
    for k, v in pairs(select_data)do
        if v.group_id == 1 and v.type_id == 2 then
            if prof > v.camp_left_value and prof < v.camp_right_value then
                table.insert(prof_select_data,v)
            end
        end
    end
    return prof_select_data
end

function AchieveData:SetAchieveProgress(protocol)
    self.recv_progress = protocol.achieve_progress
end

function AchieveData:GetAchieveProgress()
    return self.recv_progress
end

function AchieveData:SetAchieveFetch(protocol)
    self.base_data.recv_fetch = protocol.achieve_flag
end


function AchieveData:GetProgressBySeq(seq)
    return self.recv_progress[seq]
end

function AchieveData:SetProgressBySeq(protocol)
    self.recv_progress[protocol.achieve_seq] = protocol.achieve_progress
end

function AchieveData:GetFetchBySeq(seq)
    return self.base_data.recv_fetch[seq]
end

function AchieveData:SetFetchBySeq(protocol)
    self.base_data.recv_fetch[protocol.achievement_seq] = true
end

--计算孙子成就的总进度(如声望的总进度)
function AchieveData:SumGrandSonProgress(group_id,type_id)
    local achievement = Config.achievement_auto.achievement
    local prof = RoleData.Instance:GetBaseData().profession_id
    local sum_progress = 0
    for k, v in pairs(achievement) do
        if v.group_id == group_id and v.type_id == type_id then
            if group_id == 1 and type_id == 2 then
                if prof > v.camp_left_value and prof < v.camp_right_value then
                    --sum_progress = sum_progress + self:GetProgressAndFetchBySeq(v.seq).progress
                    if self:GetFetchBySeq(v.seq) then
                        sum_progress = sum_progress + 1
                    end
                end
            else 
                if self:GetFetchBySeq(v.seq) then
                    sum_progress = sum_progress + 1
                end
                --sum_progress = sum_progress + self:GetProgressAndFetchBySeq(v.seq).progress
            end
        end
    end

    return sum_progress
end


--设置所有成就的总进度和子成就的总进度
function AchieveData:SetProgress()
    local prof = RoleData.Instance:GetBaseData().profession_id
    local achievement = Config.achievement_auto.achievement
    local total = 0
    local grow = 0
    local work = 0
    local social = 0
    local task = 0
    for k, v in pairs(achievement) do
        if v.group_id == self.P.g then
            if v.type_id == 2  then
                if prof > v.camp_left_value and prof < v.camp_right_value then
                    if self:GetFetchBySeq(v.seq) then grow = grow + 1 total = total + 1 end
                end
            else
                if self:GetFetchBySeq(v.seq) then grow = grow + 1 total = total + 1 end
            end
        elseif v.group_id == self.P.w then
            if self:GetFetchBySeq(v.seq) then work = work + 1 total = total + 1 end
        elseif v.group_id == self.P.s then
            if self:GetFetchBySeq(v.seq) then social = social + 1 total = total + 1 end
        elseif v.group_id == self.P.t then
            if self:GetFetchBySeq(v.seq) then task = task + 1 total = total + 1 end
        end
    end
    self.progress[self.P.Total].progress = total
    self.progress[self.P.g].progress = grow
    self.progress[self.P.w].progress = work
    self.progress[self.P.s].progress = social
    self.progress[self.P.t].progress = task
end

function AchieveData:SaveProgressAndFetch(progress,fetch)
    local role_id = RoleData.Instance:GetBaseData().role_id
    for i=0, ACHIEVEMENT_TYPE_NUM - 1 do
        if progress[i] ~= nil then
            UnityPlayerPrefs.SetInt(PrefKeys.AchieveProgress(role_id,i),progress[i])
        end
    end

    for i=0, ACHIEVEMENT_TYPE_NUM - 1 do
        if fetch[i] ~= nil then
            UnityPlayerPrefs.SetInt(PrefKeys.AchieveFetch(role_id,i),fetch[i])
        end
    end
end

function AchieveData:SaveProgressBySeq(protocol)
    --Debuger.LogError("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu")
    local role_id = RoleData.Instance:GetBaseData().role_id
    UnityPlayerPrefs.SetInt(PrefKeys.AchieveProgress(role_id,protocol.achieve_seq),protocol.achieve_progress)
    --self.recv_progress[protocol.achieve_seq] = protocol.achieve_progress
end

function AchieveData:SaveFetchBySeq(protocol)
    local role_id = RoleData.Instance:GetBaseData().role_id
    UnityPlayerPrefs.SetInt(PrefKeys.AchieveFetch(role_id,protocol.achievement_seq),1)
    self.base_data.recv_fetch[protocol.achievement_seq] = 1
end

function AchieveData:GetProgressAndFetchBySeq(seq)
    local role_id = RoleData.Instance:GetBaseData().role_id
    return {
        progress = UnityPlayerPrefs.GetInt(PrefKeys.AchieveProgress(role_id,seq)),
        fetch = UnityPlayerPrefs.GetInt(PrefKeys.AchieveFetch(role_id,seq))
    }
end


--获取所有进度的最大进度
function AchieveData:GetTotalMaxProgress()
    -- local max_progress = 0
    -- local achievement = Config.achievement_auto.achievement
    -- for k, v in pairs(achievement) do
    --     max_progress = max_progress + v.cond_param1
    -- end
    -- --Debuger.LogError("max:"..tostring(max_progress))
    -- return max_progress
    return #(Config.achievement_auto.achievement)
end

function AchieveData:GetMaxProgressByGroupIDAndTypeID(data,group_id,type_id)
    local max_progress = 0
    for k, v in pairs(data) do
        if v.group_id == group_id and v.type_id == type_id then
            --max_progress = max_progress + v.cond_param1
            max_progress = max_progress + 1
        end
    end
    return max_progress
end

function AchieveData:GetMaxProgressByGroupID(group_id)
    local max_progress = 0
    local achievement = Config.achievement_auto.achievement
    for k, v in pairs(achievement) do
        if v.group_id == group_id then
            --max_progress = max_progress + v.cond_param1
            max_progress = max_progress + 1
        end
    end
    --.LogError("sub:"..tostring(max_progress))
    return max_progress

end

function AchieveData:GetMaxProgressBySeq(seq)
    local achievement = Config.achievement_auto.achievement
    for k, v in pairs(achievement) do
        if v.seq == seq then 
            if v.cond_param5 == 2 then
                return v.cond_param2
            elseif v.cond_param5 == 1 then
                return v.cond_param1
            elseif v.cond_param5 == 5 then
                return 1
            elseif v.cond_param5 == 3 then
                return v.cond_param3
            elseif v.cond_param5 == 4 then
                return v.cond_param4
            end
        end
    end
end

function AchieveData:GetProgress(achieve_type)
    return self.progress[achieve_type].progress
end


function AchieveData:SetRecentAndAboutComplete()
    local achievement = Config.achievement_auto.achievement
    local prof = RoleData.Instance:GetBaseData().profession_id
    local temp = {}
    local recent_complete = SmartData.New()
    local about_complete = SmartData.New()
    for k, v in pairs(achievement) do
        if v.group_id == 1 and v.type_id == 2 then
            if prof > v.camp_left_value and prof < v.camp_right_value then
                table.insert(temp,v)
            end
        else
            table.insert(temp,v)
        end
    end

    for k, v in pairs(temp)do
        if self:GetFetchBySeq(v.seq) then 
            recent_complete[k] = v
        else
            about_complete[k] = v
        end
    end
    self.base_data.complete_info[self.RA.r] = recent_complete
    self.base_data.complete_info[self.RA.a] = about_complete
end

function AchieveData:GetRecentOrAboutComplete(ra)
    return self.base_data.complete_info[ra]
end

function AchieveData:GetSubCompleteOrNot()
    local reach_or_not = {reach = {},no = {}}
    -- for k, v in pairs(self:GetSelectData())do
    --     Debuger.LogError("fectch:"..tostring(self:GetProgressAndFetchBySeq(v.seq).fetch))
    -- end
    for k, v in pairs(self:GetSelectData())do
        if self:GetFetchBySeq(v.seq) then
            table.insert(reach_or_not.reach,v)
        else
            table.insert(reach_or_not.no,v)
        end
    end
    return reach_or_not
end