-- 神龙升星
FairyDragonStarPanel = FairyDragonStarPanel or DeclareMono("FairyDragonStarPanel", UIWFlushPanel)
function FairyDragonStarPanel:FairyDragonStarPanel()
    self.dragon = FairyDragon.Instance
    self.data = FairyDragonData.Instance
    self.data_cares = {
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

function FairyDragonStarPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function FairyDragonStarPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function FairyDragonStarPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelStarClip(nil)
end

function FairyDragonStarPanel:FlushPanel()
    local stage_str = ColorStr(string.format(Language.FairyDragon.Text2, self.dragon:Stage()), COLORSTR.Red8)
    UH.SetText(self.Name, string.format("%s·%s",stage_str, self.dragon:Name()))

    local skin = self.dragon:Skin()
    if skin then
        self.ShowCell:SetData(Item.Create({item_id = skin.item_id}))
        UH.SetText(self.StarNum, skin:StarLevel())
    else
        UH.SetText(self.ShowStar, self.dragon:Star())
        self.ShowCell:SetData(Item.Create({item_id = self.dragon.item_id}))
    end

    local star_list = self.dragon:GetStarUIList()
    self.StarStarList:SetData(star_list)
    local skill_list = self.dragon:StarSkillList()
    self.dragon:SetShowSkillList(skill_list)
    self:FlushStarClip()
end

function FairyDragonStarPanel:FlushStarClip()
    local star_level = self.dragon:Star()
    local max_star = self.data:MaxStar()
    star_level = star_level >= max_star and max_star - 1 or star_level
    local cfg = self.data:GetStarGroupCfg(star_level)

    if not self.is_sel_click then
        local pre_data
        for i,v in pairs(cfg) do
            local state = self.data:GetStarClicpState(v)
            if not pre_data and state == 0 then
                pre_data = v
            end
        end
        if not pre_data then
            pre_data = cfg[1]
        end
        self.data:SetSelStarClip(pre_data)
    end
    self.is_sel_click = nil

    for i = 1, self.StarClipList:Length() do
        local mono = self.StarClipList[i]
        if cfg[i] then
            mono:SetData(cfg[i])
        end
    end

    local sel_data = self.data:GetSelStarClip()
    local clip_attri_list = self.data:GetStarClipAttri(sel_data)
    for i = 1, self.ClipAttriList:Length() do
        if clip_attri_list[i] then
            local attri_info = CommonStruct.AttrStruct(clip_attri_list[i].attri_type, clip_attri_list[i].attri_value)
            local value_str = ColorStr(attri_info.attr_percent_value, COLORSTR.Yellow6)
            UH.SetText(self.ClipAttriList[i], string.format("%s：%s", attri_info.attr_name, value_str))
        end
    end

    local is_max = self.data:IsMaxStar()
    if is_max then
        UH.SetText(self.BtnText, Language.FairyDragon.MaxLevelText)
    else
        local is_need_star = self.data:IsNeedUpStar()
        UH.SetText(self.BtnText, is_need_star and Language.FairyDragon.Text17 or Language.FairyDragon.Text18)
    end

    self.StuffInfo:SetActive(not is_max)
    self.MaxFlag:SetActive(is_max)

    self:FlushStuff()
end

function FairyDragonStarPanel:FlushStuff()
    local is_need_star = self.data:IsNeedUpStar()
    local item_id
    local need_num
    if is_need_star then
        local star_cfg = self.data:GetStarCfg(self.dragon:Star() + 1)
        item_id = star_cfg.break_expend_item
        need_num = star_cfg.break_expend_num
    else
        local sel_data = self.data:GetSelStarClip()
        item_id = sel_data.expend_item
        need_num = sel_data.expend_num
    end
    self.stuff_id = item_id
    has_num = Item.GetNum(item_id)
    UH.SetText(self.StuffText, DataHelper.ConsumeNum(has_num, need_num, false))
    UH.SetIcon(self.StuffIcon, Item.GetIconId(item_id, ICON_TYPE.ITEM))
    self.BtnRemind:SetNum(self.data:RemindStarTab())
end

function FairyDragonStarPanel:IsEmptySelClip(cfg)
    local sel_data = self.data:GetSelStarClip()
    if TableIsEmpty(sel_data) then
        return true
    end
    for i,v in pairs(cfg) do
        if v == sel_data then
            return false
        end
    end
    return true
end

function FairyDragonStarPanel:OnSelClick(data)
    self.is_sel_click = true
    self.data:SetSelStarClip(data)
    self:FlushStarClip()
end

function FairyDragonStarPanel:OnBtnClick()
    if self.data:IsMaxStar() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.Tip2)
        return
    end
    local is_need_star = self.data:IsNeedUpStar()
    if is_need_star then
        local star_cfg = self.data:GetStarCfg(self.dragon:Star() + 1)
        local flag = self.data:GetCtrlStarBreakFlag(star_cfg)
        if flag == 1 then
            FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.STAR_BREAK)
        elseif flag == 0 then
            PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
            MainOtherCtrl:GetWayViewById(star_cfg.break_expend_item)
        elseif flag == -1 then
            PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        end
    else
        local sel_data = self.data:GetSelStarClip()
        if TableIsEmpty(sel_data) then
            return
        end
        local flag = self.data:GetCtrlStarNodeFlag(sel_data)
        if flag == 1 then
            FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.ACTIVE_NODE, sel_data.point)
        elseif flag == 0 then
            PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
            MainOtherCtrl:GetWayViewById(sel_data.expend_item)
        elseif flag == -2 then
            PublicPopupCtrl.Instance:Error(Language.FairyDragon.ActiveTip)
        else
            PublicPopupCtrl.Instance:Error(Language.FairyDragon.Tip5)
        end
    end
