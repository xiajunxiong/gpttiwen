PrestigeData = PrestigeData or BaseClass()

function PrestigeData:__init()
    if PrestigeData.Instance ~= nil then
        Debuger.LogError("[PrestigeData] attempt to create singleton twice!")
        return
    end
    PrestigeData.Instance = self
    self.village_id = 1
    self.village_data = SmartData.New({village_id = 1})
    self.donate_notice = SmartData.New{donate_count = 0,add_value = 0}
    --声望任务
    self.prestige_task_table = SmartData.New({village_id = 1}) --界面信息
    self.prestige_task_data = SmartData.New({village_id = 0, today_finish_task_num = 0, today_task_num = 0, today_task_list = {}})
    self.rand_list_info = SmartData.New() --商店随机列表信息

    self:Initialization()
    self:InitPageData()
    self:InitDonateData()
    self:InitDonateList()
    self:InitPrestigeList()
end

function PrestigeData:OnSCShopRandListInfo(protocol)
    self.rand_list_info.info=protocol.info
end

function PrestigeData:OnSCShopRandListSingleInfo(protocol)
    self.rand_list_info.info.rand_item_list[protocol.shop_list_seq+1]=protocol.rand_item
    self.rand_list_info:SetDirty("info")
end

function PrestigeData:InitDonateData()
    self.donation_data = SmartData.New{level = -1,color = -1,id = 1}
    self.donation_point = SmartData.New{val = false}
end

function PrestigeData:__delete()
    PrestigeData.Instance = nil
end

--在RoleCtrl:OnUnloadGameLogic()调用
function PrestigeData:ClearData()
    TimeHelper:CancelTimer(self.prestige_task_timer)
    self.prestige_task_timer = nil
    self.last_prestige_finish_task_num = nil
end

function PrestigeData:SetDonateInfo(protocol)
    self.donate_notice.donate_count = protocol.donate_count
    self.donate_notice.add_value = protocol.add_value
end

function PrestigeData:SetAllVillagePrestige(data)
    self.village_prestige_info = SmartData.New(data)
end

function PrestigeData:SetSingleVillagePrestige(index, data)
    self.village_prestige_info[index] = data
end

function PrestigeData.GetUpgradeData(village_id)
    local list = {}
    for k, v in pairs(Config.prestige_cfg_auto.upgrade) do
        if v.village_id == village_id and v.desc ~= "" then
            table.insert(list, v)
        end
    end
    table.sort(list,function(a,b)
        return a.relation < b.relation
    end)
    return list
end

function PrestigeData.GetUpgradeConfig(village_id)
    local list = {}
    for k, v in pairs(Config.prestige_cfg_auto.upgrade) do
        if v.village_id == village_id then
            table.insert(list, v)
        end
    end
    table.sort(list,function(a,b)
        return a.relation < b.relation
    end)
    return list
end

-- 获取当天还能捐赠多少额装备
function PrestigeData:GetTodayDonateCount()
    -- 上限改为读升阶配置的
    --local max = Config.prestige_cfg_auto.other[1].donate_max_num
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local cur_config = RolePromoteData.GetProfConfig(prof_id,prof_grade) or {}
    local max = cur_config.juanzeng_up or 0
    return max - (self.donate_notice.donate_count or 0)
end

--随机物品购买次数
function PrestigeData:GetRandItemBuyTimes(index)
    local times = 0
    if self.rand_list_info.info then
        times= self.rand_list_info.info.rand_item_list[index+1].shop_item_buy_times
    end
    return times or 0
end


function PrestigeData:GetRandUpdateTime()
    if self.rand_list_info.info then
        return self.rand_list_info.info.next_refresh_time
    end
    return 0
end


--======================================声望捐赠新修改=============================================
function PrestigeData:SetDonationLevel(level)
    self.donation_data.level = level
end

function PrestigeData:SetDonationColor(color)
    self.donation_data.color = color
end

function PrestigeData:SetDonationName(name)
    self.donation_data.name = name
end

function PrestigeData:SetDonationId(village_id)
    self.donation_data.id = village_id
end

function PrestigeData:GetDonationData()
    return self.donation_data
