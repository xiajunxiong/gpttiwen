
RTParseType = {
    TopLayerFloating = 1,
    TopLayerMarquee = 2,
    SocietyMail = 3,
    SocietyChat = 4,
    QuickChat = 5,
}

function RichTextHelper.ParseAll(str, is_rt, parse_type)
    str = RichTextHelper.ParseSwitch(str, "/(%d+)e", RichTextHelper.ParseEmoj, is_rt, {parse_type = parse_type})
    str = RichTextHelper.ParseSwitch(str, "({.-})", RichTextHelper.ParseOne, is_rt, {parse_type = parse_type})
    return str
end

-- 这个方法用于解决特殊情况 比如主界面和聊天界面的表情大小不一样这样
function RichTextHelper.ParseAllFunc(str, is_rt, func, parse_type)
    str = RichTextHelper.ParseSwitch(str, "/(%d+)e", RichTextHelper.ParseEmoj, is_rt, {func = func, parse_type = parse_type})
    str = RichTextHelper.ParseSwitch(str, "({.-})", RichTextHelper.ParseOne, is_rt, {func = func, parse_type = parse_type})
    return str
end

function RichTextHelper.ParseSwitch(str, pattern, parse_func, is_rt, others)
    local s, e, e_l, f_str, p_str
    repeat
        s, e, f_str = string.find(str, pattern, e or 1)
        if nil == p_str then
            p_str = string.sub(str, 1, s and s - 1 or string.len(str))
        else
            p_str = p_str .. string.sub(str, e_l + 1, s and s - 1 or string.len(str))
        end
        if nil ~= f_str then
            local result = parse_func(f_str, is_rt, others or {})
            if "table" == type(result) then
                return result
            else
                p_str = p_str .. result
            end
        end
        e_l = e
    until nil == s
    return p_str
end

function RichTextHelper.ParseOne(str, is_rt, others)
    local a, b = string.find(str, ";")
    if a == nil then
        return str
    end
    local mark, params, strs = string.sub(str, 2, a - 1), string.sub(str, b + 1, string.len(str) - 1), string.sub(str, 2, string.len(str) - 1)
    local param_t, str_t = string.split(params, ";"), string.split(strs, ";")
    local parse_func = is_rt and RTParseLink[mark] or RTParseShow[mark]
    others.str_t = str_t
    return nil ~= parse_func and parse_func(param_t, others) or mark
end

function RichTextHelper.ParseEmoj(str, is_rt, others)
    return (others and others.func) and others.func(ChatItemEmoj.ItemParse(tonumber(str))) or str
end