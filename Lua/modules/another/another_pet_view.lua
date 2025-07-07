AnotherPetView = AnotherPetView or DeclareMono("AnotherPetView", UIWFlushPanel)

function AnotherPetView:AnotherPetView()
    self.lang = Language.Pet
    self.pet_data = AnotherData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_select, func = self.FlushPetInfo, keys = {"index"}},
        {data = self.pet_data.pet_other_info, func = self.FlushPetInfo, init_call = false},
        {data = self.pet_data.pet_list, func = self.FlushPetInfo, init_call = false},
        {data = self.pet_data.pet_list, func = self.FlushPetList, init_call = false}
    }
    self:FlushPetList()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self,show_shadow = true,shadow_dir = ModelUIShow.ShadowCameraDir.Pet})
    end
end

function AnotherPetView:FlushPetList()
    local list = self.pet_data:GetPetList()
    list = table.rearrange(list)
    local function sort_func(a, b)
        local order_a = 100000
        local order_b = 100000
        --排序优先级（出战等状态>评分>等级>品质>强化等级>序号）
        if self.pet_data:GetFightIndex() == a.index then
            order_a = order_a + 100000
        end
        if self.pet_data:GetFightIndex() == b.index then
            order_b = order_b + 100000
        end
        if self.pet_data:GetIsStandBy(a.index) then
            order_a = order_a + 50000
        end
        if self.pet_data:GetIsStandBy(b.index) then
            order_b = order_b + 50000
        end
        if a:Quality() > b:Quality() then
            order_a = order_a + 100
        elseif a:Quality() < b:Quality() then
            order_b = order_b + 100
        end
        if a:Level() > b:Level() then
            order_a = order_a + 1000
        elseif a:Level() < b:Level() then
            order_b = order_b + 1000
        end
        if a:GetScore() > b:GetScore() then
            order_a = order_a + 10000
        elseif a:GetScore() < b:GetScore() then
            order_b = order_b + 10000
        end
        if a:StrLevel() > b:StrLevel() then
            order_a = order_a + 10
        elseif a:StrLevel() < b:StrLevel() then
            order_b = order_b + 10
        end
        if a.index < b.index then
            order_a = order_a + 1
        elseif a.index > b.index then
            order_b = order_b + 1
        end
        return order_a > order_b
    end
    table.sort(list, sort_func)
    local index = self.pet_data:GetSelectPetIndex()
    self.List:SetDataList(list,self:GetIndex(list,index))
    self.MaskItem:SetActive(self.pet_data:GetPetCount() == 0)
end

function AnotherPetView:GetIndex(list,index)
    for k,v in pairs(list) do
        if v.index == index then
            return k
        end
    end
end

function AnotherPetView:FlushPetInfo()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local need_exp = PetData.Instance:GetLevelExp(pet:Level())
    if self.old_pet == nil or self.old_pet ~= pet then
        self.ExpProgress:SetProgress(pet:Exp() / need_exp)
        self.old_pet = pet
    else
        self.ExpProgress:AddToProgress(pet:Exp() / need_exp)
    end
    self.ExpProgress:SetText(string.format(Language.Common.Slash, pet:Exp(), need_exp))
    self.Element:SetData(pet:EleAttr())
    UH.SetText(self.Level, string.format(self.lang.LevelStr, pet:Level()))
    UH.SpriteName(self.Strength, PetData.PetStrLevel[pet:StrLevel()])
    self:FlushPetState(pet.index)

    if self.curResid == -1 or self.curResid ~= pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
        self.curResid = pet:ResId()
    end
    --宠物护符
    self.BtnAmulet:SetActive(pet.info.pet_equipment_id == 0)
    self.EquipCellObj:SetActive(pet.info.pet_equipment_id ~= 0)
    if pet.info.pet_equipment_id ~= 0 and pet:GetPetEquipItem() ~= nil then
        self.EquipCell:SetData(pet:GetPetEquipItem())
    end
    -- 宠物限定图片
    self.PetLimitSign:SetType(pet:LimitType())

    --宠物属性
    local attr_data = {}
    for k, v in pairs(self.pet_data:RealAttr(pet.index) or {}) do
        attr_data[AttrTypeForName[k]] = v
    end
    attr_data[BattleAttriType.CurMp] = pet:MP()
    attr_data[BattleAttriType.CurHp] = pet:HP()
    attr_data[BattleAttriType.MaxHp] = pet:HP()
    attr_data[BattleAttriType.MaxMp] = pet:MP()

    local attr_list = self.pet_data:GetAttrTypeData(attr_data,
    RoleAttributeTypeBaseConfig,0,pet:MainAttr())
    for i=1,self.AttrList:Length() do
        self.AttrList[i]:SetData(attr_list[i])
    end