end

function PrestigeData:GetDonationId()
    return self.donation_data.id
end

function PrestigeData:GetColorListData()
    local list = {}
    for i,v in ipairs(Language.Reputation.SelectColor) do
        list[i] = {color = i + 1,name = ColorStr(v,QualityColorStr[i + 1])}
    end
    table.insert(list,{color = 0,name = Language.Reputation.SelectNothing})
    return list
end

function PrestigeData:GetLevelListData()
    local list = {}
    local village_data = self:GetVillageList()
    for i=village_data.donate_min_level,village_data.donate_max_level do
        local show_level = i ~= 1 and (i- 1) * 10 or i
        table.insert(list,{level = i,name = show_level .. Language.Common.Level})
    end
    table.insert(list,{level = 0,name = Language.Reputation.SelectNothing})
    return list
end

--获取捐赠装备
function PrestigeData:GetEquipList()
    if self.donate_cache_num == nil then
        self.donate_cache_num = {}
    end
    local equip_list = {}
    local equip_data = BagData.Instance:GetGridByType({ItemType.Weapon})
    for k, v in pairs(BagData.Instance:GetGridByType({ItemType.Armor})) do
        table.insert(equip_data,v)
    end
    for k, v in pairs(BagData.Instance:GetGridByType({ItemType.Jewelry})) do
        table.insert(equip_data,v)
    end
    local village_data = self:GetVillageList()
    local level,color = self.donation_data.level,self.donation_data.color
    for k,v in pairs(equip_data) do
        if self:GetEquipColor(v.item_id) < ItemColor.Star then
            v.color = v.color or self:GetEquipColor(v.item_id)
            v.level = v.level or self:GetEquipLevel(v.item_id)
            if v.level >= village_data.donate_min_level and v.level <= village_data.donate_max_level then
                if self.donate_cache_num[v.index] == nil then
                    self.donate_cache_num[v.index] = self:GetDonateNum(v.color,v.level)
                end
                table.insert(equip_list,v)
            end
        end
    end
    if #equip_list > 1 then
        table.sort(equip_list,function(a,b)
            if a.color == b.color then
                return a.level < b.level
            end
            return a.color < b.color 
        end)
    end
    for i=#equip_list + 1,self:GetEquipListNum(#equip_list) do
        table.insert(equip_list,{item_id = 0})
    end
    return equip_list
end

function PrestigeData:GetEquipListNum(num)
    local list_num = num <= 40 and 40 or math.ceil(num / 10) * 10
    local max_num = Config.prestige_cfg_auto.other[1].donate_max_num
    return list_num >= max_num and max_num or list_num
end

function PrestigeData:GetEquipColor(item_id)
    local config = Config.equipment_auto[item_id] or Config.ornament_item_auto[item_id]
    return config and config.color or 0
end

function PrestigeData:GetEquipLevel(item_id)
    local config = Config.equipment_auto[item_id] or Config.ornament_item_auto[item_id]
    return config and config.equip_level or 0
end

function PrestigeData:InitDonateList()
    self.village_donate_list = {}
    for k,v in pairs(Config.prestige_cfg_auto.donate) do
        if self.village_donate_list[v.village_id] == nil then
            self.village_donate_list[v.village_id] = {}
        end
        table.insert(self.village_donate_list[v.village_id],v)
    end
    self.village_list_config = {}
    for k,v in pairs(Config.prestige_cfg_auto.village_list) do
        if v.is_show == 1 then
            table.insert(self.village_list_config,v)
        end
    end
    table.sort(self.village_list_config,function(a,b)
        return a.index < b.index
    end)
end

function PrestigeData:GetDonateNum(color,level,village_id)
    for k,v in pairs(self.village_donate_list[village_id or self.village_id]) do
        if v.color == color and v.level == level then
            return v.reward_prestige
        end
    end
    return 0
end

function PrestigeData:GetEquipCacheNum(index)
    return self.donate_cache_num[index] or 0
end

function PrestigeData:GetVillageListConfig()
    return self.village_list_config
end

