BodyObj = BodyObj or BaseClass()

function BodyObj:__init(res_path, def_par_obj, need_wait_anim, battle_main)
    self.child_bodys = {} --key:body,val:att_obj_key
    self.res_path = res_path
    self.default_parent_obj = def_par_obj

    self.parent_body = nil
    self.obj = nil
    self.body_com = nil
    self.renderer_com = nil
    self.model_property_com = nil
    self.anim = BodyAnim.New(self)
    self.load_event_node = EventNode.New()
    self.is_show = true
    self.destroyed = false
    self.is_loading = true
    self.renderer_effect_datas = {}
    self.loaded_callback = nil
    self.need_wait_anim = need_wait_anim
    self.battle_main = battle_main
    self.child_cache = nil
    GameObjPool:RegisterPool(self.res_path, BindTool.Bind(self.onResLoaded, self), true)
end

function BodyObj:SetShow(show)
    if self.is_show ~= show then
        self.is_show = show
        if self.obj ~= nil then
            self.obj:SetActive(self.is_show)
        end
    end
end

function BodyObj:GetLoadEventNode()
    return self.load_event_node
end

function BodyObj:IsLoading()
    return self.is_loading
end

function BodyObj:SampleAnim(anim_name, time)
    if self.is_loading then
        self.anim.animator:SampleAnim(self.obj, anim_name, time)
    else
       self.handle_runner_anim = Runner.Instance:RunUntilTrue(
            function()
                if not self.is_loading then
                    if self.anim and self.anim.animator then
                        self.anim.animator:SampleAnim(self.obj, anim_name, time)
                    end
                    return true
                end
            end
        )
    end
end

function BodyObj:__delete()
    self.loaded_callback = nil
    self.destroyed = true
    self.child_cache = nil
    -- self:RevertRenderer()
    -- self.animator_com = nil
    --dettach all childs
    self.model_property_com = nil
    for bo, _ in pairs(self.child_bodys) do
        bo:DeattachFromParent()
    end

    self:DeattachFromParent(true)
    if self.obj ~= nil then
        GameObjPool:Release(self.obj)
        self.obj = nil
    end

    if self.res_path ~= nil then
        GameObjPool:UnregisterPool(self.res_path)
        self.res_path = nil
    end
    if self.load_event_node then
        self.load_event_node:DeleteMe()
        self.load_event_node = nil
    end
    if self.handle_runner_anim then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner_anim)
        self.handle_runner_anim = nil
    end
    if self.anim then
        self.anim:DeleteMe()
        self.anim = nil
    end
    for data, _ in pairs(self.renderer_effect_datas) do
        Game.RendererEffect.Unregister(data)
    end
    self.renderer_effect_datas = nil

    self:RevertRendererMaterial()
    self.renderer_com = nil
    self.MeshGhost = nil
end

function BodyObj:Path()
    return self.res_path
end

