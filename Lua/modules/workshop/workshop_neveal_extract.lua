WorkshopNevealExtractView = WorkshopNevealExtractView or DeclareView("WorkshopNevealExtractView", "workshop/workshop_neveal_extract")
function WorkshopNevealExtractView:WorkshopNevealExtractView()

end

function WorkshopNevealExtractView:LoadCallback()
    self.Board:SetData(self:GetType(),Language.Workshop.Neveal.ExtractView , Vector2.New(1081,507))
end 

function WorkshopNevealExtractView:OnCloseClick()
    ViewMgr:CloseView(WorkshopNevealExtractView)
end

--------------------------WorkshopNevealExtractPanel--------------------------
WorkshopNevealExtractPanel = WorkshopNevealExtractPanel or DeclareMono("WorkshopNevealExtractPanel", UIWFlushPanel)
function WorkshopNevealExtractPanel:WorkshopNevealExtractPanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEnd , init_call = false},
    }
end 

function WorkshopNevealExtractPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

function WorkshopNevealExtractPanel:onFlush()
    self:FlushPanel()    
end

function WorkshopNevealExtractPanel:FlushPanel()
    -- self.equip_name
    -- self.equip_eff
    -- self.equip_set
    -- self.cost_cell
    -- self.cost_num
    -- self.ex_name
    -- self.ex_eff
    -- self.ex_set
    local select_data = WorkshopData.Instance:GetNevealSelect()
    local eff_info = select_data:Neveal()
    UH.SetText(self.equip_name,select_data:QuaName(true))
    self.equip_cell:SetData(select_data)
    local eff_cfg = WorkshopData.Instance:GetNevealCfg(eff_info.effect_id,eff_info.effect_level)
    local skill_cfg = WorkshopData.Instance:GetNevealSkillCfg(eff_info.effect_id)
    UH.SetText(self.equip_eff,Language.Workshop.Neveal.PlusEff..eff_cfg.effect_name..Language.Workshop.Neveal.Lv..eff_info.effect_level)
    UH.SetText(self.equip_set,Language.Workshop.Neveal.SetDesc..skill_cfg.set_skill_description)

    UH.SetText(self.ex_eff,Language.Workshop.Neveal.PlusEff..eff_cfg.effect_name..Language.Workshop.Neveal.Lv..eff_info.effect_level)
    UH.SetText(self.ex_set,Language.Workshop.Neveal.SetDesc..skill_cfg.set_skill_description)

    local item = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].dianhua_id,param = {has_param = 1,effect_id = eff_info.effect_id ,effect_level = eff_info.effect_level}})
    UH.SetText(self.ex_name,ColorStr(item:NameId(),item:ColorStr(true)))
    self.eff_cell:SetData(item)

    local cost = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].xifu_id})
    local cur_num = cost:Num() 
    local color_1 = cur_num >= eff_cfg.xifu_consume and COLORSTR.White or COLORSTR.Red8
    self.cost_cell:SetData(cost)
    UH.SetText(self.cost_num,"<color=#"..color_1..">".. cur_num.."</color>/"..eff_cfg.xifu_consume)
end

function WorkshopNevealExtractPanel:OnExtractClick()
    local select_data = WorkshopData.Instance:GetNevealSelect()
    local eff_info = select_data:Neveal()
    local eff_cfg = WorkshopData.Instance:GetNevealCfg(eff_info.effect_id,eff_info.effect_level)
    local lack_num =  eff_cfg.xifu_consume - Item.GetNum(Config.equipment_dianhua_auto.other[1].xifu_id)

    if lack_num > 0 then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.WithoutXiFu)
        local data = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].xifu_id})
        data.vo.jump_need_num = lack_num
        ViewMgr:OpenView(GetWayView, {item = data})
        return 
    end 

    local equip = WorkshopData.Instance:GetNevealSelect()
    WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_XI_FU,
    equip.in_bag == true and 1 or 0,equip.index)
end

function WorkshopNevealExtractPanel:FlushEnd()
    local item = WorkshopData.Instance:GetNevealExtractItem()

    if item ~= nil then 
        self.block:SetActive(true)  
        self.EffectTool:Play(5161036)
    end 
end

function WorkshopNevealExtractPanel:OnEffectEnd()
    local item = WorkshopData.Instance:GetNevealExtractItem()
    if item ~= nil then 
        ViewMgr:OpenView(GiftOpenView,{item_list = {item},gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS})
        WorkshopData.Instance:SetNevealExtractItem()
    end 
    ViewMgr:CloseView(WorkshopNevealExtractView)
end