function PrestigeData:CheckDonateCacheNum(index)
    if self.donate_cache_num == nil then
        return
    end
    if index ~= nil then
        self.donate_cache_num[index] = nil
    else
        self.donate_cache_num = nil
    end
end
---===============================================================================================

--服务端数据下发
function PrestigeData:SetAllShopInfo(data)
    self.shop_info = SmartData.New(data.buy_count_list)
end

function PrestigeData:SetShopInfo(data)
    if self.shop_info then
        self.shop_info[data.index] = data.buy_count
    end
end

function PrestigeData:GetShopBuyCount(item_index)
    return self.shop_info[item_index] or 0
end

function PrestigeData:GetShopData()
    return self.shop_info
end

function PrestigeData:SetAllCommonShopInfo(data)
    self.common_shop_info = SmartData.New(data.buy_count_list)
end

function PrestigeData:SetCommonShopInfo(data)
    if self.common_shop_info then
        self.common_shop_info[data.index] = data.buy_count
    end
end

function PrestigeData:GetCommonShopBuyCount(item_index)
    return self.common_shop_info[item_index] or 0
end

function PrestigeData:GetCommonShopData()
    return self.common_shop_info
end

function PrestigeData:SetVillageID(village_id)
    self.village_data.village_id = village_id
    self.village_id = village_id
end

function PrestigeData:GetVillageID()
    return self.village_data.village_id or 1
end

-- 获得某个村子的声望数据 prestige 声望值, level 当前村子声望等级
function PrestigeData:GetVillagePrestigeData(village_id)
    local info = self.village_prestige_info[village_id or self.village_id] or {}
    return info,self:GetPrestigeUpgradeData(info.level,village_id)
end

function PrestigeData:GetVillagePrestigeNextData(village_id)
    local info = self.village_prestige_info[village_id or self.village_id] or {}
    local level = info.level + 1
     local next_data = self:GetPrestigeUpgradeData(level,village_id)
     if next_data == nil then
        level = info.level
        next_data = self:GetPrestigeUpgradeData(level,village_id)
        info.prestige = next_data.need_prestige
     end
    return info,next_data
end

-------------------------------------声望商店-----------------------------------------------
function PrestigeData:InitPageData()
    self.prestige_all_list = {}
    for k, v in pairs(Config.prestige_cfg_auto.village_list) do
        if self.prestige_all_list[v.village_id] == nil then
            self.prestige_all_list[v.village_id] = {}
        end
        for _, btn in pairs(Config.prestige_cfg_auto.type_list) do
            self.prestige_all_list[v.village_id][btn.page] = self:GetPageTypeList(btn.show_type, v.village_id)
        end
    end
    self.general_store_list = {}
    for k, v in pairs(Config.shop_cfg_auto.shop) do
        if self.general_store_list[v.shop_id] == nil then
            self.general_store_list[v.shop_id] = {}
        end
        table.insert(self.general_store_list[v.shop_id], v)
    end

    for i = 1, #self.general_store_list do
        table.sort(self.general_store_list[i], function(a, b)
            return a.seq < b.seq
        end)
    end
end

--随机商店列表
function PrestigeData:GetPageRandData()
    local list = {}
    local shop_id=PrestigeData.GeneralStore
    if self.rand_list_info.info then
        for k, shop_cfgs in pairs(self:GetLimitNpcShopCfg()) do
            if shop_id == k then
                for _, v in pairs(shop_cfgs) do
                    local shop_pool = v.shop_pool
                    local seq = v.seq
                    local shop_info = self.rand_list_info.info.rand_item_list[seq + 1]
                    if shop_info ~= nil then
                        local item_seq = shop_info.shop_item_seq
                        local cfg = self:GetLimitShopPoolItem(shop_pool, item_seq)
                        if cfg ~= nil then
                            cfg.rand_index=seq
                            table.insert(list, cfg)
                        end
                    end
                end
                break
            end
        end
    end
    return list
end

