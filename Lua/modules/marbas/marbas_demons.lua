-- 造化装备·入魔界面
MarbasDemonsView = MarbasDemonsView or DeclareView("MarbasDemonsView", "marbas/marbas_demons", Mod.Marbas.Demons)
function MarbasDemonsView:MarbasBagView()
    -- self.board
end
function MarbasDemonsView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.DemonsView, Vector2.New(1225, 703))
end

function MarbasDemonsView:OnCloseClick()
    ViewMgr:CloseView(MarbasDemonsView)
end

MarbasDemonsPanel = MarbasDemonsPanel or DeclareMono("MarbasDemonsPanel", UIWFlushPanel)
function MarbasDemonsPanel:MarbasDemonsPanel()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.equip_list,func = self.FlushGroundPanel},
        {data = self.data.item_grid,func = self.FlushGroundPanel},
        {data = self.data.demons_view_data,func = self.FlushrDetailPanel,init_call = false},
        {data = self.data.demons_view_data,func = self.FlushGroundPanel,init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushGroundPanel, init_call = false},
    }

    for i = 1,2 do 
        self.fly_star[i]:SetData({
            mono = self,
            start_effect = 3165130,
            speed_time = 3,
            pre_pause = 1,
        })
    end
    self.fly_star[1].end_func = BindTool.Bind(self.FlyEnd1,self)
    self.fly_star[2].end_func = BindTool.Bind(self.FlyEnd2,self)
end

function MarbasDemonsPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushGroundPanel({init = true})
end

function MarbasDemonsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.init_timer)
    self.data:TypeDemonsSetSelect()

    for i = 1,2 do 
        self.fly_star[i]:AllHandleCancel()
    end 
end 

function MarbasDemonsPanel:FlushGroundPanel(param)

    local datas = self.data:TypeDemonsGetEquipList()

    if #datas == 0 then 
        self.list.gameObject:SetActive(false)
        self.down:SetActive(false)
        self.detail:SetActive(false)
        self.empty_part:SetActive(true)
        return 
    end 

    self.list:SetData(datas)

    if param and param.init then 
        self.init_timer = TimeHelper:AddDelayFrameTimer(function ()
            self.list:ClickItem(1)
		end,5) 
    end

    local other_cfg = Config.zaohua_equip_up_auto.other[1] --.enchant_consume
    self.cost_item:SetDataByItemId(other_cfg.enchant_consume)
    local num = Item.GetNum(other_cfg.enchant_consume)
    local need_num = other_cfg.enchant_consume_num
    local str = DataHelper.ConsumeNum(num, need_num,true)
    UH.SetText(self.cost_num, str)
    UH.SetText(self.cost_name, Item.GetQuaName(other_cfg.enchant_consume))
end 

function MarbasDemonsPanel:FlushrDetailPanel()
    local mark = self.data:TypeDemonsDemenosSelect()
    if mark == nil then 
        return 
    end 

    if mark.Color and mark:Color() > MarbasConfig.QuaEnter then 
        return 
    end 
    local info = {
        item_list = {mark}
    }
    ItemInfoData.Instance:SetItemInfo(info)

    self.down:SetActive(mark ~=nil)
    self.detail:SetActive(mark ~=nil)

 
    self.cur_item_info:SetData(mark)
    local next_part = self.data:TypeDemonsPreDemenos()
    self.next_item_info:SetData(next_part)
end 

function MarbasDemonsPanel:OnClickEquipCell(data)

    self.data:TypeDemonsSetSelect(data)
end 

function MarbasDemonsPanel:OnClickDemons()
    local other_cfg = Config.zaohua_equip_up_auto.other[1] --.enchant_consume
    local num = Item.GetNum(other_cfg.enchant_consume)
    local need_num = other_cfg.enchant_consume_num


    if num < need_num then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.DemonsLackCost)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = other_cfg.enchant_consume})})
        return 
    end 

    self.fly_star[1]:Play("3165131")

    self.block:SetActive(true)
end 

function MarbasDemonsPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[314].desc})
end

function MarbasDemonsPanel:FlyEnd1()
    self.fly_star[1]:CancelFlyStarEffect()

    self.fly_star[2]:Play("3165131")
end 

function MarbasDemonsPanel:FlyEnd2()
    self.fly_star[2]:CancelFlyStarEffect()

    local mark = self.data.demons_view_data.select 
    self.data.demons_view_data.demons_mark = 1
    MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.Demons, 
        mark.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped, 
        mark.index)
    
    self:FlushGroundPanel({init = true})
    self.block:SetActive(false)
