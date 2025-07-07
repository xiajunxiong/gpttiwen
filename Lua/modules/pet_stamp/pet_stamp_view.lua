local pet_stamp_attri_addition_index = 0

-- 宠物神印标签页
PetStampPanel = PetStampPanel or DeclareMono("PetStampPanel", UIWFlushPanel)

function PetStampPanel:PetStampPanel()
    GuideManager.Instance:RegisterGetGuideUi("PetStampPrayOpenBtn", function ()
        return self.PrayBtn,function ()
            self:OnPrayClick()
        end
    end)
    self.data = PetStampData.Instance
end

function PetStampPanel:Awake()
    UIWFlushPanel.Awake(self)
    local is_open_pray, open_pray_tip = FunOpen.Instance:IsFunOpen(Mod.PetStampPray.Main)
    self.Interactor.Interactable = is_open_pray
end

function PetStampPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    PetData.Instance:SetSelTab(Mod.Pet.PetStamp)
end

function PetStampPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("PetStampPrayOpenBtn")
end

-- 套装推荐
function PetStampPanel:OnSuitInfoClick()
    local pet = PetData.Instance:GetCurSelect()
    local pet_cfg = pet:PetCfg()
    local suit_info = pet_cfg.recommend_god_print_suit
    local suit_list = Split(suit_info, "|")
    ViewMgr:OpenView(PetStampSuitInfoView, suit_list)
end

-- 一键卸载
function PetStampPanel:OnAllUnfixClick()
    local pet = PetData.Instance:GetCurSelect()
    local blueprint = pet:StampBlueprint()
    blueprint:Delete()
end

-- 神印方案
function PetStampPanel:OnBlueprintClick()
    ViewMgr:OpenView(PetStampBlueprintView)
end

-- 神印分解
function PetStampPanel:OnOpenResloveClick()
    --ViewMgr:OpenView(PetStampResolveSetView)
    ViewMgr:OpenView(PetStampResolveNew)
end

-- 打开祈神
function PetStampPanel:OnPrayClick()
    --ViewMgr:OpenView(PetStampPrayView)
    ViewMgr:OpenViewByKey(Mod.PetStampPray.Main)
end

-- 神印点击事件
function PetStampPanel:OnPetStampClick(pos_index)
    -- local pet = PetData.Instance:GetCurSelect()
    -- local blueprint = pet:StampBlueprint()
    -- local stamp = blueprint:GetStampByPos(pos_index)
    -- if stamp then
    --     self.PetStampBag.PosToggles[pos_index].isOn = true
    -- end

    self.PetStampBag.PosToggles[pos_index].isOn = true
end

function PetStampPanel:OnAddExpClick()
    ViewMgr:OpenView(PetStampResolveSetView)
end

function PetStampPanel:OnHelpClick()
    local default_qua = self.data:DefaultSuitQua()
    if default_qua == ItemColor.Red then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[277].desc)
    else
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[149].desc)
    end
end

-- 宠物神印信息
PetStampInfoPanel = PetStampInfoPanel or DeclareMono("PetStampInfoPanel", UIWFlushPanel)

function PetStampInfoPanel:PetStampInfoPanel()
    self.data = PetStampData.Instance
    self.data_cares = {
        {data = PetData.Instance.pet_select, func = self.FlushPanel, init_call = false},
        {data = self.data.pet_stamp_list, func = self.FlushPanel, init_call = false}
    }
    -- 1显示属性加成，2显示套装
    self.addition_type = 1
end

function PetStampInfoPanel:Awake()
    UIWFlushPanel.Awake(self)
    local pet = PetData.Instance:GetCurSelect()
    if pet == nil then
        return
    end
    self:FlushPanel()
    self:FlushAddition()
end

function PetStampInfoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetCurBuleprint(nil)
end

function PetStampInfoPanel:FlushPanel()
    self:FlushBlueprint()
    self:FlushAddition()
end

