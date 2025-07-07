--=============== 灵骑圣物 ==================
BicycleHalidomView = BicycleHalidomView or DeclareView("BicycleHalidomView", "bicycle/bicycle_halidom_view")
function BicycleHalidomView:BicycleHalidomView()
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.Halidom,Vector2.New(786, 496))
end


BicycleHalidomPanel = BicycleHalidomPanel or DeclareMono("BicycleHalidomPanel", UIWFlushPanel)
function BicycleHalidomPanel:BicycleHalidomPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false},
    }
end

function BicycleHalidomPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function BicycleHalidomPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

function BicycleHalidomPanel:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    if not self.bicycle then
        return
    end
    local halidom = self.bicycle:Halidom()
    local cfg = halidom:Cfg()
    local next_cfg = halidom:NextCfg()
    local is_active = halidom:IsShow()
    local is_max_level = halidom:IsMaxLevel()
    local bicycle_flag = self.data:BicycleHalidomOneRemindNum(self.bicycle)
    self.Interactor.Interactable = bicycle_flag > 0
    self.RedPoint:SetNum(bicycle_flag > 0 and 1 or 0)

    self.LevelObj:SetActive(is_active)
    self.NotActive:SetActive(not is_active)
    self.NextAttriObj:SetActive(is_active and not is_max_level)
    self.UseStuffObj:SetActive(not is_max_level)

    local attri_info_1 = CommonStruct.AttrStruct(cfg.att_type, cfg.att_value)
    UH.SetText(self.AttriName, attri_info_1.attr_name)
    UH.SetText(self.AttriValue, string.format("+%s",attri_info_1.attr_percent_value))
    UH.SetText(self.Name, cfg.holy_name)

    if is_active then
        UH.SetText(self.LevelText, string.format(Language.Bicycle.Level2, halidom:Level()))
    end
    
    if not is_max_level then
        local item_id = cfg.holy_consume_item[0].item_id
        local has_num = Item.GetNum(item_id)
        local need_num = cfg.holy_consume_item[0].num
        local item = Item.Create({item_id = cfg.holy_consume_item[0].item_id, num = DataHelper.ConsumeNum(has_num,need_num,true)})
        self.StuffCell:SetData(item)
        UH.SetText(self.StuffName, item:Name())
        -- 没激活next_cfg就会为空
        if next_cfg then
            local attri_info_2 = CommonStruct.AttrStruct(next_cfg.att_type, next_cfg.att_value)
            UH.SetText(self.AttriName2, attri_info_2.attr_name)
            UH.SetText(self.AttriValue2, string.format("+%s",attri_info_2.attr_percent_value))
        end
    end
    local btn_str = is_active and Language.Bicycle.Halidom.Tip2 or Language.Bicycle.Halidom.Tip3
    UH.SetText(self.BtnText, btn_str)

    self:FlushModel(halidom)

    self.MaxShow:SetActive(is_max_level)
    self.UseBtn:SetActive(not is_max_level)
end

function BicycleHalidomPanel:FlushModel(halidom)
    local model_id = halidom:ModelId()
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(model_id))
    -- local color = halidom:ModelColor()
    -- self.ModelImg.color = color
end

function BicycleHalidomPanel:OnHelpClick()
    self.AllDescObj:SetActive(true)
    local all_desc = self.data:GetHalidomAllNatureDesc(self.bicycle:Halidom())
    UH.SetText(self.AllDescText,all_desc)
end

function BicycleHalidomPanel:OnCloseHelpClick()
    self.AllDescObj:SetActive(false)
end

function BicycleHalidomPanel:OnBtnClick()
    if self.effect_playing then
        return
    end
    local bicycle_flag = self.data:BicycleHalidomOneRemindNum(self.bicycle)
    local halidom = self.bicycle:Halidom()
    if bicycle_flag == -1 then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Halidom.Tip4)
        return
    end
    if bicycle_flag == -2 then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Halidom.Tip5)
        return
    end
    if bicycle_flag == -3 then
        PublicPopupCtrl.Instance:Center(string.format(Language.Bicycle.Halidom.Tip6, halidom:Cfg().role_level_limit))
        return
    end
    if bicycle_flag == 0 then
        local stuff_id = halidom:Cfg().holy_consume_item[0].item_id
        local item = Item.Create({item_id = stuff_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Halidom.Tip7)
        return
    end
    local is_active = halidom:IsShow()
    local op_type = is_active and BicycleCfg.OP_TYPE.UP_HALIDOM or BicycleCfg.OP_TYPE.ACTIVE_HALIDOM
    self.effect_playing = true
    self.EquipEffect:SetMovePos(Vector3.New(-2, 2, 0), Vector3.New(-1, -1, 0))
    self.EquipEffect:PlayActive()
    self.EquipEffect:SetCallback(BindTool.Bind(function ()
        BicycleCtrl.Instance:Req(op_type, self.bicycle:Index())
        self.effect_playing = nil
    end,self))
end

function BicycleHalidomPanel:OnConvertClick()
    local bicycle_flag = self.data:BicycleHalidomOneRemindNum(self.bicycle)
    if bicycle_flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Halidom.Tip4)
        return
    end
    local halidom = self.bicycle:Halidom()
    if not halidom:IsShow() then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Halidom.Tip8)
        return
    end
    ViewMgr:OpenView(BicycleHalidomConvertView, {item = halidom})
end


-- 圣物等级转换
BicycleHalidomConvertView = BicycleHalidomConvertView or DeclareView("BicycleHalidomConvertView", "bicycle/bicycle_halidom_convert_view")
function BicycleHalidomConvertView:BicycleHalidomConvertView()
    self.data = BicycleData.Instance
    self.stuff_cfg = Config.bicycle_cfg_auto.other[1].tran_cost_item 
