MaterialData = MaterialData or BaseClass()

function MaterialData:__init()
    if MaterialData.Instance ~= nil then
        Debuger.LogError("[MaterialData] attempt to create singleton twice!")
        return
    end
    MaterialData.Instance = self
    self.config = Config.material_scene_auto
    self.chapter_config = KeyList(self.config.scene_config,"seq")
    self.scene_data = SmartData.New({pass_seq = 0,obj_list = {}})
    self:InitMaterialData()
    self:Initialization()
end

function MaterialData:ClearData()
    self.info_data.pass_seq = 0
    self.info_data.chapter_info = {}
    self.info_data.intergral_info = {}
    self.reward_data.box_flag = {}
    self.reward_data.val = not self.reward_data.val
end

function MaterialData:Initialization()
    self.info_data = SmartData.New({
        val = false,
        pass_seq = 0,
        pass_time = 0,
        chapter_info = {},
        intergral_info = {}
    })
    self.reward_data = SmartData.New({box_flag = {},val = false})
end

function MaterialData:InitMaterialData()
    self.chapter_list = {}
    for k, v in pairs(self.config.scene_config) do
        if self.chapter_list[v.chapter] == nil then
            self.chapter_list[v.chapter] = {}
        end
        table.insert(self.chapter_list[v.chapter],v)
    end
    self.reward_list = {}
    for k, v in pairs(self.config.box_reward) do
        if self.reward_list[v.chapter] == nil then
            self.reward_list[v.chapter] = {[1] = {max_score = 0,chapter = v.chapter}}
        end
        table.insert(self.reward_list[v.chapter],v)
    end
end

function MaterialData:SetSelectChapterData(data)
	self.select_chapter_data = data
end

function MaterialData:GetSelectChapterData()
	return self.select_chapter_data
end

function MaterialData:SetSelectData(data)
	self.select_data = data
end

function MaterialData:GetSelectData()
	return self.select_data
end

--获取章节列表
function MaterialData:GetChapterList(chapter)
    return self.chapter_list[chapter] or {}
end

--获取场景对应配置
function MaterialData:GetChapterConfig(seq)
    return self.chapter_config[seq]
end

--获取宝箱奖励列表
function MaterialData:GetBoxRewardList(chapter)
    return self.reward_list[chapter] or {}
end

--协议下发
function MaterialData:SetMaterialSceneInfo(protocol)
    self.scene_data.obj_list = protocol.obj_list
    self.scene_data.pass_seq = self:GetChapterLevelData(protocol).seq
end

function MaterialData:SetMaterialAllInfo(protocol)
    self.info_data.chapter_info = protocol.chapter_info
    self.info_data.intergral_info = protocol.intergral_info
end

function MaterialData:SetMaterialSingleInfo(protocol)
    if self.info_data.chapter_info[protocol.chapter] == nil then
        self.info_data.chapter_info[protocol.chapter] = {}
    end
    self.old_info = self:GetChapterInfo(protocol)
    self.info_data.chapter_info[protocol.chapter][protocol.level] = protocol.chapter_info
    self.info_data.intergral_info[protocol.chapter] = protocol.integral_count
    self.info_data.val = not self.info_data.val
    self.info_data.is_first_kill = protocol.is_first_kill
end

function MaterialData:SetMaterialOtherInfo(protocol)
    self.info_data.pass_seq = self:GetChapterLevelData(protocol).seq
    self.reward_data.box_flag = protocol.box_flag
    self.reward_data.val = not self.reward_data.val
    self.info_data.pass_time = protocol.pass_time
end

function MaterialData:GetChapterInfo(param)
    if self.info_data.chapter_info[param.chapter] then
        return self.info_data.chapter_info[param.chapter][param.level] or {}
    end
    return {}
end

--当前关卡是否挑战过
function MaterialData:IsFirstKill(data)
    local first_kill = self:GetChapterInfo(data).first_kill or 0
    return first_kill == 1
end

function MaterialData:GetSceneNpcVo(id)
    return self.scene_data.obj_list[id] or id
end

function MaterialData:SetChallengeData(seq)
    self.scene_data.pass_seq = seq
end

function MaterialData:GetChallengeData()
    return self.scene_data.pass_seq
end

function MaterialData:GetPassSeq()
    return self.info_data.pass_seq
end

function MaterialData:GetBuyTimes(data)
    return self:GetOtherData().buy_times_max - self:GetInfoBuyTimes(data)
end

function MaterialData:GetHasMaxTimes(data)
    return self:GetOtherData().pass_time_max + self:GetInfoBuyTimes(data)
end

--获取体力上限
function MaterialData:GetFightStrengthNum()
    return self:GetOtherData().use_power 
end

--获取当前购买次数
function MaterialData:GetInfoBuyTimes(data)
    local info = self:GetChapterInfo(data)
    return info.buy_times or 0
end

--获取当前剩余扫荡次数
function MaterialData:GetSweepTimes(data)
    local remain_times = self:GetChapterInfo(data).remain_times or 0
    return self:GetHasMaxTimes(data) - remain_times
end

function MaterialData:GetRewardFlag(seq)
    return self.reward_data.box_flag[seq] or 0
end

--获取当前关卡星星数量
function MaterialData:GetConditonScore(data)
    local info = self:GetChapterInfo(data)
    return info.flag_num or 0
end

--获取当前关卡星星通关信息 index 1 ~ 3
function MaterialData:GetConditonFlag(data,index)
    local flag_list = self:GetChapterInfo(data).flag_list or {}
    return flag_list[index] or 0
