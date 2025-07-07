local IS_LINER = true   --是否是R线版本


--登陆
local bl_login = {
    "ui/views/welcome.prefab",
    "effect/10010211.prefab",--登录界面的特效
    "ui/localization.txt",
    "actor/__actorroot.prefab",
    "actor/__skillpoint.prefab",
    "ui/widgets/item_cell.prefab",
    "ui/widgets/item_cell_full.prefab",
    "ui/widgets/modelcamera.prefab",
    "ui/widgets/load_waiting.prefab",
    "ui/widgets/red_point.prefab",
    "ui/widgets/guide_tip.prefab",
    "effect/10010263.prefab",
    "ui/views/guide/guide_view.prefab",
    "ui/views/top_layer_view.prefab",
}

--创角
local bl_create_role = {
    "levels/jscj_1",
    "levels/jscj_1_data",
    "ui/views/login/role_create.prefab",
    "levels/jscj_particle",
}

--进入角色选择资源
local bl_role_sel = {

}

--普通进入游戏资源，主界面等
local bl_enter_game = {
    --等待界面
    "ui/views/public_popup/wait.prefab",
    --主界面相关
    "ui/views/main.prefab",
    "ui/views/main_other/joy_part.prefab",
    "ui/widgets/rage_point.prefab",
    "ui/views/main/scene_elements.prefab",
    "ui/views/main/chat.prefab",
    "ui/views/main/main_function.prefab",
    "ui/views/bicycle/bicycle_tip_sub.prefab",  --优化项
    "ui/views/challenge/courage_challenge_sub.prefab",--优化项
    "ui/views/pet_refining/lianyao_tip_sub.prefab",  --优化项
    "ui/views/main/mainviewmodbtn.prefab",
    "ui/widgets/red_point2.prefab",
    "ui/views/main/down_right.prefab",
    "ui/views/main/usually_show.prefab",
    "ui/views/bianqiang/bianqiang_main.prefab",
    "ui/views/main/main_top_right.prefab",
    "ui/views/main/top_right.prefab",
    "ui/views/main_right_sub/tt_menu.prefab",
    "ui/views/main/main_top_map.prefab",
    -- "ui/views/main/act_tips.prefab",        --优化项
    "ui/views/map/map_explore.prefab",      --优化项
    -- "ui/views/map/yinyang_explore.prefab",    --优化项
    -- "ui/views/fish/fish_explore.prefab",    --优化项
    -- "ui/views/treasure_task/treasure_task_explore.prefab", --优化项
    -- "ui/views/npc_dialog/npc_dialog_sync.prefab", --优化项

    --
    "actor/npc/350.prefab",--战斗默认模型
}

--新角色进入首场景
local bl_new_role_enter_game = {
    "levels/czdj_3",
    "levels/czdj_3_data",
    "levels/czdj_3_desc",
    
    "actor/npc/599.prefab",--第一场战斗的花妖
    "levels/szzd_3",
    "levels/szzd_3_data",


    "levels/xh_1_gai2",
    "levels/xh_1_data_gai2",


    "levels/szzd_3_desc",
    -- "levels/szzd_3_particle_1",
    "levels/xh_1_desc_gai2",
    -- "levels/xh_1_particle",
   
}
if IS_LINER then
    table.insert(bl_new_role_enter_game,3,"levels/czdj_3_particle")
    --唤灵界面
    table.insert(bl_new_role_enter_game,"ui/views/treasure_hunt/treasure_hunt.prefab") 
    table.insert(bl_new_role_enter_game,"ui/textures/treasure_hunt/treasure_hunt_bg6.png")
    table.insert(bl_new_role_enter_game,"ui/views/treasure_hunt/treasure_hunt_detail.prefab")
end

--游戏中使用资源
local bl_game = {
    --公用控件
    --board1
    "ui/widgets/board1.prefab",
    "effect/10010206.prefab",
    "effect/10010209.prefab",
    "effect/10010265.prefab",
    "effect/10010264.prefab",
    "effect/10010266.prefab",
    --currecy_show
    "ui/widgets/currency_show.prefab",
}

ab_preload = ab_preload or {
    paused = false,
    paused_idx = 0,
    preload_list = nil,
}

