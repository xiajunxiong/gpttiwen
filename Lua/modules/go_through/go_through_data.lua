GoThroughData = GoThroughData or BaseClass()

function GoThroughData:__init()
    if GoThroughData.Instance ~= nil then
        Debuger.LogError("[GoThroughData] attempt to create singleton twice!")
        return
    end
    GoThroughData.Instance = self
    self.info_data = SmartData.New({pass_level = -1,progress = 0,assist_times = 0})
    self.scene_data = SmartData.New({level = 1})
    self.pass_config = KeyList(Config.cross_robbery_auto.base,[[seq]])
end

function GoThroughData:GetPassLevelData(seq)
    return self.pass_config[seq or self:GetPassLevel()]
end

function GoThroughData:GetMaxPassLevelData()
    return self.pass_config[#self.pass_config]
end

function GoThroughData:SetGoThroughInfo(protocol)
    self.info_data.pass_level = protocol.pass_level
    self.info_data.progress = protocol.progress
    self.info_data.assist_times = protocol.assist_times
end

function GoThroughData:SetGoThroughSceneInfo(protocol)
    self.scene_data.level = protocol.level
    self.scene_data.is_finish = protocol.is_finish
end

function GoThroughData:GetPassLevel()
    return self.info_data.pass_level
end

function GoThroughData:GetProgress()
    return self.info_data.progress
end

function GoThroughData:GetChallengeLevel()
    return self.scene_data.level
end

function GoThroughData:GetNpcId()
    return Config.cross_robbery_auto.other[1].challenge_npc
end

function GoThroughData:GetTeamTarget(seq)
    return Config.cross_robbery_auto.team_target[seq].target or 0
end

function GoThroughData:GetTargetNum()
    return Config.cross_robbery_auto.other[1].target_num or 0
end

function GoThroughData:SetRewardData(data)
    self.pass_reward_data = data
end

function GoThroughData:GetRewardData()
    return self.pass_reward_data or {}
end

function GoThroughData:GetAssistTimes(seq)
    return self.info_data.assist_times
end

function GoThroughData:GetAttrList(data)
    local list = {}
    for i=1,4 do
        local type,value = data["attr_type_" .. i],data["attr_add_"..i]
        if value ~= 0 then
            table.insert(list,{attr_type = type,
            attr_name = Language.Common.AttrList[type],
            attr_value = DataHelper.Percent(type,value)})
        end
    end
    return list
end

function GoThroughData:GetEnterInfo(enter_fb_info)
    local data = self:GetPassLevelData(enter_fb_info.param1)
    if data ~= nil then
        local TitleName = Language.GoThrough.TitleName
        local name = Format(TitleName,data.name,data.reorder)
        return {npc_id = data.npc_id,name = name}
    end
end