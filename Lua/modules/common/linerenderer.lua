LineRenderer = {}
local UnityLineRenderer = typeof('UnityEngine.LineRenderer')
local UnityVector3 = typeof('UnityEngine.Vector3')
local SystemInt32 = typeof('System.Int32')
function LineRenderer.SetPosition(component, index, position)
    if VersionTag.HasTag(VersionTag.LineRenderer) then
        if component ~= nil then
            component:SetPosition(index, position)
        end
    else
        if component ~= nil then
            local func = tolua.getmethod(UnityLineRenderer, 'SetPosition', SystemInt32, UnityVector3)
            func:Call(component, index, position)
            func:Destroy()
            func = nil
        end
    end
end