DeepDarkFantasyModel = DeepDarkFantasyModel or DeclareMono("DeepDarkFantasyModel",UIWFlushPanel)
function DeepDarkFantasyModel:DeepDarkFantasyModel()
    self.data = DeepDarkFantasyData.Instance
    self.role_obj = nil
    self.boss_obj = nil

    self.data_cares = {
        {data = self.data.view_data, func = self.CreateActor,keys = {"flush_num"}},
        {data = self.data.model_data, func = self.AdjustModel,keys = {"cur_state","start"},init_call = false},
        {data = self.data.model_data, func = self.AdjustModel,keys = {"start"}},
    }
    -- self:CreateActor()

    self.Anim:ListenEvent("idle",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.data:OperateModelData("is_idle",1) 
            self.data:OperateModelData("cur_state",0) 
        end 
        if eve_param == "exit" then 
            self.data:OperateModelData("is_idle",0) 
        end 
    end)

    self.Anim:ListenEvent("enter_layer",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.data:OperateModelData("is_enter_layer",1) 
            self.data:EnterLayer()
        end 
        if eve_param == "exit" then 
            self.data:OperateModelData("is_enter_layer",0) 
        end 
       
    end)

    self.Anim:ListenEvent("go_next",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.data:OperateModelData("is_go_next",1) 
        end 
        if eve_param == "exit" then 
            self.data:OperateModelData("is_go_next",0) 
        end 
       
    end)

    self.Anim:ListenEvent("go_battle",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.data:OperateModelData("is_go_battle",1) 
        end 
        if eve_param == "exit" then 
            self.data:OperateModelData("is_go_battle",0) 
        end 
    end)

    self.Anim:ListenEvent("next_enter",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.data:OperateModelData("is_next_enter",1) 
        end 
        if eve_param == "exit" then 
            self.data:OperateModelData("is_next_enter",0) 
        end 
       
    end)
end 

function DeepDarkFantasyModel:CreateActor()
    -- LogError("?CreateActor!")
    local operate_data = self.data:CacheOperateData()
    if operate_data == nil then return end 

    local appearance = CommonStruct.AppearanceParam(RoleData.Instance:GetApearance())

    if self.role_obj == nil then 
        self.role_obj = UIChDrawer.CreateByAppearance(appearance)
        self.role_obj:Enable()
        self.role_obj:GetRoot():SetParent(self.RolePos)
        self.role_obj:GetRoot():SetLocalPosition(Vector3.zero)
        self.role_obj:GetRoot():SetLocalEulerAngles(Vector3.zero)
    end 

    local res_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(operate_data.monster_id))
    if self.boss_obj ~= nil then 
        self.boss_obj:SetData(res_path)
    else 
        self.boss_obj = UIObjDrawer.CreateByPath(res_path)
        self.boss_obj:Enable()
        self.boss_obj:GetRoot():SetParent(self.BossPos)
        self.boss_obj:GetRoot():SetLocalPosition(Vector3.zero)
        self.boss_obj:GetRoot():SetLocalEulerAngles(Vector3.zero)
    end 

    UH.LocalScale(self.BossPos.transform, Vector3.New(1+operate_data.monster_res_size,1+operate_data.monster_res_size,1+operate_data.monster_res_size))

    if not self.data:IsInitComplete() then self.data:InitComplete() end 
end

function DeepDarkFantasyModel:AdjustModel()
    self.Anim:SetInteger(APH("state"), self.data.model_data.cur_state)
    self.Anim:SetInteger(APH("start"),self.data.model_data.start)

end

function DeepDarkFantasyModel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.role_obj~= nil then
        self.role_obj:DeleteMe()
        self.role_obj = nil
    end 

    if self.boss_obj~= nil then
        self.boss_obj:DeleteMe()
        self.boss_obj = nil
    end 

    self.data:OperateModelData("is_idle",0) 
    self.data:OperateModelData("cur_state",0) 
    self.data:OperateModelData("is_enter_layer",0) 
    self.data:OperateModelData("is_go_next",0) 
    self.data:OperateModelData("is_go_battle",0) 
    self.data:OperateModelData("is_next_enter",0) 
end 

function DeepDarkFantasyModel:ModeMoveCheck(is_chara,state)
    if (is_chara == 1 and self.role_obj == nil) 
        or (is_chara == 0 and  self.boss_obj == nil) then 
        return 
    end 

    local operate = is_chara == 1 and self.role_obj or self.boss_obj
    operate:PlayAnim(state == 1 and ChAnimType.Run or ChAnimType.Idle)

end

function DeepDarkFantasyModel:ModeShowCheck(is_chara,state)
    if (is_chara == 1 and self.role_obj == nil) 
        or (is_chara == 0 and self.boss_obj == nil) then 
        return 
    end  

    local operate = is_chara == 1 and self.role_obj or self.boss_obj
    if state == 1 then operate:PlayAnim(ChAnimType.Show) end 
end

function DeepDarkFantasyModel:ModeDoorShowCheck(state)
    if state == 1 then
        self.bossdoor:SetInteger(APH("state"), self.data.model_data.start)
    end 
end

FantasyModelMoveCheck = FantasyModelMoveCheck or DeclareMono("FantasyModelMoveCheck",UIWFlushPanel)
function FantasyModelMoveCheck:OnEnable()
    self.Main:ModeMoveCheck(self.IsCharacter,1)
end 

function FantasyModelMoveCheck:OnDisable()
    self.Main:ModeMoveCheck(self.IsCharacter,0)
end 

FantasyModelShowCheck = FantasyModelShowCheck or DeclareMono("FantasyModelShowCheck",UIWFlushPanel)
function FantasyModelShowCheck:OnEnable()
    self.Main:ModeShowCheck(self.IsCharacter,1)
end 

function FantasyModelShowCheck:OnDisable()
    self.Main:ModeShowCheck(self.IsCharacter,0)
end 

FantasyModelDoorShowCheck = FantasyModelDoorShowCheck or DeclareMono("FantasyModelDoorShowCheck",UIWFlushPanel)
function FantasyModelDoorShowCheck:OnEnable()
    self.Main:ModeDoorShowCheck(1)
end 

function FantasyModelDoorShowCheck:OnDisable()
    self.Main:ModeDoorShowCheck(0)
end 