function PrestigeData:GetLimitNpcShopCfg()
    if self.limit_npc_shop_cfg == nil then
        self.limit_npc_shop_cfg = {}
        for _, v in pairs(Config.shop_cfg_auto.limit_npc_shop) do
            local shop_id = v.shop_id
            if not self.limit_npc_shop_cfg[shop_id] then
                self.limit_npc_shop_cfg[shop_id] = {}
            end
            table.insert(self.limit_npc_shop_cfg[shop_id], v)
        end
    end
    return self.limit_npc_shop_cfg
end

--限购npc商店物品列表
function PrestigeData:GetLimitNpcShopItem(shop_id)
    return self:GetLimitNpcShopCfg()[shop_id] or {}
end

--限购商品池商品
function PrestigeData:GetLimitShopPoolItem(pool, item_seq)
    if self.limit_pool_shop_cfg == nil then
        self.limit_pool_shop_cfg = {}
        for _, v in pairs(Config.shop_cfg_auto.limit_shop_pool) do
            local shop_pool = v.shop_pool
            local item_seq = v.item_seq
            if not self.limit_pool_shop_cfg[shop_pool] then
                self.limit_pool_shop_cfg[shop_pool] = {}
            end
            self.limit_pool_shop_cfg[shop_pool][item_seq]=v
        end
    end
    return self.limit_pool_shop_cfg[pool][item_seq]
end

--初始化数据
function PrestigeData:Initialization()
	self.select_data = SmartData.New({list = {},type = 1,index = 1})
end

--设置选中的数据
function PrestigeData:SetSelectData(data)
	self.select_data.list = data
    ReputationCtrl.Instance:SetSelectIndex(data.item_id,self.village_id)
end

--获取选中的数据
function PrestigeData:GetSelectData()
	return self.select_data.list
end

--选择的当前页
function PrestigeData:SetSelectType(type)
	self.select_data.type = type
end

function PrestigeData:SetSelctItemIndex(index)
	self.select_data.index = index or 1
end

function PrestigeData:GetSelctItemIndex()
	return self.select_data.index or 1
end

--获取当前页
function PrestigeData:GetSelectType()
    return self.select_data.type
end

--获取限制次数
function PrestigeData:GetLimitTimes(index)
    local data = self:GetSelectData()
    if PrestigeData.GeneralStore then
        return self.common_shop_info[index or data.index] or 0
    end
    return self.shop_info[index or data.index] or 0
end

--根据村子id活动声望等级
function PrestigeData:GetPrestigeLevel(village_id)
    local data = self:GetVillagePrestigeData(village_id or self.village_id)
    return data and data.level or 0
end

--根据村子等级获取对应的升级配置
function PrestigeData:GetPrestigeUpgradeData(relation,village_id)
    village_id = village_id or self.village_id
    for k,v in pairs(Config.prestige_cfg_auto.upgrade) do
        if v.village_id == village_id and v.relation == relation then
            return v
        end
    end
end

--获取商店名字
function PrestigeData:GetTitleName()
    if PrestigeData.GeneralStore then
        return Language.Reputation.GeneralStore[PrestigeData.GeneralStore]
    end
    return Language.Reputation.Shop
end

--获取按钮列表
function PrestigeData:GetTitleBtnList()
    if PrestigeData.GeneralStore then
        local GeneralStore = Language.Reputation.GeneralStore
        return {[1] = {name = GeneralStore[PrestigeData.GeneralStore] or GeneralStore[1],page = 1}}
    end
    return Config.prestige_cfg_auto.type_list
end

function PrestigeData:GetPageTypeList(show_type,village_id)
    local list = {}
    for k,v in pairs(Config.prestige_cfg_auto.shop) do
        if village_id == v.village_id then
            if show_type == 0 then
                table.insert(list,v)
            elseif show_type == -1 then
                if self:IsPageType(Item.GetShowType(v.item_id)) then
                    table.insert(list,v)
                end
            else
                if Item.GetShowType(v.item_id) == show_type then
                    table.insert(list,v)
                end
            end
        end
    end
    table.sort(list,function(a,b) return a.seq < b.seq end)
    return list
end

function PrestigeData:IsPageType(item_type)
    for k,v in pairs(Config.prestige_cfg_auto.type_list) do
        if v.show_type > 0 and v.show_type == item_type then
            return false
        end
    end
    return true
