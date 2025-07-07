ChAttachKeyName = {
    -- LeftHand = "HandL",
    -- RightHand = "HandR",
    Back = "Back",
    LeftWaist = "WaistL",
    RightWaist = "WaistR",
    Mount = "Mount",
    RightHand1 = "HandR1", --拳套
    RightHand2 = "HandR2", --枪，杖，斧，剑[Default]
	RightHand3 = "HandR3", --灯笼
	RightHand4 = "HandR4", --扇子
    LeftHand1 = "HandL1", --拳套(左手)，
    LeftHand2 = "HandL2", --弓
    LeftHand3 = "HandL3", --盾
    HandO = "HandO",--武器有动画且带位置变化
}

local function weapon_id_to_type(weapon_id)
    return math.floor(weapon_id / 10000)
end

local function main_res_id_to_avatar(main_res_id)
    return math.floor(main_res_id/10000)
end

local function weapon_id_to_lr(weapon_res_id)
    return tonumber(string.sub(weapon_res_id,string.len(weapon_res_id)))--这里加了时装后还得改
end

local function weapon_anim_override(type)
    for k, v in pairs(WEAPON_TYPE) do
        if v == type then
            return v
        end
    end
    return 0
end
local function weapon_anim_override2(type)
    for k, v in pairs(SP_WEAPON_TYPE) do
        if v == type then
            return v
        end
    end
    return 0
end

--defualt parent key:ChAttachKeyName.RightHand2
local weapon_parent_config_r = {
    [WEAPON_TYPE.BOW] = {ChAttachKeyName.LeftHand2,ChAttachKeyName.LeftHand2},
    [WEAPON_TYPE.GLOVES] = {ChAttachKeyName.LeftHand1,ChAttachKeyName.RightHand1},
	[WEAPON_TYPE.LANTERN] = {ChAttachKeyName.LeftHand1,ChAttachKeyName.RightHand3},
	[WEAPON_TYPE.SHAN] = {ChAttachKeyName.LeftHand1,ChAttachKeyName.RightHand4},
	[WEAPON_TYPE.SHIELD] = {ChAttachKeyName.LeftHand3,ChAttachKeyName.RightHand4},
    [WEAPON_TYPE.FISHROD] = {ChAttachKeyName.HandO, ChAttachKeyName.HandO}
}

-- local weapon_anim_override = {
--     [WEAPON_TYPE.AXE] = WEAPON_TYPE.AXE,
--     [WEAPON_TYPE.CANE] = WEAPON_TYPE.CANE,
--     [WEAPON_TYPE.SPEAR] = WEAPON_TYPE.SPEAR
-- }

DrawerHelper = {}

function DrawerHelper.GetRolePath(res_id, i_skin)
    return res_id ~= 0 and string.format("actor/role/%d/skin%d.prefab", res_id, i_skin or 0) or nil
end

function DrawerHelper.GetRidePath(res_id)
    res_id = res_id or 0
    return res_id ~= 0 and string.format("actor/ride/%d.prefab", res_id) or nil
end

function DrawerHelper.GetWingPath(res_id)
    return res_id ~= 0 and string.format("actor/wing/%d.prefab", res_id) or nil
end

function DrawerHelper.GetWeaponPath(res_id)
    return res_id ~= 0 and string.format("actor/weapon/%d.prefab", res_id) or nil
end

function DrawerHelper.GetWeaponPathGray(res_id)
    return string.format("actor/weapon/%s.prefab", res_id) 
end

function DrawerHelper.GetUiModelPath(res_id)
    return string.format( "actor/ui/%s.prefab", res_id)
end
-- NPC、怪物、宠物、伙伴都放这个文件夹里
function DrawerHelper.GetNpcPath(res_id)
    if not res_id then
        return "actor/npc/508.prefab"
    end
    return res_id ~= 0 and string.format("actor/npc/%s.prefab", res_id) or nil
end

function DrawerHelper.GetPartnerPath(res_id, skin_id)
    if not res_id then return "actor/npc/508.prefab" end
    if skin_id and skin_id > 0 and PartnerData.Instance.partner_skin_ids[skin_id] then
        local res_id = PartnerData.Instance.partner_skin_ids[skin_id].res_id
        return string.format("actor/npc/%s.prefab", res_id)
    else
        return res_id ~= 0 and string.format("actor/npc/%s.prefab", res_id) or nil
    end
