--@TODO 2022-08-23 17:14:27
-- 原bicycle_data内容太多了，后期的写这里了

BicycleOtherData = BicycleOtherData or BaseClass()

function BicycleOtherData:__init()
    if BicycleOtherData.Instance ~= nil then
        Debuger.LogError("[BicycleOtherData] attempt to create singleton twice!")
        return
    end
    BicycleOtherData.Instance = self
    self.cfg = Config.bicycle_cfg_auto

    -- 选择的材料 A,B
    self.sel_stuff_data = SmartData.New()

     -- 灵骑装备
     Cfg.Register("GetSkillItemCfg",Cfg.RealTimeSearch2Key(self.cfg.skill_scroll,"skill_id","skill_level"))
end

-- 获取背包全部技能书
function BicycleOtherData:GetAllSkillBook()
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetType(v.item_id) == ItemType.BicycleSkillBook then
            table.insert(list,v)
        end
    end
    return list
end

-- 获取背包可以一键合成的技能书
function BicycleOtherData:GetBookCanAllUpgradeList()
    local list = {}
    local bag_list = self:GetAllSkillBook()
    for i,v in pairs(bag_list) do
        local cfg = v:Config()
        local skill_id = cfg.param1
        local level = cfg.param3
        local max_level = BicycleData.Instance:GetSkillMaxLevel(skill_id)
        if level < max_level then
            table.insert(list, v)
        end
    end
    table.sort(list, BindTool.Bind(self.SkillBookListSort, self))
    return list
end

-- 一键合成预览
function BicycleOtherData:GetBookPreViewAllUpgrade(item_list)
    if TableIsEmpty(item_list) then
        return {}
    end

    local _skill_id = item_list[1]:Config().param1
    local max_level = BicycleData.Instance:GetSkillMaxLevel(_skill_id)
    local data_list = {}
    local qua_group = {}
    -- 因为item是重叠的，先拆分
    for i,v in pairs(item_list) do
        local num = v:Num()
        local item_id = v.item_id
        local qua = v:Color()
        local cfg = v:Config()
        local level = cfg.param3
        for j = 1, num do
            local t = {}
            t.item_id = item_id
            t.qua = qua
            t.level = level
            t.is_uped = false   -- is_uped标记是否升级过
            t.num = 1
            table.insert(data_list, t)
            
            if not qua_group[qua] then
                qua_group[qua] = {}
            end
            if not qua_group[qua][level] then
                qua_group[qua][level] = {}
            end
            table.insert(qua_group[qua][level], t)
        end
    end

    for i,qua_list in pairs(qua_group) do
        for level,level_list in pairs(qua_list) do
            if level < max_level then
                local count = 0
                for _,vv in pairs(level_list) do
                    if not vv.is_uped then
                        count = count + 1
                    end
                end
                if count >= 2 then
                    local compose_num = math.floor(count /  2) -- 能合多少本
                    local use_num = compose_num * 2  -- 消耗了多少本
                    for index = 1,use_num do
                        local t = level_list[index]
                        t.is_uped = true
                    end

                    for index = 1, compose_num do
                        local t = level_list[index]
                        local t_level = level + 1
                        local skill_id = Item.GetConfig(t.item_id).param1 
                        local t_cfg = self:GetSkillItemCfg(skill_id, t_level)
                        local item_id = t_cfg.item_id
                        if not qua_list[t_level] then
                            qua_list[t_level] = {}
                        end
                        table.insert(qua_list[t_level], {is_uped = false, item_id = item_id, qua = i, level = t_level, num = 1})
                    end
                end
            end
        end
    end

    local list = {}
    for i,qua_list in pairs(qua_group) do
        for level,level_list in pairs(qua_list) do
            for _,v in pairs(level_list) do
                if not v.is_uped then
                    if not list[v.item_id] then
                        list[v.item_id] = v
                    else
                        list[v.item_id].num = list[v.item_id].num + 1
                    end
                end
            end
        end
    end
    list = table.rearrange2(list)
    table.sort(list, function(a, b)
        if a.qua == b.qua then
            return a.level > b.level
        end
        return a.qua > b.qua
    end)

    item_list = {}
    for i, v in pairs(list) do
        if v.item_id then
            table.insert(item_list, Item.Create({item_id = v.item_id, num = v.num}))
        end
    end
    return item_list
end

function BicycleOtherData:SkillBookListSort(a,b)
    if a:Color() == b:Color() then
        return a:Config().param3 > b:Config().param3
    end
    return a:Color() > b:Color()
end



function BicycleOtherData:CacheSkillBookAllUpgrade(item)
    if self.delay_cache_all == nil then 
        self.delay_cache_all = TimeHelper:AddDelayFrameTimer(function ()
            table.sort(self.cache_all_medal_list,BindTool.Bind(self.SkillBookListSort, self))
            MainOtherCtrl.Instance:OpenGiftView(self.cache_all_medal_list,GIFT_OPEN_TYPE.SPECIAL,false)
            self.cache_all_medal_list = {}
            self.delay_cache_all = nil
		end,5)
    end  

    if self.cache_all_medal_list == nil then self.cache_all_medal_list = {} end 

    table.insert( self.cache_all_medal_list, item )
end


function BicycleOtherData:SetAStuff(data)
    self.sel_stuff_data[1] = data
end
function BicycleOtherData:GetAStuff()
    return self.sel_stuff_data[1]
end
function BicycleOtherData:SetBStuff(data)
    self.sel_stuff_data[2] = data
end
function BicycleOtherData:GetBStuff()
    return self.sel_stuff_data[2]
end
function BicycleOtherData:ClearStuff()
    self.sel_stuff_data:Set({})
end


function BicycleOtherData:GetSkillItemCfg(skill_id, level)
    return Cfg.GetSkillItemCfg(skill_id, level)
end


function BicycleOtherData:GetItemSkillInfo(item_id)
    local item_cfg = Item.GetConfig(item_id)
    local t = {}
    t.skill_id = item_cfg.param1
    t.skill_hole = item_cfg.param2
    t.skill_level = item_cfg.param3
    t.item_id = item_id
    return t
end