end

--获取当前页列表
function PrestigeData:GetPageVillageData(type,village_id)
    if PrestigeData.GeneralStore then
        return self.general_store_list[PrestigeData.GeneralStore]
    end
    return self.prestige_all_list[village_id or self.village_id][type]
end

--获取排序好的列表
function PrestigeData:GetPageVillageSortList(type, village_id)
    local rand_list = self:GetPageRandData()
    local list = self:GetPageVillageData(type, village_id)
    local page_list = {}
    for k, v in pairs(rand_list) do
        v.has_times = -1
        v.weight = -1
        table.insert(page_list, v)
    end
    for k, v in pairs(list) do
        v.has_times = v.limit_times > 0 and v.limit_times - self:GetLimitTimes(v.index) or -1
        v.weight = v.has_times == 0 and v.seq + 10000 or v.seq
        table.insert(page_list, v)
    end
    return self:CheckSilkRoad(page_list, type)
end

function PrestigeData:GetVillageList(village_id)
    village_id = village_id or self.village_id
    for k,v in pairs(Config.prestige_cfg_auto.village_list) do
        if v.village_id == village_id then
            return v
        end
    end
end

--通过标签类型和物品id找到item的位置
function PrestigeData:GetItemIndexById(item_id)
	for k,v in pairs(self:GetPageVillageSortList(1)) do
		if v.item_id == item_id then
			return k
		end
	end
end

--根据village_id 找到主城对应的声望村子npc_id
function PrestigeData:GetNpcIdByVillageId(village_id)
    village_id = village_id or self.village_id
    for k,v in pairs(Config.prestige_cfg_auto.village_list) do
        if v.village_id == village_id then
            return v.npc_id
        end
    end
end

function PrestigeData:IsShopProfItem(item_id)
    local config = WorkshopData.Instance:GetActivationConfig(item_id)
    if config then
        return math.floor(Equip.GetLimitProf(config.target) / 100) == RoleData.Instance:GetRoleProf()
    end
end

function PrestigeData:GetVillageByNpcId(npc_id)
    for k,v in pairs(Config.prestige_cfg_auto.village_list) do
        if v.npc_id == npc_id then
            return v
        end
    end
end

function PrestigeData:GetVillageName(village_id,relation)
    local village_data = self:GetVillageList(village_id)
    if relation then
        local config = self:GetPrestigeUpgradeData(relation,village_id)
        return village_data.name,config.name
    end
    return village_data.name
end

function PrestigeData:SetPopupParam(param_t)
    self.popup_param = param_t
end

function PrestigeData:GetPopupParam(param_t)
    return self.popup_param
end

function PrestigeData.IsPopupParam(old_data,new_data)
    return PrestigeData.IsPopupNotice == true or old_data.level ~= new_data.level 
end

function PrestigeData:SetPopupParamValue(value)
    self.popup_param_value = value
end

function PrestigeData:GetPopupParamValue()
    return self.popup_param_value or 0
end

--道具置顶最大值weight
PRESTIGE_TOP_WEIGHT = -20000
--门派任务道具weight
PRESTIGE_BOUNTY_WEIGHT = -11000
--声望任务道具weight
PRESTIGE_PRESTIGE_TASK_WEIGHT = -10000
--配方本职业道具weight
PRESTIGE_PROF_ITEM_WEIGHT = 1000

