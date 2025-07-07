PetMultiPieces = PetMultiPieces or DeclareView("PetMultiPieces", "pet/pet_multi_pieces")

function PetMultiPieces:PetMultiPieces()
    self.view_type = PetData.Instance:GetMultiPiecesType()      -- 1 是批量合成，2是批量合成并分解
    local view_name = self.view_type == 1 and Language.PetMultiPieces.Text1 or Language.PetMultiPieces.Text2
    self.Board:SetData(self:GetType(),view_name,Vector2.New(1018, 568))
    PetData.Instance.pet_multi_pieces_data = {}
end

function PetMultiPieces:LoadCallback(param_t)
    param_t = param_t or {}
    local item = param_t.item
    if item then
        for i,v in pairs(Config.pet_cfg_auto.pet_compose) do
            if v.patch_id == item.item_id then
                PetData.Instance.pet_multi_pieces_data[v.seq] = 1
                self.Panel:FlushPanel()
                break
            end
        end
    end
end

function PetMultiPieces:CloseCallback()
    PetData.Instance.pet_multi_pieces_data = {}
end

PetMultiPiecesPanel = PetMultiPiecesPanel or DeclareMono("PetMultiPiecesPanel", UIWFlushPanel)
function PetMultiPiecesPanel:PetMultiPiecesPanel()
    self.data = PetData.Instance
    self.view_type = PetData.Instance:GetMultiPiecesType()
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Task), func = self.FlushPanel, init_call = false}
    }
end

function PetMultiPiecesPanel:Awake()
    UIWFlushPanel.Awake(self)
    local btn_txt = self.view_type == 1 and Language.PetMultiPieces.Text3 or Language.PetMultiPieces.Text4
    UH.SetText(self.BtnText, btn_txt)
    self:FlushPanel()
end

function PetMultiPiecesPanel:FlushPanel()
    local list_data = self.data:GetMultiPiecesCfg()
    if TableIsEmpty(list_data) then
        ViewMgr:CloseView(PetMultiPieces)
        return
    end
    self.List:SetData(list_data)
    self:FlushBottom()
end

function PetMultiPiecesPanel:FlushBottom()
    local num1,num2 = self.data:GetPetBagCapacity()
    local sel_num = PetMultiPiecesPanel.GetSelectAllNum()
    if self.view_type == 1 then
        local tip = string.format(Language.PetMultiPieces.Text5,sel_num, num2 + sel_num, num1)
        UH.SetText(self.Tip, tip)
        self.StuffList:SetData({})
    else
        local tip = string.format(Language.PetMultiPieces.Text6,sel_num)
        UH.SetText(self.Tip, tip)
        local list_data = self:GetSelectAllStuff()
        self.StuffList:SetData(list_data)
    end
end

function PetMultiPiecesPanel.GetSelectAllNum()
    local info_list = PetData.Instance.pet_multi_pieces_data or {}
    local num = 0
    for i,v in pairs(info_list) do
        num = num + v
    end
    return num
end

function PetMultiPiecesPanel:GetSelectAllStuff()
    local info_list = PetData.Instance.pet_multi_pieces_data or {}
    local item_list = {}
    for i,num in pairs(info_list) do
        if num > 0 then
            local cfg = Config.pet_cfg_auto.pet_compose[i + 1]
            for i,v in pairs(cfg.release_item) do
                if not item_list[v.item_id] then
                    item_list[v.item_id] = {item_id = v.item_id, num = 0}
                end
                item_list[v.item_id].num = item_list[v.item_id].num + num * v.num
            end
        end
    end
    return item_list
end

