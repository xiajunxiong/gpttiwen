DataHelper = {}
DataHelper.sprite_save = {}
function DataHelper.GetDaXie(num, fan_ti)
    -- [0,1000)的数字可以使用此方法
    if type(num) ~= "number" or num < 0 or num > 1000 then 
        return Language.DataHelper.DaXie[1]
    end
    local language = fan_ti and Language.DataHelper.FanTi or Language.DataHelper.DaXie

    local function dx(num)
        return language[num]
    end

    local str = ""
    if num > 10 then
        local ge = num % 10
        local shi = math.floor(num / 10) % 10
        local s = "s"
        if num > 99 then
            local bai = math.floor(num / 100) % 10
            local b = "b"
            if num % 100 == 0 then
                str = dx(bai) .. dx(b)
            elseif num % 100 < 10 then
                str = dx(bai) .. dx(b) .. dx(0) .. dx(ge)
            elseif ge == 0 then
                str = dx(bai) .. dx(b) .. dx(shi) .. dx(s)
            else
                str = dx(bai) .. dx(b) .. dx(shi) .. dx(s) .. dx(ge)
            end
        elseif ge == 0 then
            str = dx(shi) .. dx(s)
        elseif shi == 1 then
            str = dx(s) .. dx(ge)
        else
            str = dx(shi) .. dx(s) .. dx(ge)
        end
    else
        str = dx(num)
    end
    return str
end

function DataHelper.GetRomanNum(num)
    if 0 < num and num <= 10 then
        return RomanNums[num]
    elseif num < 20 then
        return RomanNums[10] .. RomanNums[num % 10]
    end
end

--复制第一层
function DataHelper.TableCopy(t, n)
	if nil == t then return nil end
	if nil == n then
		n = 0
	end
	local new_t = {}
	for k, v in pairs(t) do
		if n > 0 and type(v) == "table" then
			local T = TableCopy(v, n - 1)
			new_t[k] = T
		else
			new_t[k] = v
		end
	end
	return new_t
end

DataSorter = {}

--[[
从大到小排序的算子(用于表项)
@sort_key_name 需要比较的表项中的key
--]]
function DataSorter.KeyUpperSorter(sort_key_name)
    return function(a, b)
        if a[sort_key_name] > b[sort_key_name] then
            return true
        else
            return false
        end
    end
end

--[[
从小到大排序的算子(用于表项)
@para2 sort_key_name 需要比较的表项中的key
--]]
function DataSorter.KeyLowerSorter(sort_key_name)
	return function(a, b)
		if a[sort_key_name] < b[sort_key_name] then
			return true
		else
			return false
		end
	end
end

--由名字重置为属性索引 "gongji" => BattleAttriType.Attack
--PS：如果传入空表则返回一张属性为0的属性表
function DataHelper.ResetAttrList(attr_list)
    local attr_value_list = {}
    for k,v in pairs(AttrTypeForName) do
        attr_value_list[v] = attr_list[k] or 0
    end
    return attr_value_list
end

--两个属性列表相加 是否需要 key 类型
function DataHelper.AttrListAdd(attr_list_1,attr_list_2,is_key_type)
    local attr_value_list = {}
    for k,v in pairs(AttrTypeForName) do
        local value_1 = 0
        if attr_list_1[k] then
            value_1 = attr_list_1[k]
        elseif attr_list_1[v] then
            value_1 = attr_list_1[v]
        end
        local value_2 = 0
        if attr_list_2[k] then
            value_2 = attr_list_2[k]
        elseif attr_list_2[v] then
            value_2 = attr_list_2[v]
        end
        if not is_key_type then
            attr_value_list[v] = value_1 + value_2
        else
            attr_value_list[k] = value_1 + value_2
        end
    end
    return attr_value_list
end

