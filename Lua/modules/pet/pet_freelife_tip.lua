--宠物放生界面
PetFreelifeTip = PetFreelifeTip or DeclareView("PetFreelifeTip", "pet/pet_freelife_tip")
VIEW_DECLARE_MASK(PetFreelifeTip, ViewMask.Block)
function PetFreelifeTip:PetFreelifeTip()
    self.pet_data = PetData.Instance
    self.other_info = self.pet_data:GetPetOtherCfg()
end

function PetFreelifeTip:LoadCallback(param_t)
    self.Alert:SetData(self:GetType(), Vector2.New(480, 356))
    self.pet = param_t.pet
    self:OnFlush()
end

function PetFreelifeTip:OnFlush()
    if self.pet == nil then
        return
    end
    self.CovertItem:SetDataByItemId(self.other_info.item_return_exp)
    
    self.covert_cfg = self.pet_data:GetExpItemCfg(self.other_info.item_return_exp)
    if self.covert_cfg == nil then
        return
    end
    if self.pet:Level() > 1 then
        local exp = self.pet_data:GetExpTotal(1, self.pet:Level() - 1)
        self.count = (exp + self.pet:Exp()) / self.covert_cfg.add_exp * self.other_info.rate_return_exp / 100 + self.other_info.return_add_num
    else
        self.count = 1
    end
    CellFlushs.SetNum(self.CovertItem, math.floor(self.count))
    UH.SetText(
        self.Desc,
        string.format(
            Language.Pet.FreeLifeTip,
            self.pet:Level(),
            self.pet:Name(),
            self.CovertItem:GetData():Name(),
            self.count
        )
    )
end
function PetFreelifeTip:OnClickCancel()
    ViewMgr:CloseView(PetFreelifeTip)
end
function PetFreelifeTip:OnClickOk()
    if self.pet == nil then return end
    PetCtrl.SendPetOp(PET_OP_TYPE.AbandonmentPet, self.pet.index)
    ViewMgr:CloseView(PetFreelifeTip)
end


PetFreelifeTip2 = PetFreelifeTip2 or DeclareView("PetFreelifeTip2", "pet/pet_freelife_tip2")
VIEW_DECLARE_MASK(PetFreelifeTip2, ViewMask.Block)
function PetFreelifeTip2:PetFreelifeTip2()
    self.pet_data = PetData.Instance
    self.other_info = self.pet_data:GetPetOtherCfg()
    GuideManager.Instance:RegisterGetGuideUi("FreeLifeConfirmBtn", function ()
        return self.FreeLifeConfirmBtn
    end)
end

function PetFreelifeTip2:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
end

function PetFreelifeTip2:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
    GuideManager.Instance:UnRegisterGetGuideUi("FreeLifeConfirmBtn")
end

