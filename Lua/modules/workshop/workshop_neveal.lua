WorkshopNeveal = WorkshopNeveal or DeclareMono("WorkshopNeveal", UIWFlushPanel)
function WorkshopNeveal:WorkshopNeveal()
    self.data = WorkshopData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEquip },
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = self.data.neveal_info, func = self.FlushCost}
    }

    self.send_data = {
        select_equip = nil,
        select_stone = nil,
        use_stone = false,
        in_bag = false,
        mark = false
    }
    self.is_init = false

    self.cur_act_list = {}

    GuideManager.Instance:RegisterGetGuideUi("NevealItemSelect", function ()
        if self.EquipList and self.EquipList:SetupCompleted() then
            if #self.EquipList.item_list == 0 then  
                GuideManager.Instance:ForceStop()
            else
                return self.EquipList:GetItem(1).gameObject
            end 
        end
        return nil,nil
    end)

end

function WorkshopNeveal:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.cur_index = nil
    self.data:SetNevealSelect()
    self.data:SetNevealStoneSelectStone()
end 

function WorkshopNeveal:onFlush()
    self:FlushEquip()
    self:FlushCost()
end 

-- 初始化刷新
function WorkshopNeveal:InitFlush()
    if self.is_init then return end

    local list = self.data:GetNevealEquipList()

    if self.data:GetNevealSelect() ~= nil then 
        local pos_ = 1
        for k,v in pairs(list) do 
            if self.data:GetNevealSelect().index == v.index 
                and self.data:GetNevealSelect().in_bag == v.in_bag 
                and self.data:GetNevealSelect().item_id == v.item_id then 
                pos_ = k
            end 
        end  

        if self.flush_left_item_handle ~= nil then
            TimeHelper:CancelTimer(self.flush_left_item_handle)
            self.flush_left_item_handle = nil
        end

        self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
            self.flush_left_item_handle = nil
            if pos_ <= #list -5 then 
                self.EquipList:JumpVerticalIndex(pos_,100)
            else 
                self.EquipList:JumpVerticalIndex(#list -5,100,65)
            end 
        end,5+math.floor(pos_/self.EquipList.StepCount))
    end 
   
    self.cost_cell:SetData(Item.Create({item_id = Config.equipment_dianhua_auto.other[1].fumo_id}))
    self.cur_act_list = BagData.Instance:GetNevealSkillActed()
    self.is_init = true
end

function WorkshopNeveal:FlushEquip()

    local equip_list = self.data:GetNevealEquipList()
    self.EquipList:SetData(equip_list)
    local empty_flag = table.nums(equip_list) == 0
    self.right_part:SetActive(not empty_flag)
    self.empty:SetActive(empty_flag)

    if self.cur_index == nil and self.data:GetNevealSelect() then
        self.EquipList:ClickItemData(self.data:GetNevealSelect())
    else
        self.EquipList:ClickItem(self.cur_index or 1)
    end

    if self.is_init then 
        local old_act_list = self.cur_act_list
        self.cur_act_list = BagData.Instance:GetNevealSkillActed()

        if old_act_list[1] ~= self.cur_act_list[1] and self.cur_act_list[1] ~= nil then 
            local skill_level = BagData.Instance:GetNevealSkillLevel(self.cur_act_list[1])
            
            local cfg = self.data:GetNevealSkillCfgByUnlockLevel(self.cur_act_list[1],skill_level)
            if cfg ~= nil then 
                ViewMgr:OpenView(WorkshopNevealSkillShowView,{icon = cfg.skill_icon,name = cfg.skill_name,desc = cfg.set_skill_description})
            end 
        end 

        -- if self.send_data.select_equip:IsRefreshNeveal() then 
        --     local cur_refresh_eff = self.send_data.select_equip.in_bag and BagData.Instance:GetItemGridByIndex(0,self.send_data.select_equip.index):RefreshNeveal().effect_id 
        --         or BagData.Instance:GetEquipList(self.send_data.select_equip.index):RefreshNeveal().effect_id
        --     if self.send_data.select_stone ~= nil and self.send_data.select_stone:Neveal().effect_id == cur_refresh_eff then 
        --         self.send_data.select_stone = nil 
        --         self.send_data.in_bag = false
        --         self.data:SetNevealStoneSelectStone()
        --     end 
        -- end 
    end 

    if self.send_data.mark then 
        self.send_data.mark = false
        self.send_data.select_stone = nil 
        self.data:SetNevealStoneSelectStone()
    end 

    self:InitFlush()
