
FILE_REQ("modules/task/treasure_task/treasure_task_data")

VIEW_REQ("modules/task/treasure_task/treasure_task_view")

TreasureTaskCtrl = TreasureTaskCtrl or BaseClass(BaseCtrl)

function TreasureTaskCtrl:__init()
    if TreasureTaskCtrl.Instance ~= nil then
        Debuger.LogError("[TreasureTaskCtrl] attempt to create singleton twice!")
        return
    end
    TreasureTaskCtrl.Instance = self
    self.data = TreasureTaskData.New()

    self:RegisterAllProtocols()
end

function TreasureTaskCtrl:__delete()
    if TreasureTaskCtrl.Instance == self then
        TreasureTaskCtrl.Instance = nil
    end
    self:UnRegisterAllProtocols()
end

function TreasureTaskCtrl:OnInit()
    ActivityData.Instance:CustomClickHandle(ActType.TreasureTask, BindTool.Bind(self.data.AccpetGoTask, self.data))
end

function TreasureTaskCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function TreasureTaskCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTreasureTaskReq)
    self:RegisterProtocol(SCTreasureTaskInfo, "OnTreasureTaskInfo")
end

function TreasureTaskCtrl:UnRegisterAllProtocols()
end

function TreasureTaskCtrl:OnTreasureTaskInfo(protocol)
	self.data:SetTreasureTaskInfo(protocol)
    self:CreateTreasureTaskBox()
end

-- 0 请求信息
-- 1 接取第一个任务
-- 2 开启宝箱
function TreasureTaskCtrl:SendTreasureTaskReq(param_t)
	local protocol = GetProtocol(CSTreasureTaskReq)
	protocol.type = param_t.type or 0
    protocol.param = param_t.param or 0
	SendProtocol(protocol)
end

function TreasureTaskCtrl:SendTreasureTaskReqInfo()
	local param_t = {
        type = 0,
    }
	self:SendTreasureTaskReq(param_t)
end

function TreasureTaskCtrl:SendTreasureTaskReqAccpet()
	local param_t = {
        type = 1,
    }
	self:SendTreasureTaskReq(param_t)
end

function TreasureTaskCtrl:SendTreasureTaskReqOpenBox()
    local param_t = {
        type = 2,
    }
    self:SendTreasureTaskReq(param_t)
end
function TreasureTaskCtrl:CreateTreasureTaskBox()
    -- TreasureTaskData.Instance:ClearCacheBoxTreasureTask()
    local list = Config.treasure_task_auto.reward_coordinate
    if list then
        for k, v in ipairs(list) do
            local obj_id = SceneObjHolder.CalcClientObjId(v.scene, SceneObjType.TreasureTaskObj, v.seq)
            if self.data:IsBoxShow(v.seq) then
                local vo = CommonStruct.NpcVo(v)
                local npc_co = NpcHelper.NpcCo(v.NPCid)
                vo.obj_id = obj_id
                vo.res_id = npc_co.res_id
                vo.treasure_task_seq = v.seq
                vo.name = npc_co.name
                vo.scene_id = v.scene
                vo.obj_type = SceneObjType.TreasureTaskObj
                vo.is_server = true
                VoMgr:AddGCObj(vo)
            else
                VoMgr:DelGCObj(obj_id)
            end
        end
    end
end