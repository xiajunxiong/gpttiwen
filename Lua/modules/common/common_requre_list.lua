--编辑器require使用
local common_requre_list = {
    "modules/common/common_enum",
    "modules/common/color_define",
    "modules/common/language",
}

for i,v in ipairs(common_requre_list) do
    require(v)
end