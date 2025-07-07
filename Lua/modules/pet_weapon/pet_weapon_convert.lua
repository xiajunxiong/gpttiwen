PetWeaponConvertView = PetWeaponConvertView or DeclareView("PetWeaponConvertView", "pet/pet_weapon/pet_weapon_convert", Mod.PetWeaponConvert.Main)
VIEW_DECLARE_LEVEL(PetWeaponConvertView, ViewLevel.L0)

PetWeaponConvertView.show_type = 1      -- 1 显示橙色魂器兑换，2显示红色

function PetWeaponConvertView:PetWeaponConvertView()
    self.data = PetWeaponData.Instance
    self.sel_index = 1
    self.one_x = 1140 / 4
    self.content_x = 0
    if self.ht == nil then
        self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.CheckClickObjActive,self),0.2)
    end
end

function PetWeaponConvertView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.mod_key == Mod.PetWeaponConvert.Qua6 then
        self.show_type = 2
    else
        self.show_type = 1 
    end
    PetWeaponConvertView.show_type = self.show_type
    if self.handle_care then
        BagData.Instance:UnCare(self.handle_care)
        self.handle_care = nil
    end
    self.handle_care = BagData.Instance:Care(BindTool.Bind(self.FlushPanel, self))
    
    local list_data = self.data:GetConvertDataList(self.show_type)
    self.sel_max_index = #list_data
    self.max_page = math.ceil(self.sel_max_index / 4)
    self.List:SetData(list_data)
    self.List:SetectItem(self.sel_index, true)

    self.RightObj:SetActive(self.max_page > 1)
    self.LeftObj:SetActive(false)
end
function PetWeaponConvertView:CloseCallback()
    if self.handle_care then
        BagData.Instance:UnCare(self.handle_care)
        self.handle_care = nil
    end
    TimeHelper:CancelTimer(self.ht)
end

function PetWeaponConvertView:FlushPanel()
    if self.sel_data == nil then
        return
    end
    local stuff_id = self.sel_data.expend_item_id
    local need_num = self.sel_data.expend_num
    local stuff_item = Item.Create({item_id = stuff_id})
    self.CommonItem:SetData(stuff_item)
    UH.SetText(self.CommonRighttNum, DataHelper.ConsumeNum(Item.GetNum(stuff_id), need_num, true))
    UH.SetText(self.StuffNoticeText, string.format(Language.PetWeapon.ConvertTip1,need_num,stuff_item:QuaName(false)))
end

function PetWeaponConvertView:OnSelClick(data)
    self.sel_data = data
    self.sel_index = self.List:GetSelectedIndex(data)
    self:FlushPanel()
end

function PetWeaponConvertView:OnClickClose()
    ViewMgr:CloseView(PetWeaponConvertView)
end

function PetWeaponConvertView:OnNextClick(dir)
    local page = self:GetPage(-self.content_x)
    page = page + dir
    page = page < 0 and 0 or page
    local one_x = 1140 / 4
    local x = page * one_x * 4
    local max_x = (self.sel_max_index - 4) * one_x
    x = x > max_x and max_x or x
    self.Content:SetLocalPosition(Vector3.New(-x, 0, 0))
end

function PetWeaponConvertView:OnConvertClick()
    local stuff_id = self.sel_data.expend_item_id
    local need_num = self.sel_data.expend_num
    local has_num = Item.GetNum(stuff_id)
    if has_num < need_num then
        --PublicPopupCtrl.Instance:Error(string.format(Language.PetWeapon.ConvertTip2,Item.GetName(stuff_id)))
        local item = Item.Create({item_id = stuff_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetWeaponConvert, self.sel_data.pet_weapon_seq)
end

function PetWeaponConvertView:CheckClickObjActive()
    if self.sel_max_index == nil then
        return
    end
    local content_x = self.Content:GetLocalPosition().x
    local min_x = 0
    local max_x = self.one_x * (self.sel_max_index - 4)
    self.RightObj:SetActive(-content_x < max_x - 20)
    self.LeftObj:SetActive(-content_x > min_x + 20)
    self.content_x = content_x
end

function PetWeaponConvertView:GetPage(content_x)
    local page = math.floor(content_x / 1140)
    local max_x = self.one_x * (self.sel_max_index - 4)
    if content_x > max_x - 20 then
        page = page + 1
    end
    return page
end



PetWeaponConvertItem = PetWeaponConvertItem or DeclareMono("PetWeaponConvertItem", UIWidgetBaseItem)
function PetWeaponConvertItem:PetWeaponConvertItem()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
            }
        )
    end
