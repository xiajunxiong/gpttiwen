-- 神龙皮肤
FairyDragonSkinPanel = FairyDragonSkinPanel or DeclareMono("FairyDragonSkinPanel", UIWFlushPanel)
function FairyDragonSkinPanel:FairyDragonSkinPanel()
    self.data = FairyDragonData.Instance
    self.dragon = FairyDragon.Instance
    self.type_list = {
        {qua = ItemColor.Purple, name = Language.FairyDragon.SkinQua[4]},
        {qua = ItemColor.Orange, name = Language.FairyDragon.SkinQua[5]},
        {qua = ItemColor.Red, name = Language.FairyDragon.SkinQua[6]},
        {qua = ItemColor.Gold, name = Language.FairyDragon.SkinQua[9]},
        {qua = ItemColor.Black, name = Language.FairyDragon.SkinQua[10]},
    }
    self.data_cares = {
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

function FairyDragonSkinPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetData(self.type_list)
    self.List:SetectItem(1, true)
    self.sel_type_data = self.type_list[1]
    self:FlushPanel()
end

function FairyDragonSkinPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.skin_group_list = nil
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function FairyDragonSkinPanel:FlushPanel()
    local qua = self.sel_type_data.qua
    local skin_list = self.data:GetSkinList(qua)
    self.CellList:SetData(skin_list)
    if not self.sel_skin then
        self.CellList:SetectItem(1, true)
    end
    self:FlushSkinInfo()
end

function FairyDragonSkinPanel:FlushSkinInfo()
    if not self.sel_skin then
        return
    end
    UH.SetText(self.Name, self.sel_skin:Name())

    local star_list = {}
    local max_star = self.sel_skin:MaxStar()
    local cur_star = self.sel_skin:StarLevel()
    for i = 1, max_star do
        local data = {}
        data.active = cur_star >= i
        table.insert(star_list, data)
    end
    self.StarList:SetData(star_list)

    local attri_list = self.sel_skin:AttriList()
    self.AttriList:SetData(attri_list)

    local btn_text = self.sel_skin:IsActive() and Language.FairyDragon.Text5 or Language.FairyDragon.Text6
    local is_max = self.sel_skin:IsMaxStar()
    if is_max then
        btn_text = Language.FairyDragon.Text7
    end
    UH.SetText(self.BtnText, btn_text)
    self.StuffInfo:SetActive(not is_max)
    self.MaxFlag:SetActive(is_max)
    
    self:FlushStuff()
    self:FlushModelInfo()
end

function FairyDragonSkinPanel:FlushStuff()
    if not self.sel_skin then
        return
    end
    local star_cfg = self.sel_skin:UpStarCfg()
    local stuff_info = {item_id = star_cfg.up_expend, need_num = 1}
    self.StuffCell:SetData(stuff_info)
    self.stuff_id = star_cfg.up_expend
    self.BtnRemind:SetNum(self.data:RemindSkin(self.sel_skin))
end

function FairyDragonSkinPanel:FlushModelInfo()
    if not self.sel_skin then
        return
    end

    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
			}
		)
    end

    UH.SetText(self.ModelName, self.sel_skin:Name())
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.sel_skin:ResId()))
    self.Score:SetScore(self.sel_skin:Score())

    self.SkinOn:SetActive(self.sel_skin:IsInUse())
    self.SkinOff:SetActive(not self.sel_skin:IsInUse())

    local skill_list = self.sel_skin:SkillList()
    self.SkillList:SetData(skill_list)
    self.SkillNone:SetActive(#skill_list == 0)
end

function FairyDragonSkinPanel:OnTypeSelClick(data)
    self.sel_type_data = data
    self.CellList:SetData({})
    self.sel_skin = nil
    self:FlushPanel()
end

function FairyDragonSkinPanel:OnSkinSelClick(data)
    self.sel_skin = data
    self:FlushSkinInfo()
end

function FairyDragonSkinPanel:OnSkinClick()
    if not self.sel_skin then
        return
    end
    if not self.sel_skin:IsActive() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.Tip1)
        return
    end
    if not self.sel_skin:IsInUse() then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.USE_SKIN, self.sel_skin.seq)
    else
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.CANCEL_SKIN)
    end
end

function FairyDragonSkinPanel:OnBtnSkinClick()
    if not self.sel_skin then
        return
    end
    local flag = self.data:GetSkinCtrlFlag(self.sel_skin)
    if flag == 2 then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.SKIN_ACTIVE, self.sel_skin.seq)
    elseif flag == 1 then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.SKIN_STAR_LEVEL_UP, self.sel_skin.seq)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.stuff_id)
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Center(Language.FairyDragon.Tip2)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.ActiveTip)
    end
end


FairyDragonSkinTypeItem = FairyDragonSkinTypeItem or DeclareMono("FairyDragonSkinTypeItem", UIWidgetBaseItem)
function FairyDragonSkinTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
end


FairyDragonSkinItem = FairyDragonSkinItem or DeclareMono("FairyDragonSkinItem", UIWidgetBaseItem)
-- data == FairyDragonSkin
function FairyDragonSkinItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    local is_grey = not data:IsActive()
    local is_in_use = data:IsInUse()
    self.ItemCell:SetData(Item.Create({item_id = item_id, is_grey = is_grey}))
    self.StarInfo:SetActive(not is_grey)
    self.State:SetActive(is_grey or is_in_use)
    if not is_grey then
        UH.SetText(self.Level, data:StarLevel())
    end
    if is_grey or is_in_use then
        local state_str = is_grey and ColorStr(Language.FairyDragon.Text8, COLORSTR.Yellow8) or Language.FairyDragon.Text9
        UH.SetText(self.StateStr, state_str)
    end
end