function BodyObj:onResLoaded(err)
    -- LogErrorHL(self.res_path, "onResLoaded")
    if self.destroyed then
        return
    end
    --set to parent body
    local par_obj = nil
    if self.parent_body ~= nil then
        par_obj = self.parent_body:getChildParentObj(self)
    end
    if par_obj == nil then
        par_obj = self.default_parent_obj
    end
    self.obj = GameObjPool:Get(self.res_path, par_obj)
    if self.obj == nil then
        Debuger.LogError("BodyObj:onResLoaded---GetObjFromPool is NULL,path===%s", self.res_path)
        self.is_loading = false
        return
    end
    --如果遇到更加特别的情况逻辑改为判断tag MainBody这样可以保证准确那个模型的renderer这样是最好的
    self.renderer_com = self.obj:GetComponentInChildren(typeof(UnityEngine.SkinnedMeshRenderer))--typeof(UnityEngine.Renderer))
    if self.renderer_com == nil then
        --尝试获取meshrender
        self.renderer_com = self.obj:GetComponentInChildren(typeof(UnityEngine.MeshRenderer))--typeof(UnityEngine.Renderer))
        if self.renderer_com and not self.renderer_com.gameObject:HasComponent(typeof(Game.RendererEffect)) then
            self.renderer_com = nil
        end
    end
    self.model_property_com = self.obj:GetComponent(typeof(Game.ModelCameraProperty))
    self.body_com = self.obj:GetLuaComponent(BodyObjComponent:GetTypeName())
    if self.battle_main == true then
        self:GetMeshGhostCom()
    end
    --set child bodys
    for bo, o_k in pairs(self.child_bodys) do
        local ch_obj = bo:getObj()
        if ch_obj ~= nil and self.body_com then
            local ch_par_obj = self.body_com[o_k]
            ch_obj:SetParent(ch_par_obj)
            ch_obj:ResetTransform()
        end
    end
    if self.is_show == false then
        self.obj:SetActive(self.is_show)
    end
    local ap = self.anim:SetAnimator(self.obj:GetComponent(typeof(UnityAnim)), self.need_wait_anim)
    if self.need_wait_anim then
        self.load_event_node.action = function(loaded)
            -- Game.MyTool.Pause()
            self.load_event_node:Set(loaded)
        end
        self.load_event_node:AddChild(ap)
    else
        self.load_event_node:Set(true)
    end
    self.load_event_node:Set(true)
    self.is_loading = false
    if self.loaded_callback then
        self.loaded_callback()
    end
end

function BodyObj:GetModelCameraPropertyCom()
    return self.model_property_com
end

function BodyObj:GetRendererMaterialCom()
    if not self.RendererMaterial and self.renderer_com then
        self.RendererMaterial = self.renderer_com:GetOrAddComponent(typeof(Game.RendererMaterial))
        self.RendererMaterial:Init()
    end
    return self.RendererMaterial
end

function BodyObj:RevertRendererMaterial()
    local RendererMaterial = self:GetRendererMaterialCom()
    if RendererMaterial then
        RendererMaterial:Revert()
    end
end

function BodyObj:RemoveRendererMaterial(layer)
    local RendererMaterial = self:GetRendererMaterialCom()
    if RendererMaterial then
        RendererMaterial:Remove(layer)
    end
end

function BodyObj:SetRendererMaterial(matOrMats, layer, copyPropertieNames)
    local RendererMaterial = self:GetRendererMaterialCom()
    if RendererMaterial then
        RendererMaterial:Add(matOrMats, layer)
        if copyPropertieNames then
            RendererMaterial:CopyPropertiesFromBase(matOrMats, copyPropertieNames)
        end
    end
end

function BodyObj:SetRenderEffect(data)
    if not self.obj then
        return
    end
    local res = self.obj:GetComponentsInChildren(typeof(Game.RendererEffect))
    Game.RendererEffect.Register(data)
    self.renderer_effect_datas[data] = true
    for i = 0, res.Length - 1 do
        local re = res[i]
        re:Play(
            data,
            function(anim)
                re:Revert()
                Game.RendererEffect.Unregister(data)
                self.renderer_effect_datas[data] = nil
            end
        )
    end
end

function BodyObj:AttachBody(obj_key, body)
    -- LogErrorHL(self.res_path, "AttachBody")
    body:DeattachFromParent(true)

    body.parent_body = self
    self.child_bodys[body] = obj_key

    if self.obj == nil then
        return
    end

    local ch_obj = body:getObj()
    if ch_obj == nil then
        return
    end
    if self.body_com == nil then
        LogError("AttachBody self.body_com is nil, res_path is",self.res_path)
    end
    ch_obj:SetParent(self.body_com[obj_key])
    ch_obj:ResetTransform()
end

function BodyObj:AttachToParent(obj_key, par_bo)
    -- LogErrorHL(self.res_path, "AttachToParent")
    self:DeattachFromParent(true)

    self.parent_body = par_bo
    self.parent_body.child_bodys[self] = obj_key

    if self.obj ~= nil then
        local par_obj = self.parent_body:getChildParentObj(self)
        if par_obj ~= nil then
            self.obj:SetParent(par_obj)
            self.obj:ResetTransform()
        else
            self.obj:SetParent(self.default_parent_obj)
        end
    end
