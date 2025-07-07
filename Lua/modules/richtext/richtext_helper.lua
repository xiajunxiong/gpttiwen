
RichTextHelper = {}
FILE_REQ("modules/richtext/richtext_loader")
FILE_REQ("modules/richtext/richtext_infos")
FILE_REQ("modules/richtext/richtext_helper_parse")
FILE_REQ("modules/richtext/richtext_helper_format")
FILE_REQ("modules/richtext/richtext_helper_parse_show")
FILE_REQ("modules/richtext/richtext_helper_parse_link")

RichTextMgr = Game.RichTextManager

RichTextType = {
    Normal = 1,
    Emoj = 2,
    Pet = 3,
    Item = 4,
    Recruit = 5,
    GroupInvite = 6,
    HongBao = 7,        --红包
    HongBaoGet = 8,     --抢红包通知,
    SocietyGift = 9,    --礼物
    SocietyGiftFlower = 10,    --鲜花
    SocietyShiTuInvite = 11,    --师徒邀请
    ShengQi = 12,   -- 圣器
}

function RichTextHelper.Bind(richtext, func)
    RichTextMgr.Bind(richtext, func)
end

function RichTextHelper.ColorStr(str, colorStr)
    return string.format("<color=#%s>%s</color>", colorStr, str)
end

function RichTextHelper.SizeStr(str, size)
    return string.format("<size=%s>%s</size>", size, str)
end

function RichTextHelper.BStr(str)
    return string.format("<b>%s</b>", str)
end

-- RichTextHelper.LinkStr(str, key1, val1, key2, val2, ...)
function RichTextHelper.LinkStr(str, ...)
    local t_kv = {...}
    local c = #t_kv
    local i = 0
    local function func_kv(s)
        i = i + 2
        return i < c + 1 and func_kv(s.." "..t_kv[i-1].."="..t_kv[i]) or s
    end
    return string.format("<a%s>%s</a>", func_kv(""), str)
end

-- RichTextHelper.LinkStrTab(str, 
--     {
--         key1 = val1,
--         ...
--     }
-- )
function RichTextHelper.LinkStrTab(str, param_t)
    local t = {}
    for key, val in pairs(param_t) do
        table.insert(t, key)
        table.insert(t, val)
    end
    return RichTextHelper.LinkStr(str, unpack(t))
end

function RichTextHelper.SpriteStrIcon(sprite, icon_type, w, h, other)
    other = other or {}
    local w = math.floor(w * (other.rate or 1))
    local h = math.floor(h * (other.rate or 1))
    local str = string.format("<s sprite=%s iconType=%s w=%s h=%s s=%s>%s</s>", sprite, icon_type, w, h, other.size or 0, other.str or "")
    if other.size then
        str = RichTextHelper.SizeStr(str, other.size)
    end
    return RichTextHelper.ColorStr(str, COLORSTR.Transparent)
end

-- atlas 请加在rich_text图集里
function RichTextHelper.SpriteStrAtlas(sprite, w, h, other, atlas)
    other = other or {}
    local w = math.floor(w * (other.rate or 1))
    local h = math.floor(h * (other.rate or 1))
    local str = string.format("<s sprite=%s atlas=%s w=%s h=%s opn=%s os=%s s=%s>%s</s>",
        sprite, atlas or "rich_text", w, h, other.pic_num or 1, other.speed or 1, other.size or 0, other.str or "")
    if other.size then
        str = RichTextHelper.SizeStr(str, other.size)
    end
    return RichTextHelper.ColorStr(str, COLORSTR.Transparent)
end

--c# Dictionary<string, string> ---> lua table
function RichTextHelper.DictionarySSToTable(dict, num_key)
    local t = {}
    local keys = dict.Keys
    local en = keys:GetEnumerator()
    while(en:MoveNext()) do
        _, t[num_key and tonumber(en.Current) or en.Current] = dict:TryGetValue(en.Current, nil)
    end
    return t
end

--解析UTF-8字符串成char_list
function RichTextHelper.ParseUTF8Str(str)
	local len = string.len(str) or 0
	local i = 1
	local char_list = {}
	local j = 1
	while i <= len do
		local byte = string.byte(str,i)
		local byte_count = 1
		if byte > 0 and byte <= 127 then
			byte_count = 1
		else
			byte_count = 3
		end
		local char = string.sub(str,i,i + byte_count - 1)
		char_list[j] = char
		j = j + 1
		i = i + byte_count
	end
	return char_list
end

-- 字符串匹配去除特殊字符 ( ) . % + - * ? [ ^ $
function RichTextHelper.RemoveSignBeforeMatch(content)
    content = string.gsub(content, "[%(%)%.%%%+%-%*%?%[%^%$]*", "")
    return content
end

--@G字符串匹配去除特殊字符 [] () {}
function RichTextHelper.RemoveMatch(content)
    return content:match("[^%[%]%（%）%{%}\x22]+")
end

--@G字符串处理成竖向字符串
function RichTextHelper.StringToVertical(str)
    local content = ""
    local list = DataHelper.GetStringWordToTable(str)
    for i,v in ipairs(list) do
        content = content .. v .. (i~=#list and "\n" or "")
    end
    return content
end

local match_img_cfg = {
    [65534] = RichTextHelper.SpriteStrAtlas(CommonItemId.Gold, 40, 40)
}

--@G解析文字中的带总括号内的参数并且处理成图片参数
function RichTextHelper.ParseMatchImg(str)
    local index,len,result,content,temp
    repeat
        index, len, result = str:find("(%[.-%])",len or 1)
        local sub_len = index and index - 1 or str:len()
        if nil == content then
            content = str:sub(1,sub_len)
        else
            content = content .. str:sub(temp + 1,sub_len)
        end
        if result ~= nil then
            local param = RichTextHelper.RemoveMatch(result)
            content = content .. match_img_cfg[tonumber(param)]
        end
        temp = len
    until nil == index
    return content
end