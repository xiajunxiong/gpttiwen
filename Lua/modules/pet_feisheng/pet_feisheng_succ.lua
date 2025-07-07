PetFeiShengSucc = PetFeiShengSucc or DeclareView("PetFeiShengSucc", "pet/pet_feisheng_succ")
VIEW_DECLARE_LEVEL(PetFeiShengSucc,ViewLevel.L1)

function PetFeiShengSucc:PetFeiShengSucc()

end


PetFeiShengSuccPanel = PetFeiShengSuccPanel or DeclareMono("PetFeiShengSuccPanel", UIWFlushPanel)
--ModelShow======ModelUIShow
--PetNameText====Text
--SwitchAnim=====Animator
--AttrTxt1======Text
--AttrTxt2======Text
--UnlockFunc====Text
--SkillLayout===GameObject
--SkillList=====UIWidgetList[PetFeiShengSuccSkillItem]
--TitleAttr=====Text
--TitleSkill====Text
--TitleFunc=====Text
--TitleSprite===UIImageSpriteSet

local max_attri_count = 4

function PetFeiShengSuccPanel:PetFeiShengSuccPanel()
    self.ui_obj = nil
    self.model_pos_offset_runner = nil
    self.wait_skillsel_close = nil
    self.cur_pet = PetData.Instance:GetPetInfo(PetFeiShengData.Instance:SuccIdx())
    self.model_move_target_pos = nil
    self.model_move_start_pos = nil
    UH.SetText(self.PetNameText, self.cur_pet:Name())
    local fs_lv = PetFeiShengData.Instance:SuccTimes()
    --设置属性加成
    local pet_id = self.cur_pet:NormalId()
    local attr_cfg = Cfg.PetFeiShengAttri(pet_id,fs_lv)
    for i=1,max_attri_count do
        local i_str = tostring(i)
        local attr_type = attr_cfg["att_type" .. i_str]
        if attr_type >= 0 then
            local attr_num = attr_cfg["att_num" .. i_str]
            UH.SetText(self["AttrTxt" .. i_str], string.format("%s +%s",Language.Common.AttrList[attr_type],DataHelper.Percent(attr_type,attr_num)))
        else
            UH.SetText(self["AttrTxt" .. i_str],"")
        end
    end
    UH.SetText(self.UnlockFunc,Language.PetFeiSheng["SuccUnlock" .. fs_lv])
    UH.SetText(self.TitleAttr,PetFeiSheng.LvNameFormatStr(fs_lv,Language.PetFeiSheng.SuccTitleAttr))
    UH.SetText(self.TitleSkill,PetFeiSheng.LvNameFormatStr(fs_lv,Language.PetFeiSheng.SuccTitleSkill))
    UH.SetText(self.TitleFunc,PetFeiSheng.LvNameFormatStr(fs_lv,Language.PetFeiSheng.SuccTitleFunc))
    UH.SpriteName(self.TitleSprite,"_LocTitle" .. tostring(fs_lv))
end

local model_move_offset = -4.5
local show_time = 2.18
function PetFeiShengSuccPanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
    end
    self.ModelShow:SetAdaptiveOffSet(Vector3.New(0,model_move_offset,0))
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.cur_pet:ResId()),self.cur_pet:FeiShengTimes())
    self.ModelShow:SetShowData({
        ui_obj = self.ui_obj,
        view = self,
        show_shadow = true,
        shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
    })
    self.SkillLayout:SetActive(PetFeiShengData.Instance:SuccTimes() == PetFeiShengData.Lv.One)
end

