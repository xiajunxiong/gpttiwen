LingBaoLvUpView = LingBaoLvUpView or DeclareView("LingBaoLvUpView", "ling_bao/ling_bao_lv_up")
function LingBaoLvUpView:LingBaoLvUpView()
    
end

function LingBaoLvUpView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.LingBao.StarUpTitle, Vector2.New(1008, 680))
end

----------------------------LingBaoLvUpPanel----------------------------
LingBaoLvUpPanel = LingBaoLvUpPanel or DeclareMono("LingBaoLvUpPanel", UIWFlushPanel)
function LingBaoLvUpPanel:LingBaoLvUpPanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data = self.Data.compose_data
    self.item_list = self.Data:ItemList()
    self.equip_list = self.Data:EquipList()
    self.Data:ResetComposeData()
    self.data.shenghen_type = self.Data:MainShengHenType()
    self.data_cares = {
        {data = self.item_list, func = self.FlushAll, init_call = false},
        {data = self.equip_list, func = self.FlushAll, init_call = false},
        {data = self.data, func = self.FlushAll, keys = {"shenghen_type"}, init_call = false},
        {data = self.data, func = self.FlushUp, keys = {"lingbao_type"}, init_call = false},
        {data = self.data, func = self.FlushRight, keys = {"state", "star"}, init_call = false},
    }
end

function LingBaoLvUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.lingbao_complete = false
    self.a_state_star_complete = false
    self.LingBaoList.StepCount = LingBaoConfig.LingBaoMax
    self.LingBaoList:SetCompleteCallBack(function()
        self.lingbao_complete = true
        self:CreateComplete()
    end)
    self.LingBaoList:SetData(self.Data:LingBaoTypeData())
    
    self.AStateStarList:SetCreateCallBack(function()
        self.a_state_star_complete = true
        self:CreateComplete()
    end)
    self.a_lv_up_data = self.Data:ALingBaoLvUpData()
    self.AStateStarList:SetData(self.a_lv_up_data, true)

    UH.SetText(self.TxtShengHenName, self.language.ShengHenName[self.data.shenghen_type])
end

function LingBaoLvUpPanel:JumpA(jump_data)
    if not TableIsEmpty(jump_data) then
        self.AStateStarList:ClickChild(jump_data)
        self.AStateStarList:AutoJumpVertical(jump_data.children_index, 64, jump_data.parent_index, 72)
    end
end

function LingBaoLvUpPanel:IsCreateComplete()
    return self.lingbao_complete and self.a_state_star_complete
end

function LingBaoLvUpPanel:CreateComplete()
    if self:IsCreateComplete() then
        self:FlushAll()
    end
end

function LingBaoLvUpPanel:FlushAll()
    if not self:IsCreateComplete() then
        return
    end
    self.ObjBlock:SetActive(false)
    UH.SetText(self.TxtShengHenName, self.language.ShengHenName[self.data.shenghen_type])
    for k, v in pairs(self.LingBaoList.item_list) do
        v:FlushShow()
    end
    local need_select = self.Data:LingBaoLvUpJumpLingBaoType()
    self.data.lingbao_type = need_select
    self.LingBaoList:SetectItem(self.data.lingbao_type, true)
    self:FlushUp()
end

function LingBaoLvUpPanel:FlushUp()
    local jump_data = self.Data:ALingBaoLvUpJumpData(self.a_lv_up_data)
    self.data.state = jump_data.parent_index
    self.data.star = jump_data.star
    self.data.level = jump_data.level
    self.AScrollRect:StopMovement()
    self:JumpA(jump_data)
    self:FlushLeft()
end

function LingBaoLvUpPanel:FlushLeft()
    self:FlushARemind()
    self:FlushRight()
end

function LingBaoLvUpPanel:FlushRight()
    self.now_cfg = self.Data:LingBaoLvUpCfg(self.data.lingbao_type, self.data.level)
    local last_state = LingBao.GetStateLevel(self.now_cfg.compose_item)
    local last_star = LingBao.GetStarLevel(self.now_cfg.compose_item)
    local tip = Format(self.language.StarTip, DataHelper.GetDaXie(LingBaoConfig.ComposeNum), DataHelper.GetDaXie(last_state), last_star, DataHelper.GetDaXie(self.data.state), self.data.star)
    UH.SetText(self.TxtTip, tip)
    self.Cell:SetData(Item.Init(self.now_cfg.item_id, 1, true))
    self.cell_list_data = self:GetCellListData()
    self.CellList:SetData(self.cell_list_data)
    self.UpRemind:SetNum(self.Data:LingBaoLvRemind2(self.data.shenghen_type, self.data.lingbao_type, self.data.level))
    UH.SetText(self.TxtState, Format(self.language.GetShowState, DataHelper.GetDaXie(self.data.state), self.data.star))
end

function LingBaoLvUpPanel:FlushARemind()
    if not self:IsCreateComplete() then
        return
    end
    for k, v in pairs(self.a_lv_up_data) do
        local parent = self.AStateStarList:GetParentMono(v)
        parent:FlushRemind()
        for l, w in pairs(v.children) do
            local child = self.AStateStarList:GetChildMono(w)
            child:FlushRemind()
        end
    end
