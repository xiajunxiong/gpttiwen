PetAddPoint = PetAddPoint or DeclareView("PetAddPoint", "pet/pet_add_point")
function PetAddPoint:PetAddPoint()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end
function PetAddPoint:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.PetAddPoint, Vector2.New(877, 521))
    self.init_point_list = {}
    self.add_point = {}
end
function PetAddPoint:OnFlush()
    self.pet = self.pet_data:GetCurSelect()
    if self.pet == nil then
        LogError("PetAddPoint cur select pet is nil,index = ", self.pet_data.pet_select.index)
        return
    end
    self:ClearPreAddPoint()
    self.points = self.pet:Point()
    self.total, self.have = self.pet:PointNum()
    self.real_total = self.pet:PointNum()
    -- Debuger.LogError(Serialize(points))
    self.attrs = self.pet_data:RealAttr()
    for k, v in pairs(self.points) do
        UH.SetText(self["Lbl" .. k], v)
        self.init_point_list[k] = v
    end
    if self.attrs == nil then
        return
    end
    for k, v in pairs(self.attrs) do
        UH.SetText(self["Lbl" .. k], math.floor(v))
    end
    self.BtnWash.Interactable = self.pet:Level() == 1 and false or true
    for i = 1, self.AttrObjs:Length(), 1 do
        if i == self.pet:MainAttr() then
            UH.SetText(self.AttrObjs[i], string.format(self.lang.TuiJian, Language.Pet.AddPointAttrName[i]))
            UH.Color(self.AttrObjs[i],COLORS.Red12)
        end
    end
    -- PetData.GetAptiAttr(t)
    UH.SetText(self.LblPoint, string.format(self.lang.LeftPoint, COLORSTR.Green3, self.total))
    UH.SetText(
        self.LblValue,
        string.format(self.lang.TotalGrowth, COLORSTR.Green13, self.pet:TotalGrowth(), self.pet:ConfigGrowth())
    )

    local cur_auto_info = self.pet:AutoAddInfo()
    for i=1,self.Interactors:Length() do
        self.Interactors[i].Interactable = cur_auto_info.index == i or cur_auto_info.index == 0
        if cur_auto_info.index == 0 then
            if i == self.pet:MainAttr() then
                self.eff_handle = self.EffectTool:Play("2161048", self.AutoEffectPar[i])
            end
        elseif self.eff_handle ~= nil then
            self.EffectTool:Stop(self.eff_handle)
        end
    end
end

function PetAddPoint:OnClickAdd(index)
    -- if self.total <= 0 then
    --     PublicPopupCtrl.Instance:Center(ErrorText[14])
    --     return
    -- end
    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- self:OnClickDown(index)
    -- self:OnClickUp(index)
    self:OpenConfirmDialog(index)
end

function PetAddPoint:OpenConfirmDialog(index)
    local info = {
        confirm = { func = function (confirm_index)
                PetCtrl.Instance:SendPetAutoAddPoint(confirm_index)
            end,
        }
    }
    if self.pet == nil then
        return
    end
    local cur_auto_info = self.pet:AutoAddInfo()
    local click_attr_name = Language.Pet.AddPointAttrName[index]
    info.confirm.index = index
    if cur_auto_info.index == 0 then
        --新增自动
        info.content = string.format(Language.Pet.PetAutoAddNewContent, click_attr_name)
        info.confirm.name = Language.Pet.PetAutoAddOn

    elseif cur_auto_info.index == index then
        --取消自动
        info.content = string.format(Language.Pet.PetAutoAddCloseContent, click_attr_name)
        info.confirm.name = Language.Pet.PetAutoAddOff
        info.confirm.index = 0
    else
        --替换自动
        local origin_attr_name = Language.Pet.AddPointAttrName[cur_auto_info.index]
        info.content = string.format(Language.Pet.PetAutoAddReplaceContent, origin_attr_name, click_attr_name)
        info.confirm.name = Language.Pet.PetAutoAddOn

    end
    ViewMgr:OpenView(PetAutoConfirmDialog, info)
end

function PetAddPoint:OnClickDec(index)

end
function PetAddPoint:OnClickDown(index, num)
    num = num or self.total
    if num > 0 and (self.total <= 0 or self.total < num) then
        PublicPopupCtrl.Instance:Center(ErrorText[14])
        return
    end
    if num == -1 and (self.real_total - self.total) == 0 then
        return
    end
    TimeHelper:CancelTimer(self.ht)
    self.ht =
        TimeHelper:AddRunTimer(
        function()
            if self.total == 0 and num == 1 then
                TimeHelper:CancelTimer(self.ht)
                PublicPopupCtrl.Instance:Center(ErrorText[14])
                return
            end
            self:CalcPoint(index, num)
        end,
        0.2
    )
