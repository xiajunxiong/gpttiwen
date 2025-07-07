WorkshopSmeltSmeltPlus = WorkshopSmeltSmeltPlus or DeclareView("WorkshopSmeltSmeltPlus", "workshop/workshop_smelt_smelt_plus")
function WorkshopSmeltSmeltPlus:WorkshopSmeltSmeltPlus()
	self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
end 

function WorkshopSmeltSmeltPlus:LoadCallback(param_t)
    self.panel:SetData(param_t)
end

function WorkshopSmeltSmeltPlus:OnClickClose()
    ViewMgr:CloseView(WorkshopSmeltSmeltPlus)
end

WorkshopSmeltSmeltPlusPanel = WorkshopSmeltSmeltPlusPanel or DeclareMono("WorkshopSmeltSmeltPlusPanel", UIWFlushPanel)
function WorkshopSmeltSmeltPlusPanel:WorkshopSmeltSmeltPlusPanel()
    self.workshop_data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_voc_list, func = self.FlushEquip, init_call = false},
        {data = self.workshop_data.smelt_select, func = self.FlushEquip, init_call = false},
    }
    self.send_data = {}

    self.fly_effect:SetData({mono = self,end_func = function()
        self.EffectTool:Play(2161012,self.fly_effect.EndPos)
        self.EffectTool:Play(2161038,self.fly_effect.EndPos)
    end})
end 

-- 注入标记数据，根据标记数据铺设静态信息
function WorkshopSmeltSmeltPlusPanel:SetData(data)
    self.mark_data = data    
    
    -- self.EquipShow:SetData(self.workshop_data:GetSmeltSelect())
   
    self:FlushStaticInfo()
    self:FlushEquip()
end

function WorkshopSmeltSmeltPlusPanel:FlushStaticInfo()

    local list = self.workshop_data:GetSmeltAttrAccRange(self.workshop_data:GetSmeltSelect().item_id)
    local qua_cfg = list[self.mark_data.info.color]
    local max_qua = self:GetQuaMax()

    self.show_cfg = nil
    for k,v in pairs(qua_cfg.children) do 
        if v.att_type == self.mark_data.info.attr_type then 
            self.show_cfg = v
        end 
    end 

    self.comsume = self.workshop_data.que_ching_comsume_list[self.mark_data.info.color]
    self.add_cfg = self.workshop_data.que_ching_add_list[self.mark_data.info.color][self.mark_data.info.attr_type]
    self.CostShow:SetData(Item.Create({item_id = self.comsume.item_id}))
    UH.SetText(self.attr_type, Language.Common.AttrList[self.mark_data.info.attr_type])

    self.flag_cur_max = self.show_cfg.max == self.mark_data.info.attr_value
    self.is_red = (self.mark_data.info.color == ItemColor.God or self.mark_data.info.color == ItemColor.Red) 
       or (self.mark_data.info.color == ItemColor.Orange and self.flag_cur_max and (max_qua == ItemColor.Red or max_qua == ItemColor.God))

    self.is_orange = (self.mark_data.info.color == ItemColor.Orange and max_qua >= ItemColor.Orange) or (self.mark_data.info.color == ItemColor.Purple and self.flag_cur_max and max_qua >= ItemColor.Orange)

    self.next_show_cfg = nil
    for k,v in pairs(list) do 
        for i,j in pairs(v.children) do 
            if j.att_type == self.mark_data.info.attr_type and j.max > self.show_cfg.max and j.quality <= max_qua then 
                self.next_show_cfg = j
                break
            end 
        end 
        if self.next_show_cfg ~= nil then break end 
    end 

    local show_next = self.flag_cur_max and self.next_show_cfg ~= nil  
    UH.SetText(self.max_limit, Language.Workshop.SmeltPlusLimit..(DataHelper.IsPercent(self.mark_data.info.attr_type) 
        and Percent(show_next and self.next_show_cfg.max or self.show_cfg.max) or (show_next and self.next_show_cfg.max or self.show_cfg.max)))

    self.purple_prog_obj:SetActive(not self.is_orange and not self.is_red)
    self.orange_prog_obj:SetActive(self.is_orange)
    self.red_prog_obj:SetActive(self.is_red)
    
    -- UH.SetText(self.desc_tips,string.format(Language.Workshop.SmeltPlusQuaDesc,Language.Workshop.SmeltPreColorStr[self.mark_data.info.color])) 

    local equip = self.workshop_data:GetSmeltSelect()
    UH.SetText(self.desc_tips,string.format(Language.Workshop.SmeltPlusQuaPlusDesc,
        ColorStr(Language.Workshop.SmeltPreOnlyColorStr[equip:Color()],QualityColorStrShallow[equip:Color()]),
        ColorStr(Language.Workshop.SmeltPreOnlyColorStr[max_qua],QualityColorStrShallow[max_qua]))) 


    local is_precent = DataHelper.IsPercent(self.mark_data.info.attr_type)
    UH.SetText(self.one_tips,string.format( Language.Workshop.SmeltPlusOnes,is_precent and Percent(self.add_cfg.once_add) or self.add_cfg.once_add))