end

--解析条件内容
function MaterialData:GetConditionList(data)
	local list = {}
	for i=1,3 do
		list[i] = CourageChallengeData.Instance:GetConditionDesc(data["condition_" .. i])
	end
	return list
end

function MaterialData:GetScoreNum(chapter)
    return self.info_data.intergral_info[chapter] or 0
end

function MaterialData:GetOtherData()
    return self.config.other[1] or {}
end

--当前关卡购买次数价格
function MaterialData:GetTimesPrice(data)
    return self:GetOtherData().extra_buy_consume
end

--根据章节和关卡获取对应配置
function MaterialData:GetChapterLevelData(data)
    for k,v in pairs(self:GetChapterList(data.chapter) or {}) do
        if v.level == data.level then
            return v
        end
    end
    return {}
end

--获取关卡奖励预览 已通关的需要灰掉
function MaterialData:GetRewardList(data)
    local reward_list = {}
    if not self:IsFirstKill(data) then
        self:GetFirstRewardData(data,reward_list)
        self:GetRewardData(data,reward_list)
    else
        self:GetRewardData(data,reward_list)
    end
    return reward_list
end

--获取首通奖励
function MaterialData:GetFirstRewardData(data,reward_list)
    for k,v in pairs(data.first_reward) do
        v.first_kill = true
        v.has_reward = self:IsFirstKill(data)
        table.insert(reward_list,v)
    end
    return reward_list
end

--获取普通奖励
function MaterialData:GetRewardData(data,reward_list)
    reward_list = reward_list or {}
    for i,v in ipairs(DataHelper.SplitData(data.show_reward,":")) do
        table.insert(reward_list,v)
    end
    return reward_list
end

--结算奖励
function MaterialData:GetFinishRewardData(data)
    local reward_list = {}
    if self:IsFhishFirstKill(data) then
        self:GetFirstRewardData(data,reward_list)
        self:GetFinishRewardList(reward_list)
    else
        self:GetFinishRewardList(reward_list)
    end
    return reward_list
end

function MaterialData:SetFinishRewardList(data)
    self.finish_reward_data = data
end

--获取下发的结算奖励
function MaterialData:GetFinishRewardList(reward_list)
    for i,v in ipairs(self.finish_reward_data or {}) do
        table.insert(reward_list,v)
    end
    return reward_list
end

--宝箱奖励
function MaterialData:GetRewardItemList(data)
    local list = DataHelper.FormatItemList(data.reward)
    list = TableCopy(list)
    for i, v in pairs(list) do
        v.item_id = BagData.GetNewItemId(v.item_id)
    end
    if data.coin_reward ~= 0 then
        table.insert(list,Item.New({item_id = CommonItemId.Coin,num = data.coin_reward}))
    end
    return list
end

function MaterialData:GetExpNum()
    return self:GetFightStrengthNum() * (Config.crystal_scene_auto.exp[RoleData.Instance:GetRoleLevel()].exp or 0)
end

function MaterialData:IsFhishFirstKill()
    return self.info_data.is_first_kill == true
end

function MaterialData:ResetsFirstKill()
    self.info_data.is_first_kill = false
end

--上一个章节最后一个关卡是否通关
function MaterialData:IsLastFirstKill(data)
    -- if RoleData.Instance:GetRoleLevel() < data.chapter_level_limit then
    --     return false
    -- end
    local config = self:GetChapterList(data.seq - 1)
    if #config ~= 0 then
        return self:IsFirstKill(config[#config] or {})
    end
	return true
end

--上一个关卡是否通关
function MaterialData:IsLastItemFirstKill(data)
    local config = self:GetChapterConfig(data.seq - 1)
    if config ~= nil then
        return self:IsFirstKill(config)
    end
	return true
end

--总红点
function MaterialData:GetRemindNum()
    for k,v in pairs(self.config.box_reward) do
        if self:GetScoreNum(v.chapter) >= v.max_score and self:GetRewardFlag(v.seq) == 0 then
            return 1
        end
    end
    return 0
end

--章节红点
function MaterialData:GetChapterNum(chapter)
    local num = self:GetScoreNum(chapter)
    for k,v in pairs(self.reward_list[chapter] or {}) do
        if v.max_score ~= 0 and num >= v.max_score and self:GetRewardFlag(v.seq) == 0 then
            return 1
        end
    end
    return 0
end

--章节列表
function MaterialData:GetMaterialList()
    local list = {{}}
    for i,v in ipairs(self.config.chapter_show) do
        table.insert(list,v)
    end
    table.insert(list,{})
    return list
end

function MaterialData:GetRecommendData()
    return self.recommend_data
end

function MaterialData:SetRecommendData(data)
    self.recommend_data = data
end

--获取下一关配置
function MaterialData:GetNextData(data)
    local role_level = RoleData.Instance:GetRoleLevel()
    local next_data = self:IsFirstKill(data) and self:GetChapterConfig(data.seq + 1) or data
    local show_data = self:GetChapterShowData(next_data.chapter)
    -- if role_level < show_data.chapter_level_limit then
    --     return  data,self:GetChapterShowData(data.chapter)
    -- end
    return next_data,show_data
end

--获取章节显示配置
function MaterialData:GetChapterShowData(chapter)
    for k,v in pairs(self.config.chapter_show) do
        if v.seq == chapter then
            return v
        end
    end
end

function MaterialData:GetOldInfoFlag(index)
    if self.old_info and self.old_info.flag_list then
        return self.old_info.flag_list[index] or 0
    end
    return 0
end