function ab_preload:Begin()
    if SIMULATE_ASSETBUNDLE or not IS_MICRO_PLAYER() then
        return
    end
    if GLOBAL_CONFIG:Flags().mini_game then
        return
    end
    Nirvana.AssetManager.UnusedBundleAutoReleaseTime = 150;
    local new_role_course = UnityPlayerPrefs.GetInt("LastEnterRoleServerId",-1) < 0
    self.preload_list = new_role_course and self:setupPreloadList(bl_login,bl_create_role,bl_enter_game,bl_new_role_enter_game,bl_game) or
        self:setupPreloadList(bl_login,bl_role_sel,bl_enter_game,bl_game)
    self:preloadBundle(self.preload_list,1,function()
        self:onPreloadCom()
        -- self.preload_list = nil
    end)
end

function ab_preload:preloadBundle(list,index,on_com)
    if list == nil or index > #list then
        on_com()
        return
    end
    -- UnityEngine.Debug.LogError("PreloadBundle===" .. list[index])
    Nirvana.AssetManager.LoadBundle(list[index],function(err,ab) 
        if not StringIsEmpty(err) then
            UnityEngine.Debug.LogError("PreloadBundleERROR===ab=" .. list[index] ..",err==" .. tostring(err))
        end
        local next_idx = index + 1
        if self.paused then
            -- UnityEngine.Debug.LogError("ab_preload:PauseEXE===" .. tostring(next_idx) .. "|" .. list[next_idx])
            self.paused_idx = next_idx
        else
            self:preloadBundle(list,next_idx,on_com)
        end
    end)
end

function ab_preload:onPreloadCom()
    -- UnityEngine.Debug.LogError("ab_preload:onPreloadCom!!!!")
    self.preload_list = nil
    if not HAS_RESCACHE() then
        return
    end
    --如果使用我们自己的res_cache，则自动缓存首包资源列表
    local install_bundle_list = {}
    local ib_map =--GLOBAL_CONFIG:PkgInfo().is_small_pkg and 
        -- require("preload/install_bundle_list_s") or
        require("preload/install_bundle_list")
    for k,_ in pairs(ib_map) do
        table.insert(install_bundle_list,k)
    end
    local start_idx = UnityPlayerPrefs.GetInt("MicroPlayerInstallBundlePreloadIdx",1)
    self:versionBundles(install_bundle_list,start_idx)

end

function ab_preload:setupPreloadList(...)
    local re_list = {}
    for _, v in pairs({...}) do
        for _,bn in pairs(v) do
            table.insert(re_list,bn)
        end
    end
    return re_list
end


function ab_preload:Pause()
    if not IS_MICRO_PLAYER() then
        return
    end
    if self.paused == true then
        return
    end
    -- UnityEngine.Debug.LogError("ab_preload:Pause")
    self.paused = true
    self.paused_idx = 0
end

function ab_preload:Resume()
    if not IS_MICRO_PLAYER() then
        return
    end
    if self.paused == false then
        return
    end
    self.paused = false
    if self.paused_idx > 0 then
        -- UnityEngine.Debug.LogError("ab_preload:Resume===" .. tostring(self.paused_idx) .. "|" .. self.preload_list[self.paused_idx])
        self:preloadBundle(self.preload_list,self.paused_idx,function()
            -- self.preload_list = nil
            self:onPreloadCom()
        end)
    end
end

local nirvanaAssetMgr = Nirvana.AssetManager
function ab_preload:versionBundles(bundle_list,index)
    if index > #bundle_list then
        -- UnityEngine.Debug.LogError("VersionBundle index===" .. tostring(index) .. ",list_count==" .. tostring(#bundle_list))
        return
    end
    local bundle_name = bundle_list[index]
    -- UnityEngine.Debug.LogError("TryUpdateBundle===" .. tostring(bundle_name) .. ",idx=" .. tostring(index) .. ",fc=" .. Time.frameCount)
    local need_update = not ResourceManager.HasBundle(bundle_name)
    local update_next = function()
        local next_idx = index + 1
        if next_idx > #bundle_list then --complete
            UnityPlayerPrefs.SetInt("MicroPlayerInstallBundlePreloadIdx",next_idx)
            -- UnityEngine.Debug.LogError("InstallPreloadComplete,RecIndex==" .. tostring(next_idx))
        else
            if next_idx%10 == 0 then    --每10个记录一下
                UnityPlayerPrefs.SetInt("MicroPlayerInstallBundlePreloadIdx",next_idx)
            end
            self:versionBundles(bundle_list,next_idx)
        end
    end 
    if need_update then
        nirvanaAssetMgr.UpdateBundle(bundle_name,function() end,function() 
            update_next()
        end)
    else
        if TimeHelper then
            TimeHelper:AddDelayFrameTimer(update_next,0)
        else
            update_next()
        end        
    end 
end


return ab_preload