-- 幻兽装备
HuanShouEquipView = HuanShouEquipView or DeclareView("HuanShouEquipView", "huanshou/huanshou_equip_view")
function HuanShouEquipView:HuanShouEquipView()
    self.Board:SetData(self:GetType(),Language.HuanShou.Title4,Vector2.New(954, 678))
end


HuanShouEquipPanel = HuanShouEquipPanel or DeclareMono("HuanShouEquipPanel", UIWFlushPanel)
function HuanShouEquipPanel:HuanShouEquipPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuffInfo, init_call = false}
    }
end

function HuanShouEquipPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function HuanShouEquipPanel:FlushPanel()
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local equip_type = self.data.select_equip_type
    local equip = huanshou:GetEquip(equip_type)
    self.equip = equip
    self.huanshou = huanshou
    local equip_item_id = equip:ItemId()
    local item = Item.Create({item_id = equip_item_id})
    self.Equip:SetData(item)
    UH.SetText(self.EquipName, item:Name())
    UH.SetText(self.Level, "Lv." .. equip.level)
    local is_one_attri = equip.level == 0 or equip:IsMax()
    self.OneAttri:SetActive(is_one_attri)
    self.TwoAttri:SetActive(not is_one_attri)
    if is_one_attri then
        self.OneAttriList:SetData(equip:AttriList())
    else
        local cur_attri_list = equip:AttriList()
        self.CurAttriList:SetData(cur_attri_list)
        local next_attri_list= equip:NextAttriList()
        for i,v in pairs(next_attri_list) do
            local attri = cur_attri_list[i]
            if attri.attri_type == v.attri_type then
                v.add_value = v.attri_value - attri.attri_value
            end 
        end
        self.NextAttriList:SetData(next_attri_list)
    end
    self.StuffInfo:SetActive(not equip:IsMax())
    self.MaxFlag:SetActive(equip:IsMax())

    self:FlushStuffInfo()
end

function HuanShouEquipPanel:FlushStuffInfo()
    if not self.equip or self.equip:IsMax() then
        return
    end
    local equip_cfg = self.equip:Cfg()
    local item_id = equip_cfg.item_id
    local need_num = equip_cfg.num
    local has_num = Item.GetNum(item_id)
    local num_str = DataHelper.ConsumeNum(has_num, need_num, true)
    local stuff_item = Item.Create({item_id = item_id})
    self.StuffItem:SetData(stuff_item)
    UH.SetText(self.StuffName, stuff_item:Name())
    UH.SetText(self.StuffNum, num_str)
    UH.SetText(self.BtnText, self.equip.level == 0 and Language.HuanShou.Text23 or Language.HuanShou.Text24)
    self.RedPoint:SetNum(has_num >= need_num and 1 or 0)
end

function HuanShouEquipPanel:OnBtnClick()
    if not self.huanshou or not self.equip then
        return
    end
    local flag = self.equip:CtrlFlag()
    if flag == 0 then
        local equip_cfg = self.equip:Cfg()
        MainOtherCtrl.Instance:GetWayViewById(equip_cfg.item_id)
    else
        if not self.EffectMgr:EffectIng() then
            self.EffectMgr:Play(function ()
                HuanShouCtrl.Instance:SendReq(HuanShouReqType.EquipLevelUp, self.huanshou.id,self.equip:Type())
            end)
        end
    end
end

function HuanShouEquipPanel:OnTipClick()
    -- local tip_id = self.data.select_equip_type == 0 and 411 or 412
    -- PublicPopupCtrl.Instance:HelpTip(tip_id)
    local param = {}
    param.title = Language.HuanShou.Text25
    param.content = self:GetAttriPreviewDesc(self.data.select_equip_type)
    ViewMgr:OpenView(HuanShouEquipAttriPreview, param)
end

function HuanShouEquipPanel:GetAttriPreviewDesc(equip_type)
    local level = self.equip.level
    local cfg = Config.huanshou_auto.huanshou_equip
    local group_cfg = DataHelper.ConvertGroup(cfg, "type")
    local desc = ""
    local attri_info1
    local attri_info2
    local str
    for i,v in pairs(Config.huanshou_auto.huanshou_equip) do
        if v.level > 0 and v.type == equip_type then
            attri_info1 = CommonStruct.AttrStruct(v.att_type_1, v.attr_value_1)
            attri_info2 = CommonStruct.AttrStruct(v.att_type_2, v.attr_value_2)
            attri_info3 = CommonStruct.AttrStruct(v.att_type_3, v.attr_value_3)
            str = string.format(Language.HuanShou.Text26, v.level, attri_info1.attr_name, attri_info1.attr_percent_value, 
            attri_info2.attr_name, attri_info2.attr_percent_value,attri_info3.attr_name, attri_info3.attr_percent_value)
            if v.level == level then
                str = ColorStr(str, COLORSTR.Yellow1)
            end
            desc = desc .. str .. "\n"
        end
    end
    return desc
end



HuanShouEquipAddAttriItem = HuanShouEquipAddAttriItem or DeclareMono("HuanShouEquipAddAttriItem", UIWidgetBaseItem)
function HuanShouEquipAddAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local name = attri_info.attr_name
    local value_str = attri_info.attr_percent_value
    UH.SetText(self.Name, name)
    UH.SetText(self.Value, value_str)
    
    local add_value = data.add_value or 0
    attri_info = CommonStruct.AttrStruct(data.attri_type, add_value)
    UH.SetText(self.AddValue, "(" .. attri_info.attr_percent_value .. ")")
end


HuanShouEquipEffectMgr = HuanShouEquipEffectMgr or DeclareMono("HuanShouEquipEffectMgr")
function HuanShouEquipEffectMgr:Awake()
    self.s_pos = self.Point:GetLocalPosition()
    self.e_pos = self.EPos:GetLocalPosition()
    self.effect_move = ShengQiMoveEffect.New()
    self.effect_ing = false
end

function HuanShouEquipEffectMgr:EffectIng()
    return self.effect_ing
end

function HuanShouEquipEffectMgr:Play(end_func)
    if self.effect_ing then
        return
    end
    self.effect_ing = true
    self.end_func = end_func
    self.EffectTool:Play("2161037")
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        local pos_list = {}
        pos_list[1] = self.s_pos
        pos_list[3] = self.e_pos
        pos_list[2] = pos_list[1] + Vector3.New(-100,0,0)
        self.effect_move:Play(pos_list, self.Point, function ()
            self.effect_ing = false
            self.EffectTool:Play("2161038")
            if self.end_func then
                self.end_func()
            end
        end)
    end,1)
end

function HuanShouEquipEffectMgr:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
    self.effect_move:Destroy()
end

function HuanShouEquipEffectMgr:EffectEndEvent()

end

HuanShouEquipAttriPreview = HuanShouEquipAttriPreview or DeclareView("HuanShouEquipAttriPreview", "huanshou/huanshou_equip_attri_preview")

function HuanShouEquipAttriPreview:LoadCallback(param_t)
    local title = param_t.title or ""
    local content = param_t.content or ""
    UH.SetText(self.Title, title)
    UH.SetText(self.Content, content)
end

function HuanShouEquipAttriPreview:OnCloseClick()
    ViewMgr:CloseView(HuanShouEquipAttriPreview)
end
 

