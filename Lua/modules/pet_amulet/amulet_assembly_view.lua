--护符合成
AmuletAssemblyView = AmuletAssemblyView or DeclareView("AmuletAssemblyView","pet_amulet/AmuletAssemblyView")

function AmuletAssemblyView:AmuletAssemblyView()
    self.Board:SetData(self:GetType(),Language.Pet.PetEquip.Assembly, Vector2.New(1188, 600))
    self.data = AmuletData.Instance
end
function AmuletAssemblyView:LoadCallback(param_t)
    local list_data = self.data:GetAssemblyList()
    self.List:SetData(list_data,true)
    if param_t.item_id and param_t.item_id > 0 then
        local item = param_t
        for i,v in pairs(list_data[1].children) do
            if v.item_id == item.item_id then
                self.List:ClickChild(v)
                self.data:SetSelectedAssemblyData(v)
                self.time_ht = Invoke(function ()
                    AmuletData.Instance:SetAssemblyData(item,1)
                end,0.1)
                break
            end
        end
    end
end

function AmuletAssemblyView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
end

function AmuletAssemblyView:OnClickItem(data)
    self.data:SetSelectedAssemblyData(data)
end


AmuletAssemblyPanel = AmuletAssemblyPanel or DeclareMono("AmuletAssemblyPanel",UIWFlushPanel)

function AmuletAssemblyPanel:AmuletAssemblyPanel()
    self.data = AmuletData.Instance
    self.data_cares = {
        {data = self.data.SelectedAssemblyData,func = self.FlushView,init_call = false},
        {data = self.data.AssemblyData,func = self.FlushCell,init_call = false},
        {data = WorkshopData.Instance.complete_item,func = self.FlushView,init_call = false}
    }
end

function AmuletAssemblyPanel:FlushView()
    self.data:SetAssemblyData({},1)
    self.data:SetAssemblyData({},2)
    local result = WorkshopData.Instance.complete_item.item
    self.ResultCell.gameObject:SetActive(result ~= nil and next(result) ~= nil)
    if result and next(result) ~= nil then
        self.ResultCell.gameObject:SetActive(false)
        local result_item=Item.Create(result)
        if self.last_tab_name and self.last_tab_name == self.data.SelectedAssemblyData.name then
            local effect_id = self.t_item_id == result_item.item_id and "3165206" or "3165207"
            self.EffectTool:Play(effect_id)
            TimeHelper:CancelTimer(self.time_ht)
            self.time_ht = Invoke(function ()
                ViewMgr:OpenView(AmuletAssemblyInfoView,{item = result})
                self.ResultCell.gameObject:SetActive(true)
                self.ResultCell:SetData(result_item or {})
            end,3)
        end
    end
    --self.ResultCell.gameObject:SetActive(result ~= nil and next(result) ~= nil)
    self:FlushShow()
    if self.last_tab_name ==nil or self.last_tab_name ~= self.data.SelectedAssemblyData.name then
        self.ResultCell.gameObject:SetActive(false)
        self.last_tab_name = self.data.SelectedAssemblyData.name
    end
end

function AmuletAssemblyPanel:FlushCell()
    local flag1 = next(self.data.AssemblyData[1]) ~= nil
    local flag2 = next(self.data.AssemblyData[2]) ~= nil
    self.Cell1.gameObject:SetActive(flag1)
    self.Cell2.gameObject:SetActive(flag2)
    self.Re1:SetActive(flag1)
    self.Re2:SetActive(flag2)
    self.Cell1:SetData(self.data.AssemblyData[1] or {})
    self.Cell2:SetData(self.data.AssemblyData[2] or {})
    self:FlushShow()
end