end

function BodyObj:FindChildByName(name)
    if self.obj == nil then
        return nil
    end
    if self.child_cache == nil then
        self.child_cache = {}
    end
    if not self.child_cache[name] then
        local children = self.obj:GetComponentsInChildren(typeof(UnityEngine.Transform), true)
        if children.Length > 0 then
            for i = 0, children.Length - 1 do
                if children[i].gameObject.name == name then
                    self.child_cache[name] = children[i]
                    return self.child_cache[name]
                end
            end
        end
        -- LogError(self.res_path, "中没有", name)
    else
        return self.child_cache[name].gameObject
    end
    -- else
    --     LogError(self.body_show.main_body.res_path, "中没有", name)
    -- end
end

function BodyObj:DeattachFromParent(data_only)
    if self.parent_body == nil then
        return
    end
    self.parent_body.child_bodys[self] = nil
    self.parent_body = nil
    if not (data_only == true) and self.obj ~= nil and not self.obj:Equals(nil) then
        self.obj:SetParent(self.default_parent_obj)
    end
end

function BodyObj:Anim()
    return self.anim
end

function BodyObj:ResetTransform()
    if self.obj ~= nil then
        self.obj:ResetTransform()
    end
end

function BodyObj:getChildParentObj(ch_body)
    if self.obj == nil then
        return nil
    end
    local att_obj_key = self.child_bodys[ch_body]
    if att_obj_key == nil then
        return nil
    end
    local re = self.body_com and self.body_com[att_obj_key] or nil
    if re == nil then
        Debuger.LogErrorSD("BodyObj:getChildParentObj ERROR att_obj_key===%s,path===%s", tostring(att_obj_key), self:Path())
    end
    return re
end

function BodyObj:getObj()
    return self.obj
end

function BodyObj:SetScale(size)
    if self.obj then
        self.obj.transform.localScale = Vector3.New(size, size, size)
    end
end

function BodyObj:SetParent(par_obj)
    self.obj:SetParent(par_obj)
end

function BodyObj:SetLoadedCallback(callback)
    self.loaded_callback = callback
    if self.is_loading == false then
        self.loaded_callback()
    end
end
function BodyObj:GetMeshGhostCom()
    if not self.MeshGhost and self.obj ~= nil then
        self.MeshGhost = self.obj:GetOrAddComponent(typeof(Game.MeshGhost))
        self.MeshGhost:Init(0.01, 0.2, PreloadCtrl.dodge_mat)
    end
    return self.MeshGhost
end
function BodyObj:MeshGhostInit(interval, duration)
   local com = self:GetMeshGhostCom()
   com:Init(interval, duration, PreloadCtrl.dodge_mat)
end

BodyObjComponent = BodyObjComponent or DeclareMono("BodyObjComponent")

local anim_eve_key = {
    ["idle"] = ChAnimType.Idle,
    ["run"] = ChAnimType.Run,
    ["death"] = ChAnimType.Death,
    ["behit"] = ChAnimType.Behit,
    ["attack"] = ChAnimType.Attack,
    ["skill"] = ChAnimType.Skill,
    ["defence"] = ChAnimType.Defence,
    ["battle_run"] = ChAnimType.BattleRun,
    ["counter_attack"] = ChAnimType.CounterAttack,
    ["gather"] = ChAnimType.Gather,
    ["fall_down"] = ChAnimType.FallDown,
    ["weapon_show"] = ChAnimType.WeaponShow,
    ["show"] = ChAnimType.Show,
    ["bianshen"] = ChAnimType.BianShen,
    ["skill1"] = ChAnimType.Skill1,
    ["skill2"] = ChAnimType.Skill2,
    ["skill3"] = ChAnimType.Skill3,
}

BodyAnim = BodyAnim or BaseClass()

