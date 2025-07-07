PetStorage = PetStorage or DeclareView("PetStorage", "pet/pet_storage", Mod.Pet.PetStorage)
VIEW_DECLARE_LEVEL(PetStorage, ViewLevel.L1)
function PetStorage:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Pet.PetStorage)
    self.Currency:DefaultSet()
end
function PetStorage:CloseCallback()
    if table.nums(PetData.Instance:GetPetList()) > 0 then
        ViewMgr:FlushView(PetView)
    end
    TaskData.Instance:TaskChainSubContinue()
end

PetStoragePanel = PetStoragePanel or DeclareMono("PetStoragePanel", UIWFlushPanel)
function PetStoragePanel:PetStoragePanel()
    self.data = PetData.Instance
    self.is_bag = false
    self.data_cares = {
        {data = self.data:GetPetList(), func = self.FlushPetList, init_call = false},
        {data = self.data:GetStorageList(), func = self.FlushStorageList, init_call = false}
    }
end
function PetStoragePanel:onFlush()
    local list1 = self.data:GetStorageList()
    local list2 = table.rearrange(self.data:GetPetList())
    function sort_fun(a,b)
        local aState = a:State()
        local bState = b:State()
        if aState ~= PetState.Train and bState ~= PetState.Train then
            if a:State() < b:State() then
                return false
            elseif a:State() > b:State() then
                return true
            else
                return a.index < b.index
            end
        elseif aState == PetState.Train then
            return false
        elseif bState == PetState.Train then
            return true
        else
            return a.index < b.index
        end
    end
    table.sort(list2, sort_fun)
    self.StorageList:SetData(list1)
    self.BagList:SetData(list2)
    self:FlushDesc()
end
function PetStoragePanel:FlushPetList()
    local list2 = table.rearrange(self.data:GetPetList())
    function sort_fun(a,b)
        if a:State() < b:State() then
            return false
        elseif a:State() > b:State() then
            return true
        else
            return a.index < b.index
        end
    end
    table.sort(list2, sort_fun)
    self.BagList:SetData(list2)
    self:FlushDesc()
end
function PetStoragePanel:FlushStorageList()
    local list1 = self.data:GetStorageList()
    self.StorageList:SetData(list1)
	self:FlushDesc()
end
function PetStoragePanel:FlushDesc()
    local max_count = self.data:GetPetMaxStorage()
    local cur_count = table.nums(self.data:GetStorageList())
    UH.SetText(self.LeftTex, string.format(Language.Pet.Storageing, cur_count, max_count))
    local other_info = self.data:GetOtherInfo()
    local max_count2 = self.data:GetPetStartBag() + ( other_info.extend_grid_num or 0 )
    local cur_count2 = table.nums(self.data:GetPetList())
    UH.SetText(self.RightTex, string.format(Language.Pet.Baging, cur_count2, max_count2))
    UH.SetText(self.SelectTex,"")
end
function PetStoragePanel:OnClickStrItem1()
    self.BagList:CancelSelect()
    self.is_bag = false
    self:FlushBottom()
end
function PetStoragePanel:OnClickStrItem2(toggle, item)
    local data = item:GetData()
    if BicycleData.Instance:JumpPet(data) then
        item:SetSelected(false)
        return
    end
    if toggle and data:State() == PetState.Fight or data:State() == PetState.StandBy or data:State() == PetState.Train then
        PublicPopupCtrl.Instance:Center(Language.Pet.PetStorageNotice)
        item:SetSelected(false)
        self.BagList:OnItemSelected(item, false)
    end
    self.StorageList:CancelSelect()
    self.is_bag = true
    self:FlushBottom()
end
function PetStoragePanel:FlushBottom()
    self.BtnPut:SetActive(self.is_bag)
    self.BtnGet:SetActive(not self.is_bag)
    --统计点了多少显示
    local count
    local data
    if self.is_bag then
        data = self.BagList:GetSelectGroupData()
    else
        data = self.StorageList:GetSelectGroupData()
    end
    count = table.nums(data)
    UH.SetText(self.SelectTex, string.format(Language.Pet.PetStorageSelect, count))
end
--存入
function PetStoragePanel:OnClickMovePut()
    local data = self.BagList:GetSelectGroupData()
    --获取仓库空位
    local sel_count = table.nums(data)
    local max_count = self.data:GetPetMaxStorage()
    local cur_count = table.nums(self.data:GetStorageList())
    --够数才给你存
    if cur_count < max_count and max_count - cur_count >= sel_count then
        local no_sel = {}
        for _, value in pairs(self.data:GetStorageList()) do
            no_sel[value.index] = true
        end
        -- LogError("仓库不可存列表", no_sel)
        for i = 0, max_count - 1 do
            if no_sel[i] then
            else
                if sel_count == 0 then
                    break
                end
                if data[sel_count] then
                    if data[sel_count]:ArenaState() then
                        PublicPopupCtrl.Instance:Center(string.format(ErrorText[126], data[sel_count]:Name()))
                    else
                        PetCtrl.SendPetOp(PET_OP_TYPE.MoveStorage, data[sel_count].index, i)
                    end
                    sel_count = sel_count - 1
                end
            end
        end
    else
        PublicPopupCtrl.Instance:Center(ErrorText[131])
    end
end
--取出
function PetStoragePanel:OnClickMoveGet()
    local data = self.StorageList:GetSelectGroupData()
    local other_info = self.data:GetOtherInfo()
    local sel_count = table.nums(data)
    local max_count = self.data:GetPetStartBag() + (other_info.extend_grid_num or 0)
    local cur_count = table.nums(self.data:GetPetList())
    --够数才给你存
    if cur_count < max_count and (max_count - cur_count) >= sel_count then
        local no_sel = {}
        for _, value in pairs(self.data:GetPetList()) do
            no_sel[value.index] = true
        end
        -- LogError("背包不可存列表", no_sel)
        for i = 0, max_count - 1 do
            if no_sel[i] then
            else
                if sel_count <= 0 then
                    break
                end
                if data[sel_count] then
                    PetCtrl.SendPetOp(PET_OP_TYPE.MovePetGrid, i, data[sel_count].index)
                    sel_count = sel_count - 1
                end
            end
        end
    else
        PublicPopupCtrl.Instance:Center(ErrorText[130])
    end
end