--远洋贸易订单
function PrestigeData:CheckSilkRoad(list,type)
    if type == 3 then
        table.sort(list,DataHelper.WeightSort("weight",function(data)
            if data.relation and (PrestigeData.Instance:GetPrestigeLevel() >= data.relation) then
                if not self:IsShopProfItem(data.item_id) then
                    return PRESTIGE_PROF_ITEM_WEIGHT
                end
            else
                if self:IsShopProfItem(data.item_id) then
                    return PRESTIGE_PROF_ITEM_WEIGHT * 2
                end
                return PRESTIGE_PROF_ITEM_WEIGHT * 3
            end
        end))
    else
        table.sort(list,DataHelper.WeightSort("weight",function(data)
            if SilkRoadData.Instance:IsBountySubmission(data.item_id) then
                return ReputationCtrl.Instance:GetSelectTaskType() == TaskConfig.TaskType.bounty and PRESTIGE_TOP_WEIGHT or PRESTIGE_BOUNTY_WEIGHT
            end
            if self:IsPrestigeTaskSubmission(data.item_id) then
                return ReputationCtrl.Instance:GetSelectTaskType() == TaskConfig.TaskType.prestige and PRESTIGE_TOP_WEIGHT or PRESTIGE_PRESTIGE_TASK_WEIGHT
            end
            if data.relation and (PrestigeData.Instance:GetPrestigeLevel() >= data.relation) then
                if Item.GetShowType(data.item_id) == 17 then
                    if self:IsShopProfItem(data.item_id) then
                        return PRESTIGE_PROF_ITEM_WEIGHT
                    end
                    return PRESTIGE_PROF_ITEM_WEIGHT * 2
                end
            else
                if Item.GetShowType(data.item_id) == 17 then
                    if self:IsShopProfItem(data.item_id) then
                        return PRESTIGE_PROF_ITEM_WEIGHT * 4
                    end
                    return PRESTIGE_PROF_ITEM_WEIGHT * 5
                end
                return PRESTIGE_PROF_ITEM_WEIGHT * 3
            end
        end))
    end
    return list
end

-------------------------------------声望任务-----------------------------------------------
function PrestigeData:InitPrestigeList()
    self.prestige_task_village_list = {}
    for i, v in pairs(Config.prestige_task_auto.task) do
        if v.is_show == 1 then
            table.insert(self.prestige_task_village_list, v)
        end
    end
    table.sort(self.prestige_task_village_list, DataHelper.SortFunc("index"))
end

function PrestigeData:GetPrestigeTaskVillageList()
    return self.prestige_task_village_list
end

--界面选择声望村庄
function PrestigeData:GetPrestigeTaskVillageId()
    return self.prestige_task_table.village_id
end
function PrestigeData:SetPrestigeTaskVillageId(village_id)
    self.prestige_task_table.village_id = village_id
end

function PrestigeData:GetPrestigeTaskVillageCfg(village_id)
    for i, v in pairs(self.prestige_task_village_list) do
        if v.village_id == village_id then
            return v 
        end
    end
end

function PrestigeData:SetPrestigeTaskInfo(protocol)
    self.prestige_task_data.village_id = protocol.village_id
    self.prestige_task_data.today_finish_task_num = protocol.today_finish_task_num
    self.prestige_task_data.today_task_num = protocol.today_task_num
    self.prestige_task_data.today_task_list = protocol.today_task_list
    self.prestige_task_data.is_accept_task = protocol.is_accept_task

    self:UpdatePrestigeTask()
    TimeHelper:CancelTimer(self.prestige_task_timer)
    self.prestige_task_timer = Invoke(BindTool.Bind1(self.CheckPrestigeTaskShow, self), 1)
end

-- 获得选择的村庄id，如果是0就没选
function PrestigeData:GetSelectVillageId()
    return  self.prestige_task_data.village_id or 0
end

function PrestigeData:CheckPrestigeTaskShow()
    if self.last_prestige_finish_task_num == nil or self.prestige_task_data.is_accept_task ~= 1 or self:IsPrestigeTaskFinish() then
        self.last_prestige_finish_task_num = self.prestige_task_data.today_finish_task_num
        return
    end
    if self.last_prestige_finish_task_num == 0 or self.last_prestige_finish_task_num < self.prestige_task_data.today_finish_task_num then
        local finish_num = self.prestige_task_data.today_finish_task_num
        if finish_num ~= 0 then
            --上个任务的星级不为0，即为当前任务是新任务
            local task_data = self.prestige_task_data.today_task_list[finish_num]
            if task_data == nil or task_data.star == 0 then
                self.last_prestige_finish_task_num = self.prestige_task_data.today_finish_task_num
                return
            end
        end
        for i = finish_num + 1, #self.prestige_task_data.today_task_list do
            local task_data = self.prestige_task_data.today_task_list[i]
            if task_data.star ~= 0 then
                local task_cfg = self:PrestigeTaskCfg(task_data.task_id)
                if task_cfg.is_activedegree == 1 then
                    local data = {}
                    data.task_id = task_cfg.task_id
                    data.task_name = task_cfg.task_name
                    data.task_index = i
                    ViewMgr:OpenView(TaskPrestigeStarSamllView, {task_data = data})
                end
                break
            end
        end
    end
    self.last_prestige_finish_task_num = self.prestige_task_data.today_finish_task_num
