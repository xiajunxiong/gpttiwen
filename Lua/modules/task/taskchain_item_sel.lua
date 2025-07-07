TaskChainItemSel = TaskChainItemSel or DeclareView("TaskChainItemSel", "task/taskchain_item_sel")

function TaskChainItemSel:TaskChainItemSel()
    self.beh_data = BehaviorData.Instance
end

function TaskChainItemSel:LoadCallback()
    self.SubItemBtnObj:SetActive(TaskChainItemSel.pet_id == nil or TaskChainItemSel.pet_id == 0)
    self.PetBtnsObj:SetActive(TaskChainItemSel.pet_id ~= nil and TaskChainItemSel.pet_id > 0)

    if(TaskChainItemSel.pet_id ~= nil and TaskChainItemSel.pet_id > 0) then
        local way_cfg = PetData.Instance:GetCatchWay(PetData.Instance:GetPetCfg(TaskChainItemSel.pet_id).catch_way)
        self.PetBtnSeal:SetActive(way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpScene)
        self.PetBtnHurt:SetActive(way_cfg[1].jump_type ~= PetData.CatchWayJumpType.JumpScene)
    end
end

function TaskChainItemSel:Awake()
    if (TaskChainItemSel.pet_id == nil or TaskChainItemSel.pet_id == 0) and self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            self:OnClickSubmit()
        end,
        self.beh_data.auto_behavior)
    end
end

function TaskChainItemSel:OnDestroy()
    TimeHelper:CancelTimer(self.timer_auto_beh)
    if TaskChainItemSel.custom_func ~= nil then 
        TaskChainItemSel.custom_func = nil
    end 
end

function TaskChainItemSel:OnClickClose()
    ViewMgr:CloseView(TaskChainItemSel)
end

function TaskChainItemSel:OnClickSubmit()
    if TaskChainItemSel.custom_func ~= nil then 
        TaskChainItemSel.custom_func()
    else
        TaskCtrl.Instance:SubmitTaskChainItemSel()
    end 
    -- 
end

function TaskChainItemSel:IsPet()
    local data = TaskChainItemSel.cur_sel_data
    if nil == data or TaskChainItemSel.pet_id == nil or TaskChainItemSel.pet_id == 0 then
        return false
    end
    local is_pet = TaskChainItemSel.pet_id == data.pet_id or (PetData.Instance:GetMutantPetCfgByNormalId(TaskChainItemSel.pet_id).pet_id or 0) == data.pet_id
    return is_pet
end

function TaskChainItemSel:OnBuyPetClick()
    local data = TaskChainItemSel.cur_sel_data
    if nil == data then
        return
    end
    if not self:IsPet() then
        return
    end
    local pet_id = data.pet_id
    TradeData.Instance:SetSearchItemId(pet_id)
    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
end

function TaskChainItemSel:OnHurtPetClick()
    ViewMgr:OpenView(TreasureHuntView)
end

function TaskChainItemSel:OnSealPetClick()
    local data = TaskChainItemSel.cur_sel_data
    if nil == data then
        return
    end
    if not self:IsPet() then
        return
    end
    local pet_id = data.pet_id
    local co = Cfg.ChainPetGroupByPI(pet_id)
    if nil == co then
        LogError(string.format("商人的烦恼 pet_id:%s 找不到巡逻场景", pet_id))
        return
    end
    local scene_id = co.scene_id
    BehaviorCtrl.ExeByCfgVo(
        {
            behavior_type = 9, --behavior_type 是 beh_id_to_tab.key
            param1 = scene_id,
            param2 = "",
            param3 = ""
        }
    )
    ViewMgr:OpenView(FightAutoSealSetView, {scene_id = scene_id, target_pets = {{pet_id = pet_id, num = 1}}})
    ViewMgr:CloseView(TaskChainItemSel)
end

TaskChainItemSelList = TaskChainItemSelList or DeclareMono("TaskChainItemSelList", UIWFlushPanel)

function TaskChainItemSelList:TaskChainItemSelList()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushItemList}
    }
end

function TaskChainItemSelList:CloseCallback()
    TaskChainItemSel.cur_sel_data = nil
end

function TaskChainItemSelList:FlushItemList()
    local item_id = TaskChainItemSel.item_id
    if not item_id then
        return
    end
    local items = BagData.Instance:GetItemsByItemId(item_id)
    if TaskChainItemSel.pet_id > 0 then
        local mutant_pet = 0
        if TaskConfig.TaskChainItemSelFrom.task_chain == TaskChainItemSel.from_type then
            mutant_pet = PetData.Instance:GetMutantPetCfgByNormalId(TaskChainItemSel.pet_id).pet_id or 0
        end
        items = {}
        for _, pet in pairs(PetData.Instance.pet_list) do
            local pet_state = pet:State()
            if (pet:ID() == TaskChainItemSel.pet_id or pet:ID() == mutant_pet) and PetState.Relax == pet_state and not pet:ArenaState() then
                table.insert(items, pet)
            end
        end
        --排序下
        table.sort(items,function(a,b)
            if a.str_level == b.str_level then
                return a.level < b.level
            end
            return a.str_level < b.str_level 
        end)                                    
    end
    if not next(items) then
        self.Default:SetActive(true)
    else
        self.ItemList:SetDataList(items)
        self.Default:SetActive(false)
    end
end

function TaskChainItemSelList:OnClickSubmit()
    TaskCtrl.Instance:SubmitTaskChainItemSel()
end

TaskChainItemSelItem = DeclareMono("TaskChainItemSelItem", UIWidgetBaseItem)

function TaskChainItemSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    self.is_pet = TaskChainItemSel.pet_id == data.pet_id or (PetData.Instance:GetMutantPetCfgByNormalId(TaskChainItemSel.pet_id).pet_id or 0) == data.pet_id
    self.PetObj:SetActive(self.is_pet)
    if not self.is_pet then return end
    UH.SetText(self.Level, data:Level())
    PetData.SetStrLevel(nil, self.StrLevel, data:StrLevel())
end

function TaskChainItemSelItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    self.Cell.Selected:SetActive(selected)
    if selected then
        TaskChainItemSel.cur_sel_data = self.data
        if self.is_pet then
            PetData.Instance.cur_sel_detail:Set(self.data)
            self.PetDetailObj:SetActive(true)
        else
            self.PetDetailObj:SetActive(false)
        end
    end
    if TaskChainItemSel.cur_sel_data == self.data and not selected then
        TaskChainItemSel.cur_sel_data = nil
        self.PetDetailObj:SetActive(false)
    end
end
