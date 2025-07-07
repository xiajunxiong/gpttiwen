

ItemInfoData = ItemInfoData or BaseClass()

function ItemInfoData:__init()
    if ItemInfoData.Instance ~= nil then
        Debuger.LogError("[ItemInfoData] attempt to create singleton twice!")
        return
    end
    ItemInfoData.Instance = self
    self:InitItemInfoData()
end

function ItemInfoData:InitItemInfoData()
    self.item_info = {}
    self.click_data = {}
    self.effect_show = false
    self.collect_group_data = SmartData.New()
    self.item_pos_data = SmartData.New({val = false})
    self.get_way_show = SmartData.New({val = false,pos = 0,start = -1})
    self.money_box_info = SmartData.New{init = true}
    self.btn_remind_func_list = {}
    self.quick_buy_tips = {}
end

function ItemInfoData:SetItemInfo(info)
    info = info or {}
    self.item_info.item_list = info.item_list or {}
    self.item_info.func = info.func
    self.item_info.list_l = #self.item_info.item_list
    self.item_info.show_ways = info.show_ways or false
    self.item_info.show_send = info.show_send or false
    info = self.item_info
end

function ItemInfoData:ItemInfo()
    return self.item_info
end

function ItemInfoData:GetItemData()
    return self.item_info.item_list[1]
end

function ItemInfoData:SetItemInfoData(info)
    self.item_info_data = info
end

function ItemInfoData:GetItemInfoData()
    return self.item_info_data
end

function ItemInfoData:ItemDeriveView(reset)
end

function ItemInfoData:SetActivityInfo(info)
    self.activity_info = info
end

function ItemInfoData:ActivityInfo()
    return self.activity_info
end

function ItemInfoData:SetGetWayInfo(info)
    self.getway_info = info
end

function ItemInfoData:GetWayInfo()
    return self.getway_info
end

function ItemInfoData:ChangeGetWayShow()
    self.get_way_show.val = not self.get_way_show.val
end

function ItemInfoData:SetCollectGroupData(data)
    self.collect_group_data:Set(data)
end

function ItemInfoData:GetCollectGroupData()
    return self.collect_group_data
end

--是否是属于红蓝药
function ItemInfoData.IsMedicinal(item_id)
    if SceneData.Instance:GetMode() == SceneModeType.FightCrystal then
        return Item.GetUseType(item_id) == 9 or Item.GetUseType(item_id) == 12
    end
    return false
end

function ItemInfoData.GetMedicinalTimes(item_id)
    if SceneData.Instance:GetMode() == SceneModeType.FightCrystal then
        return FightCtrl.Instance:Data():GetFbTimes(item_id)
    end
    if SceneData.Instance:GetMode() == SceneModeType.RushDungeon then
        return RushDungeonData.Instance:GetUseItemInfoByItemId(item_id)
    end
    return 0
end

function ItemInfoData:GetHealList()
	return TeamData.Instance:InTeam() and self:GetHealListInTeam() or self:GetHealListNoTeam()
end