end

function PrestigeData:GetPrestigeTaskData()
    return self.prestige_task_data
end

--声望任务下阶段任务描述
function PrestigeData:GetPrestigeTaskNextPhaseDesc(village_id)
    local info = self.village_prestige_info[village_id or self.village_id] or {}
    local level = info.level
    local next_data = self:GetPrestigeUpgradeData(level,village_id)
    return next_data ~= nil and next_data.task_desc or ""
end

function PrestigeData:PrestigeTaskOtherCfg()
    return Config.prestige_task_auto.other[1]
end

function PrestigeData:PrestigeTaskCfg(task_id)
    if self.prestige_task_cfg_auto == nil then
        self.prestige_task_cfg_auto = {}
        for i, v in pairs(Config.prestige_task_auto.task_line) do
            self.prestige_task_cfg_auto[v.task_id] = v
        end
    end
    return self.prestige_task_cfg_auto[task_id]
end

function PrestigeData:GetPrestigeTaskProg()
    local max_val = 0
    local cur_val = 0
    for i, v in ipairs(self.prestige_task_data.today_task_list) do
        local task_cfg = self:PrestigeTaskCfg(v.task_id)
        if task_cfg ~= nil then
            max_val = max_val + task_cfg.is_activedegree
            if i <= self.prestige_task_data.today_finish_task_num then
                cur_val = cur_val + task_cfg.is_activedegree 
            end
        end
    end
    return cur_val, max_val
end

--声望任务数量
function PrestigeData:GetPrestigeTaskNum()
    return 5
end

function PrestigeData:UpdatePrestigeTask()
    if self.prestige_task_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.prestige_task_btn_handle)
        self.prestige_task_btn_handle = nil
    end
    local data = ActiveData.Instance:GetActiveCfg(ActType.Prestige)
    if data and RoleData.Instance:CreatedDay() < data.open_day then
        return
    end
    if not self:IsPrestigeTaskFinish() and (self.prestige_task_data.village_id == 0 or self.prestige_task_data.is_accept_task == 0) then --未接受任务
        local task = SmartData.New(TaskInfoPrestigeStart.New():TaskInfo())
        local taskvo = CommonStruct.CustomTask()
        taskvo.task_id = CUSTOM_OBJID.PRESTIGE_TASK_START
        taskvo.task_name = Language.Task.TaskName.Prestige
        taskvo.task_desc = Language.Task.TaskGoal.PrestigeStart
        taskvo.task_goal = Language.Task.TaskGoal.PrestigeStart
        taskvo.npc_id = VoMgr:NpcVoBySeq(self:PrestigeTaskOtherCfg().task_npcseq).id
        task:TaskVo(taskvo)
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.PRESTIGE_TASK_START, task)

        self.prestige_task_btn_handle = BehaviorData.Instance:CustomNpcTalkBtnsId(taskvo.npc_id, BindTool.Bind1(self.CustomPrestigeTaskNpcTalkBtn, self))
    else
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.PRESTIGE_TASK_START)
    end
    ActivityData.Instance:CustomClickHandle(ActType.Prestige, function()
        TaskData.Instance:StopTaskState(TaskConfig.TaskType.prestige, CUSTOM_OBJID.PRESTIGE_TASK_START)
        if TaskData.Instance.task_list[CUSTOM_OBJID.PRESTIGE_TASK_START] ~= nil then
            TaskData.Instance.task_list[CUSTOM_OBJID.PRESTIGE_TASK_START]:GoTask()
            return
        end
        local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.prestige)
        if task_info ~= nil then
            task_info:GoTask()
        end
    end)