function BodyAnim:__init(body_obj)
    self.body_obj = body_obj
    self.animator = nil
    self.skill_player = nil
    self.cur_anim = ChAnimType.Idle
    -- self.enter_handle = nil

    self.anim_listen_handles = {} --key:animator listener handle,value:eventName

    self.anim_override = nil
    self.anim_override_path = nil
    self.anim_override_data = nil
	self.anim_synchronizer = nil
	self.anim_synchronizer_data = nil

    self.is_overrider_loading = false

    --{
    --    [string] = {[string] = {funcs}}
    --}
    --key1:event_name
    --key2:anim_name
    --value:func_list
    self.wait_anim_event_handle = {
        ["enter"] = {},
        ["hit"] = {},
        ["end"] = {},
        ["behit_back"] = {},
    }

    self.listening_flag = {}

end

function BodyAnim:__delete()
    self.wait_anim_event_handle = nil
    -- if self.enter_handle ~= nil then
    --     self.animator:UnlistenEvent("enter", self.enter_handle)
    --     self.enter_handle = nil
    -- end
    -- if self.animator ~= nil and self.animator.runtimeAnimatorController ~= nil then
    --     self.animator.runtimeAnimatorController:set_Item(AnimClip.Loop,nil)
    --     self.animator.runtimeAnimatorController:set_Item(AnimClip.LoopPD,nil)
    -- end
    if self.animator and not self.animator:Equals(nil) then
        for hand, eve_n in pairs(self.anim_listen_handles) do
            self.animator:UnlistenEvent(eve_n, hand)
        end
    end

    self.anim_listen_handles = {}
    self.listening_flag = {}

    if self.cullingModeCache and self.animator and not self.animator:Equals(nil) then
        self.animator.cullingMode = self.cullingModeCache
    end
    if self.ao_load then
        self.ao_load:DeleteMe()
        self.ao_load = nil
    end
    if self.ap then
        self.ap:DeleteMe()
        self.ap = nil
    end

    self.animator = nil

    self.anim_override_path = nil
    self.anim_override_data = nil
	self.anim_synchronizer_data = nil
    if self.anim_override ~= nil then
        self.anim_override:SetOverrideData(nil)
    end
	if self.anim_synchronizer ~=nil then
		 self.anim_synchronizer:SourceAnimator(nil)
	end
end

function BodyAnim:SetAnimator(anim, need_wait_anim)
    self.animator = anim
    if self.animator ~= nil then
        self.animator.logWarnings = false;
        self:SetAnimActive(true)
        if need_wait_anim then
            self.cullingModeCache = self.animator.cullingMode
            self.animator.cullingMode = "AlwaysAnimate"
        end

        local eve_name = "enter"
        local handle = self.animator:ListenEvent(eve_name, BindTool.Bind(self.OnEnterAnimatorState, self))
        self.anim_listen_handles[handle] = eve_name
        self.listening_flag[eve_name] = true

        eve_name = "hit"
        handle = self.animator:ListenEvent(eve_name, BindTool.Bind(self.OnAnimatorHit, self))
        self.anim_listen_handles[handle] = eve_name
        self.listening_flag[eve_name] = true


        self.anim_override = self.animator:GetComponent(typeof(Game.AnimatorClipOverride))
        if self.anim_override_data ~= nil and self.anim_override ~= nil then
            self.anim_override:SetOverrideData(self.anim_override_data)
        end
		if self.anim_synchronizer_data then
			self.anim_synchronizer = self.animator:GetComponent(typeof(Game.AnimatorSynchronizer))
			if self.anim_synchronizer then
				self.anim_synchronizer:SourceAnimator(self.anim_synchronizer_data)
			end
		end
        local ap = EventNode.New()
        self.ap = ap 
        self:WaitAnimEvent(
            self.cur_anim,
            "enter",
            function()
                ap:Set(true)
                -- Game.MyTool.Pause()
            end
        )
        self:PlayAnim(self.cur_anim)
        return self.ap
    end
end

