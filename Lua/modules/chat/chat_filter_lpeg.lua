local lpeg = require"lpeg"
local word_pinyin_cfg = config_word_pinyin

local mid_partt_no_num = (lpeg.R("\0\47")+lpeg.R("\58\64")+lpeg.R("\123\127")+lpeg.S("，。、：？‘’“”！《》·"))^0
local mid_partt = (lpeg.R("\0\64")+lpeg.R("\123\127")+lpeg.S("，。、：？‘’“”！《》·"))^0
--检查混编start 计算屏蔽词partt的时候包含以下内容混编的时候需要注意
local num_parrt = (lpeg.R("\48\57"))^1
--检查混编end------
ChatFilterLpeg = ChatFilterLpeg or {
    find_parttern_cache = {},
    word_pinyin_parttern_cache = {},
    warning_filters = nil,
    cur_war_idx = 1,
}



local function parttToCapture(partt)
    local re_val = lpeg.P{ lpeg.Cp() * partt * lpeg.Cp() + 1 * lpeg.V(1) }
    return re_val
end

local function getPinyinPartten(chi_ch)
    local re_val = ChatFilterLpeg.word_pinyin_parttern_cache[chi_ch]
    if re_val == true then
        return nil
    end
    if re_val ~= nil then
        return re_val
    end
    local chi_pinyin = word_pinyin_cfg[chi_ch]
    if chi_pinyin == nil then
        ChatFilterLpeg.word_pinyin_parttern_cache[chi_ch] = true
        return nil
    end

    local upper_str = string.upper(chi_pinyin)
    local lower_str = string.lower(chi_pinyin)
    for i=1, #chi_pinyin do
        local up_c = string.sub(upper_str,i,i)
        local lo_c = string.sub(lower_str,i,i)
        local patt = lpeg.P(up_c) + lpeg.P(lo_c)
        -- LogError(up_c,lo_c)
        if re_val ~= nil then
            re_val = re_val * patt
        else
            re_val = patt
        end
    end
    ChatFilterLpeg.word_pinyin_parttern_cache[chi_ch] = re_val
    return re_val
    -- LogError(lower_str)
    -- return lpeg.P(lower_str)
end


local function getFindPartten(filter_str)
    local i = 1
    local partt = nil
    local need_pinyin_match = true
    if #filter_str == 3 then
        local c = string.sub(filter_str,0,0)
        local b = string.byte(c)
        need_pinyin_match = false -- һ������������ʱ����ƥ��ƴ��
    end
    while i <= #filter_str do 
        local c = string.sub(filter_str,i,i)
        local b = string.byte(c)
        if b > 128 then
            local chi_ch = string.sub(filter_str,i,i+2)
            local chi_pinyin_partt = need_pinyin_match and getPinyinPartten(chi_ch) or nil
            local chi_partt = nil
            if chi_pinyin_partt ~= nil then
                chi_partt = lpeg.P(chi_ch) + chi_pinyin_partt
            else
                chi_partt = lpeg.P(chi_ch)
            end
            if partt ~= nil then
                partt = partt * mid_partt * chi_partt
            else
                partt = chi_partt
            end
            i = i + 3
        else

            if partt ~= nil then
                if num_parrt:match(c) then
                    partt = partt * mid_partt_no_num * lpeg.P(c)
                else
                    partt = partt * mid_partt * lpeg.P(c)
                end
            else
                partt = lpeg.P(c)
            end
            i = i + 1
        end	
    end
    return parttToCapture(partt)
end

function ChatFilterLpeg.GetFindParttern(filter_str)
	local re = ChatFilterLpeg.find_parttern_cache[filter_str]
	if re == nil then
        re = getFindPartten(filter_str)
		ChatFilterLpeg.find_parttern_cache[filter_str] = re
	end
	return re
end

function ChatFilterLpeg.PartternFind(content,lgep_partt)
    local s, e = lgep_partt:match(content)
    if e ~= nil and type(e) == "number" then
        e = e - 1
    end
    return s, e
    
end
-- local cross_frame = 0
local frame_war_count = 10

function ChatFilterLpeg.WarningupFilterList(filter_list)
    if #filter_list <= 0 then
        return
    end
    ChatFilterLpeg.warning_filters = filter_list
    ChatFilterLpeg.cur_war_idx = 1
    -- cross_frame = 0
end

function ChatFilterLpeg.Update()
    if ChatFilterLpeg.warning_filters == nil then
        return
    end
    -- cross_frame = cross_frame + 1
    for i = 1,frame_war_count do
        local cur_fil = ChatFilterLpeg.warning_filters[ChatFilterLpeg.cur_war_idx]
        if cur_fil == nil then
            ChatFilterLpeg.warning_filters = nil
            return
        end
        if ChatFilterLpeg.find_parttern_cache[cur_fil] == nil then
            local partten = getFindPartten(cur_fil)
            ChatFilterLpeg.find_parttern_cache[cur_fil] = partten
        end
        if ChatFilterLpeg.cur_war_idx >= #ChatFilterLpeg.warning_filters then
            ChatFilterLpeg.warning_filters = nil
            return
        else
            ChatFilterLpeg.cur_war_idx = ChatFilterLpeg.cur_war_idx + 1
        end
    end
end