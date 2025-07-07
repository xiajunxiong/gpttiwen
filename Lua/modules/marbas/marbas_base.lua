-- 造化装备·基础
MarbasBase = MarbasBase or DeclareMono("MarbasBase", UIWFlushPanel)
function MarbasBase:MarbasBase()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance
    -- self.ctrl:SendEquipOper(0)

    self.data_cares = {
        {data = self.data.equip_list,func = self.SuitFlush},
        {data = self.data.suit_enable,func = self.ShowSuitCombo,keys = {"flush"}},
        {data = self.data.base_view_data,func = self.SuitFlush},
        {data = self.data.better_equip,func = self.ArrowFlush},
    }

-- 启动界面默认最低
    self.data.base_view_data.suit_show = self.data:GetTypeBaseShouldSuit()


    for i = 1,8 do 
		local param_t = {mono = self,
		start_effect = "30000817",
		pre_pause = 1,
		end_func = BindTool.Bind2(self.EquipShowComplete, self,i),
	    end_func_time = 0}
		self.suit_effs[i]:SetData(param_t)
	end

    -- for i = 1,8 do 
    --     self.suit_effs[i].StartPos:SetParent(self.equip_show.equiplist[i].gameObject,false)
    --     UH.AnchoredPosition(self.suit_effs[i].StartPos.transform,Vector2.New(0,0))
    --     self.suit_effs[i].StartPos:SetParent(self.suit_effs[i].gameObject,true) 
    -- end 

    GuideManager.Instance:RegisterGetGuideUi("MarbasEquipSelect", function ()
		if self.equip_show == nil then 
            return nil,nil
        end 
        if self.equip_show.equiplist == nil then 
            return nil,nil
        end 

        if #self.data:GetMarbasBag(-1) == 0 then 
            GuideManager.Instance:ForceStop()
        end 

        if self.equip_show.equiplist[1]:GetData().item_id > 0 then 
            GuideManager.Instance:ForceStop()
        end 

        return self.equip_show.equiplist[1].empty_click
    end)

end 

function MarbasBase:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj_show")
end 

function MarbasBase:OnClickBag()
    ViewMgr:OpenView(MarbasBagView)
end

function MarbasBase:OnClickDemons()
    ViewMgr:OpenView(MarbasDemonsView)
end

function MarbasBase:OnClickTestPickUp()
    MarbasCtrl.Instance:TestSendPickUp()   
    -- body
end

function MarbasBase:OnClickTestPickDown()
    MarbasCtrl.Instance:TestSendPickDown()    
    -- body
end

function MarbasBase:OnClickChangeSuit()
    self.data.base_view_data.suit_show = not self.data.base_view_data.suit_show
end 

-- 
function MarbasBase:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[316].desc})
end

function MarbasBase:SuitFlush()
    local empty = self.data:IsEmptyEquipList()
    -- self.suit_empty:SetActive(empty)
    -- self.title_obj:SetActive(not empty)

    -- self.suit_3_show:SetActive(not empty)
    -- self.suit_5_show:SetActive(not empty)
    -- self.suit_8_show:SetActive(not empty)
    
    UH.SetText(self.suit_simple, self.data.base_view_data.suit_show 
        and Language.Marbas.BaseSuitSimple[MarbasConfig.QuaMaster] 
        or Language.Marbas.BaseSuitSimple[MarbasConfig.QuaEnter])

    local str = self.data:BaseTypeGetMainSuitName()
    UH.SetText(self.suit_title,str) 

    local mod_id = self.data:BaseTypeGetMainModID()

    if self.ui_obj_show == nil then
        self.ui_obj_show = UIObjDrawer.New()
        self.model_part:SetShowData({
            ui_obj = self.ui_obj_show,
            view = self,
            show_shadow = false,
        })
    end
    self.ui_obj_show:SetData(string.format("actor/npc/%s.prefab", MarbasConfig.base_model[mod_id]))

end

function MarbasBase:ShowSuitCombo()
    if self.data.suit_enable.changes == nil then 
        return
    end 
    self.block:SetActive(true)
    local mark = self.data:TypeSuitShowGetInfo()
    for k,v in pairs(mark.cell_list)do
        if self.suit_effs[v.index + 1] ~= nil then 
            self.suit_effs[v.index + 1]:Play("30000826")
        end 
    end 
    
    self.data.base_view_data.suit_show = self.data:GetTypeBaseShouldSuit()
