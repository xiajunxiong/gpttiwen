-- 幻兽属性标签
HuanShouAttriPanel = HuanShouAttriPanel or DeclareMono("HuanShouAttriPanel", UIWFlushPanel)
function HuanShouAttriPanel:HuanShouAttriPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        --{data = self.data.bag_item_list, func = self.FlushStuff, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false}
    }
end

function HuanShouAttriPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.Tip, Language.HuanShou.Text3)

    self.move_effect_list = {}
    for i = 1,self.EffectList:Length() do
        local effect = self.EffectList[i]
        if effect then
            self.move_effect_list[i] = ShengQiMoveEffect.New()
            self.move_effect_list[i]:SetEffectObj(effect)
            self.move_effect_list[i]:SetStartPos(effect:GetLocalPosition())
        end
    end

    self:FlushPanel()
end

function HuanShouAttriPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i,v in pairs(self.move_effect_list or {}) do
        v:Destroy()
    end
end

function HuanShouAttriPanel:FlushPanel()
    local huanshou = self.data:GetSelHuanShou()
    self.Info:SetActive(not TableIsEmpty(huanshou))
    if TableIsEmpty(huanshou) then
        return
    end

    local is_active = huanshou:IsActive()
    local is_max = huanshou:IsMaxLevel()
    
    self.JianTou:SetActive(is_active and not is_max)
    self.NotActive:SetActive(not is_active)
    self.MaxLevel:SetObjActive(is_max)
    UH.SetInter(self.JinHuaBtnInter, not is_max)

    if is_active then
        local level = huanshou:Level()
        if is_max then
            UH.SetText(self.MaxLevel, string.format(Language.HuanShou.Text1, level, COLORSTR.Green10))
        else
            UH.SetText(self.CurLevel, string.format("Lv.%s", level))
            UH.SetText(self.NextLevel, string.format("Lv.%s", level + 1))
        end
    end

    self.AttriList:SetData(huanshou:ShowAddAttriList())

    local cur_skill = huanshou:ExSkill()
    local next_skill = huanshou:NextSkill()
    self.CurSkill:SetData(cur_skill or {})
    if is_max or not is_active then
        self.NextSkill:SetData({})
    else
        self.NextSkill:SetData(next_skill or {})
    end
    self.NotSkillObj:SetActive(TableIsEmpty(cur_skill) and TableIsEmpty(next_skill))

    self.StuffTextObj:SetActive(not is_max and is_active)

    self.GetBtn:SetActive(not is_active)
    self.JinHuaBtn:SetActive(is_active)

    self:FlushStuff()

    self.RedPoint:SetNum(self.data:IsCanUpLevel(huanshou))

    local equip_list = huanshou:EquipList()
    for i = 1, self.EquipList:Length() do
        local equip = equip_list[i - 1]
        if equip then
            self.EquipList[i]:SetData(equip)
        end
    end
end

function HuanShouAttriPanel:FlushStuff()
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local stuff_list = huanshou:StuffList()
    local item_list = {}
    for i,v in pairs(stuff_list) do
        local need_num = v.num
        local num = Item.GetNum(v.item_id)
        local item = Item.Create({item_id = v.item_id, num = DataHelper.ConsumeNum(num, need_num, true)})
        table.insert(item_list, item)
    end
    self.StuffList:SetData(item_list)
end

function HuanShouAttriPanel:OnUpClick()
    if self.effect_ing then
        return
    end
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local flag,item_id= self.data:IsCanUpLevel(huanshou)
    if flag == 1 then
        self:OnUpEvent(huanshou)
        --self.data:UpHuanShou(huanshou)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotActive)
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.MaxLevelTip)
    end
end

function HuanShouAttriPanel:OnGetClick()
    ViewMgr:OpenViewByKey(Mod.HuanShou.Egg)
end

function HuanShouAttriPanel:OnUpEvent(huanshou)
    if TableIsEmpty(self.move_effect_list) then
        return
    end
    if TableIsEmpty(huanshou) then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PetStrength)
    self.effect_ing = true
    self.effect_end_pos = self.EndEffect:GetLocalPosition()
    for i,move_effect in pairs(self.move_effect_list) do
        local pos_list = {}
        local s_pos = move_effect:GetStartPos()
        pos_list[1] = s_pos
        pos_list[3] = self.effect_end_pos
        if i == #self.move_effect_list then
            pos_list[2] = pos_list[1] + Vector3.New(100,200,0)
            move_effect:Play(pos_list, nil, function ()
                self.effect_ing = false
                self.data:UpHuanShou(huanshou)
            end)
        else
            pos_list[2] = pos_list[1] + Vector3.New(-100,200,0)
            move_effect:Play(pos_list)
        end
    end