function PetFreelifeTip2:OnFlush()
    local exp_list = self.pet_data:GetPetExpItemList()
    self.pet = self.pet_data:GetCurSelect()
    if self.pet == nil then
        return
    end
    local str_level = self.pet:StrLevel()
    if str_level > 1 then
        UH.SetText(self.LblTip, string.format(Language.Pet.PetFreeLife2,self.pet:Level(),self.pet:Name(),str_level))
    else
        UH.SetText(self.LblTip, string.format(Language.Pet.PetFreeLife,self.pet:Level(),self.pet:Name()))
    end
    
    -- 获取可用于计算的经验
    local total_exp = 0
    local temp_list = TableCopy(exp_list)
    local result_data = {}
    if self.pet:Level() > 1 then
        local exp = self.pet_data:GetExpTotal(1, self.pet:Level() - 1)
        total_exp = (exp + self.pet:Exp()) * self.other_info.rate_return_exp / 100 --上一级+当前 * 返回比
        --对全部经验进行一个计算
        table.sort(temp_list, DataSorter.KeyUpperSorter("add_exp"))
        for index, value in ipairs(temp_list) do
            if index ~= #temp_list then
                value.num = math.floor(total_exp / value.add_exp)
                total_exp = total_exp - value.num * value.add_exp
            else
                value.num = math.ceil(total_exp / value.add_exp)
            end
        end
        local temp_list2 = {}
        for key, value in pairs(temp_list) do
            -- if value.item_id == self.other_info.item_return_exp then
            --     value.num = value.num + 1
            -- end
            if value.num > 0 then
                table.insert(temp_list2, value)
            end
        end
        result_data = temp_list2
        -- self.ResultList:SetData(temp_list2)
    else
        --一个最低的奖励
        result_data = {{item_id = self.other_info.item_return_exp, num = self.other_info.return_add_num}}
        -- self.ResultList:SetData(data)
    end
    local return_cfg = self.pet_data:GetPetReturnCfg(self.pet)
    if return_cfg and return_cfg.return_num > 0 then
        table.insert(result_data,{item_id = return_cfg.patch_id, num = return_cfg.return_num})
    end
    local info = self.pet:GetGaiMingInfo()
    total_exp = info.exp
    local config
    local tupo_num = 0
    for i = 0, info.level - 1 do
        config = self.pet_data:GaiMingLevelInfo(i)
        total_exp = total_exp + config.up_exp
        if config.break_item_num > 0 then
            tupo_num = tupo_num + config.break_item_num
        end
    end
    local num = math.ceil(total_exp / self.pet_data:GaiMingUpExp() * self.pet_data:GaiMingFanHuan())
    if num > 0 then
        table.insert(result_data,{item_id = self.pet_data:GaiMingUpItem(), num = num})
    end
    if tupo_num > 0 then
        tupo_num = math.ceil(tupo_num * self.pet_data:GaiMingFanHuan())
        table.insert(result_data,{item_id = self.pet_data:GaiMingTuPoItem(), num = tupo_num})
    end

    local item_size = self.ItemRect.sizeDelta
    if #result_data > 4 then
        -- self.Content.childAlignment = TextAnchor.UpperLeft
        self.BgRect.sizeDelta = Vector2.New(#result_data * item_size.x + 10, self.BgRect.sizeDelta.y)
        self.ScrollRect.sizeDelta = Vector2.New(#result_data * item_size.x + 10, self.ScrollRect.sizeDelta.y)
    else
        -- self.Content.childAlignment = TextAnchor.UpperCenter
        self.BgRect.sizeDelta = Vector2.New(803, self.BgRect.sizeDelta.y)
        self.ScrollRect.sizeDelta = Vector2.New(803, self.ScrollRect.sizeDelta.y)
    end
    self.ResultList:SetData(result_data)
end
function PetFreelifeTip2:OnClickOk()
    if self.pet == nil then return end
    PetCtrl.SendPetOp(PET_OP_TYPE.AbandonmentPet, self.pet.index)
    ViewMgr:CloseView(PetFreelifeTip2)
end

function PetFreelifeTip2:OnClickCancel()
    ViewMgr:CloseView(PetFreelifeTip2)
end

PetFreeLifeItem = PetFreeLifeItem or DeclareMono("PetFreeLifeItem", UIWidgetBaseItem)
function PetFreeLifeItem:PetFreeLifeItem()
-- body
end
function PetFreeLifeItem:SetData(data)
    local item = Item.Create(data)
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName())
end

---------------批量放生界面----------------------------
PetFreeLifeSeveral = PetFreeLifeSeveral or DeclareView("PetFreeLifeSeveral", "pet/pet_freelife_several")
function PetFreeLifeSeveral:PetFreeLifeSeveral()
    self.data_instance = PetData.Instance
    self.lang = Language.Pet
    self.selected_pets_index = {}
    self.need_notice_pets_index = {}         --需要提醒的列表
    self.selected_count = 0

    self.selector_list_data = {
        {text = Language.Pet.ColorLevelT[1], qua = ItemColor.Green, pet_qua = 1},
        {text = Language.Pet.ColorLevelT[2], qua = ItemColor.Blue, pet_qua = 2},
    }
end

function PetFreeLifeSeveral:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.FreeLifeSeveralTitle, Vector2.New(891, 665))
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ViewOpenL2)
    self.SelectPetsNum.text = string.format(self.lang.FreeSelectedPets, tostring(self.selected_count))
    self.SelList:SetData(self.selector_list_data)
    self.SeletorToggle.isOn = false
end

