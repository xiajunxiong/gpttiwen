local gm_requre_list = {
    "systool/util",
    "systool/debug_tool",
    "modules/gm_command/gm_behavior",
}

for i,v in ipairs(gm_requre_list) do
    require(v)
end