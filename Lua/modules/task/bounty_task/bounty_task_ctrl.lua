
FILE_REQ("modules/task/bounty_task/bounty_task_data")

VIEW_REQ("modules/task/bounty_task/bounty_task_array_view")
VIEW_REQ("modules/task/bounty_task/bounty_task_practice_view")
VIEW_REQ("modules/task/bounty_task/bounty_task_letter_view")

BountyTaskCtrl = BountyTaskCtrl or BaseClass(BaseCtrl)

function BountyTaskCtrl:__init()
    if BountyTaskCtrl.Instance ~= nil then
        Debuger.LogError("[BountyTaskCtrl] attempt to create singleton twice!")
        return
    end
    BountyTaskCtrl.Instance = self
    self.data = BountyTaskData.New()

    self:RegisterAllProtocols()
end

function BountyTaskCtrl:__delete()
    if BountyTaskCtrl.Instance == self then
        BountyTaskCtrl.Instance = nil
    end
    self:UnRegisterAllProtocols()
end

function BountyTaskCtrl:OnInit()
    -- ActivityData.Instance:CustomClickHandle(ActType.TreasureTask, BindTool.Bind(self.data.AccpetGoTask, self.data))
	TaskData.Instance:ListenTaskCompleteById(BindTool.Bind(self.OnTaskComplete, self), Config.bounty_tasklist_auto.other[1].pigeon_mail_trigger)
end

function BountyTaskCtrl:OnUnloadGameLogic()
end

function BountyTaskCtrl:RegisterAllProtocols()
    -- self:RegisterProtocol(CSTreasureTaskReq)
    -- self:RegisterProtocol(SCTreasureTaskInfo, "OnTreasureTaskInfo")
end

function BountyTaskCtrl:UnRegisterAllProtocols()
end

function BountyTaskCtrl:OnTaskComplete()
	ViewMgr:OpenView(BountyTaskLetterView)
end

-- function BountyTaskCtrl:OnTreasureTaskInfo(protocol)
-- 	self.data:SetTreasureTaskInfo(protocol)
--     self:CreateTreasureTaskBox()
-- end

-- function BountyTaskCtrl:SendTreasureTaskReq(param_t)
-- 	local protocol = GetProtocol(CSTreasureTaskReq)
-- 	protocol.type = param_t.type or 0
--     protocol.param = param_t.param or 0
-- 	SendProtocol(protocol)
-- end

-- function BountyTaskCtrl:SendTreasureTaskReqInfo()
-- 	local param_t = {
--         type = 0,
--     }
-- 	self:SendTreasureTaskReq(param_t)
-- end

-- function BountyTaskCtrl:SendTreasureTaskReqAccpet()
-- 	local param_t = {
--         type = 1,
--     }
-- 	self:SendTreasureTaskReq(param_t)
-- end

-- function BountyTaskCtrl:SendTreasureTaskReqOpenBox()
--     local param_t = {
--         type = 2,
--     }
--     self:SendTreasureTaskReq(param_t)
-- end

-- function BountyTaskCtrl:CreateTreasureTaskBox()
--     -- TreasureTaskData.Instance:ClearCacheBoxTreasureTask()
--     local list = Config.treasure_task_auto.reward_coordinate
--     if list then
--         for k, v in ipairs(list) do
--             local obj_id = SceneObjHolder.CalcClientObjId(v.scene, SceneObjType.TreasureTaskObj, v.seq)
--             if self.data:IsBoxShow(v.seq) then
--                 local vo = CommonStruct.NpcVo(v)
--                 local npc_co = NpcHelper.NpcCo(v.NPCid)
--                 vo.obj_id = obj_id
--                 vo.res_id = npc_co.res_id
--                 vo.treasure_task_seq = v.seq
--                 vo.name = npc_co.name
--                 vo.scene_id = v.scene
--                 vo.obj_type = SceneObjType.TreasureTaskObj
--                 vo.is_server = true
--                 VoMgr:AddGCObj(vo)
--             else
--                 VoMgr:DelGCObj(obj_id)
--             end
--         end
--     end
-- end