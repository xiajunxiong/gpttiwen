ResHelper = {}
--[[ local WLR = {
    [1] = 1,--弓
    [2] = 2,--剑
    [3] = 2,--斧子
    [4] = 2,--法杖
    [5] = 2,--长枪
    [6] = 1,--手套
}
 ]]
--local imgConfig = Config.image_change_auto.image_chan
ResHelper.FishermanBase = 11111
function ResHelper.RoleResId(avatar_type, fashion_body_id, advance_time)
    -- 目录命名 = 角色类型 * 10000 + 时装id * 10 + 进阶次数[1,6]
    if avatar_type == nil or avatar_type == 0 then
        LogError("未找到对应模型")
        return 10001
    end
    avatar_type = avatar_type or 1
    -- avatar_type = avatar_type + 1
    fashion_body_id = fashion_body_id or 0
    advance_time = (advance_time == nil or advance_time == 0 or fashion_body_id > 0) and 1 or advance_time
    return avatar_type * 10000 + fashion_body_id * 10 + advance_time 
end
function ResHelper.Fisherman(avartar_type)
    return avartar_type * ResHelper.FishermanBase
end
function ResHelper.FishingRod()
    return 120002
end
-- 双手 可能有盾 没盾判断 左右
function ResHelper.WeaponResId(weapon_id, fashion_weapon_id, sub_weapon_id, fashion_sub_weapon_id, advance_time, auto_weapon)
    -- 预设命名 = 类型 * 10000 + 时装id * 10 + 1/2 1左2右
    if weapon_id == nil then
        return 0,0
    end
    local fashion_weapon_id = fashion_weapon_id or 0
    -- if auto_weapon and 0 == fashion_weapon_id then
    if auto_weapon then
        local prof_list = RoleCreateData.Instance:GetRoleProfList(auto_weapon.avatar_type).prof_list
        local default_weapon = true
        for _, info in ipairs(prof_list) do
            local prof_type = RoleData.ProfIdToProfType(info.prof_id)
            if weapon_id == (PROF2WEAPON[prof_type]) then
                default_weapon = false
                break
            end
        end
        if default_weapon then
            local prof_type = RoleData.ProfIdToProfType(prof_list[1].prof_id)
            weapon_id = PROF2WEAPON[prof_type]
        end
    end
    local fashion_sub_weapon_id = fashion_sub_weapon_id or 0
    advance_time = advance_time or 1
    advance_time = (advance_time <= 0 or fashion_weapon_id > 0) and 1 or advance_time
    local sub_weapon_res_id = 0
    -- 副武器资源id
    --[[ if sub_weapon_id then
        sub_weapon_res_id = weapon_id * 10000 + fashion_weapon_id * 10 + 1
    end ]]
    if weapon_id == WEAPON_TYPE.GLOVES then--拳套6
        return weapon_id * 10000 + fashion_weapon_id * 10 + 2 + ((advance_time - 1) * 1000), weapon_id * 10000 + fashion_weapon_id * 10 + 1 + ((advance_time - 1) * 1000)
    elseif weapon_id == WEAPON_TYPE.BOW then--弓箭1
        return sub_weapon_res_id, weapon_id * 10000 + fashion_weapon_id * 10 + 1 + ((advance_time - 1) * 1000)
    elseif weapon_id == WEAPON_TYPE.SPEAR then--枪
        return weapon_id * 10000 + fashion_weapon_id * 10 + 2 + ((advance_time - 1) * 1000), 9 * 10000 + fashion_weapon_id * 10 + 1 + ((advance_time - 1) * 1000)
    else
        return weapon_id * 10000 + fashion_weapon_id * 10 + 2 + ((advance_time - 1) * 1000), sub_weapon_res_id
    end
end
-- 坐骑预设命名 = 骑姿类型 * 10000 + 坐骑id
function ResHelper.RideResId()
    return 1001
end
function ResHelper.WingResId()
    return 0
end

function ResHelper.MonsterResId(id)
    local cfg = Cfg.MonsterById(id)
    if cfg == nil then LogError("无怪物配置", id) return end
    return cfg.res_id
end
function ResHelper.MonsterResByGroup(id)
    local cfg = Cfg.MonsterGroupById(id)
    if cfg == nil then LogError("无怪物组配置",id) return end
    return ResHelper.MonsterResId(cfg.show_monster)
end
--动态加载的路径
function ResHelper.AnimPath(clickName)
    return string.format("actor/anim/%s.anim", clickName)
end