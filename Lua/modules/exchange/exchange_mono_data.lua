
ExchangeMonoData = ExchangeMonoData or BaseClass()

function ExchangeMonoData:__init()
    self:ExchangeMonoConfig()
end

function ExchangeMonoData:ExchangeMonoConfig()
    self.medaltask_exchange_npc_seq = Config.medaltask_scene_auto.other[1].npc_seq

    self.medaltask_exchange_tasks = {}
    for _, v in pairs(Config.medaltask_scene_auto.exchange) do
        self.medaltask_exchange_info = self.medaltask_exchange_info or self:AnMedaltaskExchangeCo(v)
        for _, z in pairs(string.split(v.task_id, "|")) do
            self.medaltask_exchange_tasks[tonumber(z)] = v
        end
    end
end

function ExchangeMonoData:MedalExchange()
    for task_id, co in pairs(self.medaltask_exchange_tasks) do
        local task_info = TaskData.Instance:GetTaskInfoByTaskId(task_id)
        if task_info and TaskConfig.ListType.accepted == task_info.list_type then
            self.medaltask_exchange_info = self:AnMedaltaskExchangeCo(co)
            return true
        end
    end
    return false
end

function ExchangeMonoData:AnMedaltaskExchangeCo(co)
    local t = {seq = co.seq, sub_items = {}}
    t.target = Item.Init(co.target.item_id,co.target.num,co.target.is_bind)
    for i = 1, 5 do
        if co["stuff_id_"..i] > 0 then
            local item = Item.Init(co["stuff_id_"..i])
            item.e_num = co["stuff_num_"..i]
            table.insert(t.sub_items, item)
        end
    end
    return t
end

function ExchangeMonoData:MedelExchangeTaskNpcSeq()
    return Config.medaltask_scene_auto.other[1].seq
end