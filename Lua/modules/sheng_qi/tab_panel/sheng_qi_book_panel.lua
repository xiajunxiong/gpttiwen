ShengQiBookPanel = ShengQiBookPanel or DeclareMono("ShengQiBookPanel", UIWFlushPanel)
function ShengQiBookPanel:ShengQiBookPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sel_book_data, func = self.FlushInfo, init_call = false},
    }
    self.type_list = {
        {text = Language.ShengQi.TypeName[0], type = SHENG_QI_TYPE.COMMON},
        {text = Language.ShengQi.TypeName[1], type = SHENG_QI_TYPE.ATTACK},
        {text = Language.ShengQi.TypeName[2], type = SHENG_QI_TYPE.HELP},
        --{text = Language.ShengQi.TypeName[3], type = SHENG_QI_TYPE.SHOOT},
    }
end

function ShengQiBookPanel:Awake()
    UIWFlushPanel.Awake(self)

    self.TypeList:SetData(self.type_list)
end

function ShengQiBookPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    -- if self.data:GetTargetBook() then
    --     self:FlushPanel()
    -- end
    self.cur_type_data = self.type_list[1]
    if not self.data:GetTargetBook() then
        self.data:SetTargetBook(56000)
    end
    self:FlushPanel()
end

function ShengQiBookPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetBookSelData({})
end

function ShengQiBookPanel:FlushPanel()
    self:FlushList()
    self:FlushInfo()
end

function ShengQiBookPanel:FlushList()
    UH.SetText(self.SelTypeText, self.cur_type_data.text)
    self.data:SetBookSelData({})
    local list_data = self.data:GetBookList(self.cur_type_data.type)
    self.List:SetData(list_data)
end

function ShengQiBookPanel:FlushInfo()
    local info = self.data:GetBookSelData()
    if TableIsEmpty(info) then
        return
    end
    local sheng_qi = Item.Create({item_id = info.item_id})
    self.sheng_qi = sheng_qi
    UH.SetText(self.Desc, info.shengqi_dec)
    self.ModelInfo:SetData(sheng_qi)

    local skill_list = {}
    local skill_id_list = Split(info.shanchang_skill, "|")
    for i,v in pairs(skill_id_list) do
        local skill_id = tonumber(v)
        local skill = ShengQiSkill.New({skill_id = skill_id, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, is_book = true})
        table.insert(skill_list, skill)
    end
    self.SkillList:SetData(skill_list)

    local attri_list = self.sheng_qi:MaxPreviewPetAttriList()
    self.AttriList:SetData(attri_list)
end

function ShengQiBookPanel:OnTypeClick(data)
    self.SelToggle.isOn = false
    self.cur_type_data = data
    self:FlushList()
end

function ShengQiBookPanel:OnGetClick()
    ShengQiCtrl.GetShengQiJump()
end

function ShengQiBookPanel:OnPreviewBtn()
    if not self.sheng_qi then
        return
    end
    local info = {
		item_list = {self.sheng_qi},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

ShengQiBookListItem = ShengQiBookListItem or DeclareMono("ShengQiBookListItem", UIWidgetBaseItem)
function ShengQiBookListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local type_name = Language.ShengQi.TypeName[data.type]
    UH.SetText(self.Title, type_name)
    self.List:SetData(data.list)
    -- 计算高度
    local list_h = 0
    if not TableIsEmpty(data.list) then
        list_h = math.ceil(#data.list / 4) * (76 + 14) - 14
    end
    local h = 50 + list_h
    self.Trans:SetSizeWithCurrentAnchors(1,h)
end

ShengQiBookCellItem = ShengQiBookCellItem or DeclareMono("ShengQiBookCellItem", UIWidgetBaseItem)
function ShengQiBookCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    self.item = item
    self.Cell:SetData(item)
    if TableIsEmpty(ShengQiData.Instance.sel_book_data) then
        local target_book = ShengQiData.Instance:GetTargetBook()
        if target_book == data.item_id then
            ShengQiData.Instance:SetBookSelData(data)
            self:SetSelected(true)
            ShengQiData.Instance:SetTargetBook(nil)
        elseif target_book == nil then
            ShengQiData.Instance:SetBookSelData(data)
            self:SetSelected(true)
        end
    end
end

function ShengQiBookCellItem:OnSelClick()
    ShengQiData.Instance:SetBookSelData(self.data)
end