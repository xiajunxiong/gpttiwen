DreamNotesData = DreamNotesData or BaseClass()

function DreamNotesData:__init()
    if DreamNotesData.Instance ~= nil then
        Debuger.LogError("[DreamNotesData] attempt to create singleton twice!")
        return
    end
    DreamNotesData.Instance = self
    self:Initialization()
    self.info_data = SmartData.New({
        pass_level = -1,
        remain_times = 0,
        buy_times = 0,
        use_item_add_times = 0
    })
    self.config = Config.dream_notes_challenge_auto
    self.scene_data = SmartData.New({level = 1,obj_list = {}})
    self.red_point_data = SmartData.New({val = false})
    self.remind_list = {}
end

function DreamNotesData:ClearData()
    self.info_data:Set({
        pass_level = -1,
        remain_times = 0,
        buy_times = 0,
        use_item_add_times = 0
    })
    self.remind_list = {}
    self.reward_data = nil
end

function DreamNotesData:Initialization()
	self.select_data = SmartData.New()
end

function DreamNotesData:SetSelectData(data)
	self.select_data:Set(data)
end

function DreamNotesData:GetSelectData()
	return self.select_data
end

function DreamNotesData:SetChallengeLevel(level)
    self.scene_data.level = level
end

function DreamNotesData:GetChallengeLevel()
    return self.scene_data.level
end

function DreamNotesData:GetPassLevelData(pass_level)
    return self.config.dream_config[pass_level]
end

function DreamNotesData:SetDreamNotesInfo(protocol)
    self.info_data.pass_level = protocol.pass_level
    self.info_data.remain_times = protocol.remain_times
    self.info_data.buy_times = protocol.buy_times
    self.info_data.is_first_kill = protocol.is_first_kill
    self.info_data.use_item_add_times = protocol.use_item_add_times
end

function DreamNotesData:SetDreamNotesSceneInfo(protocol)
    self.scene_data.level = protocol.level
    self.scene_data.obj_list = protocol.obj_list
end

function DreamNotesData:GetSceneNpcVo(id)
    if id == nil then return 0 end
    return self.scene_data.obj_list[id] or id
end

function DreamNotesData:IsFirstKill(data)
    if data == nil then return false end
    return self:GetPassLevel() >= data.level
end

function DreamNotesData:GetSweepTimes()
    return self.info_data.remain_times
end

function DreamNotesData:GetPassLevel()
    return self.info_data.pass_level
end

function DreamNotesData:GetRemainPass()
    return self:GetUnLockNum() - self:GetPassLevel() - 1
end

function DreamNotesData:GetUnLockNum()
    for i,v in ipairs(self:GetPassList()) do
        if RoleData.Instance:GetRoleLevel() < v.recommend_level then
            return i
        end
    end
    return #self:GetPassList()
end

function DreamNotesData:GetBuyTimes()
    return self:GetOtherData().extra_purchase_time - self:GetInfoBuyTimes()
end

function DreamNotesData:GetHasTimes()
    return self:GetSweepTimes()
end

function DreamNotesData:GetHasMaxTimes()
    return self:GetOtherData().pass_time_limit + self:GetInfoBuyTimes() + self.info_data.use_item_add_times
end

function DreamNotesData:GetHasMaxTimesDesc(need_count)
    return Format("%s（%s）",need_count,Format(Language.DreamNotes.SweepTip,COLORSTR.Yellow14,self:GetSweepTimes()))
end

function DreamNotesData:GetInfoBuyTimes()
    return self.info_data.buy_times or 0
end

--获取关卡奖励预览 已通关的需要灰掉
function DreamNotesData:GetRewardList(data)
    local reward_list = {}
    if self:IsFirstKill(data) then
        self:GetRandomRewardData(data,reward_list)
        self:GetRewardData(data,reward_list)
        self:GetFirstRewardData(data,reward_list)
    else
        self:GetFirstRewardData(data,reward_list)
        self:GetRandomRewardData(data,reward_list)
        self:GetRewardData(data,reward_list)
    end
    return reward_list
end

--获取首通奖励
function DreamNotesData:GetFirstRewardData(data,reward_list)
    for k,v in pairs(data.first_reward) do
        v.first_kill = true
        v.has_reward = self:IsFirstKill(data)
        table.insert(reward_list,v)
    end
    table.insert(reward_list,{
        item_id = CommonItemId.PartnerLevelItem,
        num = data.first_yuanling,
        first_kill = true,
        has_reward = self:IsFirstKill(data)
    })
    table.insert(reward_list,{
        item_id = CommonItemId.LuckCoin,
        num = data.first_luck,
        first_kill = true,
        has_reward = self:IsFirstKill(data)
    })
    return reward_list