end

--是否完成所有声望任务 bool
function PrestigeData:IsPrestigeTaskFinish()
    if self.prestige_task_data.village_id ~= 0 and self.prestige_task_data.today_task_num > 0 then
        return self.prestige_task_data.today_finish_task_num >= self.prestige_task_data.today_task_num
    end
    return false
end

function PrestigeData:CustomPrestigeTaskNpcTalkBtn(btn_list, obj_vo)
    local button = NpcDialogBehBtnData.FuncCreate(Language.Task.TaskName.Prestige)
    local function func_accept()
        if self.prestige_task_data.village_id == 0 then
            ViewMgr:OpenViewByKey(Mod.PrestigeTask.Main)
        else
            ViewMgr:OpenView(TaskPrestigeStarView)
        end
    end
    table.insert(btn_list, NpcDialogBehBtnData.FuncCreate(Language.Task.TaskName.Prestige, func_accept, true, nil))
end

function PrestigeData:GetPrestigeTaskAutoAwit()
    return 3
end

--是否声望任务道具商店标识提醒
function PrestigeData:IsPrestigeTaskSubmission(item_id)
    local next_task = self.prestige_task_data.today_finish_task_num + 1
    local task_info = self.prestige_task_data.today_task_list[next_task]
    if task_info ~= nil then
        local cur_task = TaskData.Instance:GetTaskInfoByTaskId(task_info.task_id)
        if cur_task ~= nil and cur_task.info.condition == 14 and cur_task.info.c_param1 == item_id then
            return Item.GetNum(item_id) < cur_task.info.c_param2
        end
    end
    return false
end

function PrestigeData:GetPrestigeTaskCurVillageCfg()
    return self:GetPrestigeTaskVillageCfg(self.prestige_task_data.village_id)
end

-----声望任务星级
function PrestigeData:GetPrestigeTaskFlushItemId()
    return self:PrestigeTaskOtherCfg().refresh_item
end

function PrestigeData:GetPrestigeTaskStarCfg(star_level)
    return Config.prestige_task_auto.star_reward[star_level]
end

function PrestigeData:GetPrestigeTaskList()
    local task_list = {}
    for i, v in ipairs(self.prestige_task_data.today_task_list) do
        local task_cfg = self:PrestigeTaskCfg(v.task_id)
        if task_cfg ~= nil and task_cfg.is_activedegree == 1 and #task_list < self:GetPrestigeTaskNum() then
            local data = {}
            data.task_id = task_cfg.task_id
            data.task_name = task_cfg.task_name
            data.task_index = i
            table.insert(task_list, data)
        end
    end
    return task_list
end

function PrestigeData:GetPrestigeTaskStar(task_index)
    local task_info = self.prestige_task_data.today_task_list[task_index]
    return task_info ~= nil and task_info.star or 1
end

function PrestigeData:GetPrestigeTaskStarByTaskId(task_id)
    local is_pass = false
    for i, v in ipairs(self.prestige_task_data.today_task_list) do
        if v.task_id == task_id then
            is_pass = true
        end
        if is_pass and v.star > 0 then
            return v.star
        end
    end
    return 1
end

function PrestigeData:GetPrestigeTaskTotalRewards()
    local show_list = {}
    local function func_push(item)
        for i, v in ipairs(show_list) do
            if v.item_id == item.item_id and v.is_bind == item.is_bind then
                v.num = v.num + item.num
                return
            end
        end
        table.insert(show_list, TableCopy(item))
    end
    --村庄奖励
    local village_cfg = self:GetPrestigeTaskCurVillageCfg()
    for k = 0, #village_cfg.item_list do
        if village_cfg.item_list[k] then
            func_push(village_cfg.item_list[k])
        end
    end
    --星级奖励
    for i, v in ipairs(self.prestige_task_data.today_task_list) do
        if v.star > 0 then
            local star_cfg = self:GetPrestigeTaskStarCfg(v.star)
            for k = 0, #star_cfg.item_list do
                if star_cfg.item_list[k] then
                    func_push(star_cfg.item_list[k])
                end
            end
        end
    end
    return show_list
end