end
function PetWeaponConvertItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.pet_weapon_item_id})
    UH.SetText(self.Name1, item:EffectName())
    UH.SetText(self.Name2, item:Name())
    if item:ModelId() then
        self.ui_obj:SetData(DrawerHelper.GetWeaponPath(item:ModelId()))
    end

    local pet_id = item:BaseCfg().pet_weapon_pet_id
    if pet_id and pet_id > 0 then
        local qua = Item.GetColor(pet_id) - 1
        UH.SpriteName(self.PetQua, string.format("_LocJieType%s",qua))
    end
end
function PetWeaponConvertItem:OnClickSelect()
    PetData.Instance:ConvertSelect(self.data)
end

function PetWeaponConvertItem:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end
function PetWeaponConvertItem:OnClickDetail()
    local list_data = PetWeaponData.Instance:GetConvertDataList(PetWeaponConvertView.show_type)
    local list = {}
    for i,v in ipairs(list_data) do
        table.insert(list, v.pet_weapon_item_id)
    end
    ViewMgr:OpenView(PetWeaponConvertInfoView, {item_id = self.data.pet_weapon_item_id, list = list})
end

-- ========== 兑换详情 =========
PetWeaponConvertInfoView = PetWeaponConvertInfoView or DeclareView("PetWeaponConvertInfoView", "pet/pet_weapon/pet_weapon_convert_info")
function PetWeaponConvertInfoView:PetWeaponConvertInfoView()
    self.data = PetWeaponData.Instance
end

function PetWeaponConvertInfoView:LoadCallback(param_t)
    param_t = param_t or {}
    self.sel_item_id = param_t.item_id
    self.list_data = param_t.list
    if not self.sel_item_id then
        return
    end

    if TableIsEmpty(self.list_data) then
        self.list_data = {self.sel_item_id}
    end

    self.sel_index = 1
    self.max_sel_index = #self.list_data
    for i,v in pairs(self.list_data) do
        if v == self.sel_item_id then
            self.sel_index = i
        end
    end

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
            }
        )
    end

    self.RightObj:SetActive(self.sel_index < self.max_sel_index)
    self.LeftObj:SetActive(self.sel_index > 1)

    self:FlushPanel()
end

function PetWeaponConvertInfoView:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end

function PetWeaponConvertInfoView:FlushPanel()
    if not self.sel_item_id then
        return
    end

    local item = Item.Create({item_id = self.sel_item_id})
    UH.SetText(self.Name1, item:EffectName())
    if item:ModelId() then
        self.ui_obj:SetData(DrawerHelper.GetWeaponPath(item:ModelId()))
    end
    local pet_id = item:BaseCfg().pet_weapon_pet_id
    if pet_id and pet_id > 0 then
        local qua = Item.GetColor(pet_id) - 1
        UH.SpriteName(self.PetQua, string.format("_LocJieType%s",qua))
    end

    UH.SetText(self.SkillDesc, item:BaseCfg().skill_txt)

    local min_attri_datas = item:AttriListSort()
    local max_attri_datas = self.data:GetWeaponMaxAttri(item)
    local attri_data_1 = {attri_type = min_attri_datas[1].attri_type, attri_value = min_attri_datas[1].attri_value, max_value = max_attri_datas[min_attri_datas[1].attri_type].attri_value}
    local attri_data_2 = {attri_type = min_attri_datas[2].attri_type, attri_value = min_attri_datas[2].attri_value, max_value = max_attri_datas[min_attri_datas[2].attri_type].attri_value}
    self.AttriItem1:SetData(attri_data_1)
    self.AttriItem2:SetData(attri_data_2)
end

function PetWeaponConvertInfoView:OnCloseClick()
    ViewMgr:CloseView(PetWeaponConvertInfoView)
end

function PetWeaponConvertInfoView:OnNextClick(dir)
    self.sel_index = self.sel_index + dir
    if self.sel_index < 1 then
        self.sel_index = 1
    end
    if self.sel_index > self.max_sel_index then
        self.sel_index = self.max_sel_index
    end
    self.RightObj:SetActive(self.sel_index < self.max_sel_index)
    self.LeftObj:SetActive(self.sel_index > 1)
    self.sel_item_id = self.list_data[self.sel_index]
    self:FlushPanel()
end

PetWeaponConvertInfoAttriItem = PetWeaponConvertInfoAttriItem or DeclareMono("PetWeaponConvertInfoAttriItem", UIWidgetBaseItem)
function PetWeaponConvertInfoAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = Language.Common.AttrList[data.attri_type]
    UH.SetText(self.Name, name)

    if DataHelper.Percent(data.attri_type) then
        UH.SetText(self.Value, string.format("+%s%%",data.attri_value/10))
        UH.SetText(self.MaxValue, string.format(Language.PetWeapon.ConvertTip3, name, data.max_value/10))
    else
        UH.SetText(self.Value, string.format("+%s",data.attri_value))
        UH.SetText(self.MaxValue, string.format(Language.PetWeapon.ConvertTip4, name, data.max_value))
    end
end
