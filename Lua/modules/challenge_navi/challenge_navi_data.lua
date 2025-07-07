ChallengeNaviData = ChallengeNaviData or BaseClass()

function ChallengeNaviData:__init()
    if ChallengeNaviData.Instance ~= nil then
        Debuger.LogError("[ChallengeNaviData] attempt to create singleton twice!")
        return
    end
    ChallengeNaviData.Instance = self
    self:InitChallengeNaviList()
end

--先根据配置的等级排顺序
function ChallengeNaviData:InitChallengeNaviList()
    self.configure_list = Config.experience_auto.challenge_navi
    table.sort(self.configure_list,ExperienceData.Instance:GetSortFunc())
end

--获取挑战界面列表
function ChallengeNaviData:GetChallengeNaviList()
    local list = {}
    for i,v in ipairs(self.configure_list) do
        if MINIGAME_OPEN_MOD[v.mod] == nil then
            table.insert(list,v)
        else
            if MINIGAME_OPEN_MOD[v.mod] == 1 then
                table.insert(list,v)
            end
        end
    end
    return ExperienceData.Instance:GetShowList(list)
end

function ChallengeNaviData:GetModData(mod)
    for _,val in pairs(ChallengeNaviConfig.Config) do
        if val.mod == mod then
            return val
        end
    end
end