end

--获取普通奖励
function DreamNotesData:GetRewardData(data,reward_list)
    for k,v in pairs(data.reward) do
        table.insert(reward_list,v)
    end
    if data.coin ~= 0 then
        table.insert(reward_list,{item_id = CommonItemId.Coin,num = data.coin})
    end
    table.insert(reward_list,{item_id = CommonItemId.LuckCoin,num = data.luck})
    table.insert(reward_list,{item_id = CommonItemId.PartnerLevelItem,num = data.yuanling})
    -- table.insert(reward_list,{item_id = CommonItemId.Exp,num = self:GetExpNum(data.power_expend)})
    return reward_list
end

--获取随机奖励
function DreamNotesData:GetRandomRewardData(data,reward_list)
    for k,v in pairs(data.partner_fragment_reward) do
        v.is_random = true
        table.insert(reward_list,v)
    end
    return reward_list
end

function DreamNotesData:GetFinishRewardData(data)
    local reward_list = {}
    if self:IsFhishFirstKill() then
        self:GetFirstRewardData(data,reward_list)
    else
        self:GetRewardData(data,reward_list)
    end
    return reward_list
end

function DreamNotesData:GetOtherData()
    return self.config.other[1] or {}
end

function DreamNotesData:GetRankList()
    return SocietyData.Instance:GetRankInfo(PERSON_RANK_TYPE.DREAM_NOTES_CHALLENGE)
end

function DreamNotesData:ResetsFirstKill()
    self.info_data.is_first_kill = false
end

function DreamNotesData:IsFhishFirstKill()
    return self.info_data.is_first_kill == true
end

function DreamNotesData:GetPassList()
    return self.config.dream_config
end

function DreamNotesData:GetIndex(list, index)
    --index = self.guide_index or index or self:GetPassLevel() + 1
    --self.guide_index = nil
    
    index = index or self:GetPassLevel() + 1
    return DataHelper.SimpleIndex(list,"level", index) or #list
end

--当前关卡购买次数价格
function DreamNotesData:GetTimesPrice(num)
    local buy_times = self:GetInfoBuyTimes() + num
    for k,v in pairs(self.config.buy_extra_times) do
        if buy_times == v.buy_times then
            return v.price
        end
    end
end

function DreamNotesData:GetTimesPriceNum(num)
    local price = 0
    for i=1,num do
        price = price + self:GetTimesPrice(i)
    end
    return price
end

--提醒红点
function DreamNotesData:CheckRemindNum()
    self:SetItemRemind(UnityPlayerPrefs.GetInt(RoleData.Instance:GetRoleId() .. "DreamNotes"))
end

function DreamNotesData:CheckRedPiontData()
    self.red_point_data.val = not self.red_point_data.val
end

function DreamNotesData:SetItemRemind(level)
    if level ~= 0 then
        self.remind_list[level] = true
        self:CheckRedPiontData()
    end
end

function DreamNotesData:GetItemRemind(level)
    return self.remind_list[level] and 1 or 0
end

function DreamNotesData:GetLockLevel()
    for i,v in ipairs(self:GetPassList()) do
        if RoleData.Instance:GetRoleLevel() == v.recommend_level then
            return v.level
        end
    end
end

function DreamNotesData:GetRemindNum()
    for k,v in pairs(self.remind_list) do
        if v == true then
            return 1
        end
    end
    return 0
end

function DreamNotesData:GetIsAccepted()
	for k,v in pairs(DataHelper.SplitList(self:GetOtherData().task_id,"|")) do
		if TaskData.Instance:GetTaskIsAccepted(v) then
            return true
        end
	end
    return false
end

function DreamNotesData:SetRandRewardData(data)
    self.reward_data = data
end

--获取随机普通奖励
function DreamNotesData:GetRandRewardData(data)
    local reward_list = {}
    for k,v in pairs(self.reward_data or {}) do
        table.insert(reward_list,v)
    end
    if data.coin ~= 0 then
        table.insert(reward_list,{item_id = CommonItemId.Coin,num = data.coin})
    end
    table.insert(reward_list,{item_id = CommonItemId.LuckCoin,num = data.luck})
    table.insert(reward_list,{item_id = CommonItemId.PartnerLevelItem,num = data.yuanling})
    return DataHelper.FormatItemList(reward_list)
end

function DreamNotesData:ClearRemindList()
	self.remind_list = {}
    self:CheckRedPiontData()
end

function DreamNotesData:GetExpNum(strength_num)
    local config = Config.crystal_scene_auto.exp[RoleLevel] or {}
    return strength_num * (config.exp or 0)
end