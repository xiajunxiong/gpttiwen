AccrssoryMainView = AccrssoryMainView or DeclareMono("AccrssoryMainView", UIWFlushPanel)
function AccrssoryMainView:AccrssoryMainView()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data_cares = {
        -- {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushMain, init_call = false},
        {data = self.Data.remind_data, func = self.FlushMain, init_call = false},
    }
    -- 当前筛选的套装
    self.now_select_suit = 0
    self.Data.now_show_panel.mod_id = Mod.Accrssory.Main
    -- ObjBlock
    self.GetWayList:SetData(Config.peishi_auto.get_way)
end

function AccrssoryMainView:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushMain()
end

function AccrssoryMainView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Data.now_show_panel.mod_id = Mod.Accrssory.Main
end

function AccrssoryMainView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function AccrssoryMainView:FlushMain()
    local now_select_name, suit_tab_data = self.Data:GetAccrssorySuitTabData(self.now_select_suit)
    UH.SetText(self.TxtSelect, now_select_name)
    self.SuitSelectList:SetData(suit_tab_data)
    self.BagList:SetData(self.Data:GetAccrssoryBagData(true, 25, 5, self.now_select_suit))
end

function AccrssoryMainView:SetSuitSelectActive(value)
    self.ObjSuitSelect:SetActive(value)
end

function AccrssoryMainView:OnSuitTabSelect(data)
    self:SetSuitSelectActive(false)
    if self.now_select_suit ~= data.suit_id then
        self.now_select_suit = data.suit_id
        self:FlushMain()
    end
end

function AccrssoryMainView:SetGetWayActive(value)
    self.ObjGetWay:SetActive(value)
end

function AccrssoryMainView:Goto(data)
    if data.mod_key then
        ViewMgr:OpenViewByKey(data.mod_key)
    end
    self:SetGetWayActive(false)
end

--------------AccrssoryInfoPanel--------------
AccrssoryInfoPanel = AccrssoryInfoPanel or DeclareMono("AccrssoryInfoPanel", UIWFlushPanel)
function AccrssoryInfoPanel:AccrssoryInfoPanel()
    self.Data = AccrssoryData.Instance
    self.data = self.Data.accrssory_equip_data
    self.data_cares = {
        -- {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushBagItem, init_call = false},
        {data = self.Data.remind_data, func = self.FlushBagItem, init_call = false},
        {data = self.data, func = self.FlushPanel, init_call = false},
        {data = self.Data.now_show_panel, func = self.FlushShowPanel, init_call = false},
        {data = self.Data.accrssory_extra_data,keys = {"flush"}, func = self.FlushPanel},
    }
    -- 当前选中的配饰类型
    self.now_select_accrssory_type = 0
    self:FlushPanel()
end

function AccrssoryInfoPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShowPanel()
end

function AccrssoryInfoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data.accrssory_inherit_data.now = {}
    self.Data.now_select_equip.info = {}
end

function AccrssoryInfoPanel:FlushPanel()
    UH.SetText(self.TxtScore, self.data.capability)
    for i = 1, self.LeftAccrssoryItem:Length() do
        self.LeftAccrssoryItem[i]:SetData(self.data.info[i] or {})
    end
    if TableIsEmpty(self.data.skill) then
        for i = 1, self.AccrssorySkillItem:Length() do
            self.AccrssorySkillItem[i]:SetData(self.Data.recommend_suit[i] or {})
        end
    else
        for i = 1, self.AccrssorySkillItem:Length() do
            self.AccrssorySkillItem[i]:SetData(self.data.skill[i] or {})
        end
    end

    local extral = self.Data:GetAllExtralEquip()
    for i = 1, 2 do 
        self.ExtralItems[i]:SetData(extral[i])
    end 
    self:FlushShowPanel()
end

