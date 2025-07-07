PetWeaponBookView = PetWeaponBookView or DeclareView("PetWeaponBookView", "pet/pet_weapon/pet_weapon_book_view")
function PetWeaponBookView:PetWeaponBookView()
    self.data = PetWeaponData.Instance
    if self.ht == nil then
        self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.CheckClickObjActive,self),0.2)
    end
    -- list的宽
    self.list_w = 990
    self.item_w = 400
    self.page_x = (self.item_w + 20) * 2
end

function PetWeaponBookView:LoadCallback()
    -- self.LeftBtnObj:SetActive(false)
    -- content_x的最大位置 = item数目 * (item宽 + spac间距) - spac间距 * 1 - list的宽 * 1
    self.max_x = #self.data.cfg.pet_weapon_cfg * (self.item_w + 20) - 20 - self.list_w
    self.one_x = self.item_w + 20  -- (item宽 + spac间距)
    local list_data = {}
    for k, v in pairs(self.data.cfg.pet_weapon_cfg) do
        if v.is_show == 1 then
            --table.insert(list_data, TableCopy(v))
            table.insert(list_data, v)
        end
    end
    table.sort(list_data,function (a,b)
        if a.quality == b.quality then
            return a.seq < b.seq
        end
        return a.quality < b.quality
    end)
    self.List:SetData(list_data)
    self.MaskObj:SetActive(false)
end

function PetWeaponBookView:CloseCallback()
    TimeHelper:CancelTimer(self.ht)
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
end

function PetWeaponBookView:CheckClickObjActive()
    if self.max_x == nil then
        return
    end
    local content_x = self.Content:GetLocalPosition().x
    -- local min_x = 0
    -- self.RightBtnObj:SetActive(-content_x < self.max_x - 20)
    -- self.LeftBtnObj:SetActive(-content_x > min_x + 20)
    self.content_x = content_x
end

function PetWeaponBookView:OnCloseClick()
    ViewMgr:CloseView(PetWeaponBookView)
end

function PetWeaponBookView:OnOpenConvertViewClick()
    ViewMgr:OpenView(PetWeaponConvertView)
end

function PetWeaponBookView:OnOpenTreasureViewClick()
    ViewMgr:OpenView(TreasureHuntView,{open_param = 2})
end

function PetWeaponBookView:OnNextClick(dir)
    if self.is_animtion then
        return
    end
    self.content_x = self.content_x or self.Content:GetLocalPosition().x
    local page = math.floor(-self.content_x / self.page_x)
    page = page < 0 and 0 or page
    page = page + dir
    local x = page * self.page_x
    if x > self.max_x then
        x = 0
        self.Content:SetLocalPosition(Vector3.New(x, 0, 0))
    elseif x < 0 then
        x = -self.max_x
        self.Content:SetLocalPosition(Vector3.New(x, 0, 0))
    else
        self:SetPos(-x)
    end
end

function PetWeaponBookView:SetPos(target_x)
    self.move_x = self.Content:GetLocalPosition().x
    self.move_num = (target_x - self.move_x) / 10
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    self.is_animtion = true
    self.MaskObj:SetActive(true)
    self.ScrollRect:StopMovement()
    self.run_count = 0
    self.item_timer = Runner.Instance:RunUntilTrue(function ()
        if self.move_x == target_x or self.run_count >= 10 then
            self.is_animtion = false
            self.MaskObj:SetActive(false)
            self.Content:SetLocalPosition(Vector3.New(target_x, 0, 0))
            return true
        end
        self.run_count = self.run_count + 1
        self.move_x = self.move_x + self.move_num
        self.Content:SetLocalPosition(Vector3.New(self.move_x, 0, 0))
    end)
end


PetWeaponBookItem = PetWeaponBookItem or DeclareMono("PetWeaponBookItem", UIWidgetBaseItem)

function PetWeaponBookItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local weapon = Item.Create({item_id = data.pet_weapon_item})
    weapon.is_book = true
    local qua = weapon:QuaType()
    UH.SpriteName(self.Bg, string.format("YanSeDi_%s", qua))
    UH.SpriteName(self.Qua, string.format("YanSeDi_%s_%s", qua, qua))
    self.Cell:SetData(weapon)
    UH.SetText(self.Name, weapon:QuaName())
    UH.SetText(self.Desc, weapon:BaseCfg().skill_txt)

    local pet_id = weapon:BaseCfg().pet_weapon_pet_id
    local skill_title
    if pet_id and pet_id > 0 then
        self.PetInfoObj:SetActive(true)
        local pet = Item.Create({item_id = pet_id})
        self.PetCell:SetData(pet)
        UH.SetText(self.PetName, pet:Name())
        skill_title = Language.PetWeapon.SkillTitle2
    else
        self.PetInfoObj:SetActive(false)
        skill_title = Language.PetWeapon.SkillTitle1
    end
    UH.SetText(self.SkillTitle, skill_title)

    local min_attri_datas = weapon:AttriListSort()
    local max_attri_datas = PetWeaponData.Instance:GetWeaponMaxAttri(weapon)
    local attri_data_1 = {attri_type = min_attri_datas[1].attri_type, attri_value = min_attri_datas[1].attri_value, max_value = max_attri_datas[min_attri_datas[1].attri_type].attri_value}
    local attri_data_2 = {attri_type = min_attri_datas[2].attri_type, attri_value = min_attri_datas[2].attri_value, max_value = max_attri_datas[min_attri_datas[2].attri_type].attri_value}
    self.AttriItem1:SetData(attri_data_1)
    self.AttriItem2:SetData(attri_data_2)
end