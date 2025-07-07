ExperienceData = ExperienceData or BaseClass()

function ExperienceData:__init()
    if ExperienceData.Instance ~= nil then
        Debuger.LogError("[ExperienceData] attempt to create singleton twice!")
        return
    end
    ExperienceData.Instance = self
    self:InitExperienceList()
end

--先根据配置的等级排顺序
function ExperienceData:InitExperienceList()
    self.configure_list = Config.experience_auto.experience_view
    table.sort(self.configure_list,self:GetSortFunc())
end

--获取历练排序
function ExperienceData:GetSortFunc()
    return function(a,b)
        local a_weight = FunOpen.Instance:GetFunOpenLevel(a.mod)
        local b_weight = FunOpen.Instance:GetFunOpenLevel(b.mod)
        if a_weight == b_weight then
            return a.sort < b.sort
        end
        return a_weight < b_weight
    end
end

--筛选显示规则
function ExperienceData:GetShowList(configure_list)
    local list = {}
    for i,v in ipairs(configure_list) do
        if i <= Config.experience_auto.other[1].default_num then
            table.insert(list,v)
        elseif v.mod == Mod.HarmonyHeaven.Main then
            table.insert(list,v)
        else
            if RoleLevel() >= FunOpen.Instance:GetFunOpenLevel(v.mod) - Config.experience_auto.other[1].advance_level then
                table.insert(list,v)
            end
        end
    end
    for i,val in ipairs(list) do
        list[i].index = i
    end
    return list
end

--获取历练界面列表
function ExperienceData:GetExperienceList()
    return self:GetShowList(self.configure_list)
end