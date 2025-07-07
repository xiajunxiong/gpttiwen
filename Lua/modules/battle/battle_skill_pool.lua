FILE_REQ("modules/battle/battle_lua_function")

local skill_point_path = "actor/__SkillPoint.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            skill_point_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("__SkillPoint pool register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

BattleSkillPool = BattleSkillPool or BaseClass()
local gameSkillMgrClass = Game.SkillManager
local gameSkillMgr = gameSkillMgrClass.Singleton()

local pos_anchor = {
    EachRoot = 0, --每个角色的脚底
    EachCenter = 1, --每个角色的中心
    ImpactRegion = 2, --作用范围的中心
    CampRegion = 3, --整个队伍的中心
    WholeRegion = 4, --整个场景的中心
    EachRootS = 5, --每个角色的脚底(施放者)
    EachCenterS = 6, --每个角色的中心(施放者)
    CampRegionS = 7, --整个队伍的中心(施放者)
    BehiterEachRoot = 8, --被打中的每个角色脚底
    BehiterEachCenter = 9, --被打中的每个角色中心
    RowRegion = 10, --整行(目标)
    AttakerHand = 11, --攻击者手上(废弃)
    RowRegionS = 12, --整行(释放者)
    ZeroPoint = 13,
    AtkerHandR = 14, -- 攻击者右手上(Bip001 R Hand)
}
BattleSkillPool.ConstAssetId = {
    Summon = "1000003", --召唤
    Defence = "1000001", --防御
    Retrieve = "ZhaoHui", --回收
    Seal = "1000004", --封印
    UseItem = "1000005", --使用道具
    UseFabao = "default_skill", --使用法宝(默认技能asset)
    Fabao1 = "ShuXingZhuanHuan",--法宝没有前置事件所以只能提前加载
    UseMp = "10060603",--回蓝
    UseUnseal = "7031001",--解封
    YouMingSummon = "1000007",--幽冥召唤
}

local main_ch_attack_assid = {
    [WEAPON_TYPE.BOW] = {default = "1002"},
    [WEAPON_TYPE.SWORD] = {default = "1001"},
    [WEAPON_TYPE.AXE] = {default = "1001"},
    [WEAPON_TYPE.CANE] = {default = "1001"},
    [WEAPON_TYPE.SPEAR] = {default = "1001", [RoleAvatarType.BanYue] = "1001"},
    [WEAPON_TYPE.GLOVES] = {default = "1001"}
}

function BattleSkillPool.MainChAttAssId(avatar_type, weapon_type)
    local av_ty_cfg = main_ch_attack_assid[weapon_type] or main_ch_attack_assid[WEAPON_TYPE.GLOVES]
    return av_ty_cfg[avatar_type] or av_ty_cfg.default
end

function BattleSkillPool:__init(displayer, data)
    if BattleSkillPool.Instance ~= nil then
        Debuger.LogError("[BattleSkillPool] attempt to create singleton twice!")
        return
    end
    BattleSkillPool.Instance = self

    self.battle_displayer = displayer
    self.data = data
    gameSkillMgrClass.GetAnchorPosDel = BindTool.Bind(self.OnSkillMgrGetAnchorPos, self)
    gameSkillMgrClass.PlayAnimDel = BindTool.Bind(self.OnSkillMgrPlayAnim, self)
    gameSkillMgrClass.PlayCompleteDel = BindTool.Bind(self.OnSkillPlayComplete, self)
    gameSkillMgrClass.BeginFlyerPathDel = BindTool.Bind(self.BeginFlyerPath, self)
    gameSkillMgrClass.GetVirtualCameraDel = BindTool.Bind(self.GetVirtualCamera, self)
    gameSkillMgrClass.GetGameObjectByIndexDel = BindTool.Bind(self.GetGameObjectByIndex, self)

    gameSkillMgrClass.WarmUpLuaFuncs()  
    -- gameSkillMgrClass.HitEventDel = BindTool.Bind(self.OnHitTrigger,self)

    self.skill_callbacks = {} --key:skill play handle  value:on complete callback
    -- self.skill_asset_resident_data = {}
end
function BattleSkillPool:Clear()
    if VersionTag.HasTag(VersionTag.SkillPlayerStop) then
        for player, _ in pairs(self.skill_callbacks) do
            player:Stop()
        end
    end
    self.skill_callbacks = {}
    if self.handle_ddsy then
        AudioMgr:StopEffectHandle(self.handle_ddsy)
        self.handle_ddsy = nil
    end
end
function BattleSkillPool:__delete()
    if BattleSkillPool.Instance == self then
        gameSkillMgrClass.GetAnchorPosDel = nil
        gameSkillMgrClass.PlayAnimDel = nil
        gameSkillMgrClass.PlayCompleteDel = nil
        -- gameSkillMgrClass.HitEventDel = nil
        BattleSkillPool.Instance = nil
    end
end

function BattleSkillPool:GetGameObjectByIndex(idx)
    local ch = self.battle_displayer:getCh(idx)
    if ch then
        return ch.root_node
    else
        return nil
    end
end

