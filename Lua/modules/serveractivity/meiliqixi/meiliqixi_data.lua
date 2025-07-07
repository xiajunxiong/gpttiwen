ActMeiLiQiXiData = ActMeiLiQiXiData or BaseClass()
ActMeiLiQiXiData.RANK_TYPE = 38 --魅力榜id
function ActMeiLiQiXiData:__init()
    if ActMeiLiQiXiData.Instance ~= nil then
        Debuger.LogError("[ActMeiLiQiXiData] attempt to create singleton twice!")
        return
    end
    ActMeiLiQiXiData.Instance = self
    self.meiliqixi_smart = SmartData.New()
    self.rank_data = SmartData.New()
    ActMeiLiQiXiData.GiftIcon = { "_LocLiBao-Cheng","_LocLiBao-Hong","_LocLiBao-Lan","_LocLiBao-Zi"}
end

function ActMeiLiQiXiData:SetMeiLiQiXiInfo(protocol)
    self.meiliqixi_smart:Set(protocol.info)
end

function ActMeiLiQiXiData:SetMeiLiBangRankList(protocol)
    self.rank_data:Set(protocol.info)
    -- LogError("self.rank_data",self.rank_data)
end

function ActMeiLiQiXiData:GetAiMuZhiItemList()
    local basis_configuration = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MEILIQIXI, Config.meiliqixi_auto.basis_configuration)

    local aimuzhi_item_list = {}

    for _, basis_configuration_value in pairs(basis_configuration) do
        if basis_configuration_value.reward_type == 0 then
            table.insert(aimuzhi_item_list,basis_configuration_value)
        end

    end

    return aimuzhi_item_list 
end
function ActMeiLiQiXiData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_MEILIQIXI) then
        return 0
    end
    if self:AiMuZhiRemind() == 1 then
        return 1
    end
    if self:MeiLiZhiRemind() == 1 then
        return 1
    end
    return 0
end
function ActMeiLiQiXiData:AiMuZhiRemind()
    local list = self:GetAiMuZhiItemList()
    for i, v in pairs(list) do
        if self.meiliqixi_smart.admiration_map and self.meiliqixi_smart.admiration_map[v.seq] then
            if self.meiliqixi_smart.admiration_value >= v.add_intimate and self.meiliqixi_smart.admiration_map[v.seq] == 0 then
                return 1
            end
        end
    end
    return 0
end
function ActMeiLiQiXiData:MeiLiZhiRemind()
    local list = self:GetMeiLiZhiItemList()
    for i, v in pairs(list) do
        if self.meiliqixi_smart.charm_map and self.meiliqixi_smart.charm_map[v.seq] then
            if self.meiliqixi_smart.charm_value >= v.add_intimate and self.meiliqixi_smart.charm_map[v.seq] == 0 then
                return 1
            end
        end
    end
    return 0
end
function ActMeiLiQiXiData:GetMeiLiZhiItemList()
    local basis_configuration = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MEILIQIXI, Config.meiliqixi_auto.basis_configuration)
    local meilizhi_item_list = {}
    for _, basis_configuration_value in pairs(basis_configuration) do
        
        if basis_configuration_value.reward_type == 1 then
            table.insert(meilizhi_item_list,basis_configuration_value)
        end
    end
    return meilizhi_item_list
end


function ActMeiLiQiXiData:GetMeiLiBangItemList()
    local reward_configuration = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MEILIQIXI, Config.meiliqixi_auto.reward_configuration)
    local meilibang_item_list = {}

    for _, rank_infos_value in pairs(self.rank_data.rank_infos) do

        -- 配置有数据才有奖励，10<第11,没有奖励
        if #reward_configuration >= rank_infos_value.rank then
            rank_infos_value.seq = reward_configuration[rank_infos_value.rank].seq
            rank_infos_value.reward_item = reward_configuration[rank_infos_value.rank].reward_item
        end
        table.insert(meilibang_item_list,rank_infos_value)
    end

    return meilibang_item_list
end

function ActMeiLiQiXiData:GetDefaultMeiLiBangItemList()
    local reward_configuration = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MEILIQIXI, Config.meiliqixi_auto.reward_configuration)
    local default_list = {}
    for i = 1, 5 do
        local nobody_item = {
            rank = i,
            seq = i - 1,
            reward_item = reward_configuration[i].reward_item
        }
        table.insert(default_list ,nobody_item)
    end
    return default_list
end

function  ActMeiLiQiXiData:GetRewardItemList(reward_item)
    local reward_item_list = {}
    for i=0,#reward_item do
        table.insert(reward_item_list, Item.Create(reward_item[i]))
    end
    return reward_item_list
end