-- 刷新加成信息
function PetStampInfoPanel:FlushAddition()
    -- self.AttriInfoObj:SetActive(self.addition_type == 1)
    -- self.SuitInfoObj:SetActive(self.addition_type == 2)
    -- local addition_title = self.addition_type == 1 and Language.PetStamp.Addition1 or Language.PetStamp.Addition2
    -- UH.SetText(self.AdditionText, addition_title)
    -- if self.addition_type == 1 then
    --     self:FlushAttriAddition()
    -- elseif self.addition_type == 2 then
    --     self:FlushSuitAddition()
    -- end
    self.AttriInfoObj:SetActive(false)
    self.SuitInfoObj:SetActive(true)
    local addition_title = Language.PetStamp.Addition1
    UH.SetText(self.AdditionText, addition_title)
    self:FlushSuitAddition()
end

-- 刷新属性加成
function PetStampInfoPanel:FlushAttriAddition()
    local pet = PetData.Instance:GetCurSelect()
    local blueprint = pet:StampBlueprint()
    local attri_list = blueprint:AttriList() or {}
    self.AttriList:SetData(attri_list)
    if TableIsEmpty(attri_list) then
        self.EmptyStateText:SetObjActive(true)
        UH.SetText(self.EmptyStateText, Language.PetStamp.Tip1)
    else
        self.EmptyStateText:SetObjActive(false)
    end
end

-- 刷新套装加成
function PetStampInfoPanel:FlushSuitAddition()
    local pet = PetData.Instance:GetCurSelect()
    local blueprint = pet:StampBlueprint()
    local suit_list = self.data:GetBlueprintSuit(blueprint)
    suit_list = suit_list or {}
    --LogError("套装加成列表", suit_list)
    self.PetStampSuitInfoShow:SetData(suit_list)
end

function PetStampInfoPanel:OnAdditionChangeClick(value)
    self.addition_type = value
    self:FlushAddition()
end


function PetStampInfoPanel:FlushBlueprint()
    local pet = PetData.Instance:GetCurSelect()
    local buleprint = pet:StampBlueprint()
    self.data:SetCurBuleprint(buleprint)
    self.BlueprintInfo:SetData(buleprint)
    UH.SetText(self.PetName, pet:Name())
    UH.SpriteName(self.PetImgType, string.format("Race%s", pet:RaceId()))
    UH.SpriteName(self.PetNameType, string.format("_LocRaceName%s", pet:RaceId()))
end

function PetStampInfoPanel:OnAttriAddClick()
    local pet = PetData.Instance:GetCurSelect()
    local blueprint = pet:StampBlueprint()
    local attri_list = blueprint:AttriList() or {}
    local param_t = {
        title = Language.PetStamp.Title3,
        attri_list = attri_list,
    }
    ViewMgr:OpenView(PetStampAttriAddition, param_t)
end

-- 方案信息
PetStampBlueprintInfo = PetStampBlueprintInfo or DeclareMono("PetStampBlueprintInfo", UIWidgetBaseItem)
function PetStampBlueprintInfo:SetData(blueprint)
    UIWidgetBaseItem.SetData(self, blueprint)
    local pet_index = blueprint:PetIndex()
    if pet_index then
        local pet = PetData.Instance:GetPetInfo(pet_index)
        self.PetCell:SetObjActive(true)
        self.PetCell:SetData(pet)
    else
        self.PetCell:SetObjActive(false)
    end
    for i = 1, PetStampCfg.StampMaxPos do
        local stamp = blueprint:GetStampByPos(i)
        self.PetStamps[i]:SetData({stamp = stamp})
    end
end


