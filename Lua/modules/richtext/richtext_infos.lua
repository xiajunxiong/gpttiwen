
local t_info = {}
local function new_handle()
    return string.split(tostring({}), ": ")[2]
end

function RichTextHelper.StoreInfo(info)
    local handle = new_handle()
    t_info[handle] = info
    return handle
end

function RichTextHelper.FetchInfo(handle)
    local info = t_info[handle]
    return t_info[handle]
end

function RichTextHelper.DeleteInfo(handle)
	if nil == handle then return end
	t_info[handle] = nil
end