end
function DrawerHelper.GetPetPath(res_id, skin_id)
    if not res_id then return "actor/npc/508.prefab" end
    local need_res_id = res_id
    if skin_id and skin_id > 0 then
        local skin_cfg = Cfg.PetSkinAttr(skin_id)
        if skin_cfg then
            need_res_id = skin_cfg.res_id
        end
    -- else
    --     return res_id ~= 0 and string.format("actor/npc/%s.prefab", res_id) or nil
    end
    return need_res_id ~= 0 and string.format("actor/npc/%s.prefab", need_res_id) or nil
end

function DrawerHelper.GetMatPath(name)
    return string.format("material/%s.mat", name)
end

function DrawerHelper.GetAnimPath(name)
    return string.format("anim/%s.anim", name)
end

function DrawerHelper.GetRideOverridePath(main_res_id, ride_res_id, l_weapon_res_id, r_weapon_res_id)
    local ride_type = math.floor(ride_res_id / 1000)
    --增加武器显示
    local type_l = weapon_id_to_type(l_weapon_res_id)
    local type_r = weapon_id_to_type(r_weapon_res_id)
    local type = WEAPON_TYPE.NONE
    if type_r > WEAPON_TYPE.NONE and type_r < WEAPON_TYPE.SUB_START then
        type = type_r
    elseif type_l > WEAPON_TYPE.NONE and type_l < WEAPON_TYPE.SUB_START then
        type = type_l
    end
    if ride_type ~= RIDE_TYPE.TYPE3 and ride_type ~= RIDE_TYPE.TYPE5 and (type == WEAPON_TYPE.NONE or type == WEAPON_TYPE.GLOVES) then
        return string.format("actor/role/%d/ride1_quan.asset", main_res_id)
    elseif ride_type ~= RIDE_TYPE.TYPE3 and ride_type ~= RIDE_TYPE.TYPE5 and type == WEAPON_TYPE.BOW then
        return string.format("actor/role/%d/ride1_gong.asset", main_res_id)
    end
    if ride_type == RIDE_TYPE.TYPE3 and type == WEAPON_TYPE.LANTERN then
        ride_type = ride_type + WEAPON_TYPE.LANTERN
    end
    if ride_type == RIDE_TYPE.TYPE3 and (type == WEAPON_TYPE.SHAN or type == WEAPON_TYPE.AXE) then
        --萝莉的扇子和斧子区别
        local role_type = weapon_id_to_type(main_res_id)
        if role_type == 2 then
            if type == WEAPON_TYPE.SHAN then
                ride_type = ride_type + WEAPON_TYPE.SHAN
            end
        else
            ride_type = ride_type + WEAPON_TYPE.SHAN
        end
    end

    return string.format("actor/role/%d/ride%d.asset", main_res_id, ride_type)
end

function DrawerHelper.GetSceneWeaponOverridePath(main_res_id, l_weapon_res_id, r_weapon_res_id)
    local type_l = weapon_id_to_type(l_weapon_res_id)
    local type_r = weapon_id_to_type(r_weapon_res_id)
    local type = WEAPON_TYPE.NONE
    if type_r > WEAPON_TYPE.NONE and type_r < WEAPON_TYPE.SUB_START then
        type = type_r
    elseif type_l > WEAPON_TYPE.NONE and type_l < WEAPON_TYPE.SUB_START then
        type = type_l
    end
    -- local override_postfix = weapon_anim_override2(type)
    return string.format("actor/role/%d/scene%d.asset", main_res_id, type)
end

function DrawerHelper.GetSceneWeaponAnimationClipOverridePath(r_weapon_res_id,main_res_id)
	local type_m = weapon_id_to_type(main_res_id)
	local path_id= (70 + type_m)
    return string.format("actor/wanim/%d/scene%d.asset",r_weapon_res_id,path_id)
end


function DrawerHelper.GetRideSceneWeaponAnimationClipOverridePath(r_weapon_res_id,main_res_id, ride_res_id)
    local ride_type = 0
    if ride_res_id then
        ride_type = math.floor(ride_res_id / 1000)
    end
    local type_m = weapon_id_to_type(main_res_id)
    local path_id= (70 + type_m)
    if ride_type == 3 then
        path_id = path_id + ride_type
    end
    return string.format("actor/wanim/%d/ride%d.asset",r_weapon_res_id,path_id)
end