end


HuanShouAttriPanelAttriItem = HuanShouAttriPanelAttriItem or DeclareMono("HuanShouAttriPanelAttriItem",UIWidgetBaseItem)
function HuanShouAttriPanelAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_active = data.is_active
    local is_max = data.is_max
    
    self.Img:SetActive(is_active and not is_max)
    self.Value:SetObjActive(is_active and not is_max)
    self.Lock:SetActive(not is_active)

    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.Name, attri_info.attr_name)
    if is_max or not is_active then
        local value_str = ColorStr("+" .. attri_info.attr_percent_value, COLORSTR.Blue13)
        UH.SetText(self.NextValue, value_str)
    else
        local next_attri_info = CommonStruct.AttrStruct(data.attri_type, data.next_value)
        UH.SetText(self.NextValue, "+" .. next_attri_info.attr_percent_value)
        UH.SetText(self.Value, "+" .. attri_info.attr_percent_value)
    end
end


HuanShouAttriPanelSkillItem = HuanShouAttriPanelSkillItem or DeclareMono("HuanShouAttriPanelSkillItem")
function HuanShouAttriPanelSkillItem:SetData(data)
    if TableIsEmpty(data) then
        self.gameObject:SetActive(false)
        return
    end
    self.gameObject:SetActive(true)
    if self.Inter then
        --UH.SetInter(self.Inter, data.is_lock ~= true)
        UH.SetInter(self.Inter, true)
    end
    if self.Lock then
        self.Lock:SetActive(data.is_lock == true)
    end
    self.SkillCell:SetData(data)
    UH.SetText(self.SkillDesc, data:Desc())
end




-- ==== 进化成功界面 ===============
HuanShouAttriUpSucc = HuanShouAttriUpSucc or DeclareView("HuanShouAttriUpSucc", "huanshou/huan_shou_up_succ")
VIEW_DECLARE_MASK(HuanShouAttriUpSucc,ViewMask.BgBlockClose)
function HuanShouAttriUpSucc:HuanShouAttriUpSucc()
    self.data = HuanShouData.Instance
end

function HuanShouAttriUpSucc:LoadCallback(param_t)
    local huanshou = param_t.huanshou
    local level = huanshou:Level()
    UH.SetText(self.CurLevel, "Lv." .. level)
    UH.SetText(self.NextLevel, "Lv." .. (level + 1))
    local show_attri_list = huanshou:ShowAddAttriList()
    self.AttriList:SetData(show_attri_list)
    local next_ex_skill = huanshou:NextSkill()
    self.Skill:SetActive(next_ex_skill ~= nil)
    self.NotTip:SetActive(next_ex_skill == nil)
    if next_ex_skill ~= nil then
        self.SkillCell:SetData(next_ex_skill)
        UH.SetText(self.Desc, next_ex_skill:Desc())
    end
end


HuanShouAttriUpSuccAttriItem = HuanShouAttriUpSuccAttriItem or DeclareMono("HuanShouAttriUpSuccAttriItem", UIWidgetBaseItem)
function HuanShouAttriUpSuccAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cur_attri = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local next_attri = CommonStruct.AttrStruct(data.attri_type, data.next_value)
    local add_attri = CommonStruct.AttrStruct(data.attri_type, data.next_value - data.attri_value)
    UH.SetText(self.Name, cur_attri.attr_name)
    UH.SetText(self.Value, cur_attri.attr_percent_value)
    local add_str = ColorStr("(+" .. add_attri.attr_percent_value .. ")", COLORSTR.Green10)
    UH.SetText(self.NextValue, string.format("%s%s", next_attri.attr_percent_value, add_str))
end


-- 幻兽装备孔
HuanShouEquipHole = HuanShouEquipHole or DeclareMono("HuanShouEquipHole", UIWidgetBaseItem)
-- data === HuanShouEquip
function HuanShouEquipHole:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local level = data:Level()
    self.AddObj:SetActive(level == 0)
    self.ItemCell:SetObjActive(level > 0)
    self.LevelBg:SetActive(level > 0)
    self.Effect:SetActive(level > 0)
    if data:Level() > 0 then
        local item = Item.Create({item_id = data:ItemId()})
        self.ItemCell:SetData(item)
        UH.SetText(self.Level, "Lv." .. level)
    end
    local flag = data:CtrlFlag()
    self.RedPoint:SetNum(flag)
end

function HuanShouEquipHole:OnClick()
    if not self.data:IsActive() then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.Tip9)
        return
    end
    HuanShouData.Instance.select_equip_type = self.data:Type()
    ViewMgr:OpenView(HuanShouEquipView)
end