function AmuletAssemblyPanel:FlushShow()
    local data = self.data:GetSelectedAssemblyData()
    local stuff_cfg = self.data:GetAssemblyStuffCfg(data.sub_type)
    local stuff_id_1 = stuff_cfg.stuff_id_1
    local stuff_id_2 = stuff_cfg.stuff_id_2
    if stuff_id_1 == stuff_id_2 then
        local stuff_str = Item.GetQuaName(stuff_id_1,true)
        local num = self.data:GetSelectedAssemblyNum(stuff_id_1)
        local need_num = stuff_cfg.stuff_num_1+stuff_cfg.stuff_num_2
        local color = num<need_num and COLORSTR.Red8 or COLORSTR.Red13
        stuff_str = stuff_str.."：("..string.format(Language.Common.Cstr,color,num,need_num)..")"
        UH.SetText(self.StuffName1,stuff_str)
        self.StuffName2.gameObject:SetActive(false)
    else
        self.StuffName2.gameObject:SetActive(true)
        local stuff_str1 = Item.GetQuaName(stuff_id_1,true)
        local num = self.data:GetSelectedAssemblyNum(stuff_id_1)
        local need_num = stuff_cfg.stuff_num_1
        local color = num<need_num and COLORSTR.Red8 or COLORSTR.Red13
        stuff_str1 = stuff_str1.."：("..string.format(Language.Common.Cstr,color,num,need_num)..")"
        UH.SetText(self.StuffName1,stuff_str1)

        local stuff_str2 = Item.GetQuaName(stuff_id_2,true)
        num = self.data:GetSelectedAssemblyNum(stuff_id_2)
        need_num = stuff_cfg.stuff_num_2
        color = num<need_num and COLORSTR.Red8 or COLORSTR.Red13
        stuff_str2 = stuff_str2.."：("..string.format(Language.Common.Cstr,color,num,need_num)..")"
        UH.SetText(self.StuffName2,stuff_str2)
    end

    local cfg = self.data:GetShowExplainCfg(data.sub_type)
    self.ACell.gameObject:SetActive(cfg[1]~=nil)
    self.BCell.gameObject:SetActive(cfg[2]~=nil)
    if cfg[1] then
        UH.SetText(self.AProbabilityText,string.format(Language.Pet.PetEquip.Probability,(cfg[1].show_probability*100)).."%")
        UH.SetText(self.ACellName,Item.GetQuaName(cfg[1].target,true))
        self.ACell:SetData(Item.Create({item_id=cfg[1].target}))
        self.t_item_id = cfg[1].target      -- 合成目标
    end
    if cfg[2] then
        UH.SetText(self.BProbabilityText,string.format(Language.Pet.PetEquip.Probability,(cfg[2].show_probability*100)).."%")
        UH.SetText(self.BCellName,Item.GetQuaName(cfg[2].target,true))
        self.BCell:SetData(Item.Create({item_id=cfg[2].target}))
        self.t_item_id = cfg[2].target      -- 合成目标
    end   
end

function AmuletAssemblyPanel:OnAssemblyClick()
    local data = self.data:GetSelectedAssemblyData()
    local seq = self.data:GetAssemblyStuffCfg(data.sub_type).seq
    local items = self.data:GetAssemblyData()
    if items[1].item_id == nil or items[2].item_id == nil then
        return
    end
    local index_1=items[1].index
    if (items[1].param.pet_index or -1) >=0 then
        index_1=self.data:GetItemIndexByItemIdAndSkill(items[1])
    end
    local index_2=items[2].index
    if (items[2].param.pet_index or -1) >=0 then
        index_2=self.data:GetItemIndexByItemIdAndSkill(items[2])
    end
    if self.data:IsNeedTip(items[1]) or self.data:IsNeedTip(items[2]) then
        PublicPopupCtrl.Instance:AlertTip(Language.Pet.PetEquip.AlertTip, function()
            AmuletCtrl.Instance:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_COMPOSE,seq,index_1,index_2)
        end)
    else
        AmuletCtrl.Instance:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_COMPOSE,seq,index_1,index_2)
    end
end

function AmuletAssemblyPanel:OnAddClick(index)
    --判断是否有物品可选
    local data = self.data:GetSelectedAssemblyData()
    local stuff_cfg = self.data:GetAssemblyStuffCfg(data.sub_type)
    local stuff_id = stuff_cfg["stuff_id_"..index]
    local added_num = self.data:GetSelectedAssemblyNum(stuff_id)
    local bag_num = Item.GetNum(stuff_id)
    if bag_num <= added_num then
        PublicPopupCtrl.Instance:Center(Language.Pet.PetEquip.NotEquipTip)
        return
    end
    self.data.assembly_pos = index
    PetData.Instance.pet_equip_select.equip = 3
    PetData.Instance.pet_equipment_select_another_open = false
    ViewMgr:OpenView(PetEquipSelectView)
    self.ResultCell.gameObject:SetActive(false)