end

function LingBaoLvUpPanel:GetCellListData()
    local list_data = {}
    local now_equip = self.Data:EquipList(self.data.shenghen_type, self.data.lingbao_type)
    if not TableIsEmpty(now_equip.item) then
        if now_equip.item.item_id == self.now_cfg.compose_item then
            table.insert(list_data, Item.Create(now_equip.item))
        end
    end
    local num, list = self.Data:ItemNum(self.now_cfg.compose_item)
    local index = 1
    while #list_data < LingBaoConfig.ComposeNum do
        if TableIsEmpty(list) or list[index] == nil then
            table.insert(list_data, Item.Create())
        else
            table.insert(list_data, self.Data:ItemList(list[index]))
            index = index + 1
        end
    end
    return list_data
end

function LingBaoLvUpPanel:OnShengHenClick()
    self.ObjArrow:SetActive(false)
    self.ObjShengHenList:SetActive(true)
    self.ShengHenList:SetData(self.Data:ShengHenTypeData())
end

function LingBaoLvUpPanel:OnShengHenSelect(data)
    self.data.shenghen_type = data.shenghen_type
    self:OnBlockClick()
end

function LingBaoLvUpPanel:OnStarUpClick()
    local remind_num = self.Data:LingBaoLvRemind2(self.data.shenghen_type, self.data.lingbao_type, self.data.level)
    if remind_num > 0 and (not TableIsEmpty(self.cell_list_data)) then
        self.EffectTool:Play("3165224")
        self.ObjBlock:SetActive(true)
    else
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.now_cfg.compose_item})})
    end
end

function LingBaoLvUpPanel:SendUp()
    local index_list = {}
    local in_bag = true
    for i, v in ipairs(self.cell_list_data) do
        if i == 1 and v.in_bag == false then
            in_bag = false
            table.insert(index_list, v:LingBaoType())
        else
            table.insert(index_list, v.index)
        end
    end
    LingBaoCtrl.Instance:SendLingBaoLvUp(in_bag and -1 or self.data.shenghen_type, index_list)
end

function LingBaoLvUpPanel:OnAChildItemClick(data)
    self.data.state = data.parent_index
    self.data.star = data.star
    self.data.level = data.level
end

function LingBaoLvUpPanel:OnBlockClick()
    self.ObjArrow:SetActive(true)
    self.ObjShengHenList:SetActive(false)
end

function LingBaoLvUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------LingBaoStrUpShengHenItem--------------
LingBaoStrUpShengHenItem = LingBaoStrUpShengHenItem or DeclareMono("LingBaoStrUpShengHenItem", UIWidgetBaseItem)
function LingBaoStrUpShengHenItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, Language.LingBao.ShengHenName[data.shenghen_type])
end

--------------LingBaoStrUpLingBaoItem--------------
LingBaoStrUpLingBaoItem = LingBaoStrUpLingBaoItem or DeclareMono("LingBaoStrUpLingBaoItem", UIWidgetBaseItem)
function LingBaoStrUpLingBaoItem:LingBaoStrUpLingBaoItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoStrUpLingBaoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, self.language.LingBaoName[data.lingbao_type])
end

function LingBaoStrUpLingBaoItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.Data.compose_data.lingbao_type = self.data.lingbao_type
    end
end

function LingBaoStrUpLingBaoItem:FlushShow()
    self.Remind:SetNum(self.Data:LingBaoLvRemind1(self.Data.compose_data.shenghen_type, self.data.lingbao_type))
end

--------------LingBaoLvUpAParentItem--------------
LingBaoLvUpAParentItem = LingBaoLvUpAParentItem or DeclareMono("LingBaoLvUpAParentItem", UIWidgetBaseItem)
function LingBaoLvUpAParentItem:LingBaoLvUpAParentItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoLvUpAParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)
end

function LingBaoLvUpAParentItem:FlushRemind()
    local compose_data = self.Data.compose_data
    local remind_num = 0
    for k, v in pairs(self.data.children) do
        if self.Data:LingBaoLvRemind2(compose_data.shenghen_type, compose_data.lingbao_type, v.level) > 0 then
            remind_num = 1
            break
        end
    end
    self.Remind:SetNum(remind_num)
end

--------------LingBaoLvUpAChildItem--------------
LingBaoLvUpAChildItem = LingBaoLvUpAChildItem or DeclareMono("LingBaoLvUpAChildItem", UIWidgetBaseItem)
function LingBaoLvUpAChildItem:LingBaoLvUpAChildItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoLvUpAChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
end

function LingBaoLvUpAChildItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
    self.Data.compose_data.state = self.data.parent_index
    self.Data.compose_data.star = self.data.star
    self.Data.compose_data.level = self.data.level
end

function LingBaoLvUpAChildItem:FlushRemind()
    local compose_data = self.Data.compose_data
    self.Remind:SetNum(self.Data:LingBaoLvRemind2(compose_data.shenghen_type, compose_data.lingbao_type, self.data.level))
end