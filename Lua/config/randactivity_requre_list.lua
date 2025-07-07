local require_func = function()
    local require_list = {}
    for k,v in pairs(Config.activity_main_auto.require_list) do
        local path = string.format("config/auto/%s_auto",v.file_name)
        table.insert(require_list,path)
    end
    ConfigRequireSelect(require_list)
    for i,v in pairs(require_list) do
        FILE_REQ(v)
    end
    return require_list
end

return require_func()