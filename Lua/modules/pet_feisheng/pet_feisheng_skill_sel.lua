

PetFeiShengSkillSel = PetFeiShengSkillSel or DeclareView("PetFeiShengSkillSel", "pet/pet_feisheng_skill_sel")
VIEW_DECLARE_LEVEL(PetFeiShengSucc,ViewLevel.L1)

function PetFeiShengSkillSel:PetFeiShengSkillSel()
    PetFeiShengData.Instance:SetSelectSkill(0)
end

function PetFeiShengSkillSel:LoadCallback()
    PetFeiShengData.Instance:CleanSeledSkillIdx()
end

----------------------------------------------------------------------------
PetFeiShengSkillSelPanel = PetFeiShengSkillSelPanel or DeclareMono("PetFeiShengSkillSelPanel", UIWFlushPanel)
--SelectedObjs======GameObject[]
--CardDropAnims=====Animation[]
--ClickBlock========GameObject



local function changeNeedCurrenyNum()
    return Config.pet_cfg_auto.other[1].change_expend_coin
end

local function skillIdx2Id(skill_idx)
    return Config.pet_cfg_auto.other[1]["fly_skill_id" .. tostring(skill_idx)]
end

local function onSkillSelected(skill_idx)
    local pet_idx
    if PetFeiShengData.Instance:IsChangeSkillSel() then
        --检查是否是宠物身上一样的技能
        local cur_pet = PetData.Instance:GetCurSelect()
        if cur_pet == nil then
            Debuger.LogError("PetFeiShengSkillSelDetails:OnClickComfirm Selected pet is NULL")
            ViewMgr:CloseView(PetFeiShengSkillSel)
            return 
        end
        pet_idx = cur_pet:Index()
        if cur_pet:FeiShengSkillId() == skillIdx2Id(skill_idx) then
            PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.SkillChangeSameErr)
            return
        end
        --先检查钱够不
        if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, changeNeedCurrenyNum()) then
            return
        end
        --请求更换技能
        PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.SkillChangeSucc)
        -- ViewMgr:CloseView(PetFeiShengSkillSel)
    else
        pet_idx = PetFeiShengData.Instance:SuccIdx()
    end
    PetFeiShengData.Instance:SetSelectSkill(skill_idx)
    --ViewMgr:CloseView(PetFeiShengSkillSel)
    if PetFeiShengData.Instance:IsChangeSkillSel() then
        PetFeiShengCtrl.Instance:RequestFeiShengSkillChange(pet_idx,skill_idx)
    else
        PetFeiShengCtrl.Instance:RequestFeiShengSkillSelect(pet_idx,skill_idx)
    end
end

function PetFeiShengSkillSelPanel:PetFeiShengSkillSelPanel()
    self.data_cares = {
        {data = PetFeiShengData.Instance:SuccData(), func = self.OnSelectedCard, keys = {"seled_skill_idx"},init_call = false},
    }
    self.wait_anim_runner = nil
    if PetFeiShengData.Instance:IsChangeSkillSel() then
        self.CostObj:SetActive(true)
        self.Tips:SetActive(false)
        UH.SetText(self.CostNum,tostring(changeNeedCurrenyNum()))
    else
        self.CostObj:SetActive(false)
        self.Tips:SetActive(true)
    end
end

function PetFeiShengSkillSelPanel:OnClickCard(skill_idx)
    -- PetFeiShengData.Instance:SetSelectSkill(skill_idx)  
    -- PetFeiShengCtrl.Instance:RequestFeiShengSkillSelect(PetFeiShengData.Instance:SuccIdx(),skill_idx)
    -- ViewMgr:CloseView(PetFeiShengSkillSel)

    -- onSkillSelected(skill_idx)
    local cur_seled_idx = skill_idx
    for i=1,self.SelectedObjs:Length() do
        self.SelectedObjs[i]:SetActive(i == cur_seled_idx)
    end
end

function PetFeiShengSkillSelPanel:OnClickComfirm()
    local cur_seled_idx = 0
    for i=1,self.SelectedObjs:Length() do
        if self.SelectedObjs[i].activeSelf then
            cur_seled_idx = i
            break
        end
    end
    if cur_seled_idx == 0 then
        PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.SkillSelectHint)
        return
    end
    onSkillSelected(cur_seled_idx)
end

local drop_anim_name = "pet_feisheng_skill_sel_Dorpcard"

function PetFeiShengSkillSelPanel:OnSelectedCard()
    local cur_seled_idx = PetFeiShengData.Instance:SuccSeledSkillIdx()
    -- for i=1,self.SelectedObjs:Length() do
    --     self.SelectedObjs[i]:SetActive(i == cur_seled_idx)
    -- end
    local anim = nil
    for i=1,self.CardDropAnims:Length() do
        if i ~= cur_seled_idx then
            self.CardDropAnims[i]:Play(drop_anim_name)
            if anim == nil then
                anim = self.CardDropAnims[i]
            end
        end
    end
    self.ClickBlock:SetActive(true)
    self:cleanWaitAnimRunner()
    self.wait_anim_runner = Runner.Instance:RunUntilTrue(function() 
        if anim == nil or anim:get_Item(drop_anim_name).normalizedTime >= 1 then
            ViewMgr:CloseView(PetFeiShengSkillSel)
            self.wait_anim_runner = nil
            return true
        else
            return false
        end
        
    end)
