PetSkillLvupTip = PetSkillLvupTip or DeclareView("PetSkillLvupTip", "pet/pet_skill_lvup_tip")
VIEW_DECLARE_MASK(PetSkillLvupTip, ViewMask.BlockClose)

function PetSkillLvupTip:LoadCallback(param)
    -- AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    -- if param ~= nil and param.pos ~= nil  then
    --     UH.AnchoredPosition(self.PosRect, param.pos)
    -- end 
end

PetSkillLvupTipPanel = PetSkillLvupTipPanel or DeclareMono("PetSkillLvupTipPanel", UIWFlushPanel)
--SkillIcon======Image
--Name===========Text
--Lv=============Text
--MaxLv==========Text
--CurEffTxt======Text
--NextEffTxt=====Text
--CostItemCell===UIWSubstitute[UIWItemCell]
--CostName=======Text
--CostNum========Text
--Split2Obj======GameObject
--LvupObj========GameObject
--NextEffObj=====GameObject
--SkillLvPrevObj==GameObject
--LvPreviewList===UIWidgetList[PetSkillLvupTipPreviewItem]
--LvupStarEff=====FlyStarEffectCell
--EffectTool======UIEffectTool
--StarEndScaler===GameObject
--EffLayout=======VerticalLayoutGroup
--LvupBlock=======GameObject

local begin_effid = "3165012"
local star_effid = "3165007"
local end_effid = "3165017"
local eff_orginal_height = 266--242 + 8*3 + 5
function PetSkillLvupTipPanel:PetSkillLvupTipPanel()
    self.orginal_end_eff_sc = self.StarEndScaler:GetLocalScale()
    self.LvupStarEff:SetData({
        mono = self,
        start_effect = begin_effid,
        complete_effect = end_effid,
        pre_pause = 0.1,
        end_func = BindTool.Bind(self.OnFlyStartPlayComplete,self),
        -- run_func = 0.1,
        -- start_wait_time = space_t * (i - 1),
        end_func_time = 0,
    })

    self.preview_panel_inited = false
   if self:flushInfo() then
        return
   end
end


function PetSkillLvupTipPanel:OnClickPreview()
    if not self.preview_panel_inited then
        self.preview_panel_inited = true
        local max_lv = PetData.Instance:GetGiftMaxLv(self.cur_data.skill_id)
        local lv_cfgs = {}
        for i=1,max_lv do 
            lv_cfgs[i] = {skill_id = self.cur_data.skill_id,skill_level = i}
        end
        self.LvPreviewList:SetData(lv_cfgs)
        self.LvPreviewList:AutoSlide(lv_cfgs[self.cur_data.skill_level],false)
    end
    self.SkillLvPrevObj:SetActive(not self.SkillLvPrevObj.activeSelf)
end


function PetSkillLvupTipPanel:OnClickLvup()
    local cur_sel_pet = PetData.Instance:GetCurSelect()
    if Pet.CanLvupGift(cur_sel_pet:FeiShengTimes())  == false then
        PublicPopupCtrl.Instance:Center(Language.PetSkillLvupTip.NeedFS)
        return
    end

    local lvup_info = PetData.Instance:GetGiftLvupCfg(self.cur_data.skill_id,self.cur_data.skill_level)
    local cur_num = BagData.Instance:GetNumByItemId(lvup_info.intensify_item_id)
    if lvup_info.item_num > cur_num then
        PublicPopupCtrl.Instance:Center(Language.PetSkillLvupTip.NoEnoughMat)
        return
    end
    local cur_sel_pet = PetData.Instance:GetCurSelect()
    local is_yao_skill = self.cur_data.is_yao_skill == true and 1 or 0
    PetFeiShengCtrl.Instance:RequestLvupPassiveSkill(cur_sel_pet:Index(),self.cur_data.index, is_yao_skill)    
    PublicPopupCtrl.Instance:Center(Language.PetSkillLvupTip.LvupSucc)
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ProgressAdd)
    local cur_height = self.EffLayout.preferredHeight
    local end_eff_sc = self.orginal_end_eff_sc:Clone()
    end_eff_sc.y = cur_height / eff_orginal_height
    self.StarEndScaler:SetLocalScale(end_eff_sc)
    self.LvupStarEff:Play(star_effid)
    self.LvupBlock:SetActive(true)