function AccrssoryInfoPanel:FlushShowPanel()
    if self.Data.now_show_panel.mod_id == Mod.Accrssory.Main then
        self.ObjSelectShow:SetActive(false)
    else
        -- 公用界面：配饰继承，配饰强化
        self.ObjSelectShow:SetActive(true)
        -- 如果当前没有选中，就去尝试选中第一个
        if self.now_select_accrssory_type == 0 or TableIsEmpty(self.data.info[self.now_select_accrssory_type]) then
            self:TrySetSelect()
        else
            self.Data.accrssory_inherit_data.now = self.data.info[self.now_select_accrssory_type]
            self.Data.now_select_equip.info = self.data.info[self.now_select_accrssory_type]
        end
    end
    self:FlushBagItem()
end

-- 这里主要刷新红点
function AccrssoryInfoPanel:FlushBagItem()
    if self.Data.now_show_panel.mod_id == Mod.Accrssory.Inherit then
        local inherit_remind = self.Data:CalculateInheritRemind()
        for i = 1, ACCRSSORY_MAX do
            self.RedPoint[i]:SetNum(inherit_remind[i])
        end
    elseif self.Data.now_show_panel.mod_id == Mod.Accrssory.Strength then
        local strength_remind = self.Data:CalculateStrengthRemind()
        for i = 1, ACCRSSORY_MAX do
            self.RedPoint[i]:SetNum(strength_remind[i])
        end
    else
        for i = 1, ACCRSSORY_MAX do
            self.RedPoint[i]:SetNum(0)
        end
    end
end

-- 继承界面时，尝试选中第一个已装备的配饰
function AccrssoryInfoPanel:TrySetSelect()
    for i = 1, ACCRSSORY_MAX do
        if not TableIsEmpty(self.data.info[i]) then
            self.now_select_accrssory_type = i
            break
        end
        if i == ACCRSSORY_MAX then
            self.now_select_accrssory_type = 0
        end
    end
    self:FlushSelectShow()
end

function AccrssoryInfoPanel:FlushSelectShow()
    self.Data.accrssory_inherit_data.now = self.data.info[self.now_select_accrssory_type] or {}
    self.Data.now_select_equip.info = self.data.info[self.now_select_accrssory_type] or {}
    for i = 1, ACCRSSORY_MAX do
        self.ObjSelect[i]:SetActive(i == self.now_select_accrssory_type)
    end
end

function AccrssoryInfoPanel:BlockClick(num)
    if not TableIsEmpty(self.data.info[num]) then
        self.now_select_accrssory_type = num
    end
    self:FlushSelectShow()
end

--------------AccrssorySkillItem--------------
AccrssorySkillItem = AccrssorySkillItem or DeclareMono("AccrssorySkillItem", UIWidgetBaseItem)
function AccrssorySkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.data.skill_config = nil
    if not data.skill_id or data.skill_id == 0 then
        self.Root:SetActive(false)
    else
        local skill_config = Cfg.AccrssorySkillDesc(data.skill_id, data.skill_level)
        if not TableIsEmpty(skill_config) then
            self.Root:SetActive(true)
            self.data.skill_config = skill_config
            UH.SetIcon(self.ImgSetSkillIcon, skill_config.icon_id)
            UH.SetText(self.TxtSkillName, Format(Language.Accrssory.SkilName, skill_config.skill_level, skill_config.skill_name))
        else
            self.Root:SetActive(false)
        end
    end
    -- 未激活的技能展示
    if self.IHAndler then
        if self.data.is_recommand then
            self.IHAndler.Interactable = false
        else
            self.IHAndler.Interactable = true
        end
    end
end

function AccrssorySkillItem:OnSkillClick()
    if self.data.skill_config then
        if self.data.is_recommand then
            if self.data.suit_type then
                local suit_name = AccrssoryData.Instance:GetSuitGetNameConfig()[self.data.suit_type]
                PublicPopupCtrl.Instance:Center(Format(Language.Accrssory.WearCanActive, self.data.skill_num_suit, suit_name))
            else
                PublicPopupCtrl.Instance:Center(Format(Language.Accrssory.WearCanActive, self.data.skill_num_suit, ""))
            end
        end
        AccrssoryCtrl.Instance:OpenSkillShowView(self.data.skill_config)
    end
end