end

function PetFeiShengSkillSelPanel:cleanWaitAnimRunner()
    if self.wait_anim_runner ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.wait_anim_runner)
        self.wait_anim_runner = nil
    end
end

function PetFeiShengSkillSelPanel:OnDestroy()
    self:cleanWaitAnimRunner()
    UIWFlushPanel.OnDestroy(self)
end

function PetFeiShengSkillSelPanel:OnClickMask()
    if PetFeiShengData.Instance:IsChangeSkillSel() then
        self.ClickBlock:SetActive(true)
        ViewMgr:CloseView(PetFeiShengSkillSel)
    end
end

--------------------------------------------
PetFeiShengSkillSelDetails = PetFeiShengSkillSelDetails or DeclareMono("PetFeiShengSkillSelDetails", UIWFlushPanel)
--SkillIdx===Int
--Icon=======Image
--Name=======Text
--Desc=======Text
--ChangeCostObj====GameObject
--ChangeCostNum====Text
--Attri1===========Text
--Attri2===========Text
--Lv===============Text
function PetFeiShengSkillSelDetails:Start()
    local skill_id = skillIdx2Id(self.SkillIdx)
    -- local skill_cfg = Cfg.SkillPassive(skill_id)
    local skill_cfg = SkillData.GetDecConfig(skill_id)
    -- local eff_cfg = Cfg.SpecialEffect(skill_cfg.sp_id)
    UH.SetIcon(self.Icon,skill_cfg.icon_id,ICON_TYPE.SKILL)
    -- LogError("ICon====",eff_cfg.icon_id,skill_cfg,eff_cfg,"|||",new_sc,"||")
    UH.SetText(self.Name,skill_cfg.skill_name)
    UH.Color(self.Name,QualityColorShallow[ItemColor.None])
    
    local desc = string.gsub(skill_cfg.description,"02CF7A",COLORSTR.Green9)
    UH.SetText(self.Desc,desc)
    -- if PetFeiShengData.Instance:IsChangeSkillSel() then
    --     self.ChangeCostObj:SetActive(true)
    --     UH.SetText(self.ChangeCostNum,tostring(changeNeedCurrenyNum()))
    -- else
    --     self.ChangeCostObj:SetActive(false)
    -- end
    local cur_pet = PetFeiShengData.Instance:IsChangeSkillSel() and PetData.Instance:GetCurSelect() or 
        PetData.Instance:GetPetInfo(PetFeiShengData.Instance:SuccIdx()) or PetData.Instance:GetCurSelect()
    local skill_lv = cur_pet:FeiShengSkillLv()
    UH.SetText(self.Lv,string.format(Language.PetFeiSheng.SkillSelectLv,skill_lv))
    local fs_attri_show_max = not PetFeiShengData.Instance:IsChangeSkillSel()
    local cfg = PetFeiShengData.SkillLvupCfg(skill_id,skill_lv)
    local format_str = "%s  +%s"
    local attri1_txt = string.format(format_str,Language.Common.AttrList[cfg.att_type1],DataHelper.Percent(cfg.att_type1, cfg.att_num1))
    local attri2_txt = string.format(format_str,Language.Common.AttrList[cfg.att_type2],DataHelper.Percent(cfg.att_type2, cfg.att_num2))
    if fs_attri_show_max then
        local max_cfg = PetFeiShengData.SkillLvupCfg(skill_id,PetFeiShengData.Instance:SkillMaxLv())
        attri1_txt = string.format(Language.PetFeiSheng.SkillSelectMaxAttriFormat,attri1_txt,DataHelper.Percent(max_cfg.att_type1, max_cfg.att_num1))
        attri2_txt = string.format(Language.PetFeiSheng.SkillSelectMaxAttriFormat,attri2_txt,DataHelper.Percent(max_cfg.att_type2, max_cfg.att_num2))
    end
    UH.SetText(self.Attri1,attri1_txt)
    UH.SetText(self.Attri2,attri2_txt)
end


function PetFeiShengSkillSelDetails:OnClickComfirm()
    -- local pet_idx
    -- if PetFeiShengData.Instance:IsChangeSkillSel() then
    --     --检查是否是宠物身上一样的技能
    --     local cur_pet = PetData.Instance:GetCurSelect()
    --     if cur_pet == nil then
    --         Debuger.LogError("PetFeiShengSkillSelDetails:OnClickComfirm Selected pet is NULL")
    --         ViewMgr:CloseView(PetFeiShengSkillSel)
    --         return 
    --     end
    --     pet_idx = cur_pet:Index()
    --     if cur_pet:FeiShengSkillId() == self:MySkillId() then
    --         PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.SkillChangeSameErr)
    --         return
    --     end
    --     --先检查钱够不
    --     if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self:changeNeedCurrenyNum()) then
    --         return
    --     end
    --     --请求更换技能
    --     PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.SkillChangeSucc)
    --     ViewMgr:CloseView(PetFeiShengSkillSel)
    -- else
    --     pet_idx = PetFeiShengData.Instance:SuccIdx()
    -- end
    -- PetFeiShengData.Instance:SetSelectSkill(self.SkillIdx)
    -- ViewMgr:CloseView(PetFeiShengSkillSel)
    -- PetFeiShengCtrl.Instance:RequestFeiShengSkillSelect(pet_idx,self.SkillIdx)
    onSkillSelected(self.SkillIdx)
end