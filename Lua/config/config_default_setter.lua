local EMPTY_TAB = {}
local ITEM_DEFAULT_VAL_TAB = {
    num = 1,
    is_bind = 1,
}
ITEM_DEFAULT_VAL_TAB.__metatable = false
local ITEM_META_TAB = {
    __index = ITEM_DEFAULT_VAL_TAB,
}

local function default_meta_newindex_func (tab, key, value)
    LogError("不能对配置对象写入值！",tab,key,value)
end

local function set_sheet_default_tab(sheet_data,sheet_data_default,sheet_data_col_flags,share_str)
    local meta_tab = nil
    if sheet_data_default ~= nil then
        meta_tab = {
            __index = function(tab, key)
                if sheet_data_col_flags ~= nil then
                    local col_f = sheet_data_col_flags[key]
                    if col_f == "itemlist" then
                        return EMPTY_TAB
                    end
                end
                local re = sheet_data_default[key]
                if re == nil and share_str ~= nil then
                    local share_key = rawget(tab,string.format("%s_share",key))
                    if share_key ~= nil then
                        re = share_str[share_key]
                        if not StringIsEmpty(re) then
                            local format_v = rawget(tab,string.format("%s_sharev",key))
                            if format_v ~= nil then
                                format_v = tostring(format_v)
                            end
                            if not StringIsEmpty(format_v) then
                                local args = string.split(format_v, "|")
                                re = string.format(re,unpack(args))
                            end
                        end
                    end
                end
                return re
            end
            -- ,__newindex = default_meta_newindex_func
        }
        -- meta_tab.__metatable = false
    end

    --准备特殊flag字段需要设置的数据
    local item_list_keys = nil
    local item_keys = nil
    if sheet_data_col_flags then
        for fk,fv in pairs(sheet_data_col_flags) do
            if fv == "itemlist" then
                if item_list_keys == nil then item_list_keys = {} end
                table.insert(item_list_keys,fk);
            elseif fv == "item" then
                if item_keys == nil then item_keys = {} end
                table.insert(item_keys,fk);
            end
        end
    end
    --设置表里每行的默认值元表
    if meta_tab ~= nil or item_list_keys ~= nil or item_keys ~= nil then
        for _,v in pairs(sheet_data) do
            if item_list_keys then
                for _,fk in pairs(item_list_keys) do
                    local il = rawget(v,fk)
                    if il then
                        for _,it in pairs(il) do
                            UnityEngine.Debug.LogError("SET ITEM META!!!!!!!!!!")
                            setmetatable(it,ITEM_META_TAB)
                        end
                    end
                end

            end
            if item_keys then
                for _,fk in pairs(item_keys) do
                    local it = rawget(v,fk)
                    if it then
                        UnityEngine.Debug.LogError("SET ITEM META!!!!!!!!!!")
                        setmetatable(it,ITEM_META_TAB)
                    end
                end
            end
            if meta_tab then
                setmetatable(v,meta_tab)
            end
        end
    end
end
-- UnityEngine.Debug.LogError("EMPTY_TAB==" .. tostring(EMPTY_TAB))
function SetConfigDefaultVal(main_tab,default_tab,col_flags,has_sheet,share_str)
    if main_tab == nil then
        return
    end
    
    if default_tab == nil then
        return
    end

    if has_sheet == nil then
        has_sheet = true
    end
    if has_sheet then
        for sheet_key,sheet_data in pairs(main_tab) do
            local sheet_data_col_flags = col_flags and col_flags[sheet_key] or nil
            local sheet_data_default = default_tab[sheet_key]
            set_sheet_default_tab(sheet_data,sheet_data_default,sheet_data_col_flags,share_str)
        end
    else
        set_sheet_default_tab(main_tab,default_tab,col_flags,share_str)
    end
end