function BodyAnim:SkillPlayer()
    if self.animator == nil then
        return nil
    end
    if self.skill_player == nil then
        self.skill_player = Game.SkillAssetPlayer.GetOrCreate(self.animator.gameObject)
    end
    return self.skill_player
end

function BodyAnim.S_PlayAnim(animator, anim)
    if animator.enabled == false then
        return
    end
    if anim == ChAnimType.Idle then
        animator:SetInteger(APH("state"), 0)
    elseif anim == ChAnimType.Run then
        animator:SetInteger(APH("state"), 1)
    elseif anim == ChAnimType.BattleRun or anim == ChAnimType.FishIdle then
        animator:SetInteger(APH("state"), 2)
    elseif anim == ChAnimType.FishWait then
        animator:SetInteger(APH("state"), 3)
    elseif anim == ChAnimType.LaganIdle then
        animator:SetInteger(APH("state"), 4)
    elseif anim == ChAnimType.Gather then
        animator:SetInteger(APH("state"), 5)
    elseif anim == ChAnimType.Death then
        animator:SetInteger(APH("state"), 8)
    elseif anim == ChAnimType.Run2 then
        animator:SetInteger(APH("state"), 9)
    elseif anim == ChAnimType.FaMu then
        animator:SetInteger(APH("state"), 10)
    elseif anim == ChAnimType.WaKuang then
        animator:SetInteger(APH("state"), 11)
    elseif anim == ChAnimType.ShouShang or anim == ChAnimType.XinMoPreWeak then
        animator:SetInteger(APH("state"), 16)
    elseif anim == ChAnimType.XinMoIdle then
        animator:SetInteger(APH("state"), 16)
    elseif anim == ChAnimType.KunBang then
        animator:SetInteger(APH("state"), 15)
    else
        animator:SetTrigger(APH(anim))
        animator:SetBool(APH("to_unloop"), true)
    end
end
function BodyAnim:PlayAnim(anim, anim_params)
    if ChAnimType.IsLoopAnim(anim) then
        self.cur_anim = anim
    end
    if not self.animator or not self.animator.enabled then
        return
    end
    if ChAnimType.IsDLoopAnim(anim) and anim_params ~= nil then
        local item = ClipGeter.GetClip(anim_params[1])
        if item then
            self.animator.runtimeAnimatorController:set_Item(AnimClip.Loop, item)
        else
            anim = ChAnimType.Idle
        end
        if anim_params[2] then
            item = ClipGeter.GetClip(anim_params[2])
            if item then
                self.animator.runtimeAnimatorController:set_Item(AnimClip.LoopPD, item)
            end
        else
            self.animator.runtimeAnimatorController:set_Item(AnimClip.LoopPD, nil)
        end
        -- ClipGeter.cache_controller[anim_params] = self.animator.runtimeAnimatorController
    end
    BodyAnim.S_PlayAnim(self.animator, anim)

    return true
end

function BodyAnim:OnEnterAnimatorState(key, anim_st_info)
    self.cur_anim = anim_eve_key[key]
    self:executeWaiAnimEventHandle("enter",key)
end

function BodyAnim:SetAnimActive(bool)
    if self.animator then
        self.animator.enabled = bool
    end
end

function BodyAnim:SetAnimSynchronizer(animator)
	self.anim_synchronizer_data = animator
	if self.animator then
		if self.anim_synchronizer == nil then
			self.anim_synchronizer = self.animator:GetComponent(typeof(Game.AnimatorSynchronizer)) 
		end
		if self.anim_synchronizer then
			self.anim_synchronizer:SourceAnimator(animator)
		end
	end
end

