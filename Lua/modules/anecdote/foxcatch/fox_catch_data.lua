CatchFoxData = CatchFoxData or BaseClass()

function CatchFoxData:__init()
    if CatchFoxData.Instance ~= nil then
		CatchFoxData.LogError("[CatchFoxData] attempt to create singleton twice!")
		return
	end
    CatchFoxData.Instance = self

    self.config = Config.catch_fox_auto

    self.fox_data = SmartData.New({catched = false})
end

function CatchFoxData:Config()
    return CatchFoxData.SendTaskFinishReq and Config.main_catch_fox_auto or self.config
end

function CatchFoxData:GetPatrolList()
    return self:Config().patrol
end

function CatchFoxData:GetFoxCatchSceneId()
    return self:Config().find_fox[1].scene_id
end

function CatchFoxData:GetFoxCatchDesc()
    return self:Config().find_fox[1].txt
end

function CatchFoxData:GetSkillRange()
    return self:Config().other[1].skill_range
end

function CatchFoxData:CachedFox(dir_y)
    self.fox_data.catched = true

    self:FoxShow(dir_y)
end

function CatchFoxData:ClearCatched()
    self.fox_data.catched = false
end

function CatchFoxData:CachSkilCD()
    return self:Config().other[1].skill_time
end

function CatchFoxData:IsCatched()
    return self.fox_data.catched
end

function CatchFoxData:CacheFoxObj(obj_id)
    if obj_id == nil then return end 
    self.fox_obj_id = obj_id
end

function CatchFoxData:GetFoxObj()
    return SceneCtrl.Instance:Objs().client_objs[self.fox_obj_id]
end

function CatchFoxData:CheckSkillCatch()
    local obj = self:GetFoxObj()
    if obj == nil then return true end 
    local target_pos = obj:GetPos()
    local main_pos = MainPlayer:GetPos()
    return GameMath.GetDistance(main_pos.x, main_pos.z, target_pos.x, target_pos.z, false) > self:GetSkillRange()
end

function CatchFoxData:SkillShow()
    local main_pos = MainPlayer:GetPos()
    SE:Play(SceneEffectConfig.EffectList.FoxSearchEffect,  main_pos)
end

function CatchFoxData:FoxShow(dir_y)
    local fox =  self:GetFoxObj()
    if fox then 
        local target_pos = fox:GetPos()
        SE:Play(SceneEffectConfig.EffectList.FoxCatchedEffect, target_pos)
        SceneCh.StopMove(fox) 

        local x, y = SceneCtrl.Instance:CurScene():PosToServer(Vector3.New(target_pos.x, 0, target_pos.z))
        AnecdoteCtrl.Instance:SendCatchFoxSpecialReq({x = x ,y = y,dir = dir_y})
    end 
end

function CatchFoxData:CreateCatchFox()
    -- 创建捉拿狐妖
    local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.CatchFox, 1)
    if self:GetFoxCatchSceneId() == SceneData.Instance:SceneId() then
        local patrol_list = CatchFoxData.Instance:GetPatrolList()
        local vo = patrol_list[1]
        vo.obj_id = obj_id
        vo.stop_time = 1
        vo.is_server = false
        vo.alway_show = true
        vo.obj_type = SceneObjType.CatchFox

        VoMgr:AddGCObj(vo)
        self:CacheFoxObj(obj_id)
    else
        VoMgr:DelGCObj(obj_id)
    end
end