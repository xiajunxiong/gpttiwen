-- =============== 神印方案 ===============
PetStampBlueprintView = PetStampBlueprintView or DeclareView("PetStampBlueprintView", "pet/pet_stamp/pet_stamp_blueprint_view")
function PetStampBlueprintView:PetStampBlueprintView()
    self.BG:SetData(self:GetType(),Language.PetStampBlueprint.Title,Vector2.New(996, 690))
end

PetStampBlueprintPanel = PetStampBlueprintPanel or DeclareMono("PetStampBlueprintPanel", UIWFlushPanel)
-- show_type = 1 显示方案列表， show_type = 2 显示编辑列表
function PetStampBlueprintPanel:PetStampBlueprintPanel()
    self.data = PetStampData.Instance
    self.show_type = 1
    self.data_cares = {
        {data = self.data.blueprint_list, func = self.FlushDisplay, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushDisplay, init_call = false},
    }
end

function PetStampBlueprintPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.cur_pet_index = PetData.Instance:GetSelectPetIndex()
    self:FlushDisplay()
end

function PetStampBlueprintPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    local pet = PetData.Instance:GetCurSelect()
    local buleprint = pet:StampBlueprint()
    self.data:SetCurBuleprint(buleprint)
    TimeHelper:CancelTimer(self.time_ht_list)
end

function PetStampBlueprintPanel:FlushDisplay()
    self:SetSelBlueprint()
    self.BtnsObj:SetActive(self.show_type == 1)
    self.BlueprintListObj:SetActive(self.show_type == 1)
    self.Bag:SetObjActive(self.show_type == 2)
    local pet_index = self.sel_blueprint:PetIndex()
    local index = self.sel_blueprint:Index()
    self.ExitBtnObj:SetActive(self.show_type == 2)
    self.SaveObj:SetActive(self.show_type == 2 and index >= 0)
    if pet_index and self.show_type == 1 and pet_index == self.cur_pet_index then
        UH.SetText(self.UseBtnText, Language.PetStampBlueprint.Btn1)
        UH.SetText(self.DelText, Language.PetStampBlueprint.Btn2)
    else
        UH.SetText(self.UseBtnText, Language.PetStampBlueprint.Btn3)
        UH.SetText(self.DelText, Language.PetStampBlueprint.Btn4)
    end

    if self.show_type == 1 then
        self:FlushBlueprintList()
    end
    self:FlushBlueprintInfo()
end

function PetStampBlueprintPanel:SetSelBlueprint()
    self.pet = PetData.Instance:GetCurSelect()
    if self.sel_blueprint == nil then
        self.pet = PetData.Instance:GetCurSelect()
        self.sel_blueprint = self.pet:StampBlueprint()
    else
        local index = self.sel_blueprint:Index()
        if index and index > -1 then
            self.sel_blueprint = self.data:GetBlueprintByIndex(index)
        else
            self.sel_blueprint = self.pet:StampBlueprint()
        end
    end
    self.data:FlushBag()
    --LogError("当前操作的神印方案::::::::",self.sel_blueprint)
end

function PetStampBlueprintPanel:FlushBlueprintInfo()
    self:SetSelBlueprint()
    self.data:SetCurBuleprint(self.sel_blueprint)
    self.BlueprintInfo:SetData(self.sel_blueprint)
    self.InputText.text = self.sel_blueprint:Name()

    -- 属性加成
    local attri_list = self.data:CompareBlueprint(self.sel_blueprint,{})
    self.AttriList:SetData(attri_list)

    -- 套装描述
    local suit_list = self.sel_blueprint:SuitInfo()
    suit_list = suit_list or {}
    local suit_len = #suit_list
    local suit_desc = Language.PetStampBlueprint.SuitTip1
    if suit_len == 2 then
        local suit_group_1 = PetStampData.Instance.suit_group[suit_list[1].suit_id][suit_list[1].qua][1]
        local suit_group_2 = PetStampData.Instance.suit_group[suit_list[2].suit_id][suit_list[2].qua][1]
        suit_desc = string.format(Language.PetStampBlueprint.SuitTip2,suit_group_1.god_print_txt,suit_group_2.god_print_txt)
    elseif suit_len == 1 then
        local suit_group = PetStampData.Instance.suit_group[suit_list[1].suit_id][suit_list[1].qua]
        if suit_list[1].num == 4 then
            suit_desc = string.format(Language.PetStampBlueprint.SuitTip4,suit_group[1].god_print_txt,suit_group[2].god_print_txt)
        else
            suit_desc = string.format(Language.PetStampBlueprint.SuitTip5,suit_group[1].god_print_txt)
        end
    end
    UH.SetText(self.SuitDesc, suit_desc)
    
end

function PetStampBlueprintPanel:FlushBlueprintList()
    local list = self.data:GetBlueprintOkList()
    self.blueprint_list = list
    TimeHelper:CancelTimer(self.time_ht_list)
    self.time_ht_list = Invoke(function (  )
        self:flushBlueprintList()
    end, 0.1)
end

