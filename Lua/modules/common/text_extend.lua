
TextExtend = {}
-- local UnityText = typeof('UnityEngine.UI.Text')
local UnityTextGenerator = typeof('UnityEngine.TextGenerator')
local UnityUILineInfo = typeof('UnityEngine.UILineInfo')
local UnityTextGenerationSettings = typeof('UnityEngine.TextGenerationSettings')
local SystemString = typeof('System.String')
local SystemInt32 = typeof('System.Int32')
local UnityVector2 = typeof('UnityEngine.Vector2')

function TextExtend.characterCount(text_generator)
    if VersionTag.HasTag(VersionTag.TextExtend) then
        return text_generator.characterCount
    else
        local property = tolua.getproperty(UnityTextGenerator, 'characterCount')
        local value = property:Get(text_generator, nil)
        property:Destroy()
        return value
    end
end

function TextExtend.lines(text_generator)
    if VersionTag.HasTag(VersionTag.TextExtend) then
        return text_generator.lines
    else
        local property = tolua.getproperty(UnityTextGenerator, 'lines')
        local value = property:Get(text_generator, nil)
        property:Destroy()
        return value
    end
end

function TextExtend.topY(line_info)
    if VersionTag.HasTag(VersionTag.TextExtend) then
        return line_info.topY
    else
        local field = tolua.getfield(UnityUILineInfo, 'topY')
        local value = field:Get(line_info)
        field:Destroy()
        return value
    end
end

function TextExtend.startCharIdx(line_info)
    if VersionTag.HasTag(VersionTag.TextExtend) then
        return line_info.startCharIdx
    else
        local field = tolua.getfield(UnityUILineInfo, 'startCharIdx')
        local value = field:Get(line_info)
        field:Destroy()
        return value
    end
end

-- function TextExtend.Substring(str, startIndex)
--     -- if VersionTag.HasTag(VersionTag.TextExtend) then
--     --     return line_info.topY
--     -- else
--         local method = tolua.getmethod(SystemString, 'Substring', SystemInt32)
--         local value = method:Call(System.String.New(str), startIndex)
--         method:Destroy()
--         return value
--     -- end
-- end

function TextExtend.Populate(text_generator, str, settings)
    if VersionTag.HasTag(VersionTag.TextExtend) then
        return text_generator:Populate(str, settings)
    else
        local method = tolua.getmethod(UnityTextGenerator, 'Populate', SystemString, UnityTextGenerationSettings)
        local value = method:Call(text_generator, str, settings)
        method:Destroy()
        return value
    end
end