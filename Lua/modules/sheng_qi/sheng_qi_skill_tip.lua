-- 圣器技能详情弹框
ShengQiSkillTip = ShengQiSkillTip or DeclareView("ShengQiSkillTip", "sheng_qi/sheng_qi_skill_tip")
VIEW_DECLARE_MASK(ShengQiSkillTip, ViewMask.BlockClose)
function ShengQiSkillTip:ShengQiSkillTip()
    
end

function ShengQiSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    local skill = param_t.skill
    if not skill then
        return
    end
    
    UH.SetText(self.LblName, skill:Name())
    UH.SetText(self.LblDesc, skill:Desc())
    self.SkillItem:SetData(skill)
    if param_t.pos ~= nil  then
        self:CheckPos(param_t.pos)
        UH.LocalPos(self.PosRect,param_t.pos)
    end 
end

function ShengQiSkillTip:CheckPos(pos)
    if pos.x > 340 then
        pos.x = 340
    end
    if pos.x < -340 then
        pos.x = -340
    end
    if pos.y > 88 then
        pos.y = 88
    end
    if pos.y < -450 then
        pos.y = -450
    end
end