end

function AnotherPetView:FlushPetState(pet_index)
    local fight_index = self.pet_data:GetFightIndex()
    local standby_list = self.pet_data:GetStandbyList()
    local is_index_standby = false
    local is_index_relax = fight_index ~= pet_index

    for k,v in pairs(standby_list) do
        if v == pet_index then
            is_index_standby = true
            is_index_relax = false
        end
    end
    self.StateGroup[1].isOn = fight_index == pet_index
    self.StateGroup[2].isOn = is_index_standby
    self.StateGroup[3].isOn = is_index_relax
end

--宠物状态
function AnotherPetView:OnClickState(state)
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local cur_state = self.pet_data:GetPetState(pet.index)
    if cur_state == state then
        return
    end
    local pet_state = state
    if state == PetState.StandBy then
        local flag = self.pet_data:GetStandbyCanUse()
        if not flag and cur_state ~= PET_STATE_OP.FIGHT then
            self:FlushPetState(pet.index)
            PublicPopupCtrl.Instance:Center(Format(Language.Pet.PetOnFightMax,PetData.Instance:GetMaxPetOn()))
            return
        end
        pet_state = PET_STATE_OP.WAIT1
    end
    if state == PetState.Relax then
        pet_state = PET_STATE_OP.RELAX
    end
    AnotherCtrl.SendPetOp(AnotherPetOpType.SetPetState, pet.index, pet_state)
    if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene then
        PublicPopupCtrl.Instance:Center(ErrorText[110])
    end
end

--点击左边宠物
function AnotherPetView:OnClickPet(data)
    self.pet_data:SetSelect(data.index)
end

function AnotherPetView:OnClickAddPoint()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then return end
    self.AddPoint:SetActive(true)
end

function AnotherPetView:OnClickCancel()
    self.AddPoint:SetActive(false)
end

function AnotherPetView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.pet_data:SetSelect(-1)
end

function AnotherPetView:OnClickAmulet()
    local equip_list = self.pet_data:GetPetEquipmentList()
    if #equip_list > 0 then
        PetData.Instance.pet_equipment_select_another_open = true
        ViewMgr:OpenView(PetEquipSelectView)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[151])
    end
end

function AnotherPetView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(155)
end

AnotherPetAddPoint = AnotherPetAddPoint or DeclareMono("AnotherPetAddPoint", UIWFlushPanel)
function AnotherPetAddPoint:AnotherPetAddPoint()
    self.pet_data = AnotherData.Instance
    self.lang = Language.Pet
    self.init_point_list = {}
    self.add_point = {}
    self.data_cares = {
        {data = self.pet_data.pet_select, func = self.FlushAddPoint, keys = {"index"}, init_call = false},
        {data = self.pet_data.pet_attr_list, func = self.FlushAddPoint, init_call = false}
    }
end

function AnotherPetAddPoint:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:ClearPreAddPoint()
    self:FlushAddPoint()
end

function AnotherPetAddPoint:FlushAddPoint()
    if not self.AddPoint.activeSelf then
        return
    end
    self:ClearPreAddPoint()
    self.pet = self.pet_data:GetCurSelect()
    if self.pet == nil then
        LogError("AnotherPetAddPoint cur select pet is nil,index = ", self.pet_data.pet_select.index)
        return
    end
    self.points = self.pet:Point()
    self.total, self.have = self.pet:PointNum()
    self.real_total = self.pet:PointNum()
    self.attrs = self.pet_data:RealAttr()
    for k, v in pairs(self.points) do
        UH.SetText(self["Lbl" .. k], v)
        self.init_point_list[k] = v
    end
    for k, v in pairs(self.attrs) do
        UH.SetText(self["Lbl" .. k], math.floor(v))
    end
    self.BtnWash.Interactable = self.pet:Level() == 1 and false or true
    for i = 1, self.AttrObjs:Length(), 1 do
        if i == self.pet:MainAttr() then
            UH.SetText(self.AttrObjs[i], string.format(self.lang.TuiJianAnother, Language.Pet.AddPointAttrName[i]))
            UH.Color(self.AttrObjs[i], COLORS.Red12)
        else
            UH.SetText(self.AttrObjs[i], Language.Pet.AddPointAttrName[i])
            UH.Color(self.AttrObjs[i], COLORS.Yellow4)
        end
    end
    UH.SetText(self.LblPoint, string.format(self.lang.LeftPoint, COLORSTR.Green3, self.total))
    UH.SetText(self.LblValue, string.format(self.lang.TotalGrowth, COLORSTR.Green3, self.pet:TotalGrowth(), self.pet:ConfigGrowth()))