PetStampBigItem = PetStampBigItem or DeclareMono("PetStampBigItem", UIWidgetBaseItem)
function PetStampBigItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pos_index = self.PosIndex
    local qua = 1

    local stamp = data.stamp
    self.PosImg:SetObjActive(stamp == nil)
    self.LevelObj:SetActive(stamp ~= nil)
    if stamp and stamp.item_id > 0 then
        pos_index = data.stamp:PosIndex()
        qua = stamp:QuaLevel()
        local icon = stamp:Icon()
        UH.SetIcon(self.Icon, icon)
        --self.QuaPoints.text = stamp:QuaShowPoint()
        self.QuaPoints.text = ""
        self.Effect:SetActive(true)
        UH.SetText(self.Pos, DataHelper.GetDaXie(pos_index, true))
        UH.SetText(self.Level, string.format(Language.PetStamp.Level,stamp:Level()))
        self.SwallowStar:SetActive(stamp:Swallowed())
    else
        UH.SetIcon(self.Icon, nil)
        self.QuaPoints.text = ""
        self.Effect:SetActive(false)
        UH.SpriteName(self.PosImg, string.format("_LocYiShuZi%s",pos_index))
        self.SwallowStar:SetActive(false)
    end
    UH.SpriteName(self.QuaFrame, string.format("pet_stamp_%s",qua))
end

