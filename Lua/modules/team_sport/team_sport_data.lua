TeamSportData = TeamSportData or BaseClass()

function TeamSportData:__init()
    if TeamSportData.Instance ~= nil then
        Debuger.LogError("[TeamSportData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    TeamSportData.Instance = self
    self.cfg = Config.team_fight_cfg_auto

    self.score_data = SmartData.New({
        is_change = false,
        data = {score = 0,rank = 0,victory_count = 0,defeat_count = 0,continue_victory_count = 0,box_fetch_flag = {}},
        score_change_val = 0,   --积分变化的值，用于做战斗结算之后的展示
    })
    
    self.fight_info = SmartData.New({
        is_change = false,
        data = {}
    })
    self.rank_data = SmartData.New({
        is_change = false,
        data = {}
    })
    self.match_data = SmartData.New({
        is_change = false,
        is_match = 0,
        is_complete = 0,
    })

    self.record_time = SmartData.New({time = 0})

    self.user_list = SmartData.New()
    
    self.all_user_list = SmartData.New({})

    self.join_data = SmartData.New({
        level = 0,      -- 参加活动时的等级
        reward_flag = 0,-- 参与奖励标记
    })

    self:InitRewardConfig()
end

function TeamSportData:SetScoreChangeVal(value)
    self.score_change_val = value
end

function TeamSportData:GetScoreChangeVal()
    return self.score_change_val or 0
end

function TeamSportData:SetScoreValue(value)
    self.score_value = value
end

function TeamSportData:GetScoreValue()
    return self.score_value or 0
end

function TeamSportData:StartCountTime()
    self.ht = TimeHelper:AddRunTimer(function()
        TeamSportData.Instance.record_time.time = TeamSportData.Instance.record_time.time + 1
    end, 1)
end

function TeamSportData:CancelCountTime()
    if self.ht then
        TimeHelper:CancelTimer(self.ht)
    end
end

function TeamSportData:IsInTeamSport()
    return SceneData.Instance:SceneId() == Config.team_fight_cfg_auto.other[1].scene_id
end

--小队竞技 开始匹配通知
function TeamSportData:SetStartMatch(protocol)
    self.user_list:Set(protocol.user_list)
end

--小队竞技 匹配信息更新（新增队友或队友离开匹配时）
function TeamSportData:SetMatchInfoUpdate(protocol)
    self.user_list:Set(protocol.user_list)
end

function TeamSportData:GetUserList()
    return self.user_list:Val()
end

function TeamSportData:SetAllUserList(protocol)
    self.all_user_list:Set(protocol)
end

function TeamSportData:GetAllUserList()
    return self.all_user_list
end

function TeamSportData:SetJoinData(protocol)
    self.join_data.level = protocol.level
    self.join_data.reward_flag = protocol.particip_award_flag
end

function TeamSportData:GetJoinData()
    return self.join_data
end

-- 初始化宝箱奖励配置
function TeamSportData:InitRewardConfig()
    self.box_reward_config = {}
    for k,v in pairs(Config.team_fight_cfg_auto.box_reward) do
        if self.box_reward_config[v.type] == nil then
            self.box_reward_config[v.type] = {}
        end
        table.insert(self.box_reward_config[v.type],v)
    end
end

function TeamSportData:GetBoxRewardConfig(type,is_help)
    local level = not is_help and self.join_data.level or RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.box_reward_config[type]) do
        if level >= v.min_level and level <= v.max_level then
            return v
        end 
    end
end

-- 宝箱领取标记
function TeamSportData:GetRewardFetchFlag(index)
    return self.score_data.data.box_fetch_flag[32-index] or 0
end

-- 获取宝箱奖励状态 -1 已领取，0不能领取，1可领取
function TeamSportData:GetBoxRewardStatus(self,type)
    if self:GetRewardFetchFlag(type) == 1 then
        return -1
    end
    local win_count = self.score_data.data.victory_count
    local sum_count = win_count + self.score_data.data.defeat_count
    if type == 0 then
        return win_count >= 1 and 1 or 0
    elseif type == 1 then
        return sum_count >= 3 and 1 or 0
    else
        return 0
    end
end

-- 获取参与奖状态 1= 已领取，0等于未领取
function TeamSportData:GetJoinRewardStatus()
    return self:GetJoinData().reward_flag
end

-- 宝箱标记
function TeamSportData:GetBoxAllRewardStatus()
    for i,v in pairs(self.box_reward_config) do
        if self:GetBoxRewardStatus(self,i) > 0 then
            return 0
        end
    end
    return 1
end

-- 整合奖励组奖励和铜币奖励还有经验
function TeamSportData:FormatGroupAndCoinReward(coin_num,exp)
    coin_num = coin_num or 0
    exp = exp or 0
    local reward_list = {}
    if exp > 0 then
        local exp_item = {
            item_id = CommonItemId.Exp,
            num = exp
        }
        table.insert(reward_list,Item.Create(exp_item))
    end
    if coin_num > 0 then
        local coin_reward = {
                item_id = CommonItemId.Coin,
                num = coin_num,
            }
        table.insert(reward_list,Item.Create(coin_reward))
    end
    return reward_list
end