end

function PetSkillLvupTipPanel:OnFlyStartPlayComplete()
    self.LvupStarEff:AllHandleCancel()
end

function PetSkillLvupTipPanel:OnEffectEvent(handle, time, key)
    if key == "flush" and handle.effId == tonumber(end_effid) then
        local cur_pet = PetData.Instance:GetCurSelect()
        local cur_gift_data = cur_pet:GetGiftSkillDataByIndex(self.cur_data.index, self.cur_data.is_yao_skill)
        if cur_gift_data ~= nil then
            PetData.Instance:SetClickSkill(cur_gift_data)
            self:flushInfo()
        end
    end
    self.LvupBlock:SetActive(false)
end


function PetSkillLvupTipPanel:flushInfo()
    self.cur_data = PetData.Instance:GetClickSkill()
    if self.cur_data == nil then
        LogError("PetSkillLvupTipPanel GetClickSkill data is nil")
        return false
    end
    local sk_lv = self.cur_data.skill_level
    local max_lv = PetData.Instance:GetGiftMaxLv(self.cur_data.skill_id)
    local show_next_lv = max_lv > sk_lv
    self.NextEffObj:SetActive(show_next_lv)
    self.Split2Obj:SetActive(show_next_lv)
    local sk_cfg = PetData.Instance:GetGiftDetail(self.cur_data.skill_id,sk_lv)
    UH.SetText(self.CurEffTxt,sk_cfg.desc_1 or sk_cfg.description_2)
    UH.SetIcon(self.SkillIcon,sk_cfg.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Name,sk_cfg.name or sk_cfg.skill_name)
    UH.SetText(self.Lv,string.format(Language.PetSkillLvupTip.CurLv,sk_lv))
    UH.SetText(self.MaxLv,string.format(Language.PetSkillLvupTip.MaxLv,max_lv))
    if show_next_lv then
        local next_sk_cfg = PetData.Instance:GetGiftDetail(self.cur_data.skill_id,sk_lv + 1)
        local desc = next_sk_cfg.desc_1 or next_sk_cfg.description_2
        UH.SetText(self.NextEffTxt,desc)
    end

    local cur_sel_pet = PetData.Instance:GetCurSelect()
    local show_lvup_obj = show_next_lv and cur_sel_pet ~= nil and 
        self.cur_data.index ~= nil and self.cur_data.index > 0
    self.LvupObj:SetActive(show_lvup_obj)
    if show_lvup_obj then
        local lvup_info = PetData.Instance:GetGiftLvupCfg(self.cur_data.skill_id,self.cur_data.skill_level)
        local cost_item = Item.Init(lvup_info.intensify_item_id)
        self.CostItemCell:SetData(cost_item)
        UH.SetText(self.CostName,cost_item:QuaName(false))
        local cur_num = BagData.Instance:GetNumByItemId(lvup_info.intensify_item_id)
        UH.SetText(self.CostNum,
            string.format(Language.PetSkillLvupTip.CostMat,DataHelper.ConsumeNum(cur_num,lvup_info.item_num,true)))
    end
    return true
end


-------------------------------------PetSkillLvupTipPreviewItem-----------------

PetSkillLvupTipPreviewItem = PetSkillLvupTipPreviewItem or DeclareMono("PetSkillLvupTipPreviewItem", UIWidgetBaseItem)
--LblDesc============Text
--NameLevelText========Text

function PetSkillLvupTipPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local sk_cfg = PetData.Instance:GetGiftDetail(self.data.skill_id,self.data.skill_level)
    if sk_cfg then
        UH.SetText(self.NameLevelText,string.format(Language.PetSkillLvupTip.PreviewName,sk_cfg.name or sk_cfg.skill_name,self.data.skill_level))
        UH.SetText(self.LblDesc,sk_cfg.desc_1 or sk_cfg.description_2)
    end
end