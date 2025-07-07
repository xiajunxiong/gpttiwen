BehaviorData = BehaviorData or BaseClass()

function BehaviorData:__init()
    if BehaviorData.Instance ~= nil then
        Debuger.LogError("[BehaviorData] attempt to create singleton twice!")
        return
    end
    BehaviorData.Instance = self

    self:BehaviorData()
end

function BehaviorData:BehaviorData()
    self.npc_talk_btns_func = {
        seq_funcs = {},
        id_funcs = {}
    }
    self.npc_talk_list_func = {
        seq_funcs = {},
        id_funcs = {}
    }

    -- 疑似自动行为的等待时间
    self.auto_behavior = 0

    -- self.npc_behavior_list = {}
end

-- replace -> 不是插入按钮列表而是直接替换，需要return
function BehaviorData:CustomNpcTalkBtnsSeq(seq, func, replace)
    self.npc_talk_btns_func.seq_funcs[seq] = self.npc_talk_btns_func.seq_funcs[seq] or {}
    local handle = {
        func = func,
        replace = replace or false
    }
    table.insert(self.npc_talk_btns_func.seq_funcs[seq], handle)
    return handle
end

function BehaviorData:RemoveCustomNpcTalkBtnsSeq(handle)
    if handle == nil then return end
    for _, seq_funcs in pairs(self.npc_talk_btns_func.seq_funcs) do
        for k, v in pairs(seq_funcs) do
            if v == handle then
                table.remove(seq_funcs, k)
            end
        end
    end
end

function BehaviorData:CustomNpcTalkBtnsId(id, func, replace)
    self.npc_talk_btns_func.id_funcs[id] = self.npc_talk_btns_func.id_funcs[id] or {}
    local handle = {
        func = func,
        replace = replace or false
    }
    table.insert(self.npc_talk_btns_func.id_funcs[id], handle)
    return handle
end

function BehaviorData:RemoveCustomNpcTalkBtnsId(handle)
    for _, id_funcs in pairs(self.npc_talk_btns_func.id_funcs) do
        for k, v in pairs(id_funcs) do
            if v == handle then
                table.remove(id_funcs, k)
            end
        end
    end
end

function BehaviorData:SetupBehaviourBtn(beh_btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
    local seq_funcs = self.npc_talk_btns_func.seq_funcs
    if seq_funcs[obj_vo.seq] then
        local funcs = seq_funcs[obj_vo.seq]
        for k, v in pairs(funcs) do
            local res = v.func(beh_btn_list, obj_vo)
            if v.replace then
                return res
            end
        end
    end
    local id_funcs = self.npc_talk_btns_func.id_funcs
    if id_funcs[obj_vo.id] then
        local funcs = id_funcs[obj_vo.id]
        for k, v in pairs(funcs) do
            local res = v.func(beh_btn_list, obj_vo)
            if v.replace then
                return res
            end
        end
    end
end

function BehaviorData:CustomNpcTalkListSeq(seq, func)
    self.npc_talk_list_func.seq_funcs[seq] = self.npc_talk_list_func.seq_funcs[seq] or {}
    table.insert(self.npc_talk_list_func.seq_funcs[seq], func)
end

function BehaviorData:RemoveCustomNpcTalkListSeq(func)
    for _, seq_funcs in pairs(self.npc_talk_list_func.seq_funcs) do
        for k, v in pairs(seq_funcs) do
            if v == func then
                table.remove(seq_funcs, k)
            end
        end
    end
end

function BehaviorData:CustomNpcTalkListId(id, func)
    self.npc_talk_list_func.id_funcs[id] = self.npc_talk_list_func.id_funcs[id] or {}
    table.insert(self.npc_talk_list_func.id_funcs[id], func)
    return func
end

function BehaviorData:RemoveCustomNpcTalkListId(func)
    for _, id_funcs in pairs(self.npc_talk_list_func.id_funcs) do
        for k, v in pairs(id_funcs) do
            if v == func then
                table.remove(id_funcs, k)
            end
        end
    end
end

function BehaviorData:SetupTalkList(beh_talk_list, obj_vo)
    if nil == obj_vo then
        return
    end
    local beh_talk_list = beh_talk_list or {}
    local seq_funcs = self.npc_talk_list_func.seq_funcs
    if seq_funcs[obj_vo.seq] then
        local funcs = seq_funcs[obj_vo.seq]
        for _, func in pairs(funcs) do
            func(beh_talk_list, obj_vo)
        end
    end
    local id_funcs = self.npc_talk_list_func.id_funcs
    if id_funcs[obj_vo.id] then
        local funcs = id_funcs[obj_vo.id]
        for _, func in pairs(funcs) do
            func(beh_talk_list, obj_vo)
        end
    end
    if not next(beh_talk_list) then
        return nil
    else
        return beh_talk_list
    end
end

function BehaviorData:NpcSeqToNpcId(npc_seq)
    return Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq].id
end

-- --重写NPC对话框行为，完全覆盖NPC原来的行为
-- function BehaviorData:CustomNpcDialogBehSeq(seq,func)
--     local handle = {func = func,seq = seq}
--     self.npc_behavior_list[handle] = handle
--     return handle
-- end

-- function BehaviorData:CustomNpcDialogBehId(id,func)
--     local handle = {func = func,id = id}
--     self.npc_behavior_list[handle] = handle
--     return handle
-- end

-- function BehaviorData:RemoveCustomNpcDialogBeh(handle)
--     if handle == nil then return end
--     self.npc_behavior_list[handle] = nil
-- end

-- function BehaviorData:SetupNpcDialogBeh(param_t,obj_vo)
--     if nil == obj_vo then
--         return false
--     end
--     for k,v in pairs(self.npc_behavior_list) do
--         if obj_vo.seq == v.seq or obj_vo.id == v.id then
--             v.func(param_t,obj_vo)
--             return true
--         end
--     end
--     return false
-- end