end

function WorkshopNeveal:OnClickItem(data)
    local temp_index = self.EquipList:GetSelectedIndex(data)
    self.send_data.select_stone = nil
    self.data:SetNevealStoneSelectStone()
    self.send_data.select_equip = data

    self.data:SetNevealSelect(data)

    self.cur_index = temp_index
    self:FlushEquipInfo()
    self:FlushCost()
end 

function WorkshopNeveal:OnClickNevealSkillShow()
    self.data:SetNevealClickSkill(self.send_data.select_equip:Neveal())
    ViewMgr:OpenView(WorkshopNevealSkillTip)
end

function WorkshopNeveal:FlushEquipInfo()
    local effect = self.send_data.select_equip:Neveal()
    local refresh = self.send_data.select_equip:RefreshNeveal()
    self.EquipShow:SetData(self.send_data.select_equip)
    self.cur_effect_info:SetData(effect)
    self.next_effect_info:SetData(refresh)

    local flag_can_save = self.send_data.select_equip:IsRefreshNeveal()
    self.btn_save:SetActive(flag_can_save)

    -- local skill_cfg = self.data:GetNevealSkillCfg(effect.effect_id)
    -- self.cur_skill_name.gameObject:SetActive(skill_cfg ~= nil)

    -- 本区域原本逻辑是现实当前装备的激活展示
    --（现改为全身的激活显示）
    local enable = BagData.Instance:GetNevealSkillEnable()
    self.TargetActiveObj:SetActive(enable ~= nil)
    if enable ~= nil then 
        local total_level = BagData.Instance:GetNevealSkillLevel(enable.effect_id)
        local skill_cfg = total_level > 0 and WorkshopData.Instance:GetNevealSkillCfgByUnlockLevel(enable.effect_id,total_level) 
            or WorkshopData.Instance:GetNevealSkillCfg(enable.effect_id)
        local equip_num,equip_total = BagData.Instance:GetNevealSkillNum(enable.effect_id)

        self.skill_initer.Interactable = total_level > 0
        UH.SetIcon(self.cur_skill_icon,skill_cfg.skill_icon)
        UH.SetText(self.cur_skill_name,total_level > 0 and ColorStr(Language.Workshop.Neveal.Lv..skill_cfg.skill_level..skill_cfg.skill_name,COLORSTR.Green9) or skill_cfg.skill_name
            .."("..equip_num.."/"..equip_total..")")

        UH.SetText(self.TargetTitle,equip_num >= equip_total and Language.Workshop.Neveal.TargetTitle[2] or Language.Workshop.Neveal.TargetTitle[1])

        local neveal_skill_cfg = self.data:GetNevealSkillUnlockCfg({type = enable.effect_id })
        local active_tips = neveal_skill_cfg ~= nil and string.format(Language.Workshop.Neveal.ActiveTips,neveal_skill_cfg.skill_unlock_level,neveal_skill_cfg.skill_level)
            or Language.Workshop.Neveal.MaxTotalActiveTips

        UH.SetText(self.ActiveTips,equip_num >= equip_total and active_tips or Language.Workshop.Neveal.UnTotalActiveTips)
    end 

    -- self.skill_show.gameObject:SetActive(false)
    -- self.TargetActiveObj:SetActive(skill_cfg ~= nil)
    -- local active_tips = string.format(Language.Workshop.Neveal.ActiveTips,5,1)
    -- if skill_cfg ~= nil then 
    --     UH.SetIcon(self.cur_skill_icon,skill_cfg.skill_icon)

    --     local total_level = BagData.Instance:GetNevealSkillLevel(effect.effect_id)
    --     local skill_cfg = total_level > 0 and WorkshopData.Instance:GetNevealSkillCfgByUnlockLevel(effect.effect_id,total_level) 
    --         or WorkshopData.Instance:GetNevealSkillCfg(effect.effect_id)

    --     local skill_num,skill_total = BagData.Instance:GetNevealSkillNum(effect.effect_id)
    --     UH.SetText(self.cur_skill_name,total_level > 0 and ColorStr( skill_cfg.skill_name.." "..Language.Workshop.Neveal.Lv..skill_cfg.skill_level,COLORSTR.Green9) or skill_cfg.skill_name
    --     .."("..skill_num.."/"..skill_total..")")

    --     self.skill_initer.Interactable = total_level > 0

    --     local neveal_skill_cfg = self.data:GetNevealSkillUnlockCfg({type = effect.effect_id })
    --     active_tips = string.format(Language.Workshop.Neveal.ActiveTips,neveal_skill_cfg.skill_unlock_level,neveal_skill_cfg.skill_level)

    --     UH.SetText(self.TargetTitle,skill_num >= skill_total and Language.Workshop.Neveal.TargetTitle[2] or Language.Workshop.Neveal.TargetTitle[1])
    -- end 

    -- UH.SetText(self.ActiveTips,active_tips)
