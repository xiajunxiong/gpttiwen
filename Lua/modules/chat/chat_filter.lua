
------------------------------------------------------
--聊天过滤，也称敏感词过滤
--@author bzw
------------------------------------------------------
ChatFilter = ChatFilter or BaseClass(BaseCtrl)

local pre_filter_list =  config_chatfilter_list[GLOBAL_CONFIG:ChatFilterTable()] or config_chatfilter_list
if ChatFilterLpeg ~= nil then
	ChatFilterLpeg.WarningupFilterList(pre_filter_list)
end
function ChatFilter:__init()
	if ChatFilter.Instance ~= nil then
		Debuger.LogError("[ChatFilter] attempt to create singleton twice!")
		return
	end	
	ChatFilter.Instance = self
	self.filter_list = pre_filter_list
	self.filter_list_username = config_usernamefilter_list
	self.filter_link_list = {}
	self.find_parttern_cache = {}
	self:InsertChannelFilter()
	self:GetHttpFilterKey()
end

function ChatFilter:__delete()
	if ChatFilter.Instance == self then
		ChatFilter.Instance = nil
	end
end

-- TODO
function ChatFilter:InsertChannelFilter()
end

function ChatFilter:GetHttpFilterKey()
	if GLOBAL_CONFIG:Flags().sensitive then
		local chatFilterUrl = GLOBAL_CONFIG:Urls().sensitive_words_url
		if chatFilterUrl == nil or StringIsEmpty(chatFilterUrl) then
			LogError("GetHttpFilterKey return 1")
			return
		end
		local url_format = "%s?spid=%s"
		local send_url = string.format(url_format,chatFilterUrl, GLOBAL_CONFIG:Urls().spid)
		--发送
		WebRequester:Request(send_url,function(data,error)
			if StringIsEmpty(error) then
				if StringIsEmpty(data) then
					LogError("GetHttpFilterKey return 2")
					return
				end
				local filter_info, status = TryParseJson(data)
				if status == false then
					UnityEngine.Debug.LogError("chatFilterUrl return not json ")
					return
				end
				for k,v in pairs(filter_info) do
					table.insert(self.filter_list,v)
				end
			end
		end)
	end
end

--过滤敏感词，将过敏感词用*号代替
function ChatFilter:Filter(content)
	-- content = self:FilterNgui(content)
	local linkHand = self:LinkHandler(content, false)
	local match_list = self:GetMatchList(linkHand, self.filter_list)
	local linkHan2 = self:ReplaceMatch(match_list, content)
	return self:LinkHandler(linkHan2, true)
end

--过滤敏感名，将过敏感词用*号代替
function ChatFilter:FilterUserName(content)
	local match_list = self:GetMatchList(self:LinkHandler(content, false), self.filter_list, self.filter_list_username)
	return self:LinkHandler(self:ReplaceMatch(match_list, content), true)
end

--过滤ngui富文本
function ChatFilter:FilterNgui(content)
	content = string.gsub(content, "%[%w-%]" , "")
	content = string.gsub(content, "%[%/%w-%]" , "")
	content = string.gsub(content,"%[%-%]","")
	return content
end

--是否含有非法字符
function ChatFilter:IsIllegal(content, is_username)
	local match_list = 0
	if is_username then
		match_list = self:GetMatchList(content, self.filter_list_username)
		if #match_list <= 0 then
			match_list = self:GetMatchList(content, self.filter_list)
		end
	else
		match_list = self:GetMatchList(content, self.filter_list)
	end
	return #match_list > 0
end

function ChatFilter:GetMatchList(content, filter_list, name_filter_list)
	if filter_list == nil then 
		return nil
	end
	local match_list = {}
	local len = #filter_list
	for i=1,len do
		local match = filter_list[i]
		local find_parttern = self:internalGetFindParttern(match)
		local s, e = self:internalPartternFind(content, find_parttern)
		if s ~= nil and e ~= nil and e >= s then
			local t = {}
			t.match = content.sub(content,s,e) --match
			t.len = self:Utfstrlen(t.match)
			table.insert(match_list, t)
		end
	end
	if name_filter_list ~= nil then
		local name_len = #name_filter_list
		for i=1,name_len do
			local match = name_filter_list[i]
			local find_parttern = self:internalGetFindParttern(match)
			local s, e = self:internalPartternFind(content, find_parttern)
			if s ~= nil and e ~= nil and e >= s then
				local t = {}
				t.match = content.sub(content,s,e) --match
				t.len = self:Utfstrlen(t.match)
				table.insert(match_list, t)
			end
		end
	end
	return match_list
end

function ChatFilter:internalGetFindParttern(filter_str)
	if VersionTag.HasTag(VersionTag.LuaPegLib) then
		return ChatFilterLpeg.GetFindParttern(filter_str)
	else
		return self:getFindParttern(filter_str)
	end
end

function ChatFilter:internalPartternFind(content,partt)
	if VersionTag.HasTag(VersionTag.LuaPegLib) then
		return ChatFilterLpeg.PartternFind(content,partt)
	else
		return string.find(content, partt)
	end
end

function ChatFilter:getFindParttern(filter_str)
	local re = self.find_parttern_cache[filter_str]
	if re == nil then
		-- re = ""
		local i = 1
		while i <= #filter_str do 
			local c = string.sub(filter_str,i,i)
			local b = string.byte(c)
			if b > 128 then
				if i ~= 1 then
					re = re .. "[%p%c%w%s]*" .. string.sub(filter_str,i,i+2)
				else
					re = string.sub(filter_str,i,i+2)
				end
				i = i + 3
			else
				-- if b == 32 then
				-- 	LogError("empty",c,",str=",filter_str)
				-- else
					if i ~= 1 then
						re = re .. "[%p%c%w%s]*" .. c
					else
						re = c
					end
				-- end
				i = i + 1
			end	
			-- if i > #filter_str then
			-- 	break
			-- end
		end
		self.find_parttern_cache[filter_str] = re
	end
	-- LogError("getFindParttern",filter_str,re)
	return re
end

function ChatFilter:ReplaceMatch(match_list, content)
	if match_list == nil then return end

	function sortfun(a, b)
		return a.len > b.len
	end
	table.sort(match_list, sortfun)

	for k,v in pairs(match_list) do
		local r_str = ""
		for i=1,v.len do
			r_str = r_str .. "*"
		end
		content = string.gsub(content, v.match , r_str)
	end

	return content
end

function ChatFilter:Utfstrlen(str)
	local len = #str;
	local left = len;
	local cnt = 0;
	local arr={0,0xc0,0xe0,0xf0,0xf8,0xfc};
	while left ~= 0 do
	local tmp=string.byte(str,-left);
	local i=#arr;
	while arr[i] do
	if tmp>=arr[i] then left=left-i;break;end
	i=i-1;
	end
	cnt=cnt+1;
	end
	return cnt;
end

-- 字符中链接过滤的处理
-- 先替换掉链接，避免被过滤掉
-- 最后过滤完后再替换回来
function ChatFilter:LinkHandler(content, is_replace)
	content = content or ""
	if is_replace then
		for i,v in ipairs(self.filter_link_list) do
			content = string.gsub(content, "#link#", v, 1)
		end
		self.filter_link_list = {}
	else
		local arr = Split(content, "{")
		for i=1,#arr do
			local s, e = string.find(content, "{.-}")
			if nil ~= s and nil ~= e then
				local str = string.sub(content, s, e)
				table.insert(self.filter_link_list, str)
				content = string.gsub(content, str, "#link#", 1)
			end
		end		
	end
	return content
end