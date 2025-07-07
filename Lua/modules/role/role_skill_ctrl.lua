RoleSkillCtrl = RoleSkillCtrl or BaseClass(BaseCtrl)

function RoleSkillCtrl:__init()
	if RoleSkillCtrl.Instance ~= nil then
		Debuger.LogError("[RoleSkillCtrl] attempt to create singleton twice!")
		return
	end	
	RoleSkillCtrl.Instance = self

    RemindCtrl.Instance:Remind():Register(
        Mod.Role.RoleSkill,
        SkillData.Instance.change_data,
        -- BindTool.Bind(SkillData.CacuRemindCount, SkillData.Instance)
        BindTool.Bind(self.GetSkillRemind, self)
    )
end

function RoleSkillCtrl:GetSkillRemind()
	if SkillData.Instance:CacuRemindCount() > 0 then
		return 1
	elseif SkillData.Instance:SkillCanUpgradeRemind() > 0 then
		return 1
	else
		return RoleSkillData.Instance:GetXiulianSkillRemind()
    end
end

function RoleSkillCtrl:__delete()

end

function RoleSkillCtrl:OnUnloadGameLogic()
	RoleSkillData.Instance.prof_skill_red_notice:Set({clicked = false})
end