function BodyAnim:SetAnimOverride(path)
    if self.anim_override_path == path then
        return
    end
    self.anim_override_path = path
    if StringIsEmpty(path) then
        self.anim_override_data = nil
        if self.anim_override ~= nil then
            self.anim_override:SetOverrideData(self.anim_override_data)
        end
        return
    end
    if self.ao_load then
        self.ao_load:DeleteMe()
        self.ao_load = nil
    end
    
    local ao_load = EventNode.New()
    self.ao_load = ao_load
    self.is_overrider_loading = true
    ResMgr:LoadObject(
        self.anim_override_path,
        function(data)
            self.is_overrider_loading = false
            if path ~= self.anim_override_path then
                ao_load:Set(true)
                return
            end
            self.anim_override_data = data
            if self.anim_override ~= nil then
                local clip1,clip2
                if self.animator ~= nil and self.animator.runtimeAnimatorController ~= nil then
                    clip1 = self.animator.runtimeAnimatorController:get_Item(AnimClip.Loop)
                    clip2 = self.animator.runtimeAnimatorController:get_Item(AnimClip.LoopPD)
                end
                self.anim_override:SetOverrideData(self.anim_override_data)
                if self.animator ~= nil and self.animator.runtimeAnimatorController ~= nil then        --传承下去！
                    if clip1 ~= nil then
                        self.animator.runtimeAnimatorController:set_Item(AnimClip.Loop,clip1)
                    end
                    if clip2 ~= nil then
                        self.animator.runtimeAnimatorController:set_Item(AnimClip.LoopPD,clip2)
                    end
                end
            end
            ao_load:Set(true)
            -- Game.MyTool.Pause()
        end,
        true
    )
    return ao_load
end

function BodyAnim:IsOverriderLoading()
    return self.is_overrider_loading
end

function BodyAnim:WaitAnimEnd(anim, func)
    -- if self.animator == nil then
    --     func()
    --     return
    -- end
    -- local handle_holder = {handle = nil}
    -- handle_holder.handle =
    --     self.animator:ListenEvent(
    --     "end",
    --     function(key, anim_st_info)
    --         if anim_eve_key[key] == anim then
    --             self.anim_listen_handles[handle_holder.handle] = nil
    --             self.animator:UnlistenEvent("end", handle_holder.handle)
    --             func()
    --         end
    --     end
    -- )
    -- self.anim_listen_handles[handle_holder.handle] = "end"
    self:WaitAnimEvent(anim, "end", func)
end

function BodyAnim:WaitAnimEvent(anim, eve_name, func)
    if self.animator == nil then
        func()
        return
    end
    if self.wait_anim_event_handle == nil then
        func()
        return
    end
    local anim_funcs = self.wait_anim_event_handle[eve_name]
    if anim_funcs == nil then 
        BuglyReport("BodyAnim:WaitAnimEvent ERROR!Unknow event_name:",eve_name,anim)
        func()
        return 
    end
    local funcs = anim_funcs[anim]
    if funcs == nil then
        funcs = {}
        anim_funcs[anim] = funcs
    end
    table.insert(funcs,func)

    if self.listening_flag[eve_name] ~= true then   --未注册过的函数，需要注册一下
        if eve_name == "end" then
            local handle = self.animator:ListenEvent(eve_name, BindTool.Bind(self.OnAnimatorEnd, self))
            self.anim_listen_handles[handle] = eve_name
            self.listening_flag[eve_name] = true
        elseif eve_name == "behit_back" then
            local handle = self.animator:ListenEvent(eve_name, BindTool.Bind(self.OnAnimatorBehitBack, self))
            self.anim_listen_handles[handle] = eve_name
            self.listening_flag[eve_name] = true
        end
    end
    -- local handle_holder = {handle = nil}
    -- handle_holder.handle =
    --     self.animator:ListenEvent(
    --     eve_name,
    --     function(key, anim_st_info)
    --         if anim_eve_key[key] == anim then
    --             -- self.anim_listen_handles[handle_holder.handle] = nil
    --             -- if self.animator ~= nil then
    --             --     LogError("UnRegister====",handle_holder.handle:GetHashCode(),eve_name,Time.frameCount)
    --             --     self.animator:UnlistenEvent(eve_name, handle_holder.handle)
    --             -- end
    --             func()
    --         end
    --     end
    -- )
    -- self.anim_listen_handles[handle_holder.handle] = eve_name
end
function BodyAnim:GetWaitAnimEventHandle()
    return self.wait_anim_event_handle
