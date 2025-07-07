TargetArrow = {}
local TargetArrowType = 'Game.TargetArrow'
local TargetArrowOrigin
local TargetArrowGuiHuo
function TargetArrow.Init()
    TargetArrowOrigin = UnityGameObj.Find('GuideArrow'):GetComponent(TargetArrowType)
end
function TargetArrow.GuiHuo()
    if TargetArrowOrigin == nil then
        TargetArrow.Init()
    end
    if not TargetArrowGuiHuo then
        TargetArrowGuiHuo = UnityGameObj.Find('GuideArrowGuiHuo'):GetComponent(TargetArrowType)
    end
    if TargetArrowGuiHuo then
        Game.GameManager.Singleton().TargetArrow:Stop()
        Game.GameManager.Singleton().TargetArrow = TargetArrowGuiHuo
    end
end
function TargetArrow.Revert()
    if TargetArrowOrigin then
        Game.GameManager.Singleton().TargetArrow:Stop()
        Game.GameManager.Singleton().TargetArrow = TargetArrowOrigin
    end
end
