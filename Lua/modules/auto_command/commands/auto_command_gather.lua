
AutoCommandGather = AutoCommandGather or BaseClass(AutoCommand)
function AutoCommandGather:__init(gather_id)
    self.auto_state = AutoCommandState.Gather
    self.gather_id = gather_id
    self.beh_cfg = Cfg.NpcById(gather_id)
end

function AutoCommandGather:DoExecute()
    local main_point = MainPlayer:GetPos()
	local x, y = SceneCtrl.Instance:CurScene():PosToServer(main_point)
    local min_info, min_dis = {}
    local snpc_pos_list = VoMgr:SnpcPosList()
    for k, v in pairs(snpc_pos_list) do
        if SceneObjTypeS.Gather == v.obj_type then
            local distance = GameMath.GetDistance(x, y, v.x, v.y, false)
            if (nil == min_dis or distance < min_dis) and v.obj_id ~= self.last_gather_obj_id then
                min_dis = distance
                min_info = v
            end
        end
	end
	local scene_id = SceneData.Instance:SceneId()
	min_info.scene_id = scene_id
    min_info.id = self.gather_id
    self.last_gather_obj_id = min_info.obj_id
    SceneCtrl.Instance:Logic():AutoToNpc(min_info, function ()
        local snpc_vo_list = VoMgr:SNpcVoList()
        local obj_vo = snpc_vo_list[min_info.obj_id]
        if nil == obj_vo then
            self:DoExecute()
        else
            MainOtherCtrl.Instance:OpenGather({
                is_auto = 1,
                seq = obj_vo.seq,
                obj_id = obj_vo.obj_id,
                desc = self.beh_cfg.param1,
                id = obj_vo.id,
                finish_func = BindTool.Bind(AutoInvokerCtrl.Instance.ExecuteCommands, AutoInvokerCtrl.Instance)
            })
        end
    end, true)
end

function AutoCommandGather:DoStop()
    SceneLogic.Instance:ClearMoveCache()
end