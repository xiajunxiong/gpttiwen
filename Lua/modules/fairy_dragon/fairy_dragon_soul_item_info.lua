-- 龙魂物品信息
DragonSoulItemInfo = DragonSoulItemInfo or DeclareMono("DragonSoulItemInfo", ItemInfoViewItem)

function DragonSoulItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    self.item = self.info.item_list[self.ShowType]
    self:FlushInfo()
end

function DragonSoulItemInfo:FlushInfo()
    local score = self.item:Score()
    self.Score.text = score
    self.UpFlag:SetActive(false)
    local item_2 = self.info.item_list[2]
    if item_2 and item_2:Score() < score then
        self.UpFlag:SetActive(true)
    end

    local base_attri_list = self.item:BaseAttriList()
    self.BaseAttriList:SetData(base_attri_list)
    local rand_attri_list = self.item:RandAttriList()
    for i,v in pairs(rand_attri_list or {}) do
        if not v.qua then
            local group_id = self.item:RandAttriGroupId()
            local attri_index = v.index
            local attri_level = v.level
            local cfg = Cfg.FairyDragonRandAttriCfg(group_id, attri_index, attri_level)
            v.qua = cfg.attri_color
        end
    end
    self.RandAttriList:SetData(rand_attri_list)
    local has_rand_attri = not TableIsEmpty(rand_attri_list)
    self.RandAttriTip:SetObjActive(not has_rand_attri)
    if not has_rand_attri then
        local max_rand_count = self.item:MaxRandCount()
        UH.SetText(self.RandAttriTip, string.format(Language.FairyDragon.Text13,max_rand_count))
    end

    local next_sp_in_cfg = self.item:GetNextSpInCfg()
    local level = self.item:Level()
    local cur_sp_id = self.item:GetSpId(level)
    local cur_sp_cfg = Cfg.FairyDragonSkillCfg(cur_sp_id)
    UH.SetText(self.CurDesc, cur_sp_cfg.skill_dec)
    self.NextDescTitle:SetActive(next_sp_in_cfg ~= nil)
    self.NextDesc:SetObjActive(next_sp_in_cfg ~= nil)
    self.SkillTipTitle:SetActive(next_sp_in_cfg ~= nil)
    self.SkillTip:SetObjActive(next_sp_in_cfg ~= nil)
    if next_sp_in_cfg ~= nil then
        local next_sp_id = next_sp_in_cfg.sp_id
        local next_sp_cfg = Cfg.FairyDragonSkillCfg(next_sp_id)
        UH.SetText(self.NextDesc, next_sp_cfg.skill_dec)
        UH.SetText(self.SkillTip, string.format(Language.FairyDragon.Text14, next_sp_in_cfg.soul_level))
    end
end

function DragonSoulItemInfo:OnRandPreviewClick()
    local param_t = {show_type = self.ShowType,pos = UH.GetRootPosition(self.gameObject)}
    param_t.pos = self.ShowType == 2 and param_t.pos + Vector2.New(436,0) or param_t.pos

    local info_rect = self.ParentMono.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
    param_t.h = info_rect.sizeDelta.y
    param_t.item = self.item

	if ItemInfoCtrl.Instance:Data().derive_view then 
		ItemInfoCtrl.Instance:ItemDeriveView()
		if not ViewMgr:IsOpen(FairyDragonSoulRandAttriPreview) then 
			ItemInfoCtrl.Instance:ItemDeriveView(FairyDragonSoulRandAttriPreview,param_t)
	    end
	else 
		ItemInfoCtrl.Instance:ItemDeriveView(FairyDragonSoulRandAttriPreview,param_t)
	end 
end

function DragonSoulItemInfo:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end


-- 神龙皮肤
FairyDragonSkinItemInfo = FairyDragonSkinItemInfo or DeclareMono("FairyDragonSkinItemInfo", ItemInfoViewItem)
function FairyDragonSkinItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    self.item = self.info.item_list[self.ShowType]
    self:FlushInfo()
end

function FairyDragonSkinItemInfo:FlushInfo()
    local cfg = FairyDragonData.Instance:GetSkinCfgByItem(self.item.item_id)
    local skin = FairyDragonSkin.New(cfg)
    self.AttriList:SetData(skin:AttriList())
    local desc = skin:AllSkillDesc()
    if desc == "" then
        desc = Language.FairyDragon.Text24
    end
    UH.SetText(self.SkillDesc, desc)
    local h = self.SkillDesc.preferredHeight
    local line_count = math.ceil(h / 22)
    if line_count < 12 then
        for i = line_count, 12 do
            desc = desc .. "\n"
        end
        UH.SetText(self.SkillDesc, desc)
    end
end


FairyDragonSoulRandAttriItem = FairyDragonSoulRandAttriItem or DeclareMono("FairyDragonSoulRandAttriItem", UIWidgetBaseItem)
function FairyDragonSoulRandAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type,data.attri_value)
    local qua = data.qua or 0
    local color = QualityColorStr[qua]
    UH.SetText(self.Name, ColorStr(attri_info.attr_name, color))
    UH.SetText(self.Value, ColorStr(attri_info.attr_percent_value, color))
end




FairyDragonSoulRandAttriPreview = FairyDragonSoulRandAttriPreview or DeclareView("FairyDragonSoulRandAttriPreview","fairy_dragon/fairy_dragon_soul_attri_preview")

VIEW_DECLARE_MASK(FairyDragonSoulRandAttriPreview, ViewMask.None)

function FairyDragonSoulRandAttriPreview:FairyDragonSoulRandAttriPreview()
    self.data = ItemInfoCtrl.Instance:Data()
    self.info = self.data:ItemInfo()
end

function FairyDragonSoulRandAttriPreview:LoadCallback(data)
    local y = data.h / 2 + data.pos.y
    local x = data.pos.x + 381 + 5
    
    y = Valve(y, UnityEngine.Screen.height / 2 - 250)
    self.InfoPosition.localPosition = Vector2.New(x,y)
    
    local item = data.item
    local list_data = item:RandAttriPreviewList()
    self.List:SetData(list_data)
    local qua = item:Color()
    UH.SetText(self.Tip, string.format(Language.FairyDragon.Text26,Language.FairyDragon.ColorLevel[qua]))
end 

function FairyDragonSoulRandAttriPreview:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end



FairyDragonSoulRandAttriPreviewItem = FairyDragonSoulRandAttriPreviewItem or DeclareMono("FairyDragonSoulRandAttriPreviewItem", UIWidgetBaseItem)
function FairyDragonSoulRandAttriPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua_text = Language.FairyDragon.ColorLevel3[data.qua]
    local color = QualityColorStr[data.qua]
    UH.SetText(self.QuaText, ColorStr(qua_text, color))
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local add_value = data.next_attri_value - data.attri_value
    local add_attri_info = CommonStruct.AttrStruct(data.attri_type, add_value)
    UH.SetText(self.AttriName, ColorStr(attri_info.attr_name, color))
    UH.SetText(self.InitValue, string.format(Language.FairyDragon.Text27, attri_info.attr_percent_value))
    UH.SetText(self.AddValue, string.format(Language.FairyDragon.Text28, add_attri_info.attr_percent_value))
end