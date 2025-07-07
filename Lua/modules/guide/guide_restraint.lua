GuideRestraint = GuideRestraint or DeclareView("GuideRestraint", "guide/guide_restraint",Mod.GuideRestraint.Main)
VIEW_DECLARE_MASK(GuideRestraint,ViewMask.BgBlockClose)
function GuideRestraint:GuideRestraint()
    BattleCtrl.Instance:SendAutoBattle(false)
    --不垃圾代码 强行修改为使用技能并且保存为默认技能
    --local skill_list = BattleData.Instance:PlayerSkillInfo()
    BattleData.Instance:BaseData().sel_tar_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
    BattleData.Instance:BaseData().sel_tar_param1 = 107--GuideData.Instance:GetSkillId()
    BattleData.Instance:BaseData().sel_tar_param2 = 6--GuideData.Instance:GetSkillId()
    -- BattleData.Instance:FlushDefaultSkill()
end
function GuideRestraint:CloseCallback()
    --进行下一步指引
    GuideManager.Instance:Continue()
end