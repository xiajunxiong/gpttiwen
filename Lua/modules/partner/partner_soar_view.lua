
PartnerSoarView = PartnerSoarView or DeclareView("PartnerSoarView", PartnerConfig.ResPath .. "partner_soar")
VIEW_DECLARE_LEVEL(PartnerSoarView, ViewLevel.L1)
function PartnerSoarView:PartnerSoarView()
end

function PartnerSoarView:OnClickClose()
    ViewMgr:CloseView(PartnerSoarView)
end

PartnerSoarViewF = PartnerSoarViewF or DeclareMono("PartnerSoarViewF", UIWFlushPanel)

function PartnerSoarViewF:PartnerSoarViewF()
	self.data = PartnerCtrl.Instance:Data()
    self.task_data = TaskData.Instance
    self.language = Language.Partner.Soar

	self.data_cares = {
        {data = self.data.item_flush, func = self.FlushShow},
        {data = self.task_data.task_list, func = self.FlushShow, init_call = false},
    }
end

function PartnerSoarViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushModel()
end

function PartnerSoarViewF:FlushShow()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo

    local soar_count = vo.soar_count or 0
    local co = self.data:GetPartnerSoarCoByCount(soar_count + 1)
    local quality_name = Language.Partner.QualityName[co.upgrade_condition]
    local segment_name = string.format(Language.Partner.QualitySeg, DataHelper.GetDaXie(co.segment_condition))
   
    self.FuYuObj:SetActive(soar_count == 0)
    UH.SetText(self.FuYuDesc, Format(Language.FuYu.Unlock, info.name))

    self.TitleShow.text = self.language.TitleShow[soar_count] or ""

    self.Condition1Desc.text = string.format(self.language.ConditionDesc1, co.soar_level)
    self.Condition2Desc.text = string.format(self.language.ConditionDesc2, RichTextHelper.ColorStr(string.format("%s·%s", quality_name, segment_name), PartnerQualityColor[co.upgrade_condition + PartnerConfig.QuaFetter.Grassy]))
    local finish1 = data:Level() >= co.soar_level
    self.Condition1Finish:SetActive(finish1)
    self.Gou1:SetActive(finish1)

    UH.SetText(self.TiaoJian1, string.format(self.language.ConditionDesc4, finish1 and COLORSTR.Green3 or COLORSTR.Yellow11, co.soar_level))
    UH.SpriteName(self.TiaoJianSp1, finish1 and "TiaoJianZhuangShiYi" or "TiaoJianZhuangShiWei")
    local condition2_finish_show = false
    if data.quality and not data.quality.init then
        condition2_finish_show = (data.quality.operate_qua > co.upgrade_condition) or ((data.quality.operate_qua == co.upgrade_condition) and (data.quality.quality_segment >= co.segment_condition))
    end
    local finish2 = condition2_finish_show
    self.Condition2Finish:SetActive(finish2)
    self.Gou2:SetActive(finish2)
    UH.SetText(self.TiaoJian2, string.format(self.language.ConditionDesc5, finish2 and COLORSTR.Green3 or COLORSTR.Yellow11, 
        RichTextHelper.ColorStr(string.format("%s·%s", quality_name, segment_name), PartnerQualityColor[co.upgrade_condition + PartnerConfig.QuaFetter.Grassy])))
    UH.SpriteName(self.TiaoJianSp2, finish2 and "TiaoJianZhuangShiYi" or "TiaoJianZhuangShiWei")

    if 0 == soar_count then
        local is_finish = self.task_data:GetTaskIsCompleted(info.soar_fin_task)
        local task_co = Config.partner_fs_tasklist_auto.task_list[info.soar_task]
        if task_co then
            self.Condition3Desc.text = string.format(self.language.ConditionDesc3, task_co.task_name)
            UH.SetText(self.TiaoJian3, string.format(self.language.ConditionDesc6, is_finish and COLORSTR.Green3 or COLORSTR.Yellow11, task_co.task_name))
        end
        self.Condition3Finish:SetActive(is_finish)

        UH.SpriteName(self.TiaoJianSp3, is_finish and "TiaoJianZhuangShiYi" or "TiaoJianZhuangShiWei")
        if not is_finish then
            local task = self.task_data:GetTaskInfoByTaskType(TaskConfig.TaskType.partner_soar)
            local show_go = task and TaskConfig.ListType.accepted == task.list_type and data.id == task:PatnerFSId()
            self.Condition3Accpet:SetActive(not show_go)
            self.Condition3Go:SetActive(show_go)
            self.Condition3Accpet1:SetActive(not show_go)
            self.Condition3Go1:SetActive(show_go)
            self.Gou3:SetActive(not show_go)
        else
            self.Condition3Accpet:SetActive(false)
            self.Gou3:SetActive(true)
            self.Condition3Go:SetActive(false)
            
            self.Condition3Accpet1:SetActive(false)
            self.Condition3Go1:SetActive(false)
        end
    else
        UH.SetText(self.TiaoJian3, "")
        self.Gou3:SetActive(false)
        self.Condition3Accpet1:SetActive(false)
        self.Condition3Go1:SetActive(false)
    end
    self.TiaoJianSp3:SetObjActive(0 == soar_count)

    self.Condition3Obj:SetActive(0 == soar_count)
    self.Line2Obj:SetActive(0 == soar_count)

    self.InfoAttrTitle.text = string.format(self.language.InfoAttrTitle, info.name, info.arms_name)
    self.InfoDescTitle.text = string.format(self.language.InfoDescTitle, info.name)
    
    local is_active = vo.soar_weapon_level > 0
    local soar_arms_co, attrs = data:GetPartnerSoarArmsCo(is_active and vo.soar_weapon_level or 1, true, true)
    local skill_co = Cfg.SkillDescPartnerAndRole(soar_arms_co.skill_id, soar_arms_co.skill_level)

    -- local soar_arms_co, attrs = data:GetPartnerSoarArmsCo(0 == soar_count and 1 or 11, true)
    -- local skill_co = Cfg.SkillDescPartnerAndRole(soar_arms_co.skill_id, soar_arms_co.skill_level)

    self.AttrList:SetData(attrs)
    UH.SetIcon(self.SkillIcon, skill_co.icon_id, ICON_TYPE.SKILL)
    self.SkillName.text = skill_co.skill_name
    self.SkillDesc.text = skill_co.description_2

    local quality_name2 = Language.Partner.QualityName[co.quality_upgrade]
    local segment_name2 = string.format(Language.Partner.QualitySeg, DataHelper.GetDaXie(co.quality_segment))

    self.DescShow1.text = string.format(self.language.DescShow1, co.upgrade_level)
    self.DescShow2.text = string.format(self.language.DescShow2, RichTextHelper.ColorStr(string.format("%s·%s", quality_name2, segment_name2), PartnerQualityColor[co.quality_upgrade + PartnerConfig.QuaFetter.Grassy]))
    self.DescShow3.text = string.format(self.language.DescShow3, co.attribute_pellet_limit)

    UH.SetText(self.XiaoGuo1, string.format(self.language.DescShow1, co.upgrade_level))
    UH.SetText(self.XiaoGuo2, string.format(self.language.DescShow2, RichTextHelper.ColorStr(string.format("%s·%s", quality_name2, segment_name2), PartnerQualityColor[co.quality_upgrade + PartnerConfig.QuaFetter.Grassy])))
    UH.SetText(self.XiaoGuo3, string.format(self.language.DescShow3, co.attribute_pellet_limit))

    self.red_point_soar = self.red_point_soar or UiRedPoint.New(self.BtnUpObj, Vector3.New(77.7,24,0))
    self.red_point_soar:SetNum(data:SoarRedPoint() and 1 or 0)