end

function MarbasBase:EquipShowComplete(index)
	if not ViewMgr:IsOpen(MarbasSuitShowView) then 
	    ViewMgr:OpenView(MarbasSuitShowView)
	end  

	for i = 1,8 do 
	    self.suit_effs[i]:AllHandleCancel()
	end 

    self.block:SetActive(false)
end


function MarbasBase:ArrowFlush()
    -- LogError("?ArrowFlush!",self.data.better_equip)
    self.arrow:SetActive(self.data.better_equip.is_better)
end

MarbasSuitShow = MarbasSuitShow or DeclareMono("MarbasSuitShow", UIWFlushPanel)
function MarbasSuitShow:MarbasSuitShow()
    self.data = MarbasData.Instance
    
    self.data_cares = {
        {data = self.data.equip_list,func = self.SuitFlush},
        {data = self.data.base_view_data,func = self.SuitFlush, init_call = false},
    }
end 

function MarbasSuitShow:SuitFlush()
    -- if self.data:IsEmptyEquipList() then return end 
    -- local info = self.data:BaseTypeGetCurSuit(self.suit_num)

    UH.SetText(self.title, info.title)
    for i = 1,self.attr_cells:Length() do 
        self.attr_cells[i].gameObject:SetActive(info.attr_list[i] ~=nil)
        if info.attr_list[i] ~=nil then 
            self.attr_cells[i]:SetData(info.attr_list[i])
        end 
    end 

    if self.sp_eff ~= nil then 
        local str = info.sp_effect ~= nil and info.sp_effect or ""        
        UH.SetText(self.sp_eff, str)
    end 

    self.inter.Interactable = info.active
end 

MarbasTotalSuitShow = MarbasTotalSuitShow or DeclareMono("MarbasTotalSuitShow", UIWFlushPanel)
function MarbasTotalSuitShow:MarbasTotalSuitShow()
    self.data = MarbasData.Instance
    
    self.data_cares = {
        {data = self.data.equip_list,func = self.SuitFlush},
        {data = self.data.base_view_data,func = self.SuitFlush, init_call = false},
    }    

    --
end

function MarbasTotalSuitShow:SuitFlush()
    UH.SetText(self.SelectedText,self.data.base_view_data.suit_show 
    and Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaMaster] 
    or Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaEnter])

    local total_suit = self.data:BaseTypeGetTotalSuit()

    self.suit_list:SetData(total_suit)
end 

function MarbasTotalSuitShow:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(MarbasConfig.base_suit_show)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 


function MarbasTotalSuitShow:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end 

function MarbasTotalSuitShow:OnClickLevelBtn(data)
    self.SelectedText.text = data.flag and Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaMaster] or Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaEnter]
    
    self.data.base_view_data.suit_show = data.flag
    self:OnClickLevelSelect()
end 

MarbasSuitSelectBtnItem = MarbasSuitSelectBtnItem or DeclareMono("MarbasSuitSelectBtnItem", UIWidgetBaseItem)
function MarbasSuitSelectBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name,data.flag and Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaMaster] or Language.Marbas.BaseSuitShowSimple[MarbasConfig.QuaEnter])
    
end

