local lua_file_require_handle = {
	req_file_list = {},
	reqed_i = 0,
	last_per = 0,
}

-- local require_files = {

-- } 
-- local view_requires = {}

local tmp_dynamic_req_i_offset = 0
function VIEW_REQ(view_file)
	table.insert(lua_file_require_handle.req_file_list,view_file)
end

function FILE_REQ(lua_file)
	tmp_dynamic_req_i_offset = tmp_dynamic_req_i_offset + 1
	table.insert(lua_file_require_handle.req_file_list,lua_file_require_handle.reqed_i+tmp_dynamic_req_i_offset,lua_file)
end

local require_lists = {
	require("systool/systool_require"),
	require("managers/manager_require_list"),
	require("protocol/protocol_require_list"),
	require("config/config_require_list"),
	require("modules/common/modules_require_list"),
	require("modules/widgets/uiwidgets_require_list"),
	-- view_requires,
	-- {"modules/common/view_config"},
}




function lua_file_require_handle:RequireOne()
	-- UnityEngine.Debug.LogError("#self.req_file_list=" .. #self.req_file_list .. "self.reqed_i=" .. self.reqed_i)
	if #self.req_file_list <= self.reqed_i then
		return
	end
	self.reqed_i = self.reqed_i + 1
	tmp_dynamic_req_i_offset = 0
	local file_name = self.req_file_list[self.reqed_i]
	if ExternalLuaUpdater and ExternalLuaUpdater:IsExternal(file_name) then
		ExternalLuaLoader:Require(file_name)
	else
		-- local status = xpcall(function ()
			require(file_name)
		-- end,function (err)
		-- 	UnityEngine.Debug.LogError("err:"..err.."\nfile_name:"..file_name.."\nreqed_i"..self.reqed_i)
		-- end)
	end
end

function lua_file_require_handle:RequiredPercent()
	local per=  self.reqed_i / #self.req_file_list
	if per > self.last_per then
		self.last_per = per
	end
	return self.last_per
end

function lua_file_require_handle:Reset()
	self.req_file_list = {}
	for _,fl in ipairs(require_lists) do
		for _,lf in ipairs(fl) do
			table.insert(self.req_file_list,lf)
		end
	end
	self.reqed_i = 0
end

function lua_file_require_handle:GetCurReqedFile()
	return self.req_file_list[self.reqed_i]
end

return lua_file_require_handle
-- return {require_files,require_lists}