end

function PartnerSoarViewF:FlushModel()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo

    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
        self.ui_obj:PlayAnim(ChAnimType.Show)
    else
        self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
    end
end

function PartnerSoarViewF:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[187].desc})
end

function PartnerSoarViewF:OnClickSoar()
    PartnerCtrl.Instance:SendPartnerSoar(self.data.cur_sel_partner)
end

function PartnerSoarViewF:OnClickAccpet()

    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local task = self.task_data:GetTaskInfoByTaskId(info.soar_task)
    if nil ~= task and TaskConfig.ListType.acceptable == task.list_type then
        PublicPopupCtrl.Instance:DialogTips{
            content = string.format(self.language.TaskAccpetTips, info.name),
            confirm = {
                func = function()
                    local task = self.task_data:GetTaskInfoByTaskId(info.soar_task)
                    if task then task:RequestAccept() end
                    ViewMgr:CloseView(DialogTipsView)
                end,
                name = self.language.TaskAccpetConfirm,
                wait_time = 5,
            }
        }
    else
        PublicPopupCtrl.Instance:Center(2 == self.data.is_cur_accpet_partner_soar_task and self.language.TaskAccpetError2 or self.language.TaskAccpetError1)
    end
end

function PartnerSoarViewF:OnClickGoTask()
    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.partner_soar)
    if task then
        task:GoTask()
        ViewMgr:CloseView(PartnerSoarView)
        ViewMgr:CloseView(PartnerView)
    end