end

function FairyDragonStarPanel:OnAttriPreviewClick()
    ViewMgr:OpenView(FairyDragonAttriPreview)
end

function FairyDragonStarPanel:OnFusionClick()
    WorkshopData.Instance:QuickComposeItem(56508)
    ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
end

function FairyDragonStarPanel:OnStuffClick()
    local data = Item.Create({item_id = self.stuff_id})
    local info = {
		item_list = {data},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end


FairyDragonStarStar = FairyDragonStarStar or DeclareMono("FairyDragonStarStar", UIWidgetBaseItem)
function FairyDragonStarStar:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetInter(self.Interactor, data.active)
end


FairyDragonStarClip = FairyDragonStarClip or DeclareMono("FairyDragonStarClip", UIWidgetBaseItem)
-- data == Config.god_dragon_auto.dragon_star
function FairyDragonStarClip:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local state = FairyDragonData.Instance:GetStarClicpState(data)
    local state_img = state == 0 and "An" or "Liang"
    UH.SpriteName(self.State, state_img)

    local sel_data = FairyDragonData.Instance:GetSelStarClip()
    self.SelImg:SetActive(sel_data == data)

    local icon_name = state == 0 and data.icon .. 1 or data.icon
    UH.SpriteName(self.Icon, icon_name)
end


-- 属性加成总览预览
FairyDragonAttriPreview = FairyDragonAttriPreview or DeclareView("FairyDragonAttriPreview", "fairy_dragon/fairy_dragon_attri_preview")
function FairyDragonAttriPreview:FairyDragonAttriPreview()
    self.Board:SetData(self:GetType(),Language.FairyDragon.Title3,Vector2.New(401 + 30, 336))
end

function FairyDragonAttriPreview:LoadCallback()
    local attri_list = FairyDragonData.Instance:StarAllAttriList()
    local is_none = TableIsEmpty(attri_list)
    if not is_none then
        self.AttriList:SetData(attri_list)
    end
    self.Show:SetActive(not is_none)
    self.None:SetActive(is_none)
end