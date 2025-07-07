StarTreasureMapData = StarTreasureMapData or BaseClass()

function StarTreasureMapData:__init()
    if StarTreasureMapData.Instance ~= nil then
        Debuger.LogError("[StarTreasureMapData] attempt to create singleton twice!")
        return
    end
    StarTreasureMapData.Instance = self
    self:InitStarTreasureMapData()
    self:InitShengQiTreasureMapData()

    self.auto_useing = false    -- 是否在自动使用星辰宝图
end

function StarTreasureMapData:InitStarTreasureMapData()
    self.reward_config = {}
    for k,v in pairs(Config.treasure_map_auto.content) do
        if v.map_type == 3 then
            table.insert(self.reward_config,v)
        end
    end
	self.scene_effect = {}
end
function StarTreasureMapData:InitShengQiTreasureMapData()
    self.reward_config2 = {}
    for k,v in pairs(Config.treasure_map_auto.content) do
        if v.map_type == 4 then
            table.insert(self.reward_config2,v)
        end
    end
	-- self.scene_effect = {}
end

function StarTreasureMapData:GetRewardItem(data)
    for k,v in pairs(self:GetConfigData()) do
        if v.index == data.index then
            return {index = v.index,reward = v.item,is_notice = v.is_notice}
        end
    end
end

function StarTreasureMapData:GetReward2Item(data)
    for k,v in pairs(self:GetConfig2Data()) do
        if v.index == data.index then
            return {index = v.index,reward = v.item,is_notice = v.is_notice}
        end
    end
end

function StarTreasureMapData:AddEffect(client_uid,obj)
    if self.scene_effect[client_uid] and self.scene_effect[client_uid].obj then
        SE:Release(self.scene_effect[client_uid].obj)
    end
    self.scene_effect[client_uid] = obj
end

function StarTreasureMapData:DeleteEffect(client_uid)
    if self.scene_effect[client_uid] and self.scene_effect[client_uid].obj then
        SE:Release(self.scene_effect[client_uid].obj)
    end
end

function StarTreasureMapData:GetConfigData()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.reward_config) do
        if role_level >= v.min_level and role_level <= v.max_level then
            table.insert(list,v)
        end
    end
    table.sort(list,function(a,b)
        return a.index < b.index
    end)
    return list
end

function StarTreasureMapData:GetConfig2Data()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.reward_config2) do
        if role_level >= v.min_level and role_level <= v.max_level then
            table.insert(list,v)
        end
    end
    table.sort(list,function(a,b)
        return a.index < b.index
    end)
    return list
end

function StarTreasureMapData:NextStarTreasureMap()
    local list = BagData.Instance:GetGridByBigType(ITEM_BIG_TYPE.TREASURE_MAP)
    local next_item = nil
    if next(list) ~= nil then
        for _, value in ipairs(list) do
            if value:IsStarTreasureMap() then
                next_item = value
                break
            end
        end
    end
    if next_item then
        next_item.key_use_func = function()
            if RoleLevel() < FunOpen.GetConfog().star_treasure_map then
                PublicPopupCtrl.Instance:Center(Format(Language.StarTreasureMap.LimitLevel,FunOpen.GetConfog().star_treasure_map))
                return
            end
            TreasureMapData.Instance:SetMapInfo(next_item)
            ViewMgr:OpenView(next_item:IsActiveMap() and StarTreasureMapView or StarTreasureMapAtlaView)
            ViewMgr:CloseView(ItemInfoView)
            ViewMgr:CloseView(BagView) 
        end
        MainOtherCtrl.Instance:OpenKeyUseView(next_item)
        Invoke(function ()
            local is_auto = self:GetAutoFlag()
            local item = next_item
            if is_auto and item and item.item_id > 0 and ViewMgr:IsOpen(KeyUseView) then
                if item.key_use_func then
                    item.key_use_func()
                end
                KeyUseData.Instance:RemoveItem()
            else
                self:SetAutoFlag(false)
            end
        end,5)
    end
end
function StarTreasureMapData:NextShengQiTreasureMap()
    local list = BagData.Instance:GetGridByBigType(ITEM_BIG_TYPE.TREASURE_MAP)
    local next_item = nil
    if next(list) ~= nil then
        for _, value in ipairs(list) do
            if value.IsShengQiMap and value:IsShengQiMap() then
                next_item = value
                break
            end
        end
    end
    if next_item then
        next_item.key_use_func = function()
            if not ShengQiData.Instance:IsOpen() then
                PublicPopupCtrl.Instance:Center(Language.StarTreasureMap.ShengQiLimit)
                return
            end
            TreasureMapData.Instance:SetMapInfo(next_item)
            ViewMgr:OpenView(ShengQiTreasureMap)
            ViewMgr:CloseView(ItemInfoView)
            ViewMgr:CloseView(BagView)
        end
        MainOtherCtrl.Instance:OpenKeyUseView(next_item)
    end
end

function StarTreasureMapData:SetItem(item)
    self.exchange_item = item
end
function StarTreasureMapData:GetExchangeMaterial()
    return self.exchange_item
end


function StarTreasureMapData:SetAutoFlag(value)
    self.auto_useing = value
end
function StarTreasureMapData:GetAutoFlag()
    return self.auto_useing
end