function PetFreeLifeSeveral:OnFlush()
    local pet_data_list = self.data_instance:GetCanFreeLifePet()
    self.FreePetList:SetData(pet_data_list)
    self.NoPetTipObj:SetActive(#pet_data_list <= 0)
end

function PetFreeLifeSeveral:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    self.selected_pets_index = nil
end

function PetFreeLifeSeveral:OnSelClick(data)
    self.SeletorToggle.isOn = false
    local mono_list = self.FreePetList.item_list
    local jump_index
    for i,mono in pairs(mono_list) do
        local pet = mono:GetData()
        local index = pet.index
        if pet:Quality() == data.pet_qua and self.selected_pets_index[index] ~= 1 then
            if pet:PrayMainIndex() == nil then
                self:OnClickPetItem(mono)
                if not jump_index then
                    jump_index = i
                end
            end
        elseif self.selected_pets_index[index] == 1 then
            self:OnClickPetItem(mono)
        end
    end

    if jump_index then
        self.FreePetList:JumpVerticalIndex(math.ceil(jump_index / 4), 234)
    end
end

function PetFreeLifeSeveral:OnClickPetItem(mono)
    local data = mono:GetData()
    if BicycleData.Instance:JumpPet(data) then
        return
    end

    local index = data.index
    mono:SetSelected(self.selected_pets_index[index] ~= 1)

    if self.selected_pets_index[index] == 1 then
        self.selected_pets_index[index] = nil
        if self.need_notice_pets_index[index] ~= nil then
            self.need_notice_pets_index[index] = nil
        end
        self.selected_count = self.selected_count - 1
    else
        self.selected_pets_index[index] = 1
        if data:Quality() >= 3 then
            self.need_notice_pets_index[index] = 1
        end
        self.selected_count = self.selected_count + 1
        self.pet = mono:GetData()
    end
    self.SelectPetsNum.text = string.format(self.lang.FreeSelectedPets, tostring(self.selected_count))
end

function PetFreeLifeSeveral:OnClickFreeLife()
    if self.selected_count <= 0 then
        PublicPopupCtrl.Instance:Center(self.lang.FreeSelectedEmpty)
    else
        local info = {
            content_func = function()
                if self.selected_pets_index ~= nil then
                    PetCtrl.SendAbandonPets(self.selected_pets_index)
                end
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.BagClose)
                ViewMgr:CloseView(PetFreeLifeSeveral)
            end
        }
        if next(self.need_notice_pets_index) then
            info.content = self.lang.FreeSelectedGoodPet
        else
            if self.selected_count > 1 then
                info.content = string.format(self.lang.FreeLifeSeveral, self.pet:Name(), self.selected_count)
            else
                info.content = string.format(self.lang.FreeLife, self.pet:Level(), self.pet:Name())
            end
        end
        PublicPopupCtrl.Instance:DialogTips(info)
    end 
end

----------------批量放生选择--------------------
PetFreeLifePetItem = PetFreeLifePetItem or DeclareMono("PetFreeLifePetItem", UIWidgetBaseItem)
function PetFreeLifePetItem:PetFreeLifePetItem()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function PetFreeLifePetItem:SetData(data)
    local pet = data
    if self.IsShop then
        local info = data
        pet = Pet.New(data.item_detail.param)
        UH.SetText(self.LblPrice, info.price)
    end
    -- self.Cell:SetData(pet)
    UH.SetText(self.PetLevel, string.format(self.lang.LevelStr, pet:Level()))
    self.PetOnDefending:SetActive(self.pet_data:GetIsArena(pet.index))
    UIWidgetBaseItem.SetData(self, data)
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblName, pet.name)
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr, COLORSTR.Yellow10, COLORSTR.Green11, v1, v2))
    PetData.SetStrLevel(nil, self.LevelNum, pet.str_level)
    UH.SetIcon(self.Icon, data:IconId())
    UH.SpriteName(self.Qua, data:QuaIcon())
    self.LockObj:SetActive(pet.info.lock == 1)
    if self.PrayFlag then
        self.PrayFlag:SetActive(pet:PrayMainIndex() ~= nil)
    end
    self.pet = pet
end

function PetFreeLifePetItem:SetSelected(selected)
    if self.pet.info.lock == 1 then
        local info = {
            content = Language.Pet.FreeSelectedLockPet,
            confirm = {
                    func = function ()
                        PetCtrl.SendPetOp(PET_OP_TYPE.Lock, self.pet.index)
                        ViewMgr:CloseView(DialogTipsView)
                    end
                },
            }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    UIWidgetBaseItem.SetSelected(self, selected)
    UH.SpriteName(self.SelectBgImg, selected and "JianBianXuanZhongDi" or "JianBianWeiXuanZhongDi")
    UH.SpriteName(self.SelectLineImg, selected and "PetSplitYellow" or "PetSplitGreen")
end

function PetFreeLifePetItem:OnClickInfo()
    ViewMgr:OpenView(PetDetailView, self.pet)
end