--------------LeftAccrssoryItem--------------
LeftAccrssoryItem = LeftAccrssoryItem or DeclareMono("LeftAccrssoryItem", UIWidgetBaseItem)
function LeftAccrssoryItem:LeftAccrssoryItem()
    UH.SpriteName(self.ImgSetIconType, AccrssoryConfig.TypeName[self.EquipType])
    UH.SetText(self.TxtTypeName, Language.Accrssory.EquipType[self.EquipType])
end

function LeftAccrssoryItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    if self.data.item_id and self.data.item_id > 0 then
        UH.SpriteName(self.ImgSetFrame, "YuanPinZhiKuang" .. data:Color())
        UH.SetIcon(self.ImgSetIcon, data:IconId())
        self.ImgSetIcon:SetObjActive(true)
        UH.SetText(self.TxtAccrssorySuit, data:SuitName())
        self.ObjLevelShow:SetActive(true)
        UH.SetText(self.TxtLevel, Format(Language.Accrssory.StrengthAdd, self.data:StrengthLevel()))
    else
        self.ImgSetIcon:SetObjActive(false)
        self.ObjLevelShow:SetActive(false)
    end
end

function LeftAccrssoryItem:OnAccrssoryClick()
    if self.data.item_id and self.data.item_id > 0 then
        CellClicks.BagGrid({GetData = function()
            return self.data
        end})
    end
end

--------------LeftAccrssoryItemOther--------------
LeftAccrssoryItemOther = LeftAccrssoryItemOther or DeclareMono("LeftAccrssoryItemOther", LeftAccrssoryItem)
function LeftAccrssoryItemOther:OnAccrssoryClick()
    if self.data.item_id and self.data.item_id > 0 then
        CellClicks.BagGridNoButton({GetData = function()
            return self.data
        end})
    end
end

--------------AccrssorySuitTab--------------
AccrssorySuitTab = AccrssorySuitTab or DeclareMono("AccrssorySuitTab", UIWidgetBaseItem)
function AccrssorySuitTab:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local color = data.suit_id == 0 and COLORSTR.Yellow8 or COLORSTR.Yellow6
    self.ObjLine:SetActive(data.suit_id ~= 0)
    UH.SetText(self.TxtSuitName, Format(Language.Common.Xstr, color, data.suit_name))
end

--------------AccrssoryBagItem--------------
AccrssoryBagItem = AccrssoryBagItem or DeclareMono("AccrssoryBagItem", UIWidgetBaseItem)
function AccrssoryBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    if data.item_id > 0 then
        local equip_type = data:AccrssoryType()
        self.ObjBetter:SetActive(AccrssoryData.Instance:CompareScore(data, equip_type))
    else
        self.ObjBetter:SetActive(false)
    end
end

--------------AccrssoryGetWayItem--------------
AccrssoryGetWayItem = AccrssoryGetWayItem or DeclareMono("AccrssoryGetWayItem", UIWidgetBaseItem)
function AccrssoryGetWayItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtGetWay, data.get_way)
end

function AccrssoryGetWayItem:GetGotoModKey()
    return {mod_key = self.data.mod_key}
end

----------- LeftAccrssoryExtraItem ------------------
LeftAccrssoryExtraItem = LeftAccrssoryExtraItem or DeclareMono("LeftAccrssoryExtraItem", UIWidgetBaseItem)
function LeftAccrssoryExtraItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    UH.SetIcon(self.Calls,data.icon)
    UH.SetText(self.LevelStr,data.level_cfg.level..Language.Common.Level)

    self.Level:SetActive(data.level > 0)
    self.Frame:SetActive(data.level > 0)
    self.Empty:SetActive(data.level == 0)


    self.red_pos = self.red_pos or UiRedPoint.New(self.RedPos, Vector3.New(0,0,0))
    self.red_pos:SetNum(AccrssoryData.Instance:GetExtralEquipRemindNum(self.Type))
end

function LeftAccrssoryExtraItem:OnClickExtral(index)
    AccrssoryData.Instance.accrssory_extra_data.type = index

    ViewMgr:OpenView(AccrssoryExtralView)
end