-- 计算字符串字符个数"中文字算一个字符"
function DataHelper.GetStringWordNum(str)
    str = tostring(str)
    local lenInByte = #str
    local count = 0
    local i = 1
    while true do
        local curByte = string.byte(str, i)
        if i > lenInByte then
            break
        end
        local byteCount = 1
        if curByte > 0 and curByte < 128 then
            byteCount = 1
        elseif curByte>=128 and curByte<224 then
            byteCount = 2
        elseif curByte>=224 and curByte<240 then
            byteCount = 3
        elseif curByte>=240 and curByte<=247 then
            byteCount = 4
        else
            break
        end
        i = i + byteCount
        count = count + 1
    end
    return count
end

function DataHelper.GetStringWordToTable(str)
    local t = {}
    local lenInByte = #str
    local i = 1
    while true do
        local curByte = string.byte(str, i)
        if i > lenInByte then
            break
        end
        local byteCount = 1
        if curByte > 0 and curByte < 128 then
            byteCount = 1
        elseif curByte>=128 and curByte<224 then
            byteCount = 2
        elseif curByte>=224 and curByte<240 then
            byteCount = 3
        elseif curByte>=240 and curByte<=247 then
            byteCount = 4
        else
            break
        end
        table.insert(t, string.sub(str, i, i + byteCount - 1))
        i = i + byteCount
    end
    return t
end

--@G新转换成中文大写方法 满足1000以上 可拓展
function DataHelper.GetWord(num)
    local table = Language.DataHelper.Word
    local function GetOne(num)
        return num ~= 0 and table[num] or ""
    end
    local function GetTen(num,is_single)
        return num ~= 0 and table[num]..table[10] or table[num]
    end
    local function GetHund(num)
        return num ~= 0 and table[num]..table[11] or table[num]
    end
    local function GetThousand(num)
        return num ~= 0 and table[num]..table[12] or ""
    end
    local post,list = DataHelper.GetValueLength(num)
    for i=1,post do
        list[i] = tonumber(list[i])
    end
    if post == 1 then
        return table[num]
    elseif post == 2 then
        return list[1] ~= 1 and GetTen(list[1])..GetOne(list[2]) or table[10]..GetOne(list[2])
    elseif post == 3 then
        local one,ten,hun = list[3],list[2],list[1]
        if one == 0 and ten == 0 then
            return GetHund(hun)
        elseif one == 0 and ten ~= 0 then
            return GetHund(hun) .. GetTen(ten)
        end
        return GetHund(hun) .. GetTen(ten) .. GetOne(one)
    elseif post == 4 then
        local one,ten,hun,tho = list[4],list[3],list[2],list[1]
        if one == 0 and ten == 0 and hun == 0 then
            return GetThousand(tho)
        elseif one == 0 and ten == 0 and hun ~= 0 then
            return GetThousand(tho) .. GetHund(hun)
        elseif one == 0 and ten ~= 0 and hun ~= 0 then
            return GetThousand(tho) .. GetHund(hun) .. GetTen(ten)
        elseif one ~= 0 and ten == 0 and hun == 0 then
            return GetThousand(tho) .. table[0].. GetOne(one)
        end
        return GetThousand(tho) .. GetHund(hun) .. GetTen(ten) .. GetOne(one)
    else
        return table[list[1]]..table[13]
    end
end

--@G计算数组里面的值 多用于二进制数组
function DataHelper.GetFetchFlagNum(can_fetch,has_fetch)
    local num = 0
    if has_fetch ~= nil then
        for i = 0,32 do
            if can_fetch[32 - i] == 1 and has_fetch[32 - i] == 0 then
                num = num + 1
            end
        end
    else
        for k,v in pairs(can_fetch or {}) do
            num = num + v
        end
        return num
    end
    return num > 0 and 1 or 0
end

--[[@G
    -- @desc: 计算活动使用的进度值 各种阶段不同用
    -- author:{guojinbin}
    -- time:2019-01-29 15:19:24
    -- --@value:进度值s
    -- --@list: 不同阶段的表 如：{[1] = 0,[2] = 200,[3] = 1000}
    -- --@is_percent:是否是百合比 默认的值是从0~1    
    -- @return:得到一个准确的比值
]]