local function syncDir(atker_ch, target_list)
    if not target_list or not next(target_list) then
        return
    end
    if BattleData.Instance:SceneData() == nil then
        return
    end
    local target_pos = Vector3.zero
    for _, tar_idx in pairs(target_list) do
        target_pos = target_pos + BattleData.Instance:SceneData():GetPos(tar_idx)
    end
    target_pos = target_pos / #target_list
    atker_ch:LookAtPos(target_pos)
end

function BattleSkillPool:Play(skill_ass_id, atker_idx, target_list, behiter_list, on_complete)
    local atker_ch = self.battle_displayer:getCh(atker_idx)
    if atker_ch == nil then
        -- LogError("atker_ch == nil")
        self.battle_displayer:ForceCompleteSkill(atker_idx, target_list, on_complete)
        return
    end
    atker_ch:EnableNCA(false)
    local skill_ass = gameSkillMgr:GetAsset(skill_ass_id)
    if skill_ass == nil then
        if skill_ass_id ~= 0 then
            LogD("skill_ass == nil", skill_ass_id)
        end
        self.battle_displayer:ForceCompleteSkill(atker_idx, target_list, on_complete)
        return
    end
    -- if not self.skill_asset_resident_data[skill_ass_id] then
    -- 	self.skill_asset_resident_data[skill_ass_id] = {}
    -- 	skill_ass:FillResidentData(self.skill_asset_resident_data[skill_ass_id])
    -- 	LogError(skill_ass_id, self.skill_asset_resident_data[skill_ass_id])
    -- end
    local skill_player = atker_ch:SkillPlayer()
    if skill_player == nil then
        -- LogError("skill_player == nil")
        self.battle_displayer:ForceCompleteSkill(atker_idx, target_list, on_complete)
        return
    end

    local on_com
    if skill_ass.NeedSyncDir then
        syncDir(atker_ch, target_list)
        on_com = function()
            if atker_ch and atker_ch.root_node and BattleData.Instance:SceneData() then
                atker_ch.root_node.transform.forward = BattleData.Instance:SceneData():GetForward(atker_idx)
            end
            if on_complete then
                on_complete()
            end
            if atker_ch then
                atker_ch:EnableNCA(true)
            end
        end
    else
        on_com = function ()
            if on_complete then
                on_complete()
            end
            if atker_ch then
                atker_ch:EnableNCA(true)
            end
        end
    end

    local param = Game.SkillPlayParam(atker_idx, target_list, behiter_list)
    self.cur_skill_param = param
    local play_suc = skill_player:Play(skill_ass, param)
    if play_suc ~= true then
        LogErrorBT("play_suc == nil")
        self.battle_displayer:ForceCompleteSkill(atker_idx, target_list, on_com)
        return
    end
    self.skill_callbacks[skill_player] = on_com
end

function BattleSkillPool:GetCurSkillParam()
    return self.cur_skill_param
end

-- function BattleSkillPool:GetResidentData(skill_ass_id)
-- 	return self.skill_asset_resident_data[skill_ass_id]
-- end

function BattleSkillPool:Register(skill_ass_id)
    if skill_ass_id and skill_ass_id ~= "" then
        gameSkillMgr:Register(skill_ass_id)
    end
end

function BattleSkillPool:Unregister(skill_ass_id)
    if skill_ass_id and skill_ass_id ~= "" then
        gameSkillMgr:Unregister(skill_ass_id)
    end
end

function BattleSkillPool:OnSkillMgrPlayAnim(animator, anim_name)
    if animator == nil then return end --warm up
    BodyAnim.S_PlayAnim(animator, anim_name)    
end

