MallData = MallData or BaseClass()

MallData.Page = {
    Limit = 0,          -- 限购商店
    Chivalrous = 1,     -- 侠义兑换
    Arena = 2,          -- 竞技兑换
    ArenaMatch = 3,     -- 擂台兑换
    Pray = 4,           -- 祈福兑换
    Gold = 5,           -- 元宝商城
    Clip = 6,           -- 碎片商店
    Fashion = 7,        -- 时装商店
    Another = 8,        -- 幻界商店
    Captain = 9,        -- 队长福利
}

function MallData:__init()
    if MallData.Instance ~= nil then
        Debuger.LogError("[MallData] attempt to create singleton twice!")
        return
    end
    MallData.Instance = self
    self:InitShopData()
end

--==============================商店装备属性相关===============================
function MallData.GetSellContent(item)
    if item == nil then return "" end

    local function Connect(connect,content)
        return connect .. content .. "\n"
    end

    local title = Language.ItemInfo.SellItem

    local temp = ""

    if item.param ~= nil then
        --基础属性
        if item.param.attr_list and #item.param.attr_list > 0 then
            temp = Connect(temp,Language.ItemInfo.ItemTitles.BaseAttribute)
            temp = Connect(temp,MallData.EquipBase(item.param.attr_list or {}))
        end
        --熔炼属性
        if item.param.attr_extra_inc then
            temp = Connect(temp,Language.ItemInfo.ItemTitles.SmeltAttribute)
            temp = Connect(temp,MallData.EquipSmelt(item.param.attr_extra_inc or {}))
        end
        --特效
        if tonumber(item.param.sp_id) and item.param.sp_id > 0 then
            local sp_cfg = BagData.Instance:GetEquipEffectCfg(item.param.sp_id)
            local color = QualityColorStr[sp_cfg.sp_priority]
            temp = Connect(temp,Format(title.Effect,color,Equip.GetEffectDesc(item.param.sp_id)))
        end
    end
    if temp == "" then
        temp = Connect(Format(Language.Mall.SellContent,item:Desc()))
    end
    return temp
end

function MallData.EquipBase(attr_list)
    local temp = ""
    for i,v in ipairs(attr_list) do
        if v.attr_value > 0 then
            local name = Language.Common.AttrList[v.attr_type]
            local value = v.attr_value
            if DataHelper.IsPercent(v.attr_type) then
                value = Percent(value)
            end
            local str = Format("<color=#%s>%s  +%s</color>",COLORSTR.Yellow13,name,value)
            temp = temp == "" and str or temp .."\n" ..str
        end
    end
    return temp
end

function MallData.EquipSmelt(inc_list)
    local temp = ""
    local tempt = ""
    local attr_index = 0
    for i,v in ipairs(inc_list) do
        attr_index = attr_index + 1
        if v.attr_value > 0 then
            local value = v.attr_value
            if DataHelper.IsPercent(v.attr_type) then
                value = Percent(value)
            end
            local str = Format(Language.ItemInfo.ItemTitles.EquipAttr3,
            QualityColorStr[v.color],Language.Common.AttrList[v.attr_type],value)
            if attr_index % 2 == 0 then 
                tempt = tempt == "" and str or tempt .."\n" ..str
            elseif attr_index % 2 == 1 then  
                temp = temp == "" and str or temp .."\n" ..str
            end 
        end
    end
	if temp == "" then 
        temp = Language.Common.Wu
    else
        temp = temp .. "\n" .. tempt
    end
    return temp
end

function MallData.JewelrySuit(item)
    local temp = Language.ItemInfo.ItemTitles.BaseAttribute  .. "\n"

    temp = temp .. MallData.EquipBase({item.param or {}}) .. "\n"

    local is_active, suit_info = item:ActiveSuitInfo()

    local title = is_active and Language.ItemInfo.Jewelry.Suit2 or Language.ItemInfo.Jewelry.Suit
    
    temp = temp .. ColorStr(title,COLORSTR.Yellow2) .. "\n"

	local suit = ""
	if suit_info then
		if suit_info.is_pe then
			suit = suit_info.content
		else
			for i = 1, 3 do
				local str = string.format("%s+%d", Language.ItemInfo.AttrList[suit_info["param_" .. i]], suit_info["attr_" .. i])
				suit = suit .. string.joint(i, str, " ")
			end
		end
    end
    suit = ColorStr(suit,is_active and COLORSTR.Green3 or COLORSTR.Gray2)

    temp = temp .. suit .. "\n"

    return temp
end