function DataHelper.GetFillAmount(value,list,is_percent)
    if value >= list[#list] then 
        return is_percent and 100 or 1
    end
    local num = 0
    for i=1,#list do
        if i ~= #list and value > list[i] and value <= list[i + 1]then
            local max_num = (list[i + 1] - list[i]) * (#list - 1)
            local prop = (value - list[i])/max_num
            num = (i - 1) /(#list - 1) + prop
        end
    end
    return is_percent and num * 100 or num
end

--@G获取一个值的长度 返回一个按高位排序的数值数组
function DataHelper.GetValueLength(value,len)
    if value == 0 then return 0,{} end
    local list = {}
    if len then
        value = string.format(("%0" .. len .. "d"),value) --格式化数字 使 1 变成 01
    end
    local s = tostring(value)
    local k = string.len(s)
    for i=1,k do
        list[i] = string.sub(s,i,i)
    end
    return k,list
end

--@G解析奖励物品配置 对 14124|14140|14184 进行处理
function DataHelper.SplitData(content,txt)
    if content == nil then return end
    local show_list = {}
    if string.find(tostring(content),tostring(txt)) == nil then
        return {Item.InitData(tonumber(content))}
    else
        local list = Split(tostring(content), tostring(txt))
        for i,v in ipairs(list or {}) do
            table.insert(show_list,Item.InitData(tonumber(v)))
        end
    end
    return show_list
end

--[[@G
 --    @desc: 技能描述 table描述的配置 对 {param_i} 类型进行替换处理
 --    author:{guojinbin}
 --    time:2018-11-28 17:23:20
 --    --@table:技能配置table skill_dec param_1 .. param_n
	-- --@strat_num:开始位置
	-- --@end_num:结束位置
	-- --@is_nought: 是否参数从4开始但技能描述从1开始
 --    @return:
]]
function DataHelper.GetSkillDesc(table,strat_num,end_num,is_nought)
    local skill_dec = table.skill_dec
    for i= strat_num or 1, end_num or 3 do
        local param = 0
        if table["param_"..i] then
            param = table["param_"..i]
        end
        if table["param"..i] then
            param = table["param"..i]
        end
        if param then
            if is_nought then
                i = i - (strat_num-1)
            end
            skill_dec = string.gsub(skill_dec, "{param_"..i.."}", param)
        end
    end
    return skill_dec
end

--@G获取消耗数量 提供两个数量及界面背景深浅
function DataHelper.ConsumeNum(cur_num, need_num, shallow)
    if cur_num ~= nil then
        need_num = need_num or 1
        local is_enough = cur_num >= need_num
        local color = is_enough and COLORSTR.Green9 or COLORSTR.Red10
        if shallow == true then
            color = is_enough and COLORSTR.Green7 or COLORSTR.Red10
        end
        return Format(Language.Common.StuffNumColorFirst,color,cur_num,need_num)
    end
end

--获取保留小数点几位 不进行四合五入
function DataHelper.GetPreciseDecimal(num, n)
    if type(num) ~= "number" then
        return num
    end
    n = n or 0
    n = math.floor(n)
    if n < 0 then
        n = 0
    end
    local decimal = 10 ^ n
    local temp = math.floor(num * decimal)
    local result = temp / decimal
    return result
end

--@G转换财富
function DataHelper.ConverMoney(value)
    if value == nil or not value then return 0 end
    if value >= 10000 and value < 100000000 then
        local result = DataHelper.GetPreciseDecimal(value / 10000,1)
        local a,b = math.modf(result)
        if b == 0 then
            return a .. Language.Common.Wan
        end
        return result .. Language.Common.Wan
    end
    if value >= 100000000 then
        local result = DataHelper.GetPreciseDecimal(value / 100000000,1)
        local a,b = math.modf(result)
        if b == 0 then
            return a .. Language.Common.Yi
        end
        return result .. Language.Common.Yi
    end
    return value
end

--@G封装一个属性列表控件使用的list
function DataHelper.FormAttrList(attr_list,is_all)
    local list = {}
    local AttrList = Language.Common.AttrList
    for k, v in pairs(AttrTypeForName) do
        local attr_value = attr_list[k] or attr_list[v]
        if attr_value and (attr_value > 0 or is_all) then
            table.insert(list, {
                attr_type = v,
                attr_name = AttrList[v],
                attr_value = attr_value
            })
        end
    end
    if #list > 1 then
        table.sort(list,DataHelper.SortFunc([[attr_type]]))
    end
	return list
end

--是否是百分比属性
function DataHelper.IsPercent(AttriType)
    if AttriType == nil then
        return false
    end
    if AttriType == BattleAttriType.AntiAll then
        return true
    end
    if AttriType == BattleAttriType.Damage then
        return true
    end
    if AttriType == BattleAttriType.GeDang then
        return false
    end
    if AttriType >= BattleAttriType.PhysicsReduce and AttriType < BattleAttriType.AntiValueEnd then
        return AttriType ~= BattleAttriType.MagicAttack and AttriType ~= BattleAttriType.MagicDefense and AttriType ~= BattleAttriType.ZhenShiShangHai
    end
    return false
end

--@G百分比属性处理
function DataHelper.Percent(attr_type,attr_value)
    if DataHelper.IsPercent(attr_type) then
        return Percent(attr_value)
    end
    return attr_value
end

--@G是否加点属性
function DataHelper.IsPoint(attr_type)
    if attr_type >= BattleAttriType.VITALITY and attr_type <= BattleAttriType.MAGIC then
        return true
    end
    return false
end

--@G获取分页数据
function DataHelper.GetPageData(list,count)
    local page_list = {}
	for i=1,#list do
        local index = math.ceil(i / (count or 1))
        if page_list[index] == nil then
            page_list[index] = {}
        end
        list[i].index = i
        table.insert(page_list[index],list[i])
    end
    return page_list
end

--@G格式化列表 处理0位置  index:开始位置，默认为0
function DataHelper.FormatItemList(item_list,index)
    if item_list == nil then return {} end
    local list = {}
    for i = index or 0, #item_list do
        if item_list[i] ~= nil then
            table.insert(list,Item.Create(item_list[i]))
        end
    end
    return list
end

--@G格式化列表 处理0位置
function DataHelper.FormatList(item_list,start_index,end_index)
    local list = {}
    for i = start_index or 0, end_index or #item_list do
        if item_list[i] ~= nil then
            table.insert(list,item_list[i])
        end
    end
    return list
end

--@G获取属性改变列表 {k = attr_type,v = attr_value}
function DataHelper.GetChangeList(old_list,attr_list)
    local list = {}
    for at,av in pairs(attr_list or {}) do
        local ov = old_list[at] or 0
        if at < BattleAttriType.AntiValueEnd and av - ov ~= 0 then
            list[at] = list[at] or {}
            list[at].change_val = av - ov
            list[at].old_val = av
        end
    end
    return list
end

--@G倒转列表
function DataHelper.ReverseList(old_list)
    local list = {}
    for i=#old_list,1,-1 do
		table.insert(list,old_list[i])
	end
	return list
end

--@G自定义条件func_list排序weight
function DataHelper.WeightSort(k,...)
    local func_list = {...}
    return function(a,b)
        local a_sort, b_sort = a[k], b[k]
        for k,v in pairs(func_list) do
            a_sort = a_sort + (v(a) or 0)
            b_sort = b_sort + (v(b) or 0)
        end
        return a_sort < b_sort
    end
end

--@G简易排序 默认从小到大，reverse == true 从大到小
function DataHelper.SortFunc(key,reverse)
    return function(a,b)
        if reverse == true then
            return a[key] > b[key]
        end
        return a[key] < b[key]
    end
end

--@G数组排序，针对一堆数字排序
function DataHelper.NumSortFunc(reverse)
    return function(a,b)
        if reverse == true then
            return a > b
        end
        return a < b
    end
end

--@G获取列表所在位置Index和配置
function DataHelper.SimpleIndex(list,key,value)
    for k,v in pairs(list or {}) do
        if v[key] == value then
            return k,v
        end
    end
end

--@G获取列表所在位置Index和配置 自定义条件
function DataHelper.SimpleCondIndex(list,condition)
    for k,v in pairs(list or {}) do
        if condition(v) then
            return k,v
        end
    end
end

--@G自定义遍历列表搜索结果
function DataHelper.GetSearch(list,key,value)
    for k,v in pairs(list or {}) do
        if v[key] == value then
            return v
        end
    end
end

--@G自定义遍历列表搜索结果
function DataHelper.GetSearchCond(list,condition)
    for k,v in pairs(list or {}) do
        if condition(v) then
            return v
        end
    end
end

--@G拆分默认转数字 返回一个list
function DataHelper.SplitList(content,txt)
    local list = {}
    for i,v in ipairs(Split(content,txt)) do
        table.insert(list,tonumber(v) or v)
    end
    return list
end

--@G获取一组随机数组成的列表
function DataHelper.GetRamdonList(n,m)
    local function IsHasNum(list,num)
        for i,v in ipairs(list) do
            if v == num then
                return true
            end
        end
        return false
    end
    local list = {}
    local num = GameMath.Ramdon(n,m)
    while #list < m do
        while IsHasNum(list,num) do
            num = GameMath.Ramdon(n,m)
        end
        table.insert(list,num)
    end
    return list
end

--t @G需要移动的table n > 0 往右移n位 n < 0 往左移n位
function DataHelper.MoveLoop(t,n)
    for m=1,math.abs(n) do
		table.insert(t,n > 0 and 1 or #t,t[n > 0 and #t or 1])
		table.remove(t,n > 0 and #t or 1)
    end
    return t
end

--配置数据分组 例如：{{seq = 1, value = 1}, {seq = 1, value = 2}, {seq = 2, value = 1}, {seq = 2, value = 2}}
-- => {[1] = {{seq = 1, value = 1}, {seq = 1, value = 2}}, [2] = {{seq = 2, value = 1}, {seq = 2, value = 2}}}
function DataHelper.ConvertGroup(base_cfg, key, is_ipairs)
    local new_cfg = {}
    if is_ipairs then
        for i, v in ipairs(base_cfg) do
            if new_cfg[v[key]] == nil then
                new_cfg[v[key]] = {}
            end
            table.insert(new_cfg[v[key]], v)
        end
        return new_cfg
    end
	for i, v in pairs(base_cfg) do
		if new_cfg[v[key]] == nil then
			new_cfg[v[key]] = {}
		end
		table.insert(new_cfg[v[key]], v)
	end
    return new_cfg
end

--配置数据根据keys细分，一定能从keys中拿到对应的唯一一条数据，数据可以不从1开始，keys传多个键{"key1","key2"}
-- 相当于可以从多个键可以拿到唯一的一条数据config[key1][key2] = v
function DataHelper.Subdivision(base_cfg, keys)
    local new_cfg = {}
    local keys_num = 1
    keys_num = table.nums(keys)
    for i, v in pairs(base_cfg) do
        local tab = new_cfg
        for j = 1, keys_num do
            local key = keys[j]
            if j == keys_num then
                tab[v[key]] = v
            else
                if tab[v[key]] == nil then
                    tab[v[key]] = {}
                end
                tab = tab[v[key]]
            end
        end
	end
    return new_cfg
end

--解析字符串里的代码逻辑并执行加载字符串的函数调用 可传参数params
function DataHelper.LoadCall(str,params)
    load(Format([[return function(param) %s end]],str))()(params)
end

-- 获得某一行数据的属性列表，数据，是否清空值（用于0级的属性显示拿1级的置空），是否用属性类型作为key，字段名1，字段名2
function DataHelper.GetAttrList(config, is_zero, is_use_type_key, type_name, value_name)
    local index = 1
    local t = {}
    local type_name = type_name or "attr_type_"
    local value_name = value_name or "attr_add_"
    if TableIsEmpty(config) then
        return t
    end
    while config[value_name..index] and config[value_name..index] > 0 do
        local t1 = CommonStruct.AttrStruct(config[type_name..index], config[value_name..index])
        if is_zero then
            t1 = CommonStruct.AttrStruct(config[type_name..index], 0)
        end
        if is_use_type_key then
            t[t1.attr_type] = t1
        else
            table.insert(t, t1)
        end
        index = index + 1
    end
    return t
end