end

function AmuletAssemblyPanel:OnRemoveClick(index)
    self.data:SetAssemblyData({},index)
end

function AmuletAssemblyPanel:OnDestroy()
    WorkshopData.Instance.complete_item.item = nil
    self.data:SetSelectedAssemblyData({})
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

--==========AmuletAssemblyChildItem
AmuletAssemblyChildItem = AmuletAssemblyChildItem or DeclareMono("AmuletAssemblyChildItem", UIWidgetBaseItem)
function AmuletAssemblyChildItem:AmuletAssemblyChildItem()
   
end

function AmuletAssemblyChildItem:SetData(data)
    if self.handle == nil then
        self.handle = BagData.Instance:Care(BindTool.Bind(self.FlushNum, self))
    end
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self,data)
    self:FlushNum()
end

function AmuletAssemblyChildItem:FlushNum()
    if self.data == nil then
        return
    end
    local cfg = AmuletData.Instance:GetAssemblyStuffCfg(self.data.sub_type)
    if cfg == nil then
        LogError("宠物护符合成界面报错 > self.data.sub_type:",self.data.sub_type)
        return
    end
    local stuff_cfg1 = Item.GetConfig(cfg.stuff_id_1)
    local stuff_cfg2 = Item.GetConfig(cfg.stuff_id_2)
    local item_id = stuff_cfg2.name == self.data.name and cfg.stuff_id_2 or cfg.stuff_id_1
    local num = Item.GetNum(item_id)
    if num>0 then
        UH.SetText(self.NumText,string.format(Language.Pet.PetEquip.Remain,num))
    else
        UH.SetText(self.NumText,Language.Pet.PetEquip.NotRemain)
    end
end

function AmuletAssemblyChildItem:OnDestroy()
    BagData.Instance:UnCare(self.handle)
    UIWFlushPanel.OnDestroy(self)
end

function AmuletAssemblyChildItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

AmuletAssemblyInfoView = AmuletAssemblyInfoView or DeclareView("AmuletAssemblyInfoView","pet_amulet/AmuletAssemblyInfo")
VIEW_DECLARE_MASK(AmuletAssemblyInfoView,ViewMask.BgBlockClose)

-- param_t = {item = {}}
function AmuletAssemblyInfoView:LoadCallback(param_t)
    TimeHelper:CancelTimer(self.handle_time)
    self.handle_time = TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(AmuletAssemblyInfoView)
    end, 5)
    param_t = param_t or {}
    if param_t.item == nil then
        return
    end
    local item = param_t.item
    local item_obj = Item.Create(item)
    local skill_list = {}
    self.Cell:SetData(item_obj)
    UH.SetText(self.CellName,item_obj:QuaName())
    for i = 1,item.param.equipment_skill_num or MAX_PET_EQUIPMENT_SKILL_NUM do
        local skill_id = item.param.equipment_skill_list[i]
        if skill_id > 0 then
            table.insert(skill_list,{skill_id = skill_id})
        end
    end
    self.SkillList:SetData(skill_list)
end

function AmuletAssemblyInfoView:CloseCallback()
    TimeHelper:CancelTimer(self.handle_time)
end

AmuletAssemblyInfoSkillItem = AmuletAssemblyInfoSkillItem or DeclareMono("AmuletAssemblyInfoSkillItem",UIWidgetBaseItem)

function AmuletAssemblyInfoSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local level = AmuletData.Instance:GetIntensifySkillLevel(data.skill_id)
    local skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id, level)
    local name = skill_cfg.name
    local qua = skill_cfg.page
    if level and level > 0 then
        name = string.format(Language.Pet.PetEquip.LevelShow,name, level)
        qua = 3
    end
    local skill_name = string.format(Language.PetEquipSelect.SkillNameColor, Language.PetEquipSelect.SkillColorD[qua], name)
    UH.SetText(self.SkillName,skill_name)
    UH.SetText(self.Desc,skill_cfg.desc_1 or skill_cfg.desc_2)
end