end
function BodyAnim:SetWaitAnimEventHandle(handles)
    self.wait_anim_event_handle = handles
end

function BodyAnim:SetOnAnimHit(func)
    self.on_anim_hit = func
end

function BodyAnim:OnAnimatorHit(key, anim_st_info)
    if self.on_anim_hit ~= nil then
        self.on_anim_hit()
    end
    self:executeWaiAnimEventHandle("hit",key)
end

function BodyAnim:OnAnimatorEnd(key,anim_st_info)
    self:executeWaiAnimEventHandle("end",key)
end

function BodyAnim:OnAnimatorBehitBack(key,anim_st_info)
    self:executeWaiAnimEventHandle("behit_back",key)
end

function BodyAnim:executeWaiAnimEventHandle(eve_name,anim_key)
    if self.wait_anim_event_handle == nil then return end
    local anim_funcs = self.wait_anim_event_handle[eve_name]
    if anim_funcs == nil then 
        BuglyReport("BodyAnim:executeWaiAnimEventHandle ERROR!Unknow event_name:",eve_name,anim_eve_key[anim_key])
        return 
    end
    local anim = anim_eve_key[anim_key]
    local funcs = anim_funcs[anim]
    if funcs == nil then
        return
    end
    anim_funcs[anim] = nil  --先清空再回调
    for _,f in ipairs(funcs) do
        f()
    end
end

function BodyAnim:UpdateAnimator(time)
    if self.animator then
        self.animator:Update(time)
    end
end

-- function BodyAnim:SampleAnimation(clipName, time)
--     if self.animator and self.body_obj.obj then
--         self.animator:GetAnimationClip(clipName):SampleAnimation(self.body_obj.obj, time)
--     end
-- end

function BodyAnim:CurAnim()
    return self.cur_anim
end
function BodyAnim:AnimClipsEmpty(name)
    if self.animator and self.animator.runtimeAnimatorController then
        local list = self.animator.runtimeAnimatorController.animationClips:ToTable()
        for _, v in pairs(list) do
            if v.name == name then
                return false
            end
        end
    end
    return true
end

function BodyAnim:HasAnim(animType)
    if self.animator == nil then
        return false
    end
    local anim_con = self.animator.runtimeAnimatorController
    if anim_con == nil then
        return false
    end
    local type_name_find = string.find(tostring(anim_con),"(UnityEngine.AnimatorOverrideController)")
    if type_name_find then 
        -- if VersionTag.HasTag(VersionTag.AnimCon_HasOverrideAnim) then
        --     return anim_con:HasOverrideAnim("d_".. animType)
        -- else
        --     if self.animator.name == "200" then
        --         LogError(animType,"|", anim_con:get_Item("d_" .. animType).name,"|",self.animator.name)
        --     end
        --     local ori_clip = anim_con:get_Item(animType)
        --     return ori_clip ~= nil
        -- end

        local d_anim_name = "d_" .. animType
        local ori_clip = anim_con:get_Item(d_anim_name)
        return ori_clip ~= nil and ori_clip.name ~= d_anim_name
    else        --不是AnimatorOverrideContrller
        local list = anim_con.animationClips:ToTable()
        for _, v in pairs(list) do
            if v.name == animType then
                return true
            end
        end
        return false
    end

end

ClipGeter = ClipGeter or {}
ClipGeter.cache_clip = {}
ClipGeter.cache_controller = {}
function ClipGeter.GetClip(clipName,func)
    local path = ResHelper.AnimPath(clipName)
    if ClipGeter.cache_clip[path] ~= nil then
        local temp = ClipGeter.cache_clip[path]
        if func ~= nil then
            func()
        end
        return temp
    end
    ResMgr:LoadObject(
        path,
        function(data)
            ClipGeter.cache_clip[path] = data
            if func ~= nil then
                func()
            end
            return ClipGeter.cache_clip[path]
            -- self.animator.runtimeAnimatorController:set_Item("d_loop", data)
        end,
        true
    )
end