function DrawerHelper.GetShowWeaponOverridePath(main_res_id, l_weapon_res_id, r_weapon_res_id)
    local type_l = weapon_id_to_type(l_weapon_res_id)
    local type_r = weapon_id_to_type(r_weapon_res_id)
    local type = WEAPON_TYPE.NONE
    if type_r > WEAPON_TYPE.NONE and type_r < WEAPON_TYPE.SUB_START then
        type = type_r
    elseif type_l > WEAPON_TYPE.NONE and type_l < WEAPON_TYPE.SUB_START then
        type = type_l
    end
    local override_postfix = weapon_anim_override(type) == 0 and 1 or weapon_anim_override(type)
    return string.format("actor/role/%d/weapon%d.asset", main_res_id, override_postfix)
end

function DrawerHelper.GetBattleWeaponOverridePath(main_res_id, l_weapon_res_id, r_weapon_res_id)
    local type_l = weapon_id_to_type(l_weapon_res_id)
    local type_r = weapon_id_to_type(r_weapon_res_id)
    local type = WEAPON_TYPE.NONE
    if type_r > WEAPON_TYPE.NONE and type_r < WEAPON_TYPE.SUB_START then
        type = type_r
    elseif type_l > WEAPON_TYPE.NONE and type_l < WEAPON_TYPE.SUB_START then
        type = type_l
    end
    -- local override_postfix = weapon_anim_override(type)
    return string.format("actor/role/%d/battle%d.asset", main_res_id, type)
end

function DrawerHelper.GetBattleWeaponAnimationClipOverridePath(r_weapon_res_id, main_res_id)
	local type_m = weapon_id_to_type(main_res_id)
	local path_id= (70 + type_m)
    return string.format("actor/wanim/%d/battle%d.asset",r_weapon_res_id,path_id)
end

function DrawerHelper.GetResPathByHuanhuaType(type, res_id)
    if type == IllusionConfig.FashionType.cloth then
        return DrawerHelper.GetRolePath(res_id)
    elseif type == IllusionConfig.FashionType.weapon then
        return DrawerHelper.GetWeaponPath(res_id)
    elseif type == IllusionConfig.FashionType.wing then
        return DrawerHelper.GetWingPath(res_id)
    elseif type == IllusionConfig.FashionType.mount then
        return DrawerHelper.GetRidePath(res_id)
    end
end

function DrawerHelper.GetWeaponTypeAndKey(weapon_res_id)
    local type = weapon_id_to_type(weapon_res_id)
    local lr = weapon_id_to_lr(weapon_res_id)
    if weapon_parent_config_r[type] then
        return type, weapon_parent_config_r[type][lr] or ChAttachKeyName.RightHand2
    else
        return type, ChAttachKeyName.RightHand2
    end
end

function DrawerHelper.GetStory(id)
    return string.format("story/%d.prefab", id)
end

function DrawerHelper.GetStoryPath(res_name)
    return string.format("story/%s.prefab", res_name)
end

function DrawerHelper.GetVideoPath(res_name)
    return string.format("video/%s.mp4", res_name)
end

function DrawerHelper.GetWeaponIsNeedOverrideClip(weapon_res_id)
	local bool_need = false
	local type = weapon_id_to_type(weapon_res_id)
	if type == WEAPON_TYPE.LANTERN then
		bool_need = true
    elseif type == WEAPON_TYPE.FISHROD then
        bool_need = true
	end
    return bool_need
end


--根据角色配置判断该角色是否匹配当前武器，目前用于决定是否加载对应匹配的override
function DrawerHelper.IsMainResMatchWeapon(main_res_id,r_weapon_res_id)
    if main_res_id and main_res_id % ResHelper.FishermanBase == 0 then
        return false
    end
    if r_weapon_res_id == 0 then
        return true
    end
    local avatar_type = main_res_id_to_avatar(main_res_id)
    local weapon_type = weapon_id_to_type(r_weapon_res_id)
    local cfg = RoleCreateData.Instance:GetRoleProfList(avatar_type)
    -- LogError("CFG====",cfg.prof_list)
    for _,v in pairs(cfg.prof_list) do
        local w_id = RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(v.prof_id)]
        local w_cfg = Config.equipment_auto[w_id]
        if w_cfg.weapon_type == weapon_type then
            return true
        end
    end
    -- LogError("IsMainResMatchWeapon RETURN FALSE!====",main_res_id,r_weapon_res_id)
    return false
end