function BattleSkillPool:OnSkillMgrGetAnchorPos(param, anchor_type, re_tran_list)
    local scene_data = BattleData.Instance:SceneData()
    if param == nil or scene_data == nil then        --warm up
        re_tran_list:Add(gameSkillMgr.transform)
        re_tran_list:Clear()
        return false
    end
    -- Debuger.LogError("OnSkillMgrGetAnchorPos====idx=%s,anchor=%s",tostring(player_idx),tostring(anchor_type))

    -- local re_val = {}
    if anchor_type == pos_anchor.AttakerHand or anchor_type == pos_anchor.AtkerHandR then
        local player_idx = param.PlayerIdx
        local atker_ch = self.battle_displayer:getCh(player_idx)
        if atker_ch ~= nil then
            local pos_obj = atker_ch:FindChildByName(BoneName.RightHand)
            if pos_obj ~= nil then
                re_tran_list:Add(pos_obj.transform)
            else
                re_tran_list:Add(atker_ch:GetRoot().transform)
            end
        end
        if anchor_type == pos_anchor.AtkerHandR then
            return true
        end
    elseif anchor_type == pos_anchor.EachRootS or anchor_type == pos_anchor.EachCenterS then
        local player_idx = param.PlayerIdx
        local atker_ch = self.battle_displayer:getCh(player_idx)
        if atker_ch ~= nil then
            local pos_obj = anchor_type == pos_anchor.EachCenterS and atker_ch:CenterPoint() or atker_ch:GetRoot()
            -- local pos_obj = atker_ch:FindChildByName("Bip001 L Hand")
            if pos_obj ~= nil then
                re_tran_list:Add(pos_obj.transform)
            end
        end
    elseif anchor_type == pos_anchor.BehiterEachRoot or anchor_type == pos_anchor.BehiterEachCenter then
        local behiter_idxs = param.BehiterIdx
        local len = behiter_idxs.Length
        for i = 0, len - 1 do
            local behit_ch = self.battle_displayer:getCh(behiter_idxs[i])
            if behit_ch ~= nil then
                local pos_obj = anchor_type == pos_anchor.BehiterEachCenter and behit_ch:CenterPoint() or behit_ch:GetRoot()
                if pos_obj ~= nil then
                    re_tran_list:Add(pos_obj.transform)
                end
            end
        end
    elseif anchor_type == pos_anchor.CampRegion then
        local target_idxs = param.TargetIdxs
        if target_idxs and target_idxs.Length > 0 then
            re_tran_list:Add(scene_data:GetCampTransform(target_idxs[0]))
        end
    elseif anchor_type == pos_anchor.CampRegionS then
        local player_idx = param.PlayerIdx
        re_tran_list:Add(scene_data:GetCampTransform(player_idx or 3))
    elseif anchor_type == pos_anchor.RowRegion then
        local target_idxs = param.TargetIdxs
        local added_lines = {}
        local len = target_idxs.Length
        for i = 0, len - 1 do
            local tar_idx = target_idxs[i]
            local row_i = math.floor((tar_idx - 1) / 5) + 1
            if added_lines[row_i] == nil then
                re_tran_list:Add(scene_data:GetRowTransform(tar_idx))
                added_lines[row_i] = true
            end
        end
    elseif anchor_type == pos_anchor.WholeRegion then
        local target_idxs = param.TargetIdxs
        if target_idxs.Length > 0 then
            re_tran_list:Add(scene_data:GetWholeTransform(target_idxs[0]))
        end
    elseif anchor_type == pos_anchor.ImpactRegion then
        local target_idxs = param.TargetIdxs
        if target_idxs.Length > 0 then
            local temp = Vector3.zero
            local len = target_idxs.Length
            for i = 0, len - 1 do
                local tar_idx = target_idxs[i]
                if tar_idx ~= nil then
                    temp = temp + scene_data:GetPos(tar_idx)
                end
            end
            local go = GameObjPool:Get(skill_point_path)
            go.transform.position = temp / len
            go.transform.forward = scene_data:GetForward(target_idxs[0])
            TimeHelper:AddDelayTimer(
                function()
                    GameObjPool:Release(go)
                end,
                10
            )
            re_tran_list:Add(go.transform)
        end
    elseif anchor_type == pos_anchor.RowRegionS then
        local player_idx = param.PlayerIdx
        re_tran_list:Add(scene_data:GetRowTransform(player_idx or 3))
    elseif anchor_type == pos_anchor.ZeroPoint then
        local go = GameObjPool:Get(skill_point_path)
        TimeHelper:AddDelayTimer(
                function()
                    GameObjPool:Release(go)
                end,
                10
            )
        re_tran_list:Add(go.transform)
    else
        local target_idxs = param.TargetIdxs
        local len = target_idxs.Length
        for i = 0, len - 1 do
            local behit_ch = self.battle_displayer:getCh(target_idxs[i])
            if behit_ch ~= nil then
                local pos_obj = anchor_type == pos_anchor.EachCenter and behit_ch:CenterPoint() or behit_ch:GetRoot()
                if pos_obj ~= nil then
                    re_tran_list:Add(pos_obj.transform)
                end
            end
        end
    end
    return false
end

function BattleSkillPool:CheckAssetRegistered(skill_ass_id)
    return gameSkillMgr:CheckRegistered(skill_ass_id)
end

function BattleSkillPool:OnSkillPlayComplete(play_handle)
    local on_com = self.skill_callbacks[play_handle]
    if on_com ~= nil then
        self.skill_callbacks[play_handle] = nil
        on_com()
    end
end

function BattleSkillPool.SetHitEvent(func)
    Game.SkillManager.HitEventDel = func
end

function BattleSkillPool:BeginFlyerPath(path_inst, param, featureInstance)
    BattleFlyerPath.Run(path_inst, param, featureInstance)
end
-- function BattleSkillPool:OnHitTrigger(hiter_idx,behiter_idx)

-- end

function BattleSkillPool:UnloadUnusedAssets()
    gameSkillMgr:UnloadUnusedRef()
end

function BattleSkillPool:GetVirtualCamera()
    return self.active_virtual_camera
end

function BattleSkillPool:SetVirtualCamera(virtual_camera)
    self.active_virtual_camera = virtual_camera
end

function BattleSkillPool:ShowShakeCamera()
    local ser_data = LuaMonoBehaviour.FindMono(DataHolder)
    local shake_camera = ser_data.ShakeCamera
    if shake_camera then shake_camera:SetActive(true) end
    self.handle_ddsy = AudioMgr:PlayEffectHandle(AudioType.Scene, "DiDongShanYao")
end