end 

function MarbasDemonsPanel:OnClickWayBuild()
    -- ViewMgr:CloseView(MarbasView)
    ViewMgr:CloseView(MarbasDemonsView)
    ViewMgr:OpenViewByKey(Mod.Marbas.Build)
end 

function MarbasDemonsPanel:OnClickWayFB()
    ViewMgr:OpenViewByKey(Mod.HarmonyHeaven.Main)

    ViewMgr:CloseView(MarbasDemonsView)
end 

MarbasDemonsCompareItemInfo = MarbasDemonsCompareItemInfo or DeclareMono("MarbasDemonsCompareItemInfo", UIWFlushPanel)
function MarbasDemonsCompareItemInfo:MarbasDemonsCompareItemInfo()end
function MarbasDemonsCompareItemInfo:SetData(data)
    self.base_part:SetData(data)
    self.equip_base:SetData(data)
    self.equip_bathe:SetData(data)
    self.equip_blood:SetData(data)
    self.equip_gem:SetData(data)
    self.equip_suit:SetData(data)
end 

MarbasDemonsEquipCell = MarbasDemonsEquipCell or DeclareMono("MarbasDemonsEquipCell", UIWidgetBaseItem)
function MarbasDemonsEquipCell:MarbasDemonsEquipCell()
    self.mb_data = MarbasData.Instance
end
function MarbasDemonsEquipCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Call:SetData(data)
    UH.SetText(self.Name,data:QuaName())
    self.is_equip:SetActive(not data.in_bag)
    -- self.ItemClick
    UH.SetText(self.prof_limit,data:LimitProfDesc())
    UH.SetText(self.level,data:LevelDesc())
    UH.RenameGame(self,Format(data.in_bag and "bag_%s" or "equiped_%s",data.index))


    self:FlushSelect()
end

function MarbasDemonsEquipCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function MarbasDemonsEquipCell:FlushSelect()
    if self.mb_data.demons_view_data.select == nil then return end 

    self.select:SetActive(self.data.index == self.mb_data.demons_view_data.select.index 
        and self.data.in_bag == self.mb_data.demons_view_data.select.in_bag)
end

-- function MarbasDemonsEquipCell:SetSelect(flag)
--     self.select:SetActive(flag)
-- end

MarbasDemonsItemInfoBase = MarbasDemonsItemInfoBase or DeclareMono("MarbasDemonsItemInfoBase", UIWFlushPanel)
function MarbasDemonsItemInfoBase:SetData(data)
    UH.SetText(self.QuaName,data:QuaName())
    UH.SetText(self.LevelDesc,data:LevelDesc())
    UH.SetText(self.LimitProfDesc,data:LimitProfDesc())
    -- UH.SpriteName(self.Quality,ItemInfoConfig.QualitySp[data:Color()])

    UH.SetText(self.Mana,data:ManaDesc())
    self.ManaObj:SetActive(data:CheckMana() > 0)
	self.Cell:SetData(data)
end 

MarbasDemonsItemInfoEquipBase = MarbasDemonsItemInfoEquipBase or DeclareMono("MarbasDemonsItemInfoEquipBase", UIWFlushPanel)
function MarbasDemonsItemInfoEquipBase:SetData(data) 
    self.data = data
    self.total:SetActive(true) -- 这个是必然有的
    self:FlushInfo()
end 
function MarbasDemonsItemInfoEquipBase:FlushInfo()
    local config = self.data:Config()
    local before_id = MarbasData.Instance:GetDemonsBefore(self.data.item_id)
    local show_plus = before_id ~= self.data.item_id
    local str_format = show_plus and "%s    %s<color=#91db64>(+%s)</color>" or  "%s    %s"

    local be_cfg = Item.GetConfig(before_id)
    for i = 1,self.nomal_cell:Length() do 
        local flag_act = config["attr_value_"..i] ~= nil and config["attr_value_"..i] >0
        self.nomal_cell[i].gameObject:SetActive(flag_act)
        if flag_act then 
            local param = {}
            param.str = show_plus and Format(str_format, Language.Common.AttrList[config["attr_type_"..i]],
                DataHelper.IsPercent(config["attr_type_"..i]) and Percent(config["attr_value_"..i]) or config["attr_value_"..i],
                DataHelper.IsPercent(config["attr_type_"..i]) and Percent(config["attr_value_"..i]- be_cfg["attr_value_"..i]) or config["attr_value_"..i]- be_cfg["attr_value_"..i])
                or Format(str_format, Language.Common.AttrList[config["attr_type_"..i]],
                DataHelper.IsPercent(config["attr_type_"..i]) and Percent(config["attr_value_"..i]) or config["attr_value_"..i])
            -- param.show_arrow = before_id ~= self.data.item_id and be_cfg ~= nil and be_cfg["attr_type_"..i] == config["attr_type_"..i]
            -- param.is_up = be_cfg ~= nil and be_cfg["attr_value_"..i] <= config["attr_value_"..i]
            self.nomal_cell[i]:SetData(param)
        end 
    end 