end

function AnotherPetAddPoint:OnClickAdd(index)
    if self.total <= 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[14])
        return
    end
    self:OnClickDown(index,self.total)
    self:OnClickUp(index)
end

function AnotherPetAddPoint:OnClickDown(index, num)
    if num == 1 and self.total == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[14])
        return
    end
    if num == -1 and (self.real_total - self.total) == 0 then
        return
    end
    TimeHelper:CancelTimer(self.ht)
    self.ht = TimeHelper:AddRunTimer(function()
            if self.total == 0 and num == 1 then
                TimeHelper:CancelTimer(self.ht)
                PublicPopupCtrl.Instance:Center(ErrorText[14])
                return
            end
        self:CalcPoint(index, num)
    end,0.2)
end

function AnotherPetAddPoint:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
end

function AnotherPetAddPoint:OnClickUp(index)
    TimeHelper:CancelTimer(self.ht)
end

function AnotherPetAddPoint:CalcPoint(index, add)
    local keys = self.lang.AttrClass[index]
    self.add_point[keys[1]] = (self.add_point[keys[1]] or 0) + add
    if self.add_point[keys[1]] < 0 then
        self.add_point[keys[1]] = 0
    else
        self.total = self.total - add
    end
    UH.SetText(self["Lbl" .. keys[5]], self.add_point[keys[1]] == 0 and 0 or "<color=#" .. COLORSTR.Green10 .. ">" .. self.add_point[keys[1]] .. "</color>")
    UH.SetText(self.LblPoint, string.format(self.lang.LeftPoint, COLORSTR.Green3, self.total))
    -- 计算加点加的属性
    local add_attr = PetData.GetAptiAttr(self.add_point)
    self.attrs = self.pet_data:RealAttr()
    local gift_attr, ratio_attr, gift_attr2, ratio_attr2 = self.pet_data:GiftAttr(self.pet, self.attrs, add_attr)
    local temp = ""
    for k, v in pairs(self.attrs) do
        if add_attr[k] >= 1 then
            temp = math.floor(add_attr[k] + (gift_attr[k] or 0) + (ratio_attr[k] or 0) + (gift_attr2[k] or 0) + (ratio_attr2[k] or 0))
            temp = string.format(self.lang.AddPointAttr1, v, temp)
            UH.SetText(self["Lbl" .. k], temp)
            UH.Color(self["Lbl" .. k], COLORS.Yellow13)
        else
            temp = math.floor(add_attr[k] + (gift_attr[k] or 0) + (ratio_attr[k] or 0) + (gift_attr2[k] or 0) + (ratio_attr2[k] or 0))
            if temp ~= 0 then
                temp = string.format(self.lang.AddPointAttr2, v, temp)
            else
                temp = v
            end
            UH.SetText(self["Lbl" .. k], temp)
            UH.Color(self["Lbl" .. k], COLORS.Yellow13)
        end
    end
    if self.total == 0 then
        TimeHelper:CancelTimer(self.ht)
        return
    end
    if (self.real_total - self.total) == 0 then
        TimeHelper:CancelTimer(self.ht)
        return
    end
end

function AnotherPetAddPoint:OnClickAddPoint()
    if next(self.add_point) == nil then
        return
    end
    local index = self.pet_data:GetSelectPetIndex()
    AnotherCtrl.SendPetPointReq(index, self.add_point)
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(ErrorText[115])
    end
    self.AddPoint:SetActive(false)
    AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.PET_POINT,index)
end

function AnotherPetAddPoint:ClearPreAddPoint()
    for i = 1, 5 do
        local keys = self.lang.AttrClass[i]
        self.add_point[keys[1]] = 0
        UH.SetText(self["Lbl" .. keys[5]], 0)
    end
end

function AnotherPetAddPoint:OnClickWashPoint()
    if self.have == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    local index = self.pet_data:GetSelectPetIndex()
    local info = {
        content = self.lang.AnotherWashTip,
        cancel = {},
        confirm = {
            func = function()
                self:ClearPreAddPoint()
                AnotherCtrl.SendPetOp(AnotherPetOpType.RefreshPoints, index)
                AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.PET_POINT,index)
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end