end

PartnerSoarViewAttrItem = DeclareMono("PartnerSoarViewAttrItem", UIWidgetBaseItem)

function PartnerSoarViewAttrItem:PartnerSoarViewAttrItem()
   self.language = Language.Partner.Soar
end

function PartnerSoarViewAttrItem:SetData(data)
    self.AttrShow.text = string.format(self.language.AttrShow, Language.Common.AttrList[data.attr_type], DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
end


PartnerSoarSuccView = PartnerSoarSuccView or DeclareView("PartnerSoarSuccView", PartnerConfig.ResPath .. "partner_soar_succ")
VIEW_DECLARE_LEVEL(PartnerSoarSuccView, ViewLevel.L1)

function PartnerSoarSuccView:PartnerSoarSuccView()
end

function PartnerSoarSuccView:OnClickClose()
    ViewMgr:CloseView(PartnerSoarSuccView)
end

PartnerSoarSuccViewF = PartnerSoarSuccViewF or DeclareMono("PartnerSoarSuccViewF", UIWFlushPanel)

function PartnerSoarSuccViewF:PartnerSoarSuccViewF()
	self.data = PartnerCtrl.Instance:Data()
    self.task_data = TaskData.Instance
    self.language = Language.Partner.Soar

	self.data_cares = {
        {data = self.data.item_flush, func = self.FlushShow},
    }
end

function PartnerSoarSuccViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushModel()
end

function PartnerSoarSuccViewF:FlushShow()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    local vo = data.vo
    local soar_count = vo.soar_count or 0
    local co = self.data:GetPartnerSoarCoByCount(soar_count)
    local is_active = vo.soar_weapon_level > 0
    
    self.LeftTitle.text = string.format(self.language.SuccLeftTitle, info.arms_name)

    -- local soar_arms_co, attrs = data:GetPartnerSoarArmsCo(0 == soar_count and 1 or 11, true)
    local soar_arms_co, attrs = data:GetPartnerSoarArmsCo(is_active and vo.soar_weapon_level or 1, true, true)
    local skill_co = Cfg.SkillDescPartnerAndRole(soar_arms_co.skill_id, soar_arms_co.skill_level)

    self.AttrList:SetData(attrs)
    UH.SetIcon(self.SkillIcon, skill_co.icon_id, ICON_TYPE.SKILL)
    self.SkillName.text = skill_co.skill_name
    self.SkillDesc.text = skill_co.description_2

    local quality_name2 = Language.Partner.QualityName[co.quality_upgrade]
    local segment_name2 = string.format(Language.Partner.QualitySeg, DataHelper.GetDaXie(co.quality_segment))

    self.DescShow1.text = string.format(self.language.DescShow1, co.upgrade_level)
    self.DescShow2.text = string.format(self.language.DescShow2, RichTextHelper.ColorStr(string.format("%s·%s", quality_name2, segment_name2), PartnerQualityColor[co.quality_upgrade + PartnerConfig.QuaFetter.Grassy]))
    self.DescShow3.text = string.format(self.language.DescShow3, co.attribute_pellet_limit)

    UH.SetText(self.XiaoGuo1, string.format(self.language.DescShow1, co.upgrade_level))
    UH.SetText(self.XiaoGuo2, string.format(self.language.DescShow2, RichTextHelper.ColorStr(string.format("%s·%s", quality_name2, segment_name2), PartnerQualityColor[co.quality_upgrade + PartnerConfig.QuaFetter.Grassy])))
    UH.SetText(self.XiaoGuo3, string.format(self.language.DescShow3, co.attribute_pellet_limit))
end

function PartnerSoarSuccViewF:FlushModel()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    local vo = data.vo

    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
        self.ui_obj:PlayAnim(ChAnimType.Show)
    else
        self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
    end
end

PartnerSoarSuccViewAttrItem = DeclareMono("PartnerSoarSuccViewAttrItem", UIWidgetBaseItem)

function PartnerSoarSuccViewAttrItem:PartnerSoarSuccViewAttrItem()
   self.language = Language.Partner.Soar
end

function PartnerSoarSuccViewAttrItem:SetData(data)
    self.AttrShow.text = string.format(self.language.AttrShow, Language.Common.AttrList[data.attr_type], DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
end


PartnerSoarWeaponView = PartnerSoarWeaponView or DeclareView("PartnerSoarWeaponView", PartnerConfig.ResPath .. "partner_soar_weapon")

function PartnerSoarWeaponView:PartnerSoarWeaponView()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner
end

function PartnerSoarWeaponView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.PartnerSoarWeapon, Vector2.New(1080, 640))
end

PartnerSoarWeaponViewF = PartnerSoarWeaponViewF or DeclareMono("PartnerSoarWeaponViewF", UIWFlushPanel)

function PartnerSoarWeaponViewF:PartnerSoarWeaponViewF()
	self.data = PartnerCtrl.Instance:Data()
    self.task_data = TaskData.Instance
    self.language = Language.Partner.Soar

	self.data_cares = {
        {data = self.data.item_flush, func = self.FlushShow},
        {data = self.data.partner_soar_weapon_effect, func = self.FlushEffect, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushShow, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Task), func = self.FlushShow, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushFuYu, init_call = false},
        {data = self.data.fuyu_flush, func = self.FlushFuYu, init_call = false},
    }
