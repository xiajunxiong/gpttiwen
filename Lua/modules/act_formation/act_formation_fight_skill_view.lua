
ActFormationFightSkillView = ActFormationFightSkillView or DeclareView("ActFormationFightSkillView", "act_formation/act_formation_fight_skill")
VIEW_DECLARE_MASK(ActFormationFightSkillView,ViewMask.BlockClose)

function ActFormationFightSkillView:ActFormationFightSkillView()
	self.data = ActFormationData.Instance
	self.language = Language.Fight
end

function ActFormationFightSkillView:LoadCallback()
	local skill_list = self.data:GetSkillList()
	self.SkillGrid:SetData(skill_list)
end

ActFormationFightSkillViewItem1 = DeclareMono("ActFormationFightSkillViewItem1", UIWidgetBaseItem)

function ActFormationFightSkillViewItem1:ActFormationFightSkillViewItem1()
	self.language = Language.Fight.SkillTitle
end

function ActFormationFightSkillViewItem1:SetData(data)
	self.Title.text = self.language[data.type]
	self.SkillGrid:SetData(data.skills)
end

ActFormationFightSkillViewItem2 = DeclareMono("ActFormationFightSkillViewItem2", UIWidgetBaseItem)
function ActFormationFightSkillViewItem2:ActFormationFightSkillViewItem2()
	self.fight_data = ActFormationData.Instance
end

function ActFormationFightSkillViewItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = nil == data.skill_cfg and data or data.skill_cfg
	self.Name.text = info.skill_name
	UH.SetIcon(self.IconSSp, info.icon_id, ICON_TYPE.SKILL)
	self.SkillCorner.text = info.corner_icon or ""
	self.SkillCornerObj:SetActive(not StringIsEmpty(info.corner_icon))

	if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(info.asset_id) then
	    self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(info.asset_id), self.EffectParent)
	end
end

function ActFormationFightSkillViewItem2:OnClickItem()
	local info = nil == self.data.skill_cfg and self.data or self.data.skill_cfg
	if info.is_base and BATTLE_MOVE_TYPE.AI_SKILL == info.skill_id then
		local skill_list = FightData.Instance:GetRoleSkillList()
		local skill1 = skill_list.skills and skill_list.skills[1] or nil
		if skill1 then
			local other_auto_seq = 1 == self.fight_data.cur_auto_seq and 2 or 1
			local move_type2 = self.fight_data.cur_sel_skills[other_auto_seq].move_type
			-- 在没有上阵宠物时，选择智能施法需要把另一个技能改成防御
			if -1 == self.fight_data:GetFightIndex() and BATTLE_MOVE_TYPE.NORMAL_DEFENCE ~= move_type2 and BATTLE_MOVE_TYPE.NORMAL_ATTACK ~= move_type2 then
				self.fight_data.cur_sel_skills[other_auto_seq].detail:Set{
					move_type = BATTLE_MOVE_TYPE.NORMAL_DEFENCE,
					skill_id = 0,
					skill_level = 0,
				}
				self.fight_data.cur_sel_skills[other_auto_seq].move_type = BATTLE_MOVE_TYPE.NORMAL_DEFENCE
			end
		end
	end
	local info = self.data.skill_cfg == nil and TableCopy(self.data) or TableCopy(self.data.skill_cfg)
	self.fight_data.cur_sel_skills[self.fight_data.cur_auto_seq].detail:Set(info)
	if info.skill_id > 11 then
		self.fight_data.cur_sel_skills[self.fight_data.cur_auto_seq].move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
	else
		self.fight_data.cur_sel_skills[self.fight_data.cur_auto_seq].move_type = info.skill_id
	end
	if self.fight_data.formation_smart.skill_change ~= info.skill_id then
		if  info.skill_id <= 11 then
			self.fight_data.formation_smart.skill_change = info.skill_id
		elseif self.fight_data.formation_smart.skill_change ~= BATTLE_MOVE_TYPE.PERFORM_SKILL then
			self.fight_data.formation_smart.skill_change = BATTLE_MOVE_TYPE.PERFORM_SKILL
		else
			self.fight_data.formation_smart:SetDirty("skill_change")
		end
	else
		self.fight_data.formation_smart:SetDirty("skill_change")
	end
	ViewMgr:CloseView(ActFormationFightSkillView)
end
---------------------------------------------------------------------
ActFormationCoPetSkillView = ActFormationCoPetSkillView or DeclareView("ActFormationCoPetSkillView","act_formation/act_formation_pet_skill")
VIEW_DECLARE_MASK(ActFormationFightSkillView,ViewMask.BlockClose)