end 

function WorkshopNeveal:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup{tips_str =  Config.language_cfg_auto.textdesc[184].desc}
end

function WorkshopNeveal:OnClickStone(flag)
    self.send_data.use_stone = flag

    self:FlushCost()
end

function WorkshopNeveal:OnClickQuick(flag)
    self.send_data.use_quick = flag

    self:FlushCost()
end

function WorkshopNeveal:FlushCost()
    local show_extract = #BagData.Instance:GetGridByType({ItemType.Neveal}) > 0
    self.extract_show:SetActive(show_extract)
    if not show_extract and self.send_data.use_stone then 
        self.extract_toggle.isOn = false
        self:OnClickStone(false) 
    end 

    self.send_data.select_stone = self.data:GetNevealStoneSelectStone()

    self.plus_obj:SetActive( self.send_data.use_stone and self.send_data.select_stone == nil) 
    self.change_obj:SetActive(self.send_data.use_stone and self.send_data.select_stone ~= nil) 
    self.plus_cell.gameObject:SetActive(self.send_data.use_stone)

    -- 自动购买只能在非附魔石的情况下使用
    self.quick_show:SetActive(not self.send_data.use_stone)

    self.cost_cell.gameObject:SetActive(not self.send_data.use_stone)
    self.cost_obj:SetActive(not self.send_data.use_stone) 
    self.cost_num.gameObject:SetActive(not self.send_data.use_stone)
    
    self.plus_cell:SetData(self.send_data.select_stone or Item.New()) 
    local data = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].fumo_id})
    local cur_num = data:Num() 
    local color_1 = cur_num >= 1 and COLORSTR.White or COLORSTR.Red8
    UH.SetText(self.cost_num,"<color=#"..color_1..">".. cur_num.."</color>/"..1)

    UH.SetText(self.btn_text,self.send_data.use_stone and Language.Workshop.Neveal.BtnTextE or Language.Workshop.Neveal.BtnTextN)
end

function WorkshopNeveal:OnClickOperate()
    -- LogError("!OnClickOperate?@")
    -- WORLSHOP_OP.WORKSHOP_OP_DIAN_HUA
    if self.send_data.use_stone and self.send_data.select_stone == nil  then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.WithoutStone)
        self:OnClickPopOpen()
        return 
    elseif not self.send_data.use_stone and Item.GetNum(Config.equipment_dianhua_auto.other[1].fumo_id) == 0 and not self.send_data.use_quick then 
		PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.WithoutFumo)
        local data = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].fumo_id})
        ViewMgr:OpenView(GetWayView, {item = data})
        return 
    elseif not self.send_data.use_stone and Item.GetNum(Config.equipment_dianhua_auto.other[1].fumo_id) == 0 and self.send_data.use_quick then 
        local price = MarketData.Instance:GetItemPrice(Config.equipment_dianhua_auto.other[1].fumo_id)
        if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,price) then
            local param_t = {
                config = StoreData.Instance:GetItemConfig(CommonItemId.ImmortalCoinId,5),
                comfirm_func = function(data)
                    ShoppingCtrl.Instance:SendStoreBuyReq(data)
                    ViewMgr:CloseView(MoneyExchangeView)
                end
            }

            param_t.money_type = CurrencyType.ImmortalCoin
            -- 这个配置的道具是1000个灵玉

            MallData.JumpItemNum = math.floor(price/1000)
            ViewMgr:OpenView(MoneyExchangeView,param_t)

            return 
        else 
            MarketCtrl.Instance:SendMarketReq(1,Config.equipment_dianhua_auto.other[1].fumo_id,1)
        end
    end 

    local cur_eff = self.send_data.select_equip:Neveal()
    local refresh_eff = self.send_data.select_equip:RefreshNeveal()

    local function send_msg()
        local cost_item = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].fumo_id})
        local data = self.send_data.use_stone and self.send_data.select_stone or cost_item
        self.send_data.in_bag = self.send_data.select_equip.in_bag
        if self.send_data.use_stone then 
            WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_DIAN_HUA,
            self.send_data.select_equip.in_bag == true and 1 or 0,
            self.send_data.select_equip.index,
            data:GridColumn(),
            data.index)
    
            self.send_data.select_stone  = nil
        else
            WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_FU_MO,
            self.send_data.select_equip.in_bag == true and 1 or 0,
            self.send_data.select_equip.index)
        end 
    
        self.EffetTool:Play(5161034)
        self.block:SetActive(true)
        self.send_data.mark = true

        if ViewMgr:IsOpen(DialogTipsView) then 
            ViewMgr:CloseView(DialogTipsView)
        end 
    end

    if refresh_eff.effect_level > cur_eff.effect_level then
        local info = {
            content = Language.Workshop.Neveal.RefreshLevelhigh,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = send_msg
            },
            new_tnr = DTNewTodayNotRemind.WorkshopNeveal,
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    else
        send_msg()
    end 