end
function PetAddPoint:OnDestroy()
    TimeHelper:CancelTimer(self.ht)
end
function PetAddPoint:OnClickUp(index)
    TimeHelper:CancelTimer(self.ht)
end
function PetAddPoint:CalcPoint(index, add)
    -- Debuger.LogError(self.real_total.."|"..self.total)
    local keys = self.lang.AttrClass[index]
    self.add_point[keys[1]] = (self.add_point[keys[1]] or 0) + add
    if self.add_point[keys[1]] < 0 then
        self.add_point[keys[1]] = 0
    else
        self.total = self.total - add
    end
    UH.SetText(
        self["Lbl" .. keys[5]],
        self.add_point[keys[1]] == 0 and 0 or
            "<color=#" .. COLORSTR.Green10 .. ">" .. self.add_point[keys[1]] .. "</color>"
    )
    UH.SetText(self.LblPoint, string.format(self.lang.LeftPoint, COLORSTR.Green3, self.total))
    -- 计算加点加的属性
    local add_attr = PetData.GetAptiAttr(self.add_point)
    self.attrs = self.pet_data:RealAttr()
    if self.attrs == nil then
        ViewMgr:CloseView(PetAddPoint)
        return
    end
    local gift_attr, ratio_attr, gift_attr2, ratio_attr2 = self.pet_data:GiftAttr(self.pet, self.attrs, add_attr)
    local temp = ""
    for k, v in pairs(self.attrs) do
        if add_attr[k] >= 1 then
            temp =
                math.floor(
                add_attr[k] + (gift_attr[k] or 0) + (ratio_attr[k] or 0) + (gift_attr2[k] or 0) + (ratio_attr2[k] or 0)
            )
            temp = string.format(self.lang.AddPointAttr1, v, temp)
            UH.SetText(self["Lbl" .. k], temp)
            UH.Color(self["Lbl" .. k], COLORS.Yellow13)
        else
            temp =
                math.floor(
                add_attr[k] + (gift_attr[k] or 0) + (ratio_attr[k] or 0) + (gift_attr2[k] or 0) + (ratio_attr2[k] or 0)
            )
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
function PetAddPoint:OnClickAddPoint()
    if next(self.add_point) == nil then
        return
    end
    local index = self.pet_data:GetSelectPetIndex()
    ViewMgr:CloseView(PetAddPoint)
    PetCtrl.SendPetAssignAttributePoints(index, self.add_point)
    
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(ErrorText[115])
    end
end
function PetAddPoint:OnClickCancel()
    ViewMgr:CloseView(PetAddPoint)
end

function PetAddPoint:ClearPreAddPoint()
    for i=1,5 do
        local keys = self.lang.AttrClass[i]
        self.add_point[keys[1]] = 0
        UH.SetText(self["Lbl" .. keys[5]], 0)
    end
end

function PetAddPoint:OnClickWashPoint()
    if self.have == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end

    local item_id = self.pet_data:GetPetOtherCfg().respec_item
    local item = Item.Create({item_id = item_id})
    if item:Num() == 0 and self.pet.info.has_refresh_points == 1 then
        local info = {
            content = Language.Role.PopupTips5,
            confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    MallCtrl.Instance:Jump(item_id)
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    else
        local index = self.pet_data:GetSelectPetIndex()
        local info = {
            content = self.pet.info.has_refresh_points == 1 and self.lang.WashTip or self.lang.FirstTimeWash,
            cancel = {},
            confirm = {
                func = function()
                    self:ClearPreAddPoint()
                    PetCtrl.SendPetOp(PET_OP_TYPE.RefreshPoints, index)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    end
end
-----------------------------------------
PetAutoConfirmDialog = PetAutoConfirmDialog or DeclareView("PetAutoConfirmDialog", "pet/pet_auto_add_confirm_dialog")
function PetAutoConfirmDialog:PetAutoConfirmDialog()
    self.data = PetData.Instance
end

function PetAutoConfirmDialog:LoadCallback(param)
    UH.SetText(self.Content, param.content)
    UH.SetText(self.ConfirmName, param.confirm.name)
    self.confirm_func = param.confirm.func
    self.confirm_index = param.confirm.index
end

function PetAutoConfirmDialog:OnClickConfirm()
    if self.confirm_func ~= nil then
        self.confirm_func(self.confirm_index)
    end
    self:OnClickClose()
end

function PetAutoConfirmDialog:OnClickClose()
    ViewMgr:CloseView(PetAutoConfirmDialog)
end