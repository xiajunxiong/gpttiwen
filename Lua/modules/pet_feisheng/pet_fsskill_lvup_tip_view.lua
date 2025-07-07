PetFSSkillLvupTipView = PetFSSkillLvupTipView or DeclareView("PetFSSkillLvupTipView", "pet/pet_fsskill_lvup_tip")
-- VIEW_DECLARE_LEVEL(PetFeiShengSucc,ViewLevel.L1)

function PetFSSkillLvupTipView:PetFSSkillLvupTipView()

end

PetFSSkillLvupTipPanel = PetFSSkillLvupTipPanel or DeclareMono("PetFSSkillLvupTipPanel", UIWFlushPanel)
--Name===========Text
--LvList=========UIWidgetList[PetFSSkillLvupTipItem]
--LvupBlock======GameObject
--CostItem=======UIWSubstitute[UIWItemCell]
--CostItemName===Text
--CostItemNum====Text
--EffectTool=====UIEffectTool
--LvupObj========GameObject
--ListRect=======GameObject


function PetFSSkillLvupTipPanel:PetFSSkillLvupTipPanel()
    self.orginal_size = self.ListRect.sizeDelta
    -- self.data_cares = {
	-- 	{data = PetFeiShengData.Instance:ViewData(), func = self.FlushSelf,keys = {"flush_lvup_tip"}},
    -- }
    self.cost_item_num = 0
    self:FlushSelf()
end

function PetFSSkillLvupTipPanel:FlushSelf(item_num_off)
    if item_num_off == nil then
        item_num_off = 0
    end
    local skill_data = PetData.Instance:GetClickSkill()
    -- LogError("skill_data===",skill_data)
    UH.SetText(self.Name,skill_data.skill_cfg.skill_name)
    -- local owner_pet = skill_data.owner_pet
    -- if owner_pet == nil then
    --     owner_pet = skill_data.pet_idx ~= nil and PetData.Instance:GetPetInfo(skill_data.pet_idx) or nil
    -- end
    local cur_skill_lv = skill_data.skill_level
    local max_lv = cur_skill_lv >= PetFeiShengData.Instance:SkillMaxLv()
    self.LvupObj:SetActive(not max_lv)
    local list_size = self.orginal_size:Clone()
    if max_lv then
        list_size.y = 525
    end
    self.ListRect.sizeDelta = list_size
    local list_data = {}
    for lv=1,PetFeiShengData.Instance:SkillMaxLv() do
        -- LogError("===",lv,skill_data.skill_id)
        local cfg = PetFeiShengData.SkillLvupCfg(skill_data.skill_id,lv,false)
        if cfg == nil then
            break
        end
        if cur_skill_lv == lv then
            cfg.is_cur_lv = true
            if not max_lv then  --设置升级消耗
                local cost_item = Item.Init(cfg.strengthen_item)
                self.cost_item_num = cfg.strengthen_num
                self.CostItem:SetData(cost_item)
                UH.SetText(self.CostItemName,cost_item:QuaName(false))
                local cur_num = BagData.Instance:GetNumByItemId(cfg.strengthen_item)
                UH.SetText(self.CostItemNum,
                    string.format(Language.PetSkillLvupTip.CostMat,DataHelper.ConsumeNum(cur_num + item_num_off,cfg.strengthen_num,true)))
            end
        else
            cfg.is_cur_lv = false
        end
        table.insert(list_data,cfg)
    end
    self.LvList:SetData(list_data)
    self.LvList:AutoSlide(list_data[skill_data.skill_level],false)
    self.cur_skill = skill_data
end

function PetFSSkillLvupTipPanel:OnClickLvup()
    -- LogError("OnClickLvup",self.cur_skill.pet_idx,self.cur_skill)
    local lvup_info = PetFeiShengData.SkillLvupCfg(self.cur_skill.skill_id,self.cur_skill.skill_level)
    local cur_num = BagData.Instance:GetNumByItemId(lvup_info.strengthen_item)
    -- LogError(lvup_info.strengthen_num ,cur_num)
    if lvup_info.strengthen_num > cur_num then
        PublicPopupCtrl.Instance:Center(Language.PetSkillLvupTip.NoEnoughMat)
        return
    end

    -- ViewMgr:CloseView(PetFSSkillLvupTipView)
    PetFeiShengCtrl.Instance:RequestFeiShengSkillLvup(self.cur_skill.pet_idx)
    ViewMgr:OpenView(PetFSSkillLvupRetView,{skill_id = self.cur_skill.skill_id,level = self.cur_skill.skill_level})

    local cur_sel_skill = PetData.Instance:GetClickSkill()
    if cur_sel_skill ~= nil then
        cur_sel_skill.skill_level = cur_sel_skill.skill_level + 1
    end
    self:FlushSelf(-self.cost_item_num)
    -- PublicPopupCtrl.Instance:Center(Language.PetSkillLvupTip.LvupSucc)
end

function PetFSSkillLvupTipPanel:OnClickMask()
    ViewMgr:CloseView(PetFSSkillLvupTipView)
end


----------------------PetFSSkillLvupTipItem-----------------------------------

PetFSSkillLvupTipItem = PetFSSkillLvupTipItem or DeclareMono("PetFSSkillLvupTipItem", UIWidgetBaseItem)
--LblDesc========Text
--Attri1========Text
--Attri2========Text
--Icon==========UIImageSpriteSet
--Lv============Text
--NormalBg======GameObject
--CurBg=========GameObject

--data:pet_cfg_auto.fly_skill_strengthen
function PetFSSkillLvupTipItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local lv = self.data.levels + 1
    local desc_cfg = PetData.Instance:GetGiftDetail(self.data.skill_id,lv)
    UH.SetIcon(self.Icon,desc_cfg.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Lv,string.format("lv.%d",lv))
    self.NormalBg:SetActive(not data.is_cur_lv)
    self.CurBg:SetActive(data.is_cur_lv)
    local format_str = "%s <color=#"..COLORSTR.Green3..">+%s</color>"
    UH.SetText(self.Attri1, string.format(format_str,Language.Common.AttrList[self.data.att_type1],DataHelper.Percent(self.data.att_type1,self.data.att_num1)))
    UH.SetText(self.Attri2, string.format(format_str,Language.Common.AttrList[self.data.att_type2],DataHelper.Percent(self.data.att_type2,self.data.att_num2)))
    UH.SetText(self.LblDesc,string.format(Language.PetFeiSheng.SkillLvupDesc,desc_cfg.description))
end