end

function BicycleHalidomConvertView:LoadCallback(param_t)
    self.care_ht = BagData.Instance:Care(BindTool.Bind(self.FlushStuff,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.Title6,Vector2.New(934, 495))
    local stuff_item = Item.Create(self.stuff_cfg)
    self.Stuff:SetData(stuff_item)
    UH.SetText(self.ItemName, stuff_item:QuaName())

    self.l_item = param_t.item
    self.r_item = {}
    if not self.l_item then
        return
    end
    self.LInfo:SetData(self.l_item)
    self.RInfo:SetData(self.r_item)
    self:FlushPanel()
end

function BicycleHalidomConvertView:CloseCallback()
    BagData.Instance:UnCare(self.care_ht)
end

function BicycleHalidomConvertView:FlushPanel()
    self:FlushStuff()
end

function BicycleHalidomConvertView:FlushStuff()
    local has_num = Item.GetNum(self.stuff_cfg.item_id)
    local need_num = self.stuff_cfg.num
    UH.SetText(self.NumText, DataHelper.ConsumeNum(has_num,need_num))
end


function BicycleHalidomConvertView:OnAddClick()
    if not self.l_item then
        return
    end
    local ok_func = function (item)
        if not ViewMgr:IsOpen(BicycleHalidomConvertView) then
            return
        end
        self.r_item = item or {}
        self.RInfo:SetData(self.r_item)
    end
    ViewMgr:OpenView(BicycleHalidomConvertSelect, {ok_func = ok_func, sel_item = self.r_item})
end


function BicycleHalidomConvertView:OnBtnClick()
    if TableIsEmpty(self.l_item) then
        return
    end
    if TableIsEmpty(self.r_item) then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Halidom.Tip9)
        return
    end
    local has_num = Item.GetNum(self.stuff_cfg.item_id)
    local need_num = self.stuff_cfg.num
    if has_num < need_num then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Halidom.Tip7)
        MainOtherCtrl.Instance:GetWayViewById(self.stuff_cfg.item_id)
        return
    end
    local l_bicycle = self.l_item:Bicycle()
    local r_bicycle = self.r_item:Bicycle()

    PublicPopupCtrl.Instance:AlertTip(string.format(Language.Bicycle.Halidom.Tip10, self.l_item:Name(), self.r_item:Name()), function()
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.HOLY_CONVERT, l_bicycle.index, r_bicycle.index)
        ViewMgr:CloseView(BicycleHalidomConvertView)
    end)
end


BicycleHalidomConvertItemInfo = BicycleHalidomConvertItemInfo or DeclareMono("BicycleHalidomConvertItemInfo", UIWidgetBaseItem)
function BicycleHalidomConvertItemInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_none = TableIsEmpty(data)
    self.Show:SetActive(not is_none)
    self.None:SetActive(is_none)
    if not is_none then
        local item = Item.Create({item_id = data:ItemId()})
        self.ItemCell:SetData(item)
        self.AttriItem:SetData(data:Attri())
        UH.SetText(self.ItemName, data:Name())
        UH.SetText(self.LevelText, string.format(Language.Bicycle.Level2, data:Level()))
        local is_active = data:IsShow()
        self.LevelDi:SetActive(is_active)
        self.NotActive:SetActive(not is_active)
    end
end



BicycleHalidomConvertSelect = BicycleHalidomConvertSelect or DeclareView("BicycleHalidomConvertSelect", "bicycle/bicycle_halidom_select")
function BicycleHalidomConvertSelect:BicycleHalidomConvertSelect()
    self.data = BicycleData.Instance
end

function BicycleHalidomConvertSelect:LoadCallback(param_t)
    self.Board:SetData(self:GetType(),Language.Bicycle.Title7,Vector2.New(636, 560))
    self.ok_func = param_t.ok_func
    self.sel_item = param_t.sel_item
    local bicycle = self.data:GetSelectedBicycle()
    local item_list = {}
    for i,v in pairs(self.data.bicycle_list) do
        if not bicycle or bicycle.id ~= v.id then
            local halidom = v:Halidom()
            table.insert(item_list, halidom)
        end
    end

    self.None:SetActive(TableIsEmpty(item_list))
    self.List:SetData(item_list)
    if not TableIsEmpty(item_list) then
        local sel_index = 1
        for i,v in pairs(item_list) do
            if v.id == self.sel_item.id then
                sel_index = i
            end
        end
        self.List:SetectItem(sel_index, true)
    end
end

function BicycleHalidomConvertSelect:OnSelClick(data, value)
    if value then
        self.sel_item = data
    else
        self.sel_item = nil
    end
end

function BicycleHalidomConvertSelect:OnOkClick()
    if self.ok_func then
        self.ok_func(self.sel_item)
    end
    ViewMgr:CloseView(BicycleHalidomConvertSelect)
end


BicycleHalidomConvertSelectItem = BicycleHalidomConvertSelectItem or DeclareMono("BicycleHalidomConvertSelectItem", UIWidgetBaseItem)
function BicycleHalidomConvertSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data:ItemId()})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
    self.AttriItem:SetData(data:Attri())
    UH.SetText(self.LevelText, string.format(Language.Bicycle.Level2, data:Level()))
    --local bicycle_id = data:BicycleId()
    local is_active = data:IsShow()
    self.NotActive:SetActive(not is_active)
    self.LevelDi:SetActive(is_active)
end

function BicycleHalidomConvertSelectItem:OnLockClick()
    PublicPopupCtrl.Instance:Error(Language.Bicycle.Halidom.Tip1)
end

