
ChallengeTestData = ChallengeTestData or BaseClass()

function ChallengeTestData:__init()
    self:ChallengeTestConfig()
    self:ChallengeTestData()
end

function ChallengeTestData:ChallengeTestConfig()
    self.config = Config.challenge_test_auto
    self.config_pass_reward = self.config.pass_reward
    self.config_layer_reward = self.config.layer_reward
    self.config_monster_description = self.config.monster_description
    self.config_other = self.config.other[1]

    Cfg.Register("ChallengeTestMonsterById", Cfg.RealTimeSearch(self.config_monster_description, "challenge_id"))
    Cfg.Register("ChallengeTestInfoByIdLevel", Cfg.RealTimeSearch2Key(self.config_pass_reward, "challenge_id", "role_level"))

    self.layer_rewards = {}
    self.layer_max = 0
    for _, v in pairs(self.config_layer_reward) do
        self.layer_rewards[v.role_level] = self.layer_rewards[v.role_level] or {}
        table.insert(self.layer_rewards[v.role_level], v)
        self.layer_max = math.max(self.layer_max, v.layer)
    end
end

function ChallengeTestData:ChallengeTestData()
    self.info_sm = SmartData.New{
        pass_layer = 0,
        layer_max = 0,
        next_refresh_tamp = 0,
        init = true,
    }
    self.rank_list = SmartData.New()
    self.show_butterfly = SmartData.New{val = false}
end

function ChallengeTestData:GetChallengeLayer()
    return self.info_sm.cur_layer
end

function ChallengeTestData:SetLunHuiWangChuanInfo(protocol)
    local info = protocol.info
    info.cur_layer = 0
    info.pass_layer = 0
    for layer, flag in pairs(info.pass_flags) do
        if 0 == flag and layer > 0 then
            info.cur_layer = math.min(self.layer_max, layer)
            info.pass_layer = layer - 1
            break
        end
    end
    self.info_sm:Set(protocol.info)
end

function ChallengeTestData:AccountSeq()
    return self.config_other.account_seq
end

function ChallengeTestData:GetLayerRewards(role_level)
    return self.layer_rewards[role_level] or {}
end

function ChallengeTestData:GetLayerRewardsShow(rewards)
    local rewards_show = {}
    for i = 0, #rewards.item_list do
        local item_data = rewards.item_list[i]
        table.insert(rewards_show, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
    end
    rewards.rewards_show = rewards_show
end

function ChallengeTestData:GetInfoDetailByLayerId(cur_layer, monster_id, role_level)
    local info = {
        name = "",
        desc = "",
        icon_id = 0,
        crystal = 0,
        rewards = {},
    }
    local co = Cfg.ChallengeTestMonsterById(cur_layer)
    if co then
        if monster_id == co.monster_group_id_1 then
            info.desc = co.monster_des_1
            info.crystal = co.recommend_1
        elseif monster_id == co.monster_group_id_2 then
            info.desc = co.monster_des_2
            info.crystal = co.recommend_2
        end
    end
    local co = Cfg.ChallengeTestInfoByIdLevel(cur_layer, role_level)
    if co then
        for i = 0, #co.item_list do
            local item_data = co.item_list[i]
            table.insert(info.rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
        end
    end
    local co = Cfg.MonsterGroupById(monster_id)
    if co then
        info.name = co.name
        local co = Cfg.MonsterById(co.show_monster)
        if co then
            info.icon_id = co.icon_id
        end
    end
    return info
end

function ChallengeTestData:SetGetPersonRankListAck(protocol)
    local info = protocol.info
    self.rank_list:Set(info.rank_infos)
    if nil == info.rank_info_m then
        self.rank_info_m = {
            role = {
                name = RoleData.Instance:GetRoleName(),
                prof = RoleData.Instance:GetRoleProfId(),
                avatar_type = RoleData.Instance:GetRoleAvatarType(),
		        avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
                avatar_id = RoleData.Instance:GetRoleAvatarId(),
            },
            rank_value = self.info_sm.pass_layer,
            rank = 0,
        }
    else
        self.rank_info_m = info.rank_info_m
    end
end

function ChallengeTestData:ShowButterfly()
    self.show_butterfly.val = not self.show_butterfly.val
end

function ChallengeTestData:GetChallengeTestRemindNum()
    if self.info_sm.init then return 0 end
    local reward_list = self:GetLayerRewards(self.info_sm.start_level)
    local reward_flags = self.info_sm.reward_flags
    local pass_layer = self.info_sm.pass_layer
    for _, reward in ipairs(reward_list) do
        local value = 0 == reward_flags[reward.layer] and pass_layer >= reward.layer
        if value then return 1 end
    end
    return 0
end

function ChallengeTestData:GetChallengeTestMaxLayer()
    return self.layer_max
end

function ChallengeTestData:GetChallengeTestPassLayer()
    return self.info_sm.pass_layer
end

function ChallengeTestData:IsChallengeTestLastLayer()
    return self.layer_max == self.info_sm.pass_layer
end