end

function PartnerSoarWeaponViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushModel()
end

function PartnerSoarWeaponViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dft)
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
    end
end

function PartnerSoarWeaponViewF:FlushShow()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local soar_count = vo.soar_count or 0
    local co = self.data:GetPartnerSoarCoByCount(soar_count)
    local is_active = vo.soar_weapon_level > 0
    if vo.soar_weapon_level == 1 then
        self:FlushModel()
    end
    self.WeaponName.text = string.format(info.arms_name)
    self.WeaponLevel.text = is_active and string.format(self.language.WeaponLevel, vo.soar_weapon_level) or ""
    self.WeaponDesc.text = is_active and string.format(self.language.WeaponDesc, info.name) or string.format(self.language.WeaponDesc2, info.name)

    UH.SetText(self.CostTitle, is_active and Language.Partner.Soar.StrCost or Language.Partner.Soar.ActiveCost)

    local soar_arms_co, attrs = data:GetPartnerSoarArmsCo(is_active and vo.soar_weapon_level or 1, true, true)
    local soar_arms_co_next, attrs_next = data:GetPartnerSoarArmsCo(vo.soar_weapon_level + 1, true, true)
    local is_max_level = nil == soar_arms_co_next
    local skill_co = Cfg.SkillDescPartnerAndRole(soar_arms_co.skill_id, soar_arms_co.skill_level)
    local soar_arms_co_nexts = Cfg.PartnerSoarArmsByIdSkillLevel(data.info.arms_id, soar_arms_co.skill_level + 1, false)

    if self.skill_level and 1 == soar_arms_co.skill_level - self.skill_level then
        self.show_skill_effect = true
    end 
    self.skill_level = soar_arms_co.skill_level

    UH.SetIcon(self.SkillIcon, skill_co.icon_id, ICON_TYPE.SKILL)
    self.SkillName.text = skill_co.skill_name
    self.SkillDesc.text = is_active and skill_co.description or skill_co.description_2
    self.SkillLevel.text = string.format(self.language.WeaponSkillLevel, soar_arms_co.skill_level) 
        .. (is_active and (nil ~= soar_arms_co_nexts and string.format(self.language.WeaponSkillNext, soar_arms_co_nexts.arms_level) or self.language.WeaponSkillMax) or "")
    
    self.FuYuGroup:SetActive(true)--is_active)
    self.FuYuBtn:SetActive(true)--is_active)
    -- self.SkillDescObj:SetActive(not is_active)
    -- self.FuYuGroup:SetActive(false)
    -- self.FuYuBtn:SetActive(false)
    -- self.SkillDescObj:SetActive(true)
    local can_jihuo = data.quality.operate_qua >= self.data:WeaponLimit()
    local consumes = {}
    for i = 0, #soar_arms_co.item_list do
        local item_data = soar_arms_co.item_list[i]
        local have_num = BagData.Instance:GetNumByItemId(item_data.item_id)
        local need_num = item_data.num
        table.insert(consumes, Item.Init(item_data.item_id, string.format(self.language.WeaponConsumeNum, RichTextHelper.ColorStr(have_num, have_num < need_num and COLORSTR.Red8 or COLORSTR.Green4), need_num), item_data.is_bind))
    end
    self.ConsumeList:SetData(consumes)

    self.AttrList:SetData(self.data:GetPartnerSoarWeaponAttrsShow(attrs, attrs_next))
    self.MaxLevelObj:SetActive(is_max_level)
    self.ConsumeObj:SetActive(not is_max_level)
    self.BtnUpObj:SetActive(not is_max_level)-- and is_active)
    UH.SetText(self.BtnDesc, is_active and Language.Partner.Soar.BtnDesc2 or Language.Partner.Soar.BtnDesc1)
    self.BtnInter.Interactable = data.quality.operate_qua >= self.data:WeaponLimit()
    self.red_point_up = self.red_point_up or UiRedPoint.New(self.BtnUpObj, Vector3.New(77.7,24,0))
	self.red_point_up:SetNum(data:SoarWeaponRedPoint(true) and 1 or 0)
    self:FlushFuYu()