----------------新修改--------------
function MallData.Attribute(item)
    local temp_list = {}

    local temp = ""

    local title = Language.ItemInfo.SellItem
    --类型
    if item:IsEquip() then
        temp = temp .. title.Type .. Item.EquipTypeName(item.item_id).. "\n"
    elseif item:IsJewelry() then
        temp = temp .. title.Type .. Language.Common.Jewelry[Jewelry.GetJewelryType(item.item_id)] .. "\n"
    else
        temp = temp .. title.Type .. Language.Common.ShowType[23] .. "\n"
    end
    --等级
    temp = temp ..  Format(Language.ItemInfo.ItemTitles.UseLevel,item:LimitLv())
    --适用
    if item:IsEquip() then
        temp = temp .. "\n" .. title.Apply .. Language.Common.Prof[Equip.GetLimitProf(item.item_id) / 100]
    end
    temp_list.name = item:QuaName()

    temp_list.desc = temp

    temp_list.attr_list = {}

    for i,v in ipairs(item.param.attr_list or {}) do
        if v.attr_value > 0 then
            table.insert(temp_list.attr_list,v)
        end
    end

    if #temp_list.attr_list == 0 then
        table.insert(temp_list.attr_list,{})
    end

    if item:IsEquip() or item:IsJewelry()then
        temp_list.inc_list = {}

        for i,v in ipairs(item.param.attr_extra_inc or {}) do
            if v.attr_value > 0 then
                table.insert(temp_list.inc_list,v)
            end
        end

        for i,v in ipairs(item.param.refresh_attr_extra_inc or {}) do
            if v.attr_value > 0 then
                table.insert(temp_list.inc_list,v)
            end
        end
    
        if #temp_list.inc_list == 0 then
            temp_list.inc_list = nil
        end
    end

    local effect_desc = Equip.GetEffectDesc(item.param.sp_id)
    
    if effect_desc ~= "" then
        temp_list.effect_desc = Format(title.Effect,QualityColorStr[ItemColor.Blue],effect_desc)
    end

    local effect_2_desc = Equip.GetEffectDesc(item.param.second_sp_id)
    
    if effect_2_desc ~= "" then
        temp_list.effect_desc = temp_list.effect_desc .. "\n"..Format(title.Effect,QualityColorStr[ItemColor.Blue],effect_2_desc)
    end

    if item:IsEquip() or item:IsJewelry()then
        temp_list.creater_name = item.param.creater_name ~= "" and item.param.creater_name or Language.ItemInfo.NoneName
    end

    return temp_list
end
--==============================商店相关的配置放这里来 id托管接口===============================
function MallData:InitShopData()
    self.shop_config_list = {
        [Mod.Mall.Shop] = KeyList(Config.shop_cfg_auto.mourinho,"item_id"),
        [Mod.Mall.Market] = KeyList(Config.shop_cfg_auto.market,"item_id"),
        ["StoreShop"] = KeyList(Config.shop_cfg_auto.department_store,"item_id"),
    }
    self.shop_other_config_list = {
        ["NpcShop"] = KeyList(Config.shop_cfg_auto.shop,"item_id"),
        ["Reputation"] = KeyList(Config.prestige_cfg_auto.shop,"item_id")
    }
    self.shop_config_func = {
        [Mod.Mall.Shop] = function(param_t)
            CommerceData.Instance:SetJumpIndex(param_t.item_id)
        end,
        [Mod.Mall.Market] = function(param_t)
            MarketData.Instance:SetJumpIndex(param_t.item_id)
        end,
        ["StoreShop"] = function(param_t)
            if param_t.conver_type == GOLD_STORE_TYPE then
                StoreData.Instance:SetJumpIndex(param_t,param_t.item_id)
                ViewMgr:OpenViewByKey(Mod.Store.Gold)
            else
                StoreData.Instance:SetJumpIndex(param_t,param_t.item_id)
                ViewMgr:OpenViewByKey(Mod.Store.Main)
            end
        end,
        ["NpcShop"] = function(param_t,is_auto)
            local shop_id = param_t.shop_id
            if is_auto == nil then
                ReputationCtrl.Instance:Reputation(shop_id,param_t.item_id)
                ViewMgr:OpenViewByKey(Mod.Reputation.Main)
            else
                local config = Config.shop_cfg_auto.shop_npc[shop_id]
                local npc_config = VoMgr:NpcVoBySeq(config.npc_seq)
                local scene_id = npc_config.scene_id or 0
                if SceneData.Instance:SceneId() ~= scene_id and config.scene_id ~= "" then
                    scene_id = config.scene_id
                end
                SceneLogic.Instance:AutoToPos(scene_id,nil,function()
                    SceneLogic.Instance:AutoToNpc(npc_config.id)
                end,nil,true)
                PrestigeData.StoreItemId = param_t.item_id
            end
        end,
        ["Reputation"] = function(param_t,is_auto)
            local village_id = param_t.village_id
            if is_auto == nil then
                ReputationCtrl.Instance:SetSelectIndex(param_t.item_id,village_id)
                ViewMgr:OpenViewByKey(Mod.Reputation.Main)
            else
                SceneLogic.Instance:AutoToNpc(PrestigeData.Instance:GetNpcIdByVillageId(village_id))
                PrestigeData.StoreItemId = param_t.item_id
            end
        end
    }
end

function MallData:GetShopItemConfig(item_id,mod_key)
    for k,v in pairs(self.shop_config_list) do
        if mod_key == nil then
            if v[item_id] then
                return k,v[item_id]
            end
        else
            if mod_key == k and v[item_id] then
                return k,v[item_id]
            end
        end
        --mod_key是商城的也开放
        if v[item_id] then
            for _,value in pairs(Mod.Store) do
                if value==mod_key then
                    return k,v[item_id]
                end
            end
        end
    end
    for k,v in pairs(self.shop_other_config_list) do
        if mod_key == nil then
            if v[item_id] then
                return k,v[item_id]
            end
        else
            if mod_key == k and v[item_id] then
                return k,v[item_id]
            end
        end
    end
    return 0,{}
end

function MallData:SetJumpItemNum(need_num)
    if need_num ~= nil and need_num > 0 then
        MallData.JumpItemNum = need_num
    end
end

function MallData.GetRequireValue(type,value)
    if type == 4 then
        value = CloudArenaData.GetRankingTitle(value)
    elseif type == 6 then
        value = BattlefieldData.Instance:GetShopRequireTip(value)
    elseif type == 7 then
	    local partner = PartnerData.Instance:GetPartnerInfoById(value)
        value = partner.info.name
    end
    return Format(Language.Store.RequireTip[type] or "",value or "")
end