function PetFeiShengSuccPanel:OnEffectEvent(handle, time, key)
    if handle.effId == 3165038 then
        if key == "show" then
            AudioMgr:PlayEffect(AudioType.UI,CommonAudio.JinJie2)
            self:UnlistenModelRunner()
            local begin_time = Time.time
            self.model_pos_offset_runner = Runner.Instance:RunUntilTrue(function()
                local cross_time = Time.time - begin_time
                local per = cross_time/show_time
                if per >= 1 then
                    self.ModelShow:SetAdaptiveOffSet(Vector3.zero)
                    self.model_pos_offset_runner = nil
                    return true
                else
                    local off_y = model_move_offset * (1-per)
                    self.ModelShow:SetAdaptiveOffSet(Vector3.New(0,off_y,0))
                    return false
                end
            end)
        elseif key == "end" then
            if PetFeiShengData.Instance:SuccTimes() == PetFeiShengData.Lv.One then
                PetFeiShengData.Instance:MarkIsChangeSkill(false)
                PetFeiShengData.Instance:SetSelectSkill(0)
                ViewMgr:OpenView(PetFeiShengSkillSel)
                self:CleanWaiting()
                self.wait_skillsel_close = Runner.Instance:RunUntilTrue(function()
                    if PetFeiShengData.Instance:SuccSeledSkillIdx() ~= 0 and ViewMgr:IsClosed(PetFeiShengSkillSel) == true then
                        self:flushSkillList()
                        self:ShowAttriAnim()
                        self.wait_skillsel_close = nil
                        return true
                    end
                    return false
                end)
            else
                PetFeiShengCtrl.Instance:RequestFeiShengSkillSelect(self.cur_pet:Index(),0)
                self:ShowAttriAnim()
            end
        end
    end
end

function PetFeiShengSuccPanel:flushSkillList()
    local list_data = {}
    for i = 1,PetFeiShengData.SKILL_COUNT do
        local skill_id = Config.pet_cfg_auto.other[1]["fly_skill_id" .. tostring(i)]
        local skill_cfg = SkillData.GetDecConfig(skill_id)
        if i == PetFeiShengData.Instance:SuccSeledSkillIdx() then
            table.insert(list_data,1,skill_cfg)
        else
            table.insert(list_data,skill_cfg)
        end
    end
    self.SkillList:SetData(list_data)
end

function PetFeiShengSuccPanel:ShowAttriAnim()
    self.SwitchAnim:SetTrigger(APH("attri"))
end

function PetFeiShengSuccPanel:CleanWaiting()
    if self.wait_skillsel_close ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.wait_skillsel_close)
        self.wait_skillsel_close = nil
    end
end

function PetFeiShengSuccPanel:UnlistenModelRunner()
    if self.model_pos_offset_runner ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.model_pos_offset_runner)
        self.model_pos_offset_runner = nil
    end
end

function PetFeiShengSuccPanel:OnDestroy()
    self.ModelShow:SetAdaptiveOffSet(Vector3.zero)
    self:CleanWaiting()
    self:UnlistenModelRunner()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    UIWFlushPanel.OnDestroy(self)
end

function PetFeiShengSuccPanel:OnClickCloseBlock()
    ViewMgr:CloseView(PetFeiShengSucc)
    ViewMgr:CloseView(PetFeiSheng)
end



----------------------PetFeiShengSuccSkillItem-----------------------------------

PetFeiShengSuccSkillItem = PetFeiShengSuccSkillItem or DeclareMono("PetFeiShengSuccSkillItem", UIWidgetBaseItem)
--Icon========Image
--Name========Text
--Desc========Text
--Attri1======Text
--Attri2======Text

--data:skill_desc_cfg
function PetFeiShengSuccSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetIcon(self.Icon,self.data.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Name,self.data.skill_name)
    UH.SetText(self.Desc,self.data.description)
    local lvup_cfg = PetFeiShengData.SkillLvupCfg(self.data.skill_id,1)
    local attri_format = "%s<color=#"..COLORSTR.Green9..">+%s</color>"
    UH.SetText(self.Attri1,string.format(attri_format,Language.Common.AttrList[lvup_cfg.att_type1],DataHelper.Percent(lvup_cfg.att_type1, lvup_cfg.att_num1)))
    UH.SetText(self.Attri2,string.format(attri_format,Language.Common.AttrList[lvup_cfg.att_type2],DataHelper.Percent(lvup_cfg.att_type2, lvup_cfg.att_num2)))
end