function ActFormationCoPetSkillView:ActFormationCoPetSkillView()
	self.data = ActFormationData.Instance
end
function ActFormationCoPetSkillView:LoadCallback()
	self.Board3:SetData(ActFormationCoPetSkillView, Language.ActFormation.CopetSkillSetView, Vector2.New(896,528))
	self.PetList:SetCompleteCallBack(function ()
		self.PetList:ClickItem(1)
	end)
	self.PetList:SetData(self.data:GetCopetList())
end
-----------------------------------------------
ActFormationCoPetSkillPanel = ActFormationCoPetSkillPanel or DeclareMono("ActFormationCoPetSkillPanel", UIWFlushPanel)
function ActFormationCoPetSkillPanel:ActFormationCoPetSkillPanel()
	self.data = ActFormationData.Instance
	self.pet_data = PetData.Instance
	self.data_cares = {
		{data = self.data.formation_smart, func = self.FlushPetInfo, keys = {"cur_copet_index"}}
	}
end

function ActFormationCoPetSkillPanel:FlushPetInfo()
	local pet = self.data.cur_copet
	if pet == nil then
		return
	end
	local total_attr = self.pet_data:RealAttr(pet.index)
    if total_attr == nil then
        total_attr = pet:TotalAttr()
    end
    for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(pet:HP() / v)
            self.ProgMaxHp:SetText(string.format(Language.ActFormation.ProgressTxtColorFormat, pet:HP(), v))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(pet:MP() / v)
            self.ProgMaxMp:SetText(string.format(Language.ActFormation.ProgressTxtColorFormat, pet:MP(), v))
        elseif self["Lbl"..k] ~= nil then
            UH.SetText(self["Lbl" .. k], v)
        end
    end
	self.PetMainSkillList:SetData(self.data:GetCurCopetMainSkillList())
end

function ActFormationCoPetSkillPanel:OnClickPet(mono)
	self.data:SetCurCopetInfo(mono:GetData())
end
-------------------------------------------
ActFormationCoPetItem = ActFormationCoPetItem or DeclareMono("ActFormationCoPetItem", UIWidgetBaseItem)
function ActFormationCoPetItem:ActFormationCoPetItem()
	self.formation_data = ActFormationData.Instance
	self.pet_data = PetData.Instance
end

function ActFormationCoPetItem:SetData(data)
	if self.FlyBgSub then
        self.FlyBgSub:SetData({})
    end

    local state = self.formation_data:PetFormationStatus(data.index)
    UH.SpriteName(self.PetState, state)

    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.PetName, data:Name())
    UH.Color(self.PetName, data:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)

    local score_num, score_level = data:GetScore()
    UH.SetText(self.PetScore, score_num)
    UH.SpriteName(self.PetScoreLevel, "scoreLevel"..tostring(score_level))

    self.HeadCell:SetDataByPet(data)
    local fly_flag = data:FeiShengTimes()
    if self.FlyBgSub then
        self.FlyBgSub:SetData({fly_flag = fly_flag})
    end
    local is_lianyao = self.data:IsLianYao()
    if self.YaoObj then
        self.YaoObj:SetActive(is_lianyao)
    end
    self.PetName.alignment = is_lianyao and 3 or 4
end

function ActFormationCoPetItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
------------------------------------------------------------------
ActFormationPetSkillItem = ActFormationPetSkillItem or DeclareMono("ActFormationPetSkillItem", UIWidgetBaseItem)
function ActFormationPetSkillItem:ActFormationPetSkillItem()
	self.formation_data = ActFormationData.Instance
end

function ActFormationPetSkillItem:OnDestroy()
	self.GiftItem:SetGray(false)
end

function ActFormationPetSkillItem:SetData(data)
	self.GiftItem:SetData(data.gift_info)
	--1为被禁用，显示启用按钮
	self.ForbidBtn:SetActive(data.is_forbid == 0)
	self.PermitBtn:SetActive(data.is_forbid == 1)
	self.GiftItem:SetGray(data.is_forbid == 1)
	self.data = data
end

function ActFormationPetSkillItem:OnClickPermitBtn()
	self.ForbidBtn:SetActive(true)
	self.PermitBtn:SetActive(false)
	self.data.is_forbid = 0
	self.formation_data:SetCopetSkillPermission(self.data)
	self.GiftItem:SetGray(false)
end

function ActFormationPetSkillItem:OnClickForbidBtn()
	self.ForbidBtn:SetActive(false)
	self.PermitBtn:SetActive(true)
	self.data.is_forbid = 1
	self.GiftItem:SetGray(true)
	self.formation_data:SetCopetSkillPermission(self.data)
end

function ActFormationPetSkillItem:OnClickGift()
end