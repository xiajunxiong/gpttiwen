TouchBehavior = TouchBehavior or BaseClass()
function TouchBehavior:__init()
    self.touch_helper = Game.SceneTouchHelper
    self.last_hit_pos = nil
    self.touch_began_handle = EventSys:Bind(GlobalEventKeys.TOUCH_BEGAN, BindTool.Bind(self.onTouchBegan, self))
    self.touch_moved_handle = EventSys:Bind(GlobalEventKeys.TOUCH_MOVED, BindTool.Bind(self.onTouchMoved, self))
    self.touch_ended_handle = EventSys:Bind(GlobalEventKeys.TOUCH_ENDED, BindTool.Bind(self.onTouchEnded, self))
end

function TouchBehavior:__delete()
    EventSys:UnBind(self.touch_began_handle)
    EventSys:UnBind(self.touch_moved_handle)
    EventSys:UnBind(self.touch_ended_handle)
end

function TouchBehavior:onTouchBegan()
    RabbitRaceData.Instance:OnTouchEndEvent()
    StarTreasureMapData.Instance:SetAutoFlag(false)
    if RoleSettingData.Instance:IsShowJoyStick() and not JoyStickData.Instance:IsHideJoyStick() then 
        local hit, pos, obj = self.touch_helper.Raycast(nil, nil)
        if hit and obj ~= nil and obj.layer == GameObjLayer.ColliderRole then
            return 
        end 
        JoyStickData.Instance:ClearTimer()
        JoyStickData.Instance:SetFirstClick(UnityEngine.Input.mousePosition)
    end
end

function TouchBehavior:onTouchMoved()
    if not RoleSettingData.Instance:IsShowJoyStick() or not JoyStickData.Instance:CheckAreaJoyStick() 
        or JoyStickData.Instance:IsHideJoyStick() then 
        if MainPlayer == nil then
            return
        end
        local hit, pos, obj = self.touch_helper.Raycast(nil, nil)
        if not hit then
            return
        end
        if obj.layer == GameObjLayer.ColliderScene then
            if Vector3.Distance2(MainPlayer:GetPos(), pos) > 0.6 then
                MainPlayer:MoveTo(pos, true)
            end
        end
        return 
    end 
    JoyStickData.Instance:SetPosClick(UnityEngine.Input.mousePosition)
    JoyStickData.Instance:ClearTimer()
end

function TouchBehavior:onTouchEnded()
    if MainPlayer == nil then
        return
    end
    if IS_EDITOR == false and UnityEngine.Input.touchCount > 1 then
        return 
    end
    if JoyStickData.Instance.pos_change_click.StartJoyStick and RoleSettingData.Instance:IsShowJoyStick() then 
        JoyStickData.Instance:ClearJoyStickInfo()
        return 
    end 
    if JoyStickData.Instance:IsFrameTimer() then 
        JoyStickData.Instance:ClearJoyStickInfo() 
    end 
    if LuckyRingData.Instance:InRing() then 
        return 
    end 

    local hit, pos, obj = self.touch_helper.Raycast(nil, nil)
    if not hit then
        if pos.y > 0 then
            MainPlayer:MoveTo(pos, true)
            local dragon = TeamCtrl.Instance:Dragon()
            if dragon then
                dragon:Step(0)
            end
        end
        return
    end
    self.last_hit_pos = nil
    --梦渊笔录场景特殊处理
    DreamNotesCtrl.Instance:AutoFunctionEvent()
    
    AutoInvokerCtrl.Instance:ClearCommands()
    ReputationCtrl.Instance:ClearCache()
    if obj.layer == GameObjLayer.ColliderScene then
        if Vector3.Distance2(MainPlayer:GetPos(), pos) > 0.6 then
            MainPlayer:MoveTo(pos, true)
            local pos = MainPlayer:GetMoveEndRealPos()
            -- 挂机时有设置自动抓宠就中断抓宠
            if FightData.Instance:IsSetAutoSealPet() and FightData.Instance:IsSealing() then
                FightData.Instance:SetSealing(false)
                FightCtrl.Instance:OpenAutoSealInfoView(FightData.OPEN_AUTO_SEAL_INFO_TYPE.INTERRUPT)
                TaskData.Instance:TaskChainSubContinue()
            end
            SE:Play(SceneEffectConfig.EffectList.ClickEffect, pos)
        end
    elseif obj.layer == GameObjLayer.ColliderRole then
        SceneCtrl.Instance:Objs():ForeachAllObj(function(scene_obj)
            if not (scene_obj.root_node == obj) then
                return false
            end
            if self:SpecialCondition(scene_obj.vo) then
                return true
            end
            if scene_obj:Type() == SceneObjType.Role then
                SceneCtrl.Instance:Logic():ClickToRole(scene_obj)
            else
                SceneCtrl.Instance:Logic():ClickToObj(scene_obj, scene_obj.vo.is_server)
            end
            return true
        end)
    end
end

function TouchBehavior:SpecialCondition(vo)
    if SCENE_TYPE.PLATFORM_BATTLE == SceneData.Instance:GetMode() then
        SceneLogic.Instance:AutoToNpc(vo.seq, nil, false, false)
        return true
    end
    return false
end