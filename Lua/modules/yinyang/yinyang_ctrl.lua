FILE_REQ('modules/yinyang/yinyang_data')
VIEW_REQ('modules/yinyang/yinyang_explore')
YinYangCtrl = YinYangCtrl or BaseClass(BaseCtrl)
function YinYangCtrl:__init()
    YinYangCtrl.Instance = self
    self.data = YinYangData.New()
    self:RegisterProtocol(CSYinYangEyeOpReq)
    self:RegisterProtocol(SCYinYangEyeInfo, 'SCYinYangEyeInfo')
    self.objs_list = {}
end
function YinYangCtrl:__delete()
end
function YinYangCtrl:OnUnloadGameLogic()
    -- self.data:Clear()
end
function YinYangCtrl:CheckGetReward()
    if self.data.cur_box_finish_seq ~= -1 and self.data:IsCanGet(0, self.data.cur_box_finish_seq) then
        YinYangCtrl.CSYinYangEyeOpReq(1, self.data.cur_box_finish_seq)
    end
    if self.data.cur_npc_finish_seq ~= -1 and self.data:IsCanGet(1, self.data.cur_npc_finish_seq) then
        YinYangCtrl.CSYinYangEyeOpReq(2, self.data.cur_npc_finish_seq)
    end
   --[[  TimeHelper:AddDelayTimer(function ()
        
    end, 3) ]]
end
function YinYangCtrl:SCYinYangEyeInfo(protocol)
    -- LogDG(protocol.cur_box_finish_seq)
    -- LogDG(protocol.cur_npc_finish_seq)
    if protocol.open_mark == 1 then
        GuideManager.Instance:EnableYinYangEye()
        MapExploreData.Instance.explore_data.reward_show = -1
        AudioMgr:PlayEffect(AudioType.Scene, CommonAudio.YinYangEye)
    else
        GuideManager.Instance:RevertYinYangEye()
        self.data.explore_data.vo = nil
    end
    self.data.view_data.opened = protocol.open_mark == 1
    -- LogDG(protocol)
    --[[ LogDG(protocol.open_mark)
    LogDG(protocol.box_flag)
    LogDG(protocol.npc_flag) ]]
    --对开启宝箱进行处理
    local box_flag = protocol.box_flag
    local vo = self.data.explore_data.vo
    local old_box_flag = self.data.box_flag
    local old_npc_flag = self.data.npc_flag
    --宝箱
    if vo and vo.show_type == 0 then
        local seq = vo.seq
        if (not old_box_flag[seq] or old_box_flag[seq] == 0) and box_flag[seq] == 1
        and self.objs_list[0] then
            --开宝箱
            local obj_id = self.objs_list[0][seq]
            local obj = SceneCtrl.Instance:Objs().client_objs[obj_id]
            if obj ~= nil then
                obj:OpenBox()
            end
            if vo.is_disappear == 1 then
                VoMgr:DelGCObj(obj_id, true)
                -- SceneCtrl.Instance:Objs():DeleteClientObj(obj_id)
            end
            self.data.explore_data.vo = nil
        end
    end
    --npc
    if vo and vo.show_type == 1 then
        local seq = vo.seq
        if (not old_npc_flag[seq] or old_npc_flag[seq] == 0) and box_flag[seq] == 1
        and self.objs_list[1] then
            --开宝箱
            local obj_id = self.objs_list[1][seq]
            if vo.is_disappear == 1 then
                VoMgr:DelGCObj(obj_id, true)
                -- SceneCtrl.Instance:Objs():DeleteClientObj(obj_id)
            else
                local obj = SceneCtrl.Instance:Objs().client_objs[obj_id]
                if obj ~= nil then
                    obj:SetTouchAble(false)
                end
            end
            self.data.explore_data.vo = nil
        end
    end
    self.data.box_flag = protocol.box_flag
    self.data.npc_flag = protocol.npc_flag
    self.data.cur_box_finish_seq = protocol.cur_box_finish_seq
    self.data.cur_npc_finish_seq = protocol.cur_npc_finish_seq
    self:CreateYinYangObj()
end
function YinYangCtrl:CreateYinYangObj()
    if not self.data:FuncIsOpen() then
        return
    end
    local list = self.data:ObjsList()
    -- LogDG(table.nums(list))
    --哪些要创建哪些要删除
    local scene_id = SceneData.Instance:SceneId()
    local obj_id = 0
    for k, v in pairs(list) do
        obj_id = SceneObjHolder.CalcClientObjId(scene_id, SceneObjType.YinYangObj, k)
        if v.show_type == 0 then
            if self.data.box_flag[v.seq] == 0 then
                v.obj_id = obj_id
                -- SceneCtrl.Instance:Objs():CreateClientObj(SceneObjType.YinYangObj, v)
                VoMgr:AddGCObj(v)
                self.objs_list[v.show_type] = self.objs_list[v.show_type] or {}
                self.objs_list[v.show_type][v.seq] = obj_id
                --保存obj_id
            else
                VoMgr:DelGCObj(obj_id)
            end
        end
        if v.show_type == 1 then    
            if self.data.npc_flag[v.seq] == 0 then
                v.obj_id = obj_id
                -- SceneCtrl.Instance:Objs():CreateClientObj(SceneObjType.YinYangObj, v)
                VoMgr:AddGCObj(v)
                self.objs_list[v.show_type] = self.objs_list[v.show_type] or {}
                self.objs_list[v.show_type][v.seq] = obj_id
                --保存obj_id
            else
                VoMgr:DelGCObj(obj_id)
            end
        end
    end
end
-- 0 开关 1 宝箱 p seq 2 npc p seq
function YinYangCtrl.CSYinYangEyeOpReq(op_type, param)
    local protocol = GetProtocol(CSYinYangEyeOpReq)
    protocol.op_type = op_type or 0
    protocol.param = param or 0
    SendProtocol(protocol)
    -- LogDG(protocol)
end
function YinYangCtrl:OnModeSceneLoaded()
    YinYangCtrl.Instance:CheckGetReward()
	YinYangCtrl.Instance:CreateYinYangObj()
    local scene_id = SceneData.Instance:SceneId()
    local last_scene_id = SceneData.Instance:LastSceneId()
    local is_cant_show = self.data:IsCantShow(YinYangData.ShieldType.Scene, scene_id)
    local last_cant_show = self.data:IsCantShow(YinYangData.ShieldType.Scene, last_scene_id)
    if is_cant_show == true then
        GuideManager.Instance:RevertYinYangEye(true)
    else
        if last_cant_show == true then
            GuideManager.Instance:ShowYinYangEye(true, true)
        end
    end
end
function YinYangCtrl:WhenViewChange(view, is_close)
    local mod_key = ViewToMod[view]
    if mod_key then
        local is_cant_show = self.data:IsCantShow(YinYangData.ShieldType.Panel, mod_key)
        if is_cant_show == true then
            if not is_close then
                GuideManager.Instance:RevertYinYangEye(true)
            else
                GuideManager.Instance:ShowYinYangEye(true, true)
            end
        end
    end
end