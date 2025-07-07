-- ========== 灵骑卷抽合成 ==========

BicycleSkillComposeView = BicycleSkillComposeView or DeclareView("BicycleSkillComposeView", "bicycle/new_skill/bicycle_skill_compose_view")
function BicycleSkillComposeView:BicycleSkillComposeView()
    self.Board:SetData(self:GetType(),Language.Bicycle.Title2,Vector2.New(912, 598))
end


BicycleSkillComposePanel = BicycleSkillComposePanel or DeclareMono("BicycleSkillComposePanel", UIWFlushPanel)
function BicycleSkillComposePanel:BicycleSkillComposePanel()
    self.data = BicycleOtherData.Instance
    self.data_cares = {
        {data = self.data.sel_stuff_data, func = self.FlushPanel, init_call = false},
    }
end

function BicycleSkillComposePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function BicycleSkillComposePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearStuff()
end

function BicycleSkillComposePanel:FlushPanel()
    local a_data = self.data:GetAStuff()
    local b_data = self.data:GetBStuff()
    self.StuffList[1]:SetData(a_data or {})
    self.StuffList[2]:SetData(b_data or {})

    if not TableIsEmpty(a_data) and not TableIsEmpty(b_data) then
        local a_cfg = a_data:Config()
        if a_cfg == nil then
            --某些未知条件下点了确定合成后会造成stuffA物品中的物品id变化导致触发smartData刷新后这里的a_cfg为空,先添加容错
            --问题1：背包物品变化，已提前ClearStuff
            LOG("卷轴合成异常", a_data)
            self.data:ClearStuff()
            PublicPopupCtrl.Instance:Center(Language.Bicycle.Tip4)
            return
        end
        local skill_id = a_cfg.param1
        local level = a_cfg.param3 + 1
        local skill_item_cfg = self.data:GetSkillItemCfg(skill_id, level)
        local item_id = skill_item_cfg.item_id
        self.t_item_id = item_id
        local t_item = Item.Create({item_id = item_id})
        self.TargetCell:SetData(t_item)
        UH.SetText(self.TargetName, t_item:QuaName())
    else
        self.TargetCell:SetData(Item.Create({item_id = 0}))
        UH.SetText(self.TargetName, Language.Bicycle.Text5)
    end
end

function BicycleSkillComposePanel:OnBtnClick()
    local a_data = self.data:GetAStuff()
    local b_data = self.data:GetBStuff()
    if TableIsEmpty(a_data) then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Text6)
        return
    end
    if TableIsEmpty(b_data) then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Text7)
        return
    end
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SKILL_ITEM_COMPOSE, a_data.item_id, b_data.item_id, self.t_item_id)
    BicycleOtherData.Instance:ClearStuff()
end

function BicycleSkillComposePanel:OnAllUpClick()
    ViewMgr:CloseView(BicycleSkillComposeView)
    ViewMgr:OpenView(BicycleSkillBookAllUp)
end

BicycleSkillComposeStuffItem = BicycleSkillComposeStuffItem or DeclareMono("BicycleSkillComposeStuffItem", UIWidgetBaseItem)
function BicycleSkillComposeStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_none = TableIsEmpty(data)
    self.ItemCell:SetObjActive(not is_none)
    self.AddObj:SetActive(is_none)
    if not is_none then
        self.ItemCell:SetData(data)
        UH.SetText(self.Name, data:Name())
    else
        local name = self.Type == 1 and Language.Bicycle.Text8 or Language.Bicycle.Text9
        UH.SetText(self.Name, name)
    end
end

function BicycleSkillComposeStuffItem:OnClick()
    if self.Type == 2 then
        local a_data = BicycleOtherData.Instance:GetAStuff()
        if TableIsEmpty(a_data) then
            PublicPopupCtrl.Instance:Error(Language.Bicycle.Text6)
            return
        end
    end
    ViewMgr:OpenView(BicycleSelSkillStuffView, {show_type = self.Type})
end