-- 获取单个排名奖励
function TeamSportData:GetRankReward(rank,level)
    for k,v in pairs(self.cfg.reward) do
        if rank >= v.min_rank and rank <= v.max_rank and level >= v.level_min and level <= v.level_max then
            local item_list = DataHelper.FormatItemList(v.reward)
            return AndList(item_list,self:FormatGroupAndCoinReward(v.bind_coin,v.exp))
        end
    end
end

-- 获取全部排名奖励
function TeamSportData:GetAllRankReward(is_help)
    local level
    if not is_help then
        level = self.join_data.level
    else
        level = RoleData.Instance:GetRoleLevel()
    end
    local all_reward_list = {}
    for i , v in ipairs(self.cfg.reward) do
        if level >= v.level_min and level <= v.level_max then
            local coin_num = v.bind_coin
            local info = {}
            local rank_title
            if v.min_rank == v.max_rank then
                rank_title = string.format(Language.TeamSport.RankStr1,v.max_rank)
            else
                rank_title = string.format(Language.TeamSport.RankStr2,v.min_rank,v.max_rank)
            end
            info.item_list = DataHelper.FormatItemList(v.reward)
            AndList(info.item_list,self:FormatGroupAndCoinReward(coin_num,v.exp))
            info.title_name = rank_title
            if not is_help then
                info.tip_str = Language.TeamSport.RankRewardTip
            end
            info.title_type = 2
            table.insert(all_reward_list,info)
        end
    end
    return all_reward_list
end

-- 获取宝箱奖励信息
function TeamSportData:GetAllBoxRewardInfo(is_help)
    local reward_info_list = {}
    local get_func = function(type)
        local flag = self:GetBoxRewardStatus(self,type)
        if flag > 0 then
            TeamSportCtrl.Instance:SendTeamFightMatchReq(6,type)
        elseif flag == 0 then
            PublicPopupCtrl.Instance:Center(string.format(Language.TeamSport.GetTip1,Language.TeamSport.BoxItemName2[type]))
        else
            PublicPopupCtrl.Instance:Center(Language.TeamSport.GetTip2)
        end
    end
    local get_btn_flag_func = function (type)
        -- BtnState = {[0] = "领取","已领取","领取"},
        local flag = self:GetBoxRewardStatus(self,type)
        if  flag == 0 then
            return 2
        elseif flag == 1 then
            return 0
        elseif flag == -1 then
            return 1
        else
            return 1
        end
    end
    -- i = type
    for i,v in pairs(self.box_reward_config) do
        local reward_cfg = self:GetBoxRewardConfig(i,is_help)
        if reward_cfg then
            local info = {}
            info.child_title = Language.TeamSport.BoxItemName2[i]
            info.item_list = {Item.Create(reward_cfg.reward)}
            if next(reward_cfg.reward) ~=nil then
                AndList(info.item_list,self:FormatGroupAndCoinReward(reward_cfg.coin,reward_cfg.exp))
            end
            if not is_help then
                --info.tip_str = Language.TeamSport.BoxRewardTip
            end
            --暂时不要宝箱领取按钮，以后可能要
            info.click_func = get_func
            info.func_param = i
            info.btn_flag = get_btn_flag_func(i)
            table.insert(reward_info_list,info)
        end
    end
    return reward_info_list
end

-- 获取基础奖励
function TeamSportData:GetCommonReward(is_help)
    local level = not is_help and self.join_data.level or RoleData.Instance:GetRoleLevel()
    local reward_list = {}
    for i,v in pairs(self.cfg.participation_reward) do
        if level >= v.level_min and level <= v.level_max then
            reward_list = self:FormatGroupAndCoinReward(v.coin,v.exp)
            if next(v.reward) ~= nil then
                AndList(reward_list,DataHelper.FormatItemList(v.reward))
            end
            break
        end
    end
    return reward_list
end

-- 格式全部的展示奖励
function TeamSportData:GetFormatAllReward(is_help)
    local reward_list = {}
    -- 基础奖励
    local basics_info = {}
    basics_info.toggle_name = Language.TeamSport.BaseReward
    basics_info.reward_item_list = {
        [1] = {
            title_name = Language.TeamSport.JoinRewardTitle,
            item_list = self:GetCommonReward(is_help),
        },
        [2] = {
            title_name = Language.TeamSport.BoxRewardTitle,
            child_list = self:GetAllBoxRewardInfo(is_help),
        }
    }
    if not is_help then
        basics_info.reward_item_list[1].btn_flag = self:GetJoinRewardStatus()
        basics_info.reward_item_list[1].click_func = TeamSportCtrl.Instance.GetJoinReward
        basics_info.reward_item_list[1].func_param = {}
    end
    table.insert(reward_list,basics_info)
    -- 排名奖励
    local rank_info = {}
    rank_info.toggle_name = Language.TeamSport.RankReward
    rank_info.reward_item_list = self:GetAllRankReward(is_help)
    table.insert(reward_list,rank_info)
    -- 规则信息
    local tip_info = {}
    tip_info.toggle_name = Language.Activity.TipsNotice
    tip_info.reward_item_list = {{tips_act_type = ActType.TeamSport}}
    table.insert(reward_list,tip_info)
    return reward_list
end

function TeamSportData:GetHlepRewardInfo()
    return self:GetFormatAllReward(true)
end