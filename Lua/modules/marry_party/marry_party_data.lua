
MarryPartyData = MarryPartyData or BaseClass()

function MarryPartyData:__init()
    if MarryPartyData.Instance ~= nil then
        Debuger.LogError("[MarryPartyData] attempt to create singleton twice!")
        return
    end
    MarryPartyData.Instance = self
    
    self:MarryPartyConfig()
    self:MarryPartyData()
end

function MarryPartyData:MarryPartyConfig()
    self.config = Config.Love_meeting_auto
    self.config_game_link = self.config.game_link
    self.config_game_desc = self.config.game_desc
    -- self.config_wedding_party = self.config.wedding_party
    -- self.config_heat_reward = self.config.heat_reward
    -- self.config_wedding_party_order = self.config.wedding_party_order
    -- self.config_float_parade = self.config.float_parade
    -- self.config_flowerball_way = self.config.flowerball_way
    self.config_other = self.config.other[1]

    Cfg.Register("LoveMeetingAnswerById",Cfg.RealTimeSearch(Config.Love_meeting_auto.zeus_and_roxanne,"question_id"))
end

function MarryPartyData:MarryPartyData()
    self.marry_party_scene_info = SmartData.New{init = true}
    self.marry_party_role_info = SmartData.New{init = true}
    self.marry_party_question_info = SmartData.New{
        info = SmartData.New{init = true},
        result = SmartData.New{init = true},
    }
    self.marry_party_fight_info = SmartData.New{
        role_infos = SmartData.New{init = true},
        route = SmartData.New{init = true},
    }

    self.marry_party_game_sm = SmartData.New{}
end

function MarryPartyData:ConfigOtherQuestionMount()
    return self.config_other.question_amount
end

function MarryPartyData:ConfigOtherLanguageCfg()
    return self.config_other.language_cfg
end

function MarryPartyData:ConfigOtherFlowerAmpResource()
    return self.config_other.flower_amp_resource
end


function MarryPartyData:SetQingYuanShengHuiSceneInfo(protocol)
    -- ylog("SetQingYuanShengHuiSceneInfo",protocol)
    self.marry_party_scene_info:Set(protocol.info)
end

function MarryPartyData:SetQingYuanShengHuiRoleInfo(protocol)
    -- ylog("SetQingYuanShengHuiRoleInfo",protocol)
    self.marry_party_role_info:Set(protocol.info)
end

function MarryPartyData:SetQingYuanShengHuiQuestionResult(protocol)
    -- ylog("SetQingYuanShengHuiQuestionResult", protocol)
    -- self.marry_party_question_info.result:Set(protocol.info)
    PublicPopupCtrl.Instance:Center(Language.MarryParty.Answer.ResultShow[protocol.info.is_same] or "")
end

function MarryPartyData:SetQingYuanShengHuiQuestionRoute(protocol)
    -- ylog("SetQingYuanShengHuiQuestionRoute", protocol)
    local info = protocol.info
    if 1 == info.is_open then
        ViewMgr:OpenView(MarryPartyAnswerView)
    else
        ViewMgr:CloseView(MarryPartyAnswerView)
    end
end

function MarryPartyData:SetQingYuanShengHuiQuestionInfo(protocol)
    -- ylog("SetQingYuanShengHuiQuestionInfo", protocol)
    self.marry_party_question_info.info:Set(protocol.info)
end

function MarryPartyData:SetQingYuanShengHuiOpponentInfo(protocol)
    self.marry_party_fight_info.role_infos:Set(protocol.role_infos)

    TimeHelper:AddDelayTimer(function ()
        local left_user_list = self:GetMemberRoleList()
        local right_user_list = protocol.role_infos
        MainOtherCtrl.Instance:OnTeamSport({left_user_list = left_user_list, right_user_list = right_user_list})
    end, 1)
end

function MarryPartyData:SetQingYuanShengHuiMatchOpRoute(protocol)
    -- ylog("SetQingYuanShengHuiMatchOpRoute", protocol)
    local info = protocol.info
    self.marry_party_fight_info.route:Set(info)

    if 1 == info.is_open then
        ViewMgr:OpenView(MarryPartyFightView)
    else
        ViewMgr:CloseView(MarryPartyFightView)
    end
end

function MarryPartyData:SetQingYuanShengHuiInfo(protocol)
end

function MarryPartyData:GetMarryPartyGameList()
    if nil == self.marry_party_game_show_list then
        self.marry_party_game_show_list = {}
        for _, v in ipairs(self.config_game_desc) do
            v.reward_items = {}
            for y, z in pairs(v.reward_show) do
                table.insert(v.reward_items, Item.Init(z.item_id, z.num, z.is_bind))
            end
            table.insert(self.marry_party_game_show_list, v)
        end
    end
    return self.marry_party_game_show_list
end

function MarryPartyData:GetMemberRoleList()
    local list = {}
    local members = TeamData.Instance:GetMemberRoleList()
    for i = 1, #members do
        local member = members[i]
        if member and member.info then
            local info = member.info
            table.insert(list, {
                uid = info.uid,
                name = info.name,
                level = info.level,
                prof = info.prof,
                avatar_type = info.appearance.avatar_type,
                avatar_id = info.appearance.avatar_id,
            })
        end
    end
    return list
end

function MarryPartyData:MarryPartyGather()
    local role_info = self.marry_party_role_info
    if role_info.init then return end
    local language = Language.MarryParty.Menu
    if TimeHelper.GetServerTime() < role_info.can_gather_next_time then
        PublicPopupCtrl.Instance:Center(string.format(language.GatherMenuTime, math.floor(role_info.can_gather_next_time - TimeHelper.GetServerTime())))
    else
        -- if self:ConfigOtherEatTimesMax() == role_info[MarryConfig.FBCommonOperType.meal] then
        --     PublicPopupCtrl.Instance:Center(language.GatherMenuFinish)
        -- else
            MarryPartyCtrl.Instance:SendQingYuanShengHuiGather()
        -- end
    end
end