end
function PartnerSoarWeaponViewF:FlushFuYu()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local fuyu_list = data:FuYuList()
    --FuYuItems
    for i = 1, PartnerConfig.FUYU_MAX_NUM do
        self.FuYuAdds[i]:SetActive(fuyu_list[i - 1] == nil)
        self.FuYuItems[i]:SetObjActive(fuyu_list[i - 1] ~= nil)
        if fuyu_list[i - 1] ~= nil then
            self.FuYuItems[i]:SetData(Item.Create(fuyu_list[i - 1]))
        end
        self.RedPointList[i]:SetNum((data.vo.fight_state == PartnerConfig.FightState.fight and fuyu_list[i - 1] == nil and data:FuYuCanPutOn()) and 1 or 0)
        --[[ if not data:FuYuCanPutOn() then
            self.RedPointList[i]:SetNum(data:FuYuCanChange() and 1 or 0)
        end ]]
    end
end
function PartnerSoarWeaponViewF:OnClickFuYu(key)
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local fuyu_list = data:FuYuList()
    if fuyu_list[key] == nil then
        ViewMgr:OpenView(PartnerFuYuBag)
    end
    
    if data.fuyu_put_on_red_point_show then
        data.fuyu_put_on_red_point_show = false
        self.data.item_flush.val = not self.data.item_flush.val
        self.data:PartnerSoarWeaponRedPoint()
    end
    if data.fuyu_change_better_red_show then
        data.fuyu_change_better_red_show = false
        self.data.item_flush.val = not self.data.item_flush.val
        self.data:PartnerSoarWeaponRedPoint()
    end