end

function WorkshopNeveal:OnClickSave()
    local empty_flag = self.send_data.select_equip:IsRefreshNeveal()

    if not empty_flag then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.WithoutRefresh)
        return 
    end 
    
    WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_SAVE_DIAN_HUA,
    self.send_data.select_equip.in_bag == true and 1 or 0,
    self.send_data.select_equip.index)
    
    self.EffetTool:Play(5161035)
    self.block:SetActive(true)


    -- WORLSHOP_OP.WORKSHOP_OP_SAVE_DIAN_HUA
end

function WorkshopNeveal:OnClickAtlas()
    ViewMgr:OpenView(WorkshopNevealAtlasView)
    -- WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_DIAN_HUA,0,0,0,2)
end

function WorkshopNeveal:OnClickSelect()
    self.pop_part:SetActive(false) 

    local data = self.select_list:GetSelectedData()
    self.send_data.select_stone = Item.Create(data[1])

    self:FlushCost()
end

function WorkshopNeveal:OnClickExtract()
    local effect = self.send_data.select_equip:Neveal()
    if effect.effect_id == 0 or effect.effect_level == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.WithoutEffToExtract)
        return 
    end 
    ViewMgr:OpenView(WorkshopNevealExtractView)
    -- WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_SAVE_DIAN_HUA,0,0)
end

function WorkshopNeveal:OnClickPopOpen()
    ViewMgr:OpenView(WorkshopNevealSelect)
    -- self.pop_part:SetActive(true)    
    -- local data = BagData.Instance:GetGridByType({ItemType.Neveal})
    -- self.select_list:SetData(data)
end

function WorkshopNeveal:OnClickPopClose()
    -- self.pop_part:SetActive(false)    
end

function WorkshopNeveal:OnEffectComplete()
    -- self.block:SetActive(false)
end
function WorkshopNeveal:OnClickChange()
    ViewMgr:OpenView(WorkshopNevealChange)    
end

WorkshopNevealEffInfo = WorkshopNevealEffInfo or DeclareMono("WorkshopNevealEffInfo", UIWFlushPanel)
function WorkshopNevealEffInfo:WorkshopNevealEffInfo() 
    self.data = WorkshopData.Instance
