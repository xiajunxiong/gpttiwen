
FightSkillView = FightSkillView or DeclareView("FightSkillView", FightConfig.ResPath .. "fight_skill")
VIEW_DECLARE_MASK(FightSkillView,ViewMask.BlockClose)

function FightSkillView:FightSkillView()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight
end

function FightSkillView:LoadCallback()
	local skill_list = self.data:GetSkillList()
	self.SkillGrid:SetData(skill_list)
end

FightSkillViewItem1 = DeclareMono("FightSkillViewItem1", UIWidgetBaseItem)

function FightSkillViewItem1:FightSkillViewItem1()
	self.fight_data = FightCtrl.Instance:Data()
	self.language = Language.Fight.SkillTitle
end

function FightSkillViewItem1:SetData(data)
	self.Title.text = self.language[data.type]
	self.SkillGrid:SetData(data.skills)
end

FightSkillViewItem2 = DeclareMono("FightSkillViewItem2", UIWidgetBaseItem)

function FightSkillViewItem2:FightSkillViewItem2()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightSkillViewItem2:SetData(data)
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

function FightSkillViewItem2:OnClickItem()
	local seq = 1 == self.fight_data.cur_auto_seq and "first_" or "second_"
	local info = nil == self.data.skill_cfg and self.data or self.data.skill_cfg
	if info.is_base and BATTLE_MOVE_TYPE.AI_SKILL == info.skill_id then
		local skill_list = self.fight_data:GetRoleSkillList()
		local skill1 = skill_list.skills and skill_list.skills[1] or nil
		if skill1 then
			local other_auto_seq = 1 == self.fight_data.cur_auto_seq and 2 or 1
			local seq2 = 1 == other_auto_seq and "first_" or "second_"
			local skill2
			local move_type2 = self.fight_data.skill_protocol[seq2.. "move_type"]
			-- 在没有上阵宠物时，选择只能施法需要把另一个技能改成防御
			if -1 == PetData.Instance:GetFightIndex() and BATTLE_MOVE_TYPE.NORMAL_DEFENCE ~= move_type2 and BATTLE_MOVE_TYPE.NORMAL_ATTACK ~= move_type2 then
				skill2 =  {
					[seq2 .. "move_type"] = BATTLE_MOVE_TYPE.NORMAL_DEFENCE,
					[seq2 .. "skill_id"] = 0,
					[seq2 .. "skill_level"] = 0,
				}
			end
			FightCtrl.Instance:SendSetBattleAutoMove{
				[self.fight_data.cur_auto_seq] = {
					[seq .. "move_type"] = info.is_base and info.skill_id or BATTLE_MOVE_TYPE.PERFORM_SKILL,
					[seq .. "skill_id"] = skill1.skill_id,
					[seq .. "skill_level"] = skill1.skill_level,
				},
				[other_auto_seq] = skill2
			}
		end
	else
		FightCtrl.Instance:SendSetBattleAutoMove{
			[self.fight_data.cur_auto_seq] = {
				[seq .. "move_type"] = info.is_base and info.skill_id or BATTLE_MOVE_TYPE.PERFORM_SKILL,
				[seq .. "skill_id"] = info.is_base and 0 or info.skill_id,
				[seq .. "skill_level"] = info.is_base and 0 or info.skill_level,
			}
		}
	end
	ViewMgr:CloseView(FightSkillView)
end