MarbasTotalSuitShowCell = MarbasTotalSuitShowCell or DeclareMono("MarbasTotalSuitShowCell", UIWidgetBaseItem)
function MarbasTotalSuitShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    local status = MarbasData.Instance:CheckSuitActStatus(data)
    self.eff_bg:SetActive(status == 1)
    self.uneff_bg:SetActive(status == 2)
    self.nomal_bg:SetActive(status == 3)
    
    self.eff_tips:SetActive(status == 1)
    self.show_tips.gameObject:SetActive(status >1)
    local equip_num = MarbasData.Instance:GetVigorousLevelCheck(data.level_show)
    local equip_str = data.color == MarbasConfig.QuaEnter and Language.Marbas.FixSelect[1] or Language.Marbas.FixSelect[2]
    UH.SetText(self.show_tips,string.format(Language.Marbas.BaseSuitTotal,equip_num,data.level_show,equip_str))

    UH.SetText(self.name,data.god_print_name)
    local attr_list = {}
    if data.full_potential > 0 then
        local str = data.name1 .." +"..data.potential_add_1.."%"
        table.insert(attr_list,str)
    else
        for i = 1,7 do 
            if data["potential_add_"..i] > 0 then 
                local str = data["name"..i] .." +"..data["potential_add_"..i].."%"
                table.insert(attr_list,str)
            end 
        end 
    end 

    for i = 1,3 do 
        local flag = attr_list[i] ~= nil 
        self["eff_"..i.."_attr"].gameObject:SetActive(flag)
        if flag then UH.SetText(self["eff_"..i.."_attr"],attr_list[i]) end 
    end 

    -- for i = 1,3 do 
    -- end 
    -- local attr_desc = ""
    -- for i = 1,2 do 
    --     local with = data["attr_add_"..i] > 0
    --     if with then 
    --         local attr_str = Language.Common.AttrList[data["attr_type_"..i]].." +"..(
    --             DataHelper.IsPercent(data["attr_type_"..i]) and Percent(data["attr_add_"..i]) or data["attr_add_"..i])
    --         attr_desc = attr_desc .. attr_str ..(i == 2 and "" or "  ")
    --     end 
    -- end 
    -- UH.SetText(self.eff_attr,attr_desc)
    
    -- local sp_effect = ""
    -- local show_sp = false
    -- if data.sp_id > 0 then 
    --     local cfg = Cfg.SpecialEffect(data.sp_id)
    --     sp_effect = Language.ItemInfo.ItemTitles.SuitSp..cfg.description
    --     show_sp = true
    -- elseif data.attr_add_3 > 0 then 
    --     sp_effect = Language.Common.AttrList[data.attr_type_3].."  +"
    --     ..(DataHelper.IsPercent(data.attr_type_3) and Percent(data.attr_add_3) or data.attr_add_3)
    --     show_sp = true
    -- end 

    -- self.eff_desc.gameObject:SetActive(show_sp)
    -- UH.SetText(self.eff_desc,sp_effect)
    self.inter.Interactable = status ~= 2

end 

MarbasBaseAttrCell = MarbasBaseAttrCell or DeclareMono("MarbasBaseAttrCell", UIWidgetBaseItem)
function MarbasBaseAttrCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_type, Language.Common.AttrList[data.attr_type])
    UH.SetText(self.attr_value, "+".. (DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value))
    self.attr_inter.Interactable = data.active
end 

MarbasBaseEquips = MarbasBaseEquips or DeclareMono("MarbasBaseEquips", UIWFlushPanel)
function MarbasBaseEquips:MarbasBaseEquips()
    self.data = MarbasData.Instance
    self.data_cares = {
        {data = self.data.equip_list,func = self.EquipFlush},
        {data = self.data.refine_info,func = self.EquipFlush},
    }
end 

function MarbasBaseEquips:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    self.data.suit_enable.changes = nil
end 

function MarbasBaseEquips:EquipFlush()
    for i = 1,self.equiplist:Length() do 
        if self.data.equip_list[i - 1] ~= nil then 
            self.equiplist[i]:SetData(self.data.equip_list[i - 1])
        else 
            self.equiplist[i]:SetData(Item.New())
        end 
    end 
end

MarbasBaseEquipCell = MarbasBaseEquipCell or DeclareMono("MarbasBaseEquipCell",UIWidgetBaseItem)
function MarbasBaseEquipCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.call:SetData(data)
    self.level_obj:SetActive(data.item_id > 0)
    if data.item_id > 0 then 
        UH.SetText(self.equip_level,Language.Marbas.ItemInfo.LevelShow..data:LevelShow())
    end 

    self.empty_click:SetActive(data.item_id == 0)
end 

function MarbasBaseEquipCell:OnClickEmpty()
    if not MarbasData.Instance:IsWithEquipInBag() then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.BaseEmptyTips)

        local lock_pos = self.root.transform:InverseTransformPoint(self.gameObject.transform.position)
        ViewMgr:OpenView(MarbasGetwayView,{pos = lock_pos})
        return 
    end 

    ViewMgr:OpenView(MarbasBagView)
end