end
function PartnerSoarWeaponViewF:OnClickFuYuBook()
    local param_t = self.data:GetBagEffectShowParam()
	ViewMgr:OpenView(PartnerFuYuEffectShow, param_t)
end
function PartnerSoarWeaponViewF:OnClickGetWay()
    GetWayData.OnOpenPanel(Mod.Mall.Market, 63001)
end
function PartnerSoarWeaponViewF:OnClickBag()
    ViewMgr:OpenView(PartnerFuYuBag)
end
function PartnerSoarWeaponViewF:FlushModel()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local is_active = vo.soar_weapon_level > 0

    if is_active then
        TimeHelper:CancelTimer(self.timer_dft)
        if self.handle_effect then
            self.EffectTool:Stop(self.handle_effect)
        end
        self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
            self.handle_effect = self.EffectTool:PlayByEffectId(info.special_effects, self.EffectParent)
        end)
    else
        self.ImageSetterSoarWeapon:SetByPath(string.format(PartnerConfig.PngPathSoarWeaponB, info.arms_id))
    end
end

function PartnerSoarWeaponViewF:FlushEffect()
    self.EffectTool:Play(3165028)
    self.EffectTool:Play(3165030)
    self.EffectTool:Play(3165027)
    
    if self.show_skill_effect then
        self.show_skill_effect = false
        self.EffectTool:Play(3165029)
    end
end

function PartnerSoarWeaponViewF:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[188].desc})
end

function PartnerSoarWeaponViewF:OnClickUp()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local qua = data.quality
    local vo = data.vo
    local soar_count = vo.soar_count or 0
    -- LogDG(qua.operate_qua, vo.soar_weapon_leve)
    if vo.soar_weapon_level == 0 and qua.operate_qua < self.data:WeaponLimit() then
        PublicPopupCtrl.Instance:Center(string.format(Language.Partner.Soar.ActiveTip, info.name))
        return
    end
    local soar_arms_co = data:GetPartnerSoarArmsCo(vo.soar_weapon_level)
    if soar_arms_co == nil then
        return
    end
    for i = 0, #soar_arms_co.item_list do
        local item_data = soar_arms_co.item_list[i]
        local have_num = BagData.Instance:GetNumByItemId(item_data.item_id)
        local need_num = item_data.num
        if have_num < need_num then
            MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = item_data.item_id})})
            break
        end
    end

    --[[ if not data:PartnerWeaponUpCanSoar() and self.language.WeaponSoarTips[vo.soar_count] then
        PublicPopupCtrl.Instance:Center(self.language.WeaponSoarTips[vo.soar_count])
        return
    end ]]

    PartnerCtrl.Instance:SendPartnerSoarWeapon(self.data.cur_sel_partner)
end


PartnerSoarWeaponViewAttrItem = DeclareMono("PartnerSoarWeaponViewAttrItem", UIWidgetBaseItem)

function PartnerSoarWeaponViewAttrItem:PartnerSoarWeaponViewAttrItem()
   self.language = Language.Partner.Soar
end

function PartnerSoarWeaponViewAttrItem:SetData(data)
    self.AttrShow.text = string.format(self.language.AttrShow2, Language.Common.AttrList[data.attr_type], DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value_now or data.attr_value) or (data.attr_value_now or data.attr_value))
    self.ArrowObj:SetActive(nil ~= data.attr_value_now and data.attr_value ~= data.attr_value_now)
    self.AttrNext.text = (nil ~= data.attr_value_now and data.attr_value ~= data.attr_value_now) and (DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or (data.attr_value)) or ""
end