function DrawerHelper.GetDataByItem(data_item)
    local ui_obj_data=nil
    local item_cfg = data_item.item_config or Item.New(data_item):Config()
    if item_cfg.item_type == ItemType.Fashion then
        local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
            if surface_cfg.type == IllusionConfig.FashionType.mount then
                -- 坐骑单独显示
                ui_obj_data=DrawerHelper.GetRidePath(surface_cfg.res_id)
            else
                -- 武器、羽翼、角色时装
                local appearance = TableCopy(RoleData.Instance:GetApearance())
                if surface_cfg.type == IllusionConfig.FashionType.cloth then
                    appearance.color = 0
                    appearance.fashion_body_id = surface_cfg.res_id
                    appearance.special_appearance_type = 0
                    appearance.special_appearance_param = 0
                elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
                    appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
                    appearance.fashion_weapon_id = surface_cfg.res_id
                end
                ui_obj_data=appearance
            end
        elseif DEBUG_MODE then
            LogError("幻化配置为空 id：", item_cfg.param1, "物品id：", item_cfg.id, "配置：", item_cfg)
        end
    elseif item_cfg.item_type == ItemType.Skin then
        -- 角色、宠物、伙伴皮肤需要带上模型
        local skin_cfg = nil
        if item_cfg.show_type == ShowType.PetSkin then
            skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
            if DEBUG_MODE and skin_cfg == nil then
                LogError("宠物皮肤物品", item_cfg.id, "的皮肤配置为空")
            end
        elseif item_cfg.show_type == ShowType.PartnerSkin then
            skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
            if DEBUG_MODE and skin_cfg == nil then
                LogError("伙伴皮肤物品", item_cfg.id, "的皮肤配置为空")
            end
        end
        if skin_cfg and skin_cfg.res_id then
            ui_obj_data=DrawerHelper.GetNpcPath(skin_cfg.res_id)
        end
    elseif item_cfg.item_type == ItemType.PetWeapon then
        local weapon_config = Cfg.PetWeaponBaseCfg(item_cfg.id)
        if weapon_config then
            ui_obj_data=DrawerHelper.GetWeaponPath(weapon_config.pet_weapon_res)
        end
    elseif item_cfg.show_type == ShowType.Pet then
        local pet_cfg = PetData.Instance:GetPetCfg(item_cfg.id)
        if pet_cfg and pet_cfg.res_id then
            ui_obj_data=DrawerHelper.GetNpcPath(pet_cfg.res_id)
        end
    end
    return ui_obj_data
end


-- 通过物品类型获取模型Drawer
function DrawerHelper.CreatByItem(data_item)
    local ui_obj = UIObjDrawer.New()
    return ui_obj
end

local npc_monster_drawer_id_map ={
    [2220] = 40717,
    [2250] = 40702,
    [2280] = 40708,
    [2310] = 40705,
    [2340] = 40711,
    [2370] = 40714,
    [2400] = 40720,
    [2430] = 40723,
    [2460] = 40726,
    [2490] = 40744,
    [2520] = 40729,
    [2550] = 40735,
    [2580] = 40732,
    [2610] = 40738,
    [2640] = 40741,
    [2670] = 40747,
    [2700] = 40750,
    [2730] = 40753,

} 
--获取怪物模型配置
function DrawerHelper.GetMonsterDrawerCfg(vo)
    local id = 0
    if(vo.monster_group_id and vo.client_show_id and 
        vo.monster_group_id ~= 0 and vo.client_show_id ~= 0)  then
            -- LogError("ZZZZZZ===",vo.client_show_id)
        id = vo.client_show_id 
        if npc_monster_drawer_id_map[id] then  --特殊写死逻辑，暂时处理战斗中的怪物ID配置尺寸与NPC表ID配置尺寸不同的问题
            -- LogError("GGGGGG===",npc_monster_drawer_id_map[id],"||")
            id = npc_monster_drawer_id_map[id]
        end
    else
        id = vo.id
    end
    return Cfg.MonsterById(id,false) or Cfg.NpcById(id, false)
end

--获取宝箱模型配置
function DrawerHelper.GetBoxDrawerCfg(vo)
    local cfg = NpcDialogCtrl.Instance:NpcInfo(vo.id)
    if GuildData.Instance:IsInGuildFightFB() then
        cfg = GuildData.Instance:BosConfig(vo.id)
    end
    return cfg or {res_id = 102}
end

function DrawerHelper.GetDoorDrawerPath(vo)
    return Format("effect/1002000%s.prefab",(vo.type == 1 or vo.type == 2) and 7 or 1)
end

FILE_REQ("modules/drawer/body_obj")

FILE_REQ("modules/drawer/scene_obj_drawer")
FILE_REQ("modules/drawer/scene_ch_drawer")
FILE_REQ("modules/drawer/battle_ch_drawer")
FILE_REQ("modules/drawer/battle_obj_drawer")
FILE_REQ("modules/drawer/ui_obj_drawer")
FILE_REQ("modules/drawer/ui_ch_drawer")