end 

MarbasDemonsItemInfoBathe = MarbasDemonsItemInfoBathe or DeclareMono("MarbasDemonsItemInfoBathe", UIWFlushPanel)
function MarbasDemonsItemInfoBathe:SetData(data) 
    self.data = data
    self.total:SetActive(true)
    self:FlushInfo()
end 
function MarbasDemonsItemInfoBathe:FlushInfo()
    local info = MarbasData.Instance:GetBatheInfoByType(self.data:EquipType())

    local l_num = MarbasData.Instance:BatheAffixMaxNum(MarbasData.Instance:GetDemonsBefore(self.data.item_id))
    for i = 1,self.solt_cell:Length() do
        local vo =  {} 
        local oper = info.slot_info[i-1]
        local can_un_lock = MarbasData.Instance:BatheSlotUnLockRemind(self.data:EquipType(), i-1) > 0
        if oper == nil then 
            vo.attr_str = Language.Marbas.Bathe.EmptyBathe
        else 
            local affix_id = oper.now_attr.affix_id
            local level = oper.now_attr.level
            if affix_id > 0 then 
                local name, attr, icon = MarbasData.Instance:BatheAffixAttr(affix_id, level)
                local qua = Cfg.MarbasBatheAffixQua(level).quality
                local color = QualityColorStr[qua]
                vo.attr_str = Format( Language.Marbas.Bathe.Level2, level, ColorStr(name, color)).." "..
                    Format(Language.Marbas.Bathe.Attr, color, attr.attr_name, attr.attr_percent_value)
            else 
                vo.attr_str = can_un_lock and Language.Marbas.Bathe.EmptyBathe or Language.Marbas.Bathe.EmptyUnlockBathe
            end 
        end 

        self.solt_cell[i]:SetData(vo)
        self.solt_cell[i].gameObject:SetActive((i <= l_num 
            and ( can_un_lock or oper.now_attr.affix_id > 0)) or i > l_num)
        self.solt_cell[i]:ShowNomal(i <= l_num)
        self.solt_cell[i]:ShowUnlock(i > l_num and self.data:Color() == MarbasConfig.QuaMaster)
        self.solt_cell[i]:Showlock(i > l_num and self.data:Color() == MarbasConfig.QuaEnter)
    end
end 

MarbasBatheInfoCell = MarbasBatheInfoCell or  DeclareMono("MarbasBatheInfoCell", UIWidgetBaseItem)
function MarbasBatheInfoCell:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.attr_str,data.attr_str)
end 

function MarbasBatheInfoCell:ShowUnlock(flag) 
    if self.unlock ~= nil then self.unlock:SetActive(flag) end 
end 
function MarbasBatheInfoCell:Showlock(flag) 
    if self.lock ~= nil then self.lock:SetActive(flag) end 
end 
function MarbasBatheInfoCell:ShowNomal(flag) 
    if self.nomal ~= nil then self.nomal:SetActive(flag) end 
end

MarbasDemonsItemInfoBlood = MarbasDemonsItemInfoBlood or DeclareMono("MarbasDemonsItemInfoBlood", UIWFlushPanel)
function MarbasDemonsItemInfoBlood:SetData(data) 
    self.data = data
    -- local param = MarbasData.Instance:GetBloodPlusAttrs(self.data)
    self.total:SetActive(false)
    -- if param.is_act then 
    --     self:FlushInfo(param)
    -- end 
end 
function MarbasDemonsItemInfoBlood:FlushInfo(param)
    local str_format = "%s    %s"
    for i = 1,self.nomal_cell:Length() do 
        local flag = param.list[i] ~= nil
        self.nomal_cell[i].gameObject:SetActive(flag)
        if flag then 
            local vo = {}
            vo.str = Format(str_format, Language.Common.AttrList[param.list[i].attr_type],
                DataHelper.IsPercent(param.list[i].attr_type) and Percent(param.list[i].attr_value) or param.list[i].attr_value)
            self.nomal_cell[i]:SetData(vo)
        end 
    end 