function ItemInfoData:GetHealListNoTeam()
    local list = {}
    table.insert(list, {
        is_role = true,
        param1 = 0,
        name = RoleData.Instance:GetRoleName(),
        cur_hp = RoleData.Instance:GetBattleAttri(BattleAttriType.CurHp),
        max_hp = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxHp),
        cur_mp = RoleData.Instance:GetBattleAttri(BattleAttriType.CurMp),
        max_mp = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxMp),
        avatar_type = RoleData.Instance:GetRoleAvatarType(),
    })
	for _, pet in pairs(PetData.Instance.pet_list) do	
        local pet_state = pet:State()
        if PetState.Fight == pet_state then
            local total_attr = PetData.Instance:RealAttr(pet.index) or pet:TotalAttr()
            table.insert(list, {
                is_pet = true,
                pet = pet,
                param1 = 1,
                param2 = pet.index,
                name = pet:Name(),
                cur_hp = pet:HP(),
                max_hp = total_attr.maxhp,
                cur_mp = pet:MP(),
                max_mp = total_attr.maxmp,
                pp_state_sp = PPStateSp.zhan

            })
		end
    end
    for _, pet in pairs(PetData.Instance.pet_list) do	
		local pet_state = pet:State()
        if PetState.StandBy == pet_state then
            local total_attr = PetData.Instance:RealAttr(pet.index) or pet:TotalAttr()
            table.insert(list, {
                is_pet = true,
                pet = pet,
                param1 = 1,
                param2 = pet.index,
                name = pet:Name(),
                cur_hp = pet:HP(),
                max_hp = total_attr.maxhp,
                cur_mp = pet:MP(),
                max_mp = total_attr.maxmp,
                pp_state_sp = PPStateSp.dai
            })
		end
    end
    for _, partner in pairs(PartnerData.Instance.partner_list) do
		local vo = partner.vo
        if PartnerConfig.FightState.fight == vo.fight_state then
            local info = partner.info
			table.insert(list, {
                is_partner = true,
                param1 = 2,
                param2 = partner.id,
                name = info.name,
                cur_hp = vo.attr_list[BattleAttriType.CurHp],
                max_hp = vo.attr_list[BattleAttriType.MaxHp],
                cur_mp = vo.attr_list[BattleAttriType.CurMp],
                max_mp = vo.attr_list[BattleAttriType.MaxMp],
                icon_id = info.icon_id,
            })
		end
    end
    return list
end

function ItemInfoData:GetHealListInTeam()
    local list = {}
    table.insert(list, {
        is_role = true,
        param1 = 0,
        name = RoleData.Instance:GetRoleName(),
        cur_hp = RoleData.Instance:GetBattleAttri(BattleAttriType.CurHp),
        max_hp = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxHp),
        cur_mp = RoleData.Instance:GetBattleAttri(BattleAttriType.CurMp),
        max_mp = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxMp),
        avatar_type = RoleData.Instance:GetRoleAvatarType(),
    })
	for _, pet in pairs(PetData.Instance.pet_list) do	
        local pet_state = pet:State()
        if PetState.Fight == pet_state then
            local total_attr = PetData.Instance:RealAttr(pet.index) or pet:TotalAttr()
            table.insert(list, {
                is_pet = true,
                pet = pet,
                param1 = 1,
                param2 = pet.index,
                name = pet:Name(),
                cur_hp = pet:HP(),
                max_hp = total_attr.maxhp,
                cur_mp = pet:MP(),
                max_mp = total_attr.maxmp,
                pp_state_sp = PPStateSp.zhan
            })
		end
    end
    for _, pet in pairs(PetData.Instance.pet_list) do	
		local pet_state = pet:State()
        if PetState.StandBy == pet_state then
            local total_attr = PetData.Instance:RealAttr(pet.index) or pet:TotalAttr()
            table.insert(list, {
                is_pet = true,
                pet = pet,
                param1 = 1,
                param2 = pet.index,
                name = pet:Name(),
                cur_hp = pet:HP(),
                max_hp = total_attr.maxhp,
                cur_mp = pet:MP(),
                max_mp = total_attr.maxmp,
                pp_state_sp = PPStateSp.dai
            })
		end
    end
    local member_list = TeamData.Instance:GetMemberList()
    for i = 1, TeamConfig.TEAM_MEMBER_MAX do
        if member_list[i] and (member_list[i]:IsFollow() or member_list[i]:IsLeader()) and not member_list[i]:IsSelf() then
            local info = member_list[i].info
            local member_index = info.member_index
            table.insert(list, {
                is_role = 1 ~= info.is_partner,
                is_partner = 1 == info.is_partner,
                param1 = 1 == info.is_partner and 4 or 3,
                param2 = 1 == info.is_partner and info.uid or member_index,
                name = info.name,
                cur_hp = info.cur_hp,
                max_hp = info.max_hp,
                cur_mp = info.cur_mp,
                max_mp = info.max_mp,
                avatar_type = info.appearance.avatar_type,
                icon_id = info.icon_id,
            })
            if info.pet_id > 0 then
                local pet = Pet.New{id = info.pet_id, level = info.pet_level, str_level = info.pet_str_level}
                table.insert(list, {
                    is_pet = true,
                    pet = pet,
                    param1 = 3,
                    param2 = member_index,
                    param3 = 1,
                    name = pet:Name(),
                    cur_hp = info.pet_cur_hp,
                    max_hp = info.pet_max_hp,
                    cur_mp = info.pet_cur_mp,
                    max_mp = info.pet_max_mp,
                    pp_state_sp = PPStateSp.zhan
                })
            end
        end
    end
	return list