function PetMultiPiecesPanel:OnBtnClick()
    local info_list = {}
    for i,v in pairs(PetData.Instance.pet_multi_pieces_data) do
        if v > 0 then
            table.insert(info_list,{seq = i, num = v})
        end
    end
    if #info_list > 0 then
        local func = function ()
            -- #1583739 极少数玩家出现界面关闭了执行弹框func
            if not ViewMgr:IsOpen(PetMultiPieces) then
                return
            end
            PetCtrl.Instance:SendMultiPieces(info_list, self.view_type)
            self:FlushPanel()
            if self.view_type == 2 then
                local item_obj_list = {}
                local item_list = self:GetSelectAllStuff()
                for i,v in pairs(item_list) do
                    local item = Item.Create({item_id = v.item_id, num = v.num})
                    table.insert(item_obj_list, item)
                end
                ViewMgr:OpenView(GiftOpenView,{item_list = item_obj_list,gift_type = GIFT_OPEN_TYPE.SPECIAL})

            end
            PetData.Instance.pet_multi_pieces_data = {}
        end
        if self.view_type == 1 then
            func()
        else
            local sel_num = PetMultiPiecesPanel.GetSelectAllNum()
            PublicPopupCtrl.Instance:AlertTip(string.format(Language.PetMultiPieces.Text10, sel_num),function ()
                func()
            end)
        end
    end
end

PetMultiPiecesItem = PetMultiPiecesItem or DeclareMono("PetMultiPiecesItem", UIWidgetBaseItem)
function PetMultiPiecesItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.patch_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, item:Name())
    self:FlushNum()
end

function PetMultiPiecesItem:FlushNum()
    local clip_num = Item.GetNum(self.data.patch_id)
    local num = PetData.Instance.pet_multi_pieces_data[self.data.seq] or 0

    self.max_num = math.floor(clip_num / self.data.patch_num)

    local need_num = num * self.data.patch_num
    UH.SetText(self.Num, num)
    local stuff_str = DataHelper.ConsumeNum(clip_num, need_num, true)
    UH.SetText(self.StuffNum, stuff_str)
end

function PetMultiPiecesItem:OnAddClick(value)
    value = value or 0
    local num = PetData.Instance.pet_multi_pieces_data[self.data.seq] or 0
    num = num + value
    if num < 0 then
        num = 0
    end
    self.max_num = self.max_num or 0
    if num > self.max_num then
        PublicPopupCtrl.Instance:Center(Language.PetMultiPieces.Text8)
        return
    end

    local view_type = PetData.Instance:GetMultiPiecesType()
    if view_type == 1 then
        local num1,num2 = PetData.Instance:GetPetBagCapacity()
        local sel_all_num = PetMultiPiecesPanel.GetSelectAllNum()
        local can_num = num1 - num2 - sel_all_num - value
        if can_num < 0 then
            PublicPopupCtrl.Instance:Center(Language.PetMultiPieces.Text9)
            return
        end
    end
    PetData.Instance.pet_multi_pieces_data[self.data.seq] = num
    self:FlushNum()
end

PetMultiPiecesStuffItem = PetMultiPiecesStuffItem or DeclareMono("PetMultiPiecesStuffItem", UIWidgetBaseItem)
function PetMultiPiecesStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.Text, "x " .. data.num)
end




PetMultiPiecesResult = PetMultiPiecesResult or DeclareView("PetMultiPiecesResult", "pet/pet_multi_pieces_result")
VIEW_DECLARE_MASK(PetMultiPiecesResult,ViewMask.BgBlockClose)
function PetMultiPiecesResult:LoadCallback(param_t)
    --local pet_list = PetData.Instance:GetPetList()
    local index_list = param_t.index_list or {}
    local pet_list = {}
    for i,v in pairs(index_list) do
        local pet = PetData.Instance:GetPetInfo(v)
        table.insert(pet_list, pet)
    end
    self.List:SetData(pet_list)
end

PetMultiPiecesResultPetItem = PetMultiPiecesResultPetItem or DeclareMono("PetMultiPiecesResultPetItem", UIWidgetBaseItem)
-- data === pet
function PetMultiPiecesResultPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    UH.SetText(self.Level, string.format("Lv.%s",  data:Level()))
    UH.SetText(self.Name, data:Name())

    local v1 = data:TotalGrowth()
    local v2 = data:ConfigGrowth()
    v1 = ColorStr(v1, COLORSTR.Green11)
    UH.SetText(self.Value, string.format(Language.PetMultiPieces.Text7, v1, v2))
end

function PetMultiPiecesResultPetItem:OnTipClick()
    ViewMgr:OpenView(PetDetailView, self.data)
end