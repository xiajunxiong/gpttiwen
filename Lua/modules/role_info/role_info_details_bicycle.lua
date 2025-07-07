RoleInfoDetailsBicycle = RoleInfoDetailsBicycle or DeclareMono("RoleInfoDetailsBicycle",UIWFlushPanel)

function RoleInfoDetailsBicycle:RoleInfoDetailsBicycle()
    self.data = RoleInfoData.Instance
    self.make_num = 1
    self.cur_model_type = 0
end

function RoleInfoDetailsBicycle:Awake()
	UIWFlushPanel.Awake(self)
    local bicycle_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().bicycle_list
    self.max_num = #bicycle_info_list
    self:OnClickItem(0)
end

function RoleInfoDetailsBicycle:FlushPanel()
    local bicycle_info_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().bicycle_list
    local bicycle = Bicycle.New(bicycle_info_list[self.make_num])
    local ornament = bicycle:Ornament()
    local ornament_is_show = ornament:IsShow()
    self.cur_model_type = ornament_is_show and 1 or 0
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = BicycleCommonPanel,show_shadow = false})
	end
    local model_id = ornament_is_show and bicycle:Model2() or bicycle:Model1()
    self.ui_obj:SetData(DrawerHelper.GetRidePath(model_id))
    self.ui_obj:PlayAnim(ChAnimType.Show)

    UH.SetText(self.ModelName,bicycle:Name(self.cur_model_type) .. [[  Lv.]] .. bicycle.level)
    UH.SetText(self.CapabilityText,bicycle:Capability())
    --灵骑装备
    local equip_list = bicycle:Equip()
    self.Equip0:SetData({equip = equip_list[0]})
    self.Equip1:SetData({equip = equip_list[1]})
    --灵骑圣物
    local halidom = bicycle:Halidom()
    local level_str = halidom:IsShow() and string.format(Language.Bicycle.Level2, halidom:Level()) or Language.Bicycle.Halidom.Tip1
    UH.SetText(self.HalidomLevel, level_str)
    if self.ui_halidom_obj == nil then
		self.ui_halidom_obj = UIObjDrawer.New()
        self.HalidomMode:SetShowData({ui_obj = self.ui_halidom_obj,view = self,show_shadow = false})
    end
    self.ui_halidom_obj:SetData(DrawerHelper.GetNpcPath(halidom:ModelId()))
    --灵骑饰品
    local ornament_item = Item.Create({item_id = ornament.cfg.jewelry_icon})
    self.EquipCell:SetData(ornament_item)
    UH.SetText(self.OrnamentName,ornament_item:Name())
    if ornament_is_show == true then
        UH.SetText(self.OrnamentLevel,string.format(Language.Bicycle.Level2,ornament:Level()))
    else
        UH.SetText(self.OrnamentLevel,Language.ItemInfo.NotActive)
    end
    --灵骑属性
    local attri_data_list = BicycleData.Instance:ConvertAttriToData2(bicycle.info.attr_list)
    local nature = ornament_is_show and ornament:Nature() or bicycle:Cfg().bicycle_grow_min
    nature = (nature * 100) .. "%"
    table.insert(attri_data_list,1,{name = Language.Bicycle.NatureAttriName .. [[：]],value = ColorStr(nature,COLORSTR.Yellow22)})
    self.AttriList:SetData(attri_data_list)
    --灵骑技能
    local base_skill_list = bicycle:BaseSkillList()
    for i = 1,self.BaseSkillList:Length() do
        self.BaseSkillList[i]:SetData(base_skill_list[i - 1])
        self.BaseSkillList[i]:ShowType(1)
    end
    UH.SetText(self.EquipAttr,Language.Bicycle.IntansifyTip .. (ornament_is_show and ((ornament:Nature() * 100) .. "%") or Language.ItemInfo.None))
    self.selected_bicycle = bicycle
end

function RoleInfoDetailsBicycle:OnHalidomClick()
    ViewMgr:OpenView(RoleInfoDetailsBicycleTipView,{
        halidom = self.selected_bicycle:Halidom()
    })
end

function RoleInfoDetailsBicycle:OnClickItem(num)
    self.make_num = self.make_num + num
    self:FlushButtonView()
    self:FlushPanel()
end

function RoleInfoDetailsBicycle:FlushButtonView()
    self.BtnLeft:SetActive(self.make_num > 1)
    self.BtnRight:SetActive(self.make_num < self.max_num)
end

function RoleInfoDetailsBicycle:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ui_obj]])
    Delete(self,[[ui_halidom_obj]])
end

RoleInfoDetailsBicycleTipView = RoleInfoDetailsBicycleTipView or DeclareView("RoleInfoDetailsBicycleTipView","role_info/role_info_details_bicycle_tip")
VIEW_DECLARE_MASK(RoleInfoDetailsBicycleTipView,ViewMask.BlockClose)

function RoleInfoDetailsBicycleTipView:LoadCallback(param_t)
    local halidom = param_t.halidom
    local cfg = param_t.halidom:Cfg()
    local is_active = halidom:IsShow()
    UH.SetText(self.Title, cfg.holy_name)
    self.Cell:SetData(Item.Create({item_id = cfg.holy_consume_item[0].item_id}))
	UH.SetText(self.Level,is_active and Format(Language.Bicycle.Level2, halidom:Level()) or Language.Bicycle.Halidom.Tip1)
    local attri_info = CommonStruct.AttrStruct(cfg.att_type, cfg.att_value)
    UH.SetText(self.Desc,is_active and attri_info.attr_name .. string.format("+%s",attri_info.attr_percent_value) or Language.ItemInfo.None)
end