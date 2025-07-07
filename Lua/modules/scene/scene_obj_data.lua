SceneObjData = SceneObjData or BaseClass()

function SceneObjData:__init()
    if SceneObjData.Instance ~= nil then
        Debuger.LogError("[SceneObjData] attempt to create singleton twice!")
        return
    end
    SceneObjData.Instance = self
end

--获取默认模型配置
function SceneObjData.GetDrawerCfg(vo)
    local type = vo and vo.obj_type or SceneObjType.None
    if type == SceneObjType.Npc then
        return Cfg.NpcById(vo.id, false)
    elseif type == SceneObjType.Monster then
        return DrawerHelper.GetMonsterDrawerCfg(vo)
    elseif type == SceneObjType.Box then
        return DrawerHelper.GetBoxDrawerCfg(vo)
    end
    return {res_id = vo.res_id, res_size = vo.res_size}
end

--获取默认模型路径 配置有res_id或res 或vo.res_id
function SceneObjData.GetDrawerPath(vo,co)
    local res_id = co.res_id or co.res or vo.res_id
    local type = vo and vo.obj_type or SceneObjType.None
    if type == SceneObjType.Door then
        return DrawerHelper.GetDoorDrawerPath(vo)
    elseif type == SceneObjType.ShengQi then
        return DrawerHelper.GetWeaponPath(res_id)
    end
    return DrawerHelper.GetNpcPath(res_id) or "actor/monster/test.prefab"
end