function PetStampBlueprintPanel:flushBlueprintList()
    if self.blueprint_list == nil then
        return
    end
    local list = self.blueprint_list
    local index = 1
    local data
    if self.sel_blueprint then
        for i,v in pairs(list) do
            if v.index == self.sel_blueprint.index then
                index = i
                break
            end
        end
    end
    self.BlueprintList:SetData(list)
    self.BlueprintList:SetectItem(index,true)
end


function PetStampBlueprintPanel:OnSelClick(data)
    self.sel_blueprint = data
    self:FlushDisplay()
end

function PetStampBlueprintPanel:OnItemReplaceClick(data)
    local is_pet_index
    for i,v in pairs(data:Stamps()) do
        local stamp = self.data:GetPetStampByIndex(v)
        if stamp and stamp:PetIndex() then
            is_pet_index = true
        end
    end
    local replace_func = function ()
        PetStampCtrl.Instance:UseBlueprint(data.index)
    end
    if is_pet_index then
        PublicPopupCtrl.Instance:AlertTip(Language.PetStamp.Tip4, function (  )
            replace_func()
        end)
    else
        replace_func()
    end
end

-- -------------   Click事件
-- 删除事件
function PetStampBlueprintPanel:OnDeleteClick()
    self.sel_blueprint:Delete()
    self.sel_blueprint = nil
    self:SetSelBlueprint()
end
-- 编辑事件
function PetStampBlueprintPanel:OnEditClick()
    self.show_type = 2
    self:FlushDisplay()
end
-- 分享事件
function PetStampBlueprintPanel:OnShareClick()

end
-- 替换/新加事件
function PetStampBlueprintPanel:OnUseClick()
    local pet_index = self.sel_blueprint:PetIndex()
    if pet_index and pet_index == self.cur_pet_index then
        if self.sel_blueprint:IsOk() then
            self:OnAddClick(self.sel_blueprint)
        else
            PublicPopupCtrl.Instance:Error(Language.PetStampBlueprint.Error3)
        end
    else
        self:OnItemReplaceClick(self.sel_blueprint)
    end
end
-- 保存事件
function PetStampBlueprintPanel:OnSaveClick()

    if ChatFilter.Instance:IsIllegal(self.InputText.text, true) then
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    end

    local string_len = DataHelper.GetStringWordNum(self.InputText.text)
    if string_len > 7 then
        PublicPopupCtrl.Instance:Error(Language.PetStampBlueprint.Error4)
        return
    end
    self.sel_blueprint.info.name = self.InputText.text
    PetStampCtrl.Instance:SetBlueprint(self.sel_blueprint)
    PublicPopupCtrl.Instance:Center(Language.PetStampBlueprint.Tip1)
end
-- 退出编辑
function PetStampBlueprintPanel:OnExitClick()
    if not self.sel_blueprint:IsOk() then
        self.sel_blueprint = self.last_buleprint
    end
    self.show_type = 1
    self:FlushDisplay()
end
-- 添加方案事件
function PetStampBlueprintPanel:OnAddClick(blueprint)
    local add_blueprint = self.data:GetEmptyBlueprint()
    if not add_blueprint then
        PublicPopupCtrl.Instance:Error(Language.PetStampBlueprint.Tip2)
        return
    end
    if blueprint then
        add_blueprint:Copy(blueprint)
        add_blueprint.info.name = string.format(Language.PetStampBlueprint.Name,add_blueprint.index + 1)
        PetStampCtrl.Instance:SetBlueprint(add_blueprint)
    else
        self.show_type = 2
    end
    self.last_buleprint = self.sel_blueprint
    self.sel_blueprint = add_blueprint
    self:FlushDisplay()
end

PetStampBlueprintItem = PetStampBlueprintItem or DeclareMono("PetStampBlueprintItem", UIWidgetBaseItem)
-- data ：blueprint
function PetStampBlueprintItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data:Name())
    local suit_list = data:SuitInfo()
    local suit_group
    if not TableIsEmpty(suit_list) then
        local info = suit_list[#suit_list]
        suit_group = PetStampData.Instance.suit_group[info.suit_id][info.qua]
    end
    if suit_group then
        UH.SetIcon(self.Icon, suit_group[1].god_print_icon)
    else
        UH.SetIcon(self.Icon, nil)
    end
    local cur_pet = PetData.Instance:GetCurSelect()
    self.UseObj:SetActive(data:PetIndex() ~= cur_pet.index)
end

PetStampBlueprintAttriItem = PetStampBlueprintAttriItem or DeclareMono("PetStampBlueprintAttriItem", UIWidgetBaseItem)
function PetStampBlueprintAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.BgObj:SetActive(data.show_bg == true)
    local attri_name = Language.Common.AttrList[data.attri_type]
    UH.SetText(self.Name, attri_name)

    local value_str
    if DataHelper.IsPercent(data.attri_type) then
        value_str = string.format("+%s%%",data.attri_value/10)
    else
        value_str = string.format("+%s",data.attri_value)
    end
    UH.SetText(self.Value,value_str)
end