end

-- 刷新动态信息
function WorkshopSmeltSmeltPlusPanel:FlushEquip()
    self.EquipShow:SetData(self.workshop_data:GetSmeltSelect())

    local operate_data = self.workshop_data:GetSmeltSelectAttr()[self.mark_data.info.index]
    if operate_data ~= self.mark_data then
        self.mark_data = operate_data
    end 

    self:FlushStaticInfo()
    
    local prof_type = RoleData.Instance:ProfType()
    local is_precent = DataHelper.IsPercent(self.mark_data.info.attr_type)
    local attr_value = is_precent and Percent(self.mark_data.info.attr_value,nil,true) or self.mark_data.info.attr_value
    local attr_max = is_precent and Percent(self.show_cfg.max,nil,true) or self.show_cfg.max
    local attr_next_max = self.next_show_cfg ~= nil and (is_precent and  Percent(self.next_show_cfg.max,nil,true) or self.next_show_cfg.max) or attr_max

    local cur_point = self.workshop_data:CatchSmeltAttrScore(self.workshop_data:GetSmeltSelectAttr(),self.workshop_data:GetSmeltSelect():IsEquip())

    UH.SetText(self.point_show , string.format(Language.Workshop.SmeltAttrScore,math.floor(cur_point)))

    local operate = self.is_red and self.red_prog or (self.is_orange and self.orange_prog or self.purple_prog)

    if self.flag_cur_max and self.next_show_cfg ~= nil then 
        operate:SetProgress(self.mark_data.info.attr_value /self.next_show_cfg.max)
        operate:SetText((is_precent and (attr_value.."%") or attr_value).."/"..(is_precent and (attr_next_max.."%") or attr_next_max))
    else
        operate:SetProgress(self.mark_data.info.attr_value /self.show_cfg.max)
        operate:SetText((is_precent and (attr_value.."%") or attr_value).."/"..(is_precent and (attr_max.."%") or attr_max))
    end 

    local cur_num = Item.GetNum(self.comsume.item_id)
    local enough = cur_num >= self.comsume.consume_num
    local color = enough and COLORSTR.White or COLORSTR.Red8
    UH.SetText(self.item_num,"<color=#"..color..">".. cur_num.."</color>/"..self.comsume.consume_num)

    self.btn_inter.Interactable = cur_num >= self.comsume.consume_num

    if self.block.activeSelf then 
        self.block:SetActive(false)
    end 
end

function WorkshopSmeltSmeltPlusPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

function WorkshopSmeltSmeltPlusPanel:OnClicktips()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral2)
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[157].desc})
end

function WorkshopSmeltSmeltPlusPanel:OnClickCheck()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral2)
    ViewMgr:OpenView(SmeltPreRangeView, self.workshop_data:GetSmeltSelect())
end

function WorkshopSmeltSmeltPlusPanel:GetQuaMax()
    local list = self.workshop_data:GetSmeltAttrAccRange(self.workshop_data:GetSmeltSelect().item_id)
    for k,v in pairs(list) do 
        for i,j in pairs(v.children) do 
            if j.att_type == self.mark_data.info.attr_type and j.max == self.mark_data.max then 
                return k
            end  
        end 
    end 
    return 0
end

function WorkshopSmeltSmeltPlusPanel:CheckIsMax()
    return self.mark_data.info.attr_value >= self.mark_data.max --self.show_cfg.max
end

function WorkshopSmeltSmeltPlusPanel:EffectComplete()
    local param_t = {}
    param_t.change_voc_type = Item.GetBigType(self.workshop_data:GetSmeltSelect().item_id) == ITEM_BIG_TYPE.EQUIPMENT and 2 or 3
    param_t.role_type = AnotherData.Instance:CheckEquipmentUniqueId(self.workshop_data:GetSmeltSelect(),self.workshop_data:GetSmeltSelect():IsEquip()) and 0 or 1
    param_t.replacer_posi = self.workshop_data:GetSmeltSelect().in_bag and 1 or 0
    param_t.replacer_grid = self.workshop_data:GetSmeltSelect().index 
    param_t.replacer_voc_index = self.mark_data.info.index -1

    param_t.bereplacer_posi = 0
    param_t.bereplacer_grid = 0
    param_t.bereplacer_voc_index = 0

    WorkshopCtrl.Instance:ChangeEquipVocReq(param_t)

    self.fly_effect:AllHandleCancel()
end

function WorkshopSmeltSmeltPlusPanel:OnClickSmelt()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral2)
    local cur_num = Item.GetNum(self.comsume.item_id)

    if self:CheckIsMax() then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltPlusComplete)
        return 
    end 

    if cur_num <self.comsume.consume_num then 
        local lack = self.comsume.consume_num - cur_num
        PublicPopupCtrl.Instance:Center(string.format(ErrorText[5],  Item.GetName(self.comsume.item_id),lack))

        ItemInfoCtrl.Instance:ItemInfoView({item_list = {Item.Create({item_id = self.comsume.item_id})},show_ways = true})
        return 
    end 

    self.block:SetActive(true)

    self.fly_effect:Play(2161011)
end