-- 神印背景
PetStampBgPanel = PetStampBgPanel or DeclareMono("PetStampBgPanel", UIWidgetBaseItem)
function PetStampBgPanel:SetData(view_type, title, title2,v2)
    self.view_type = view_type
    UH.SetText(self.Title, title)
    if size then
        self.Trans.sizeDelta = v2
    end
    self.Title2:SetObjActive(title ~= nil)
    if title then
        UH.SetText(self.Title2, title2)
    end
end

function PetStampBgPanel:OnCloseClick()
    if self.view_type then
        ViewMgr:CloseView(self.view_type)
    end
end

-- =============== 套装信息 ===============
PetStampSuitInfoView = PetStampSuitInfoView or DeclareView("PetStampSuitInfoView", "pet/pet_stamp/pet_stamp_suit_info_view")
function PetStampSuitInfoView:PetStampSuitInfoView()
    self.BG:SetData(self:GetType(),Language.PetStamp.Title1,Language.PetStamp.Title2)
    self.index = 1
end

function PetStampSuitInfoView:LoadCallback(suit_id_list)
    self.suit_id_list = suit_id_list or {}
    self:FlushPanel()
end

function PetStampSuitInfoView:FlushPanel()
    if TableIsEmpty(self.suit_id_list) then
        return
    end
    local suit_id = self.suit_id_list[self.index]
    suit_id = tonumber(suit_id)
    self.LeftBtnObj:SetActive(self.suit_id_list[self.index - 1] ~= nil)
    self.RightBtnObj:SetActive(self.suit_id_list[self.index + 1] ~= nil)
    local color_level = 5
    local suit_group = PetStampData.Instance.suit_group[suit_id][color_level]
    if not suit_group then
        return
    end
    UH.SetText(self.Name, suit_group[1].god_print_name)
    UH.SetText(self.Tips, suit_group[1].recommend_txt)
    UH.SetIcon(self.Icon, suit_group[1].god_print_icon)
    --local desc = string.format(Language.PetStamp.SuitTowTip, suit_group[1].god_print_txt, suit_group[2].god_print_txt)
    local desc = PetStampData.Instance:GetSuitDesc(suit_id, 4, Language.PetStamp.ItemTip3)
    UH.SetText(self.Desc, desc)
end

function PetStampSuitInfoView:OnClick(value)
    self.index = self.index + value
    self:FlushPanel()
end

function PetStampSuitInfoView:OnOpenClick()
    ViewMgr:OpenView(PetStampSuitListView)
end

PetStampSuitListView = PetStampSuitListView or DeclareView("PetStampSuitListView","pet/pet_stamp/pet_stamp_suit_list_view")
function PetStampSuitListView:PetStampSuitListView()
    self.data = PetStampData.Instance
    self.qua = ItemColor.Orange
end

function PetStampSuitListView:LoadCallback(param_t)
    UH.SetText(self.Title, Language.PetStamp.Title4)
    local qua = self.data:DefaultSuitQua()
    self.Btns:SetActive(qua == ItemColor.Red)
    self:FlushPanel()
end

function PetStampSuitListView:FlushPanel()
    local suit_list = self.data:GetSuitListData(nil, self.qua)
    self.List:SetData(suit_list)
end

function PetStampSuitListView:OnCloseClick()
    ViewMgr:CloseView(PetStampSuitListView)
end

function PetStampSuitListView:OnQuaClick(qua)
    self.qua = qua
    self:FlushPanel()
end

PetStampSuitListItem = PetStampSuitListItem or DeclareMono("PetStampSuitListItem", UIWidgetBaseItem)
function PetStampSuitListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local suit_id = data.id
    local qua = data.qua
    local suit_group = PetStampData.Instance.suit_group[suit_id][qua]
    UH.SetText(self.Desc, PetStampData.Instance:GetSuitDescByQua(suit_id, 4, qua, nil,COLORSTR.Red8))
    if data.qua and data.qua == 6 then
        UH.SetText(self.Name, ColorStr(suit_group[1].god_print_name, COLORSTR.Red8))
    else
        UH.SetText(self.Name, suit_group[1].god_print_name)
    end
    UH.SetIcon(self.Icon, suit_group[1].god_print_icon)
end


PetStampSuitInfoDialog = PetStampSuitInfoDialog or DeclareView("PetStampSuitInfoDialog", "pet/pet_stamp/pet_stamp_suit_info_dialog")
VIEW_DECLARE_MASK(PetStampSuitInfoDialog,ViewMask.BlockClose)
function PetStampSuitInfoDialog:PetStampSuitInfoDialog()
    
end

function PetStampSuitInfoDialog:LoadCallback(param_t)
    param_t = param_t or {}
    local suit_id = param_t.suit_id
    if suit_id == nil or suit_id < 1 then
        return 0
    end
    local qua = param_t.qua or 5
    suit_id = tonumber(suit_id)
    local suit_group = PetStampData.Instance.suit_group[suit_id][qua]
    if not suit_group then
        return
    end
    UH.SetText(self.Name, suit_group[1].god_print_name)
    UH.SetIcon(self.Icon, suit_group[1].god_print_icon)
    local desc = PetStampData.Instance:GetSuitDescByQua(suit_id, 4,qua, Language.PetStamp.ItemTip3, COLORSTR.Yellow1)
    UH.SetText(self.Desc, desc)
end