function PetStampBigItem:OnClick()
    local stamp = self.data.stamp
    if not stamp then
        return
    end
    local info = {
        item_list = {stamp},
        func = function ()
            local buleprint = PetStampData.Instance:GetCurBuleprint()
			if buleprint then
				local pet_index = buleprint:PetIndex()
				if pet_index then
					if stamp:Level() >= 20 and stamp:QuaLevel() >= 6 and not stamp:Swallowed() then
						return { ItemInfoFunc.PetStampUnfix, ItemInfoFunc.PetStampSwallow, ItemInfoFunc.PetStampIntensify}
					else
						return { ItemInfoFunc.PetStampUnfix, ItemInfoFunc.PetStampIntensify}
					end
				else
					return { ItemInfoFunc.PetStampBlueprintUnfix}
				end
			end
			return {}
        end,
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end



PetStampAttriItem1 = PetStampAttriItem1 or DeclareMono("PetStampAttriItem1", UIWidgetBaseItem)
function PetStampAttriItem1:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local attri_name = Language.AttrItems.AttrNames[data.attri_type]
    self.BgObj:SetActive(data.is_bg)
    UH.SetText(self.Name, attri_name)
    UH.SetText(self.Value, data.attri_value)
end




PetStampSuitInfoItem = PetStampSuitInfoItem or DeclareMono("PetStampSuitInfoItem", UIWidgetBaseItem)
function PetStampSuitInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local first_suit = data[1]
    local suit_group = PetStampData.Instance.suit_group[first_suit.suit_id][first_suit.qua]
    --local index = first_suit.num >= 4 and 2 or 1
    local cfg = suit_group[1] 
    UH.SetText(self.Name, cfg.god_print_name)
    local title = #data > 1 and Language.PetStamp.SuitTitle1 or Language.PetStamp.SuitTitle2
    local desc = PetStampData.Instance:GetSuitDescBySuitList(data,ColorStr(title, COLORSTR.Green1))
    UH.SetText(self.Desc, desc)
    UH.SetIcon(self.Icon, cfg.god_print_icon)
end



-- 神印背包信息
-- self.IsPet ::::::::: 是否把已穿戴的神印放入背包
PetStampBagPanel = PetStampBagPanel or DeclareMono("PetStampBagPanel", UIWFlushPanel)

function PetStampBagPanel:PetStampBagPanel()
    self.data = PetStampData.Instance
    
    self.data_cares = {
        {data = self.data.bag_sel_data, func = self.FlushPanel, init_call = false},
        {data = self.data.pet_stamp_list, func = self.FlushPanel, init_call = false},
        {data = self.data.flush_bag_data, func = self.FlushPanel, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
    }
end

function PetStampBagPanel:Awake()
    UIWFlushPanel.Awake(self)
    local show_type = self.data.bag_sel_data.show_type or 2
    self.ShowTypes[show_type].isOn = true
    if self.data.bag_sel_data.pos_index and self.data.bag_sel_data.pos_index > 0 then
        self.PosToggles[self.data.bag_sel_data.pos_index].isOn = true
    end
    UH.SetText(self.SortText, self.data.stamp_cfg.rank_type[self.data.bag_sel_data.sort_type].type_name)
    self:FlushPanel()
end

function PetStampBagPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.bag_sel_data.suit_id = 0
end

function PetStampBagPanel:FlushPanel()
    self:FlushDisplay()
end

function PetStampBagPanel:FlushDisplay()
    local is_show_suit = self.data.bag_sel_data.show_type == 1 and (self.data.bag_sel_data.suit_id == 0 or not self.data.bag_sel_data.suit_id)
    self.GridList:SetObjActive(not is_show_suit)
    self.SuitList:SetObjActive(is_show_suit)
    self.PosListObj:SetActive(not is_show_suit)
    self.BackBtnObj:SetActive(self.data.bag_sel_data.suit_id ~= 0)
    if is_show_suit then
        self:FlushSuitList()
        UH.SetText(self.Count, "")
    else
        self:FlushGridList()
        UH.SetText(self.Count, string.format("%s/%s",self.data:Count(),PetStampCfg.BagMaxCapacity))
    end
end

function PetStampBagPanel:FlushGridList()
    local suit_id = self.data.bag_sel_data.suit_id
    local pos_index = self.data.bag_sel_data.pos_index
    local sort_type = self.data.bag_sel_data.sort_type
    local grid_list_data = self.data:GetBagGridList(suit_id, pos_index, sort_type, self.IsPet or false)
    self.GridList:SetData(grid_list_data)
end

function PetStampBagPanel:FlushSuitList()
    local suit_list = self.data:GetSuitListData(true)
    self.SuitList:SetData(suit_list)
end

function PetStampBagPanel:OnSelPosToggleEvent(value, pos_index)
    if value then
        self.data.bag_sel_data.pos_index = pos_index
    else
        self.data.bag_sel_data.pos_index = 0
    end
end

function PetStampBagPanel:OnSelTypeToggleEvent(show_type)
    if show_type == 1 then
        local suit_list = self.data:GetSuitListData(true)
        if TableIsEmpty(suit_list) then
            self.ShowTypes[1].isOn = false
            self.ShowTypes[2].isOn = true
            PublicPopupCtrl.Instance:Center(Language.PetStamp.Tip9)
            return
        end
    end
    self.data.bag_sel_data.show_type = show_type
    self.data.bag_sel_data.suit_id = 0
end

function PetStampBagPanel:OnSelToggleEvent(toggle_value)
    self.SelList:SetObjActive(toggle_value)
    if toggle_value then
        self.SelList:SetData(self.data.stamp_cfg.rank_type)
    end
end

function PetStampBagPanel:OnSelClick(data)
    self.data.bag_sel_data.sort_type = data.seq
    self.SortToggle.isOn = false
    UH.SetText(self.SortText, data.type_name)
end

function PetStampBagPanel:OnSuitItemClick(data)
    if data.num == 0 then
        PublicPopupCtrl.Instance:Error(Language.PetStamp.Error1)
        return
    end
    self.data.bag_sel_data.suit_id = data.id
    self.data:SetSuitNewFlag(data.id,false)
end

function PetStampBagPanel:OnBackClick()
    self.data.bag_sel_data.suit_id = 0
end

PetStampBagSelectItem = PetStampBagSelectItem or DeclareMono("PetStampBagSelectItem", UIWidgetBaseItem)
-- data:{text, qua}
function PetStampBagSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local text = data.type_name or data.text
    UH.SetText(self.Text, data.type_name)
end

PetStampBagItem = PetStampBagItem or DeclareMono("PetStampBagItem", UIWidgetBaseItem)
function PetStampBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end

PetStampBagSuitItem = PetStampBagSuitItem or DeclareMono("PetStampBagSuitItem", UIWidgetBaseItem)
function PetStampBagSuitItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local suit_id = data.id
    local suit_group = PetStampData.Instance.suit_show_group[suit_id]
    UH.SetText(self.Desc, PetStampData.Instance:GetSuitDesc(suit_id, 4, nil,COLORSTR.Red8))
    UH.SetText(self.Count, data.num)
    UH.SetText(self.Name, suit_group[1].god_print_name)
    UH.SetIcon(self.Icon, suit_group[1].god_print_icon)
    self.NewFlagObj:SetActive(PetStampData.Instance:GetSuitNewFlag(data.id))
end

-- 属性加成信息
PetStampAttriAddition = PetStampAttriAddition or DeclareView("PetStampAttriAddition", "pet/pet_stamp/pet_stamp_attri_addition")
VIEW_DECLARE_MASK(PetStampAttriAddition,ViewMask.BlockClose)
function PetStampAttriAddition:PetStampAttriAddition()
    pet_stamp_attri_addition_index = 0
end

function PetStampAttriAddition:LoadCallback(param_t)
    UH.SetText(self.Title,param_t.title or "")
    self.List:SetData(param_t.attri_list or {})
    self.EmptyObj:SetActive(TableIsEmpty(param_t.attri_list))
end

PetStampAttriAdditionItem = PetStampAttriAdditionItem or DeclareMono("PetStampAttriAdditionItem", UIWidgetBaseItem)
function PetStampAttriAdditionItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = data.name or Language.AttrItems.AttrNames[data.attri_type]
    local value = data.attri_value or data.value
    value = string.format("+%s", value)
    UH.SetText(self.Name, name)

    local value_str
    if DataHelper.IsPercent(data.attri_type) then
        value_str = string.format("+%s%%",data.attri_value/10)
    else
        value_str = string.format("+%s",data.attri_value)
    end
    UH.SetText(self.Value,value_str)
    pet_stamp_attri_addition_index = pet_stamp_attri_addition_index + 1
    if pet_stamp_attri_addition_index > 2 then
        pet_stamp_attri_addition_index = 1
    end
    UH.SpriteName(self.Bg, string.format("Tiao%s",pet_stamp_attri_addition_index))
end

PetStampCurrExp = PetStampCurrExp or DeclareMono("PetStampCurrExp", UIWFlushPanel)
function PetStampCurrExp:PetStampCurrExp()
    self.data = PetStampData.Instance
    self.data_cares = {
        {data = self.data.exp_info, func = self.FlushPanel, init_call = true},
    }
end

function PetStampCurrExp:FlushPanel()
    UH.SetText(self.Value, self.data:GetExp())
end

function PetStampCurrExp:OnAddExpClick()
    ViewMgr:OpenView(PetStampResolveSetView)
end

PetStampSuitInfoShow = PetStampSuitInfoShow or DeclareMono("PetStampSuitInfoShow", UIWidgetBaseItem)
-- data:[{suit_id = i,num = v}]
function PetStampSuitInfoShow:SetData(data)
    data = data or {}
    UIWidgetBaseItem.SetData(self, data)
    local suit_list = data

    local is_skill = false
    for i,v in pairs(suit_list) do
        if v.num >= 4 then
            is_skill = true
            break
        end
    end

    -- 有多少种套装id
    local len = #suit_list
    local suit_count = is_skill and 1 or len

    self.SuitInfos[1]:SetObjActive(suit_count == 1)
    self.SuitInfos[2]:SetObjActive(suit_count == 2)
    self.SuitInfos[3]:SetObjActive(suit_count == 2)
    if suit_count == 1 then
        self.SuitInfos[1]:SetData(data)
    elseif suit_count == 2 then
        self.SuitInfos[2]:SetData({data[1]})
        self.SuitInfos[3]:SetData({data[2]})
    end
    if TableIsEmpty(suit_list) then
        self.EmptyStateText:SetObjActive(true)
        UH.SetText(self.EmptyStateText, Language.PetStamp.Tip2)
    else
        self.EmptyStateText:SetObjActive(false)
    end
end