end

function ItemInfoData:CheckItemPosData()
    self.item_pos_data.val = not self.item_pos_data.val
end

--物品设置点击位置
function ItemInfoData:SetClickDataPos(pos)
    self.click_data.pos = pos
end

--获取物品点击位置
function ItemInfoData:GetClickDataPos()
    return self.click_data.pos
end

-- 注册按钮红点 func_name : ItemInfoFunc,如果func_name重复会覆盖掉
function ItemInfoData:RegisterBtnRemind(func_name, func)
    self.btn_remind_func_list[func_name] = func
end

function ItemInfoData:UnRegisterBtnRemind(func_name)
    self.btn_remind_func_list[func_name] = nil
end

function ItemInfoData:GetBtnRemind(func_name, item_info)
    local func = self.btn_remind_func_list[func_name]
    if not func then
        return 0
    end
    return func(item_info) or 0
end

function ItemInfoData:SetHongBaoUseRet(protocol)
    self.money_box_info:Set(protocol.info)
end

-- 获取具体的购买途径信息
function ItemInfoData:CatchForgeBuyWay(item)
    if item:GetWay() == nil then return nil end 
    for k,v in pairs(item:GetWay()) do 
        local info = BagData.Instance:GetWayCfg(v)
        -- 持有入手途径·市场购买
        if info.open_panel ~= "" and Mod.Mall.Market == tonumber(info.open_panel) then 
            local shop_data = MarketData.Instance:GetParam(item)
            local market_price = MarketData.Instance:GetItemPrice(item.item_id)
            if market_price > 0 and shop_data.price ~= market_price then
                shop_data.price = market_price
            end
            return shop_data,Mod.Mall.Market
        end 
        -- 持有入手途径·商会购买
        if info.open_panel ~= "" and Mod.Mall.Shop == tonumber(info.open_panel) then 
            local shop_data = CommerceData.Instance:GetMourinhoCfgById(item.item_id)

            return shop_data,Mod.Mall.Shop
        end
        -- 持有入手途径·元宝商店
        if info.open_panel ~= "" and Mod.Store.Gold == tonumber(info.open_panel) then 
            -- local shop_data = CommerceData.Instance:GetMourinhoCfgById(item.item_id)
            local _, shop_data = MallData.Instance:GetShopItemConfig(item.item_id, "StoreShop")--Mod.Store.Gold)
            return shop_data, Mod.Store.Gold
        end
        -- 持有入手途径·声望商店
        if info.npc ~= "" then 
            local vil_cfg = PrestigeData.Instance:GetVillageByNpcId(tonumber(info.npc))
            if vil_cfg ~= nil then 
                local list = PrestigeData.Instance:GetPageVillageData(1,vil_cfg.village_id) 
                for i,j in pairs(list or {}) do 
                    if item.item_id == j.item_id then 
                        return j,Mod.Reputation.Main
                    end 
                end 
            end 
        end 
    end 
end
function ItemInfoData:SetQuickBuyTips(key, value)
    self.quick_buy_tips[key] = value
end