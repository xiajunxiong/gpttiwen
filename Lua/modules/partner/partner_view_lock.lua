
PartnerViewLock = PartnerViewLock or DeclareView("PartnerViewLock", PartnerConfig.ResPath .. "partner_lock", Mod.Partner.PartnerLock)

function PartnerViewLock:PartnerViewLock()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner
end

function PartnerViewLock:LoadCallback(param)
    self.Title1:SetActive(not self.data.lock_special)
    self.Title2:SetActive(self.data.lock_special)
    self.close_call = nil
    if param then
        if param.partner_name then
            UH.SetText(self.Title3,param.partner_name)
            self.Title3:SetObjActive(true)
        end
        if param.close_call then
            self.close_call = param.close_call
        end
    end

    if self.data.is_quality then
        self.data.is_quality = false
        self.Title1:SetActive(false)
        self.Title2:SetActive(false)
        self.Title4:SetActive(true)
    else
        TimeHelper:CancelTimer(self.timer_dt)
        if 1 == self.data.new_partner or 4 == self.data.new_partner then
            self.timer_dt = TimeHelper:AddDelayTimer(function ()
                ViewMgr:CloseView(PartnerViewLock)
            end, 10)
        end
    end

end

function PartnerViewLock:CloseCallback()
    if self.close_call ~= nil then
        self.close_call()
    end
    if TreasureHuntData.Instance.treasure_hunt_count == 5 then
        TreasureHuntData.Instance.smart_data.is_change = not TreasureHuntData.Instance.smart_data.is_change
    end
    if TreasureHuntData.Instance.smart_data.is_guide_draw then
        ViewMgr:CloseView(TreasureHuntView)
        ViewMgr:CloseView(TreasureHuntDetailView)
        if TreasureHuntData.Instance.guide_id == 117 then
            -- GuideManager.Instance:Start(119)
            TreasureHuntData.Instance.guide_id = -1
        end
        TreasureHuntData.Instance.smart_data.is_guide_draw = false
    end
    TimeHelper:CancelTimer(self.timer_dt)
end

function PartnerViewLock:OnClickClose()
    ViewMgr:CloseView(PartnerViewLock)
end

PartnerViewLockInfo = PartnerViewLockInfo or DeclareMono("PartnerViewLockInfo", UIWFlushPanel)

function PartnerViewLockInfo:PartnerViewLockInfo()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewLockInfo:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    self.Name.text = info.name

    self.ImageSetterSoarWeapon:SetByPath(string.format(PartnerConfig.PngPathSoarWeaponS, info.arms_id))
    self.SoarWeaponName.text = info.arms_name
    self.Weapon:SetActive(data:ShowWeapon())
end

PartnerViewLockRight = PartnerViewLockRight or DeclareMono("PartnerViewLockRight", UIWFlushPanel)

function PartnerViewLockRight:PartnerViewLockRight()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Attr
end

function PartnerViewLockRight:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    local vo = data.vo

    -- self.Name.text = info.name
    -- local level = RoleData.Instance:GetBaseData().level
    -- self.Level.text = "Lv.".. level
    -- local prof = Language.Common.Prof[info.job]
    -- self.Prof.text = string.format(self.language.prof, prof)

    -- for i = 1, PartnerConfig.Attr_Ele_MAX do
    --     if #info.element >= i then
    --         self.EleObj[i]:SetActive(true)
    --         self.EleIcon[i].SpriteName = info.element[i].type
    --         self.EleNum[i].text = info.element[i].num
    --     else
    --         self.EleObj[i]:SetActive(false)
    --     end
    -- end
    
    -- self.Attr1:SetData(vo.attr_list)


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

    self.SkillList:SetData(data.quality_skill_list)
    -- self.SkillList:SetData(self.data.lock_special and data.up_lock_skills or data.cur_skill_list_sort)
    -- self.data.lock_special = false

    if data.quality_up.is_show then 
        self.ListObj:SetActive(false)
        self.QualityObj:SetActive(true)

        local attr_param,item_show,skill_list = PartnerData.Instance:QualityUpLockInfoShow(self.data.new_partner)
        self.QuaAttrChange:SetData(attr_param)
        self.PartnerCellL:SetData(item_show.old)
        self.PartnerCellR:SetData(item_show.new)
        self.QuaSkillList:SetData(skill_list)
    
    end 
end

PartnerViewLockLeft = PartnerViewLockLeft or DeclareMono("PartnerViewLockLeft", UIWFlushPanel)

function PartnerViewLockLeft:PartnerViewLockLeft()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Attr
end

function PartnerViewLockLeft:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local info = data.info
    local vo = data.vo
    local cur_up_info = data.cur_up_info
    -- local gift_info = PartnerCtrl.Instance:Data():GetPartnerGiftWithIdAndLevel(cur_up_info.skill_gift, cur_up_info.skill_gift_level)
    local skill_f = data.skill_f

    self.Name.text = info.name
    local level = vo.level
    self.Level.text = "Lv.".. level
    local prof = Language.Common.Prof[info.job]
    local prof_sp = RoleData.GetProfSp(info.job)
    self.Prof.text = prof
    self.ProfI.SpriteName = prof_sp

    local element = {}
    for i,v in ipairs(info.element) do
        local type = ElementSpriteType[v.type] or 0
        element[type] = v.num
    end
    self.Element:SetData(element)
    
     
    self.GiftObj:SetActive(not data.quality_up.is_show)
    self.QualityObj:SetActive(data.quality_up.is_show)
    
    self.GiftCell:SetData(skill_f)
    local friendly_desc = self.language.friendly_desc
    for _, attr in ipairs(skill_f.attrs_next or {}) do
        friendly_desc = friendly_desc .. attr.attr_name .. "、"
    end
    self.GiftContent.text = string.sub(friendly_desc, 1, -2)

    -- self.Attr1:SetData(vo.attr_list)

end

PartnerViewLockSkill = PartnerViewLockSkill or DeclareMono("PartnerViewLockSkill", UIWFlushPanel)

function PartnerViewLockSkill:PartnerViewLockSkill()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewLockSkill:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.new_partner)
    local cur_up_info = data.cur_up_info
    local sel_skill_list = data.sel_skill_list
    local gift_info = PartnerCtrl.Instance:Data():GetPartnerGiftWithIdAndLevel(cur_up_info.skill_gift, cur_up_info.skill_gift_level)

    for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
        self.Skills[i]:SetData(sel_skill_list[i])
    end
    self.GiftCell:SetData(gift_info)
end

PartnerLockQuaUpCell = PartnerLockQuaUpCell or  DeclareMono("PartnerLockQuaUpCell", UIWidgetBaseItem)
function PartnerLockQuaUpCell:PartnerLockQuaUpCell()
end

function PartnerLockQuaUpCell:SetData(data)
    UH.SpriteName(self.Qua, "_Loc_"..PartnerConfig.QuaQuaPicName[ data.quality])
    UH.SetText(self.level, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.quality_segment)))

    local name = Split(Language.Partner.QualityPreName[ data.quality],"|")
    for i = 1,2 do 
        UH.SetText(self["name_"..i],name[i])
    end 
    -- UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.ITEM)
    -- PartnerInfo.SetQuality(self.Qua, data.quality)
end