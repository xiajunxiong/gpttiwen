local external_lua_loader = {
	

}

function external_lua_loader:Require(file_name)
	--读取包外外部文件(下载目录)
	-- local local_config_data = ExternalLuaUpdater:GetConfigData()
	-- local local_file_data = local_config_data ~= nil and 
	-- 	local_config_data.act_file_list ~= nil and local_config_data.act_file_list or {
	-- 		-- {
	-- 		-- 	filename = ExternalLua.C_AutoTreasureActivity,
	-- 		-- },
	-- 	}
	-- 	LogError("external_lua_loader:Require===",file_name,local_file_data)
	-- for k,v in pairs(local_file_data) do
		local ext_file_name = file_name-- .. ".lua"
		-- if v.filename == ext_file_name then
			-- LogError("Try New Require===",ext_file_name)

			local file_path = ExternalLuaUpdater.FileDir() .. ext_file_name
			if FileTool.Exists(file_path) == true then
				-- LogError("New Require===",ext_file_name)
				local luaStr = FileTool.ReadAllText(file_path)
				local l_fun = loadstring(luaStr)
				l_fun()
				return
			end
		-- end
	-- end

	require(file_name)
end

return external_lua_loader