end 

MarbasDemonsItemInfoGem = MarbasDemonsItemInfoGem or DeclareMono("MarbasDemonsItemInfoGem", UIWFlushPanel)
function MarbasDemonsItemInfoGem:SetData(data) 
    self.data = data
    self.total:SetActive(true)
    self:FlushInfo()
end 
function MarbasDemonsItemInfoGem:FlushInfo()
    for i = 1,4 do 
        local act_flag = MarbasData.Instance:GetBloodSoltLock(self.data,i)
        local is_lock = MarbasData.Instance:GetBloodSoltLock(self.data,i,i == 4)
        self.gem_cell[i].gameObject:SetActive(is_lock or i == 4) -- 4号位必然显示
        self.gem_cell[i]:SetData(Item.Create(self.data:GetStone(i)))
        self.gem_cell[i]:ShowLock(not is_lock)
    end 

    local param = MarbasData.Instance:GetBloodGemAttrs(self.data)
    self.sp_eff:SetActive(param.with_sp)
    UH.SetText(self.sp_eff_str,param.sp_desc )
end 

MarbasItemInfoGemCell = MarbasItemInfoGemCell or DeclareMono("MarbasItemInfoGemCell", UIWidgetBaseItem)
function MarbasItemInfoGemCell:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)

    if data == nil or data.item_id == 0 then 
        self.icon.gameObject:SetActive(false) 
        self.red_frame:SetActive(false)
        self.aya_frame:SetActive(false)
        return 
    end

    self.icon.gameObject:SetActive(true) 
    UH.SetIcon(self.icon,data:IconId())
    self.red_frame:SetActive(data:Color() == ItemColor.Red)
    self.aya_frame:SetActive(data:Color() == ItemColor.Gold)

    -- if self.unlock_sp then self.unlock_sp:SetActive(true) end 
end 

function MarbasItemInfoGemCell:ShowLock(flag)
    self.lock:SetActive(flag)
    self.unlock:SetActive(not flag)
end

MarbasDemonsItemInfoSuitInfo = MarbasDemonsItemInfoSuitInfo or DeclareMono("MarbasDemonsItemInfoSuitInfo", UIWFlushPanel)
function MarbasDemonsItemInfoSuitInfo:MarbasDemonsItemInfoSuitInfo() end 
function MarbasDemonsItemInfoSuitInfo:SetData(data) 
    self.data = data
    self.total:SetActive(true) -- 这个必然有
    self:FlushInfo()
end 
function MarbasDemonsItemInfoSuitInfo:FlushInfo()
    UH.SetText(self.Title,string.format(Language.Marbas.DemonsSuitTitle,Language.Marbas.BaseSuitSimple[self.data:Color()]))
    local suit_list = MarbasData.Instance:GetMarbasDemonsSuitShow(self.data)

    for i = 1,self.suits:Length() do 
        self.suits[i]:SetData(suit_list[i])
    end 
end 

MarbasDemonsSuitItem = MarbasDemonsSuitItem or DeclareMono("MarbasDemonsSuitItem", UIWidgetBaseItem)
function MarbasDemonsSuitItem:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.title,self.data.desc_title)
    for i = 1,3 do 
        if self.data.attr_list[i] ~= nil then 
            self.attrs[i].gameObject:SetActive(not self.data.attr_list[i].is_empty ) 
            self.attrs[i]:SetData(self.data.attr_list[i])
        else 
            self.attrs[i].gameObject:SetActive(false) 
        end 
    end 
end

MarbasDemonsSuitAttrItem = MarbasDemonsSuitAttrItem or DeclareMono("MarbasDemonsSuitAttrItem", UIWidgetBaseItem)
function MarbasDemonsSuitAttrItem:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    self.value.gameObject:SetActive(data.is_attr)
    self.type.gameObject:SetActive(data.is_attr)
    if self.sp_desc ~= nil then self.sp_desc.gameObject:SetActive(not data.is_attr) end 
    if data.is_attr then 
        UH.SetText(self.type,self.data.type)
        UH.SetText(self.value,self.data.value)
    elseif self.sp_desc ~= nil then
        UH.SetText(self.sp_desc,self.data.sp_effect)
    end 
end