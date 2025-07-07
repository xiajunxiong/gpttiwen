ParadeFloatCh = ParadeFloatCh or BaseClass(SceneObj)

function ParadeFloatCh:__init()
    self.vo = nil
end

function ParadeFloatCh:__delete()
    MarryData.Instance:SetParadeFloatInfo()
    Runner.Instance:RemoveRunUntilTrue(self.wait_load_1)
    Runner.Instance:RemoveRunUntilTrue(self.wait_load_2)
    Runner.Instance:RemoveRunUntilTrue(self.camera_wait_load)
    MainViewData.Instance:RemoveObjVo(self.role_vo_1)
    MainViewData.Instance:RemoveObjVo(self.role_vo_2)
    for k,v in pairs(self:GetHostInfo()) do
        local board = MainViewData.Instance:GetBoard([[role_id]],v.uid)
        if board and board.ch_vo and board.ch_vo.root_obj then
            board.Follow.Target = board.ch_vo.root_obj.transform
        end
        local ch_obj = SceneCtrl.Instance:Objs():GetObjectByObjId(v.obj_id)
        if ch_obj and ch_obj.body then
            ch_obj:SetVisible(true)
        end
    end
    if self:IsHasSelf() == true then
        SceneCtrl.Instance:CurScene():ResetCamera()
    end
    Delete(self,[[role_obj_1]])
    Delete(self,[[role_obj_2]])
    if self.root_node ~= nil then
        if self.nav_agent_enabled == true then
            self.nav_agent.enabled = false
            self.nav_agent_enabled = false
        end
        local release_func = function ()
            if self.collider ~= nil then
                self.collider.enabled = true
            end
            if self.body ~= nil then
                self.body:DeleteMe()
                self.body = nil
            end
            GameObjPool:Release(self.root_node)
        end
    end
    if self.effect_handle then
        SE:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if self.effect_handle_1 then
        SE:Stop(self.effect_handle_1)
        self.effect_handle_1 = nil
    end
    self.click_camera = nil
end

function ParadeFloatCh:FlushDrawer()
    self.body:SetData(string.format(DrawerHelper.GetRidePath(self.vo.res_id)))
end

function ParadeFloatCh:InitEnd()
    for i,v in ipairs(self:GetHostInfo()) do
        if self["role_obj_"..i] == nil then
            self["role_obj_"..i] = self:CreateBodyObj(v,i)
        end
    end
    --隐藏角色UI图标和自身
    for k,v in pairs(self:GetHostInfo()) do
        local board = MainViewData.Instance:GetBoard([[role_id]],v.uid)
        if board ~= nil then
            board.Follow.Target = nil
        end
        local ch_obj = SceneCtrl.Instance:Objs():GetObjectByObjId(v.obj_id)
        if ch_obj and ch_obj.body then
            ch_obj:SetVisible(false)
        end
    end
    self:PlayShowEffect(self:GetIsStop())
    --强制设置摄像机
    self:ForceCamera()
end

function ParadeFloatCh:CreateBodyObj(info,index)
    local appearance = CommonStruct.AppearanceParam(info)
    appearance.fashion_body_id = MarryConfig.FASHION_BODY_ID
    local res_id = ResHelper.RoleResId(appearance.avatar_type, 
    appearance.fashion_body_id, appearance.advance_time)
    local path = DrawerHelper.GetRolePath(res_id, appearance.color)
    local obj = BodyObj.New(path, self.root_node)
    Runner.Instance:RemoveRunUntilTrue(self["wait_load_"..index])
    self["wait_load_"..index] = Runner.Instance:RunUntilTrue(function()
        if not obj:IsLoading() and self.body.main_body.body_com then
            self.body.main_body:AttachBody("Dummy00"..index, obj)
            obj:SampleAnim(ChAnimType.Idle,0.1)
            obj:getObj().name = res_id
            obj:ResetTransform()
            if self["role_vo_"..index] == nil then
                self["role_vo_"..index] = CommonStruct.SceneBoardParam({
                    obj_id = info.obj_id,
                    name = info.role_name,
                    role_name = info.role_name,
                    obj_type = SceneObjType.Statue,
                    appearance = appearance,
                    root_obj = obj:getObj(),
                })
                MainViewData.Instance:AddObjVo(self["role_vo_"..index])
            end
            return true
        end
    end,true)
    return obj
end

function ParadeFloatCh:PlayShowEffect(is_stop)
    if self.effect_handle then
        SE:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if self.effect_handle_1 then
        SE:Stop(self.effect_handle_1)
        self.effect_handle_1 = nil
    end
    if is_stop == 0 then
        return
    end
    if self.effect_handle == nil then
        local effect_config = SceneEffectConfig.EffectList["ParadeFloatEffect"..self:GetFloatType()]
        self.effect_handle = SE:Play(effect_config, self:GetPos(), function(obj, key, time)end, self.root_node)
    end
    if self:GetFloatType() == 2 and self.effect_handle_1 == nil then
        self.effect_handle_1 = SE:Play(SceneEffectConfig.EffectList.ParadeFloatEffect1, self:GetPos(), function(obj, key, time)end, self.root_node)
    end
end

function ParadeFloatCh:GetHostInfo()
    return self.vo.info.host_info
end

function ParadeFloatCh:GetFloatType()
    return self.vo.info.float_type or 1
end

function ParadeFloatCh:GetIsStop()
    return self.vo.info.is_stop
end

function ParadeFloatCh:IsHasSelf()
    local is_has_self = false
    for k,v in pairs(self:GetHostInfo()) do
        if v.uid == RoleData.Instance:GetRoleId() then
            is_has_self = true
        end
    end
    return is_has_self
end

function ParadeFloatCh:MoveTo(pos, server_sync, end_func)
    SceneCh.MoveTo(self,pos, server_sync, end_func)
    if self.click_camera == nil then
        self:PlayShowEffect(self:GetIsStop())
        self:ForceCamera()
        self.click_camera = true
    end
end

--强制设置摄像机
function ParadeFloatCh:ForceCamera()
    if self:IsHasSelf() == true then
        Runner.Instance:RemoveRunUntilTrue(self.camera_wait_load)
        self.camera_wait_load = Runner.Instance:RunUntilTrue(function()
            if SceneLogic.Instance:SceneID() == SceneLogic.Instance:MainCityID() and SceneCtrl.Instance:CurScene():LoadingComplete() and SceneCtrl.Instance:CurScene():GetClassType() == GameScene then
                SceneLogic.Instance:ForceCamera(self.root_node)
                return true
            end
        end,true)
    end
end