end
function WorkshopNevealEffInfo:SetData(data)
    self.enter_data = data
    local cfg = self.data:GetNevealCfg(data.effect_id,data.effect_level)
    local empty_flag = data.effect_id == 0 or data.effect_level == 0 or cfg == nil
    self.eff_empty:SetActive(empty_flag)
    self.eff_desc.gameObject:SetActive(not empty_flag)
    self.eff_set.gameObject:SetActive(not empty_flag)
    self.cur_skill_show:SetActive(not empty_flag)
    if empty_flag then return end 
    local str_desc = Language.Workshop.Neveal.PlusEff..cfg.effect_name..Language.Workshop.Neveal.Lv..data.effect_level
    str_desc = str_desc .. "\n"..Language.Workshop.Neveal.EffAttr..Language.Common.AttrList[cfg.bonus_type].."+"..cfg.bonus_attributes
    UH.SetText(self.eff_desc, str_desc)

    local unlock_level = BagData.Instance:GetNevealSkillLevel(data.effect_id)
    local skill_cfg = unlock_level > 0 and self.data:GetNevealSkillCfgByUnlockLevel(data.effect_id,unlock_level) or self.data:GetNevealSkillCfg(data.effect_id)
    local str_set = Language.Workshop.Neveal.SetDesc..skill_cfg.set_skill_description..(self.show_act and (unlock_level > 0 and Language.Workshop.Neveal.Actived or Language.Workshop.Neveal.UnActive) or "")

    UH.SetText(self.eff_set, str_set)

    self.cur_skill_name.gameObject:SetActive( data.effect_id > 0) 
    if data.effect_id > 0 then 
        UH.SetIcon(self.cur_skill_icon,skill_cfg.skill_icon)
        UH.SetText(self.cur_skill_name,skill_cfg.skill_name)
    end 
end

function WorkshopNevealEffInfo:OnClickNevealSkillShow()
    self.data:SetNevealClickSkill(self.enter_data)
    ViewMgr:OpenView(WorkshopNevealSkillTip)
end


WorkshopNevealItem =  WorkshopNevealItem or DeclareMono("WorkshopNevealItem", UIWidgetBaseItem)
function WorkshopNevealItem:WorkshopNevealItem() end
function WorkshopNevealItem:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.NameTex, data:NameId())

    local sp_cfg = data:IsNeveal() and WorkshopData.Instance:GetNevealCfg(data.param.cur_effect.effect_id,data.param.cur_effect.effect_level) or nil
    local sp_name = sp_cfg ~= nil and sp_cfg.effect_name or ""
    UH.SetText(self.EffNameTex, "【"..sp_name.."】")
    UH.SetText(self.EffPlusName, Item.GetName(data.item_id))

    self.NameTex.gameObject:SetActive(not data:IsNeveal() or sp_cfg == nil)
    self.EffNameTex.gameObject:SetActive(data:IsNeveal() and sp_cfg ~= nil)
    self.InBagObj:SetActive(not data.in_bag)
    local attr_str = data:IsNeveal() and Language.Workshop.Neveal.EffLevel..data:Neveal().effect_level or Language.Workshop.Neveal.None
    UH.SetText(self.AttrTex, attr_str)
end
function WorkshopNevealItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

WorkshopNevealexSelectCell = WorkshopNevealexSelectCell or DeclareMono("WorkshopNevealexSelectCell", UIWidgetBaseItem)
function WorkshopNevealexSelectCell:WorkshopNevealexSelectCell() end
function WorkshopNevealexSelectCell:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    self:OnSelect(false)
    UH.SetText(self.name,data:QuaName())
    local info = data:Neveal()

    -- LogError("!?@1",info)
    local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
    local str_desc = Language.Workshop.Neveal.PlusEff..cfg.effect_name..Language.Workshop.Neveal.Lv..info.effect_level
    str_desc = str_desc .. "\n"..Language.Workshop.Neveal.EffAttr..Language.Common.AttrList[cfg.bonus_type].."+"..cfg.bonus_attributes
    UH.SetText(self.sp_desc, str_desc)

    local unlock_level = BagData.Instance:GetNevealSkillLevel(info.effect_id)
    local skill_cfg = WorkshopData.Instance:GetNevealSkillCfg(info.effect_id)
    local str_set = Language.Workshop.Neveal.SetDesc..skill_cfg.set_skill_description

    UH.SetText(self.skill_desc, str_set)
end 

function WorkshopNevealexSelectCell:OnSelect(selected)
    if not self.is_long_click then
        self:SetSelected(selected)
    else 
        self:SetSelected(not selected)
    end 
end

function WorkshopNevealexSelectCell:OnRelease()
    TimeHelper:CancelTimer(self.handle_delay)
end

function WorkshopNevealexSelectCell:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.Cell)
            end
        end,
        0.2
    )
end

function WorkshopNevealexSelectCell:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end
