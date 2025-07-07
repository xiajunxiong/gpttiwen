--data_config 结构
-- {
-- 	"act_version": "123456",
-- 	"act_update_url": "http:\/\/192.168.11.5:82\/tt01\/cn\/client\/version\/act_conf_1\/",
-- 	"act_update_url2": "http:\/\/192.168.11.5:82\/tt01\/cn\/client\/version\/act_conf_1\/",
-- 	"act_file_list": [{
-- 		"filename": "combineserveractivity_auto.lua",
-- 		"v": "8025471",
-- 		"s": "123"
-- 	}, {
-- 		"filename": "commonactivitycfg_auto.lua",
-- 		"v": "8025471",
-- 		"s": "1234"
-- 	}]
-- }
local PULL_CONFIG_MAX_RETRY = 20
local DOWNLOAD_MAX_RETRY = 20



ExternalLuaErrorCode = {
	PullConfigFailed = 1,		--配置拉取失败
	FileDownloadFailed = 2,		--文件下载失败
	CancelByNewPerform = 3,		--被新的执行请求打断
}


local ext_lua_flag = nil

local SIMULAR_FAILED = false

local remoteConfigFailedPer = 80
local downloadFileFailedPer = 80


local random_seed_num = -2147483648

local function probability(per)
	random_seed_num = random_seed_num + 1
	math.randomseed(tostring(os.time() + random_seed_num):reverse():sub(1, 7))
	local rnd_num = math.random(0, 100)
	return rnd_num <= per
end


local external_lua_updater = {
	local_config_data = nil,
	remote_config_data = nil,		--远程下载下来的更新信息,更新的流程中临时有值
	update_com_callback = nil,
	update_file_data = nil,
	retry_count = 0,
	error = nil,
	progress_data={
		total = 0,		--总进度0~1
	},

	--download state use
	cur_download_index = 0,

	--状态机相关
	state_funcs = nil,
	cur_state_index = 0,
	next_state_index = 0,

	perform_index = 0,		--只递增，用于做异步多次执行容错
}

-- local config_data_url = "http://192.168.11.5:81/api/act_query.php?spid=dev&version&pkg=XXX&asset=XXX"

local file_dir = UnityApp.persistentDataPath .. "/ExternalScript/"
function external_lua_updater.FileDir()
	return file_dir
end

local function localConfigPath()
	return external_lua_updater.FileDir() .. "config_data.txt"
end

function external_lua_updater:IsExternal(file_name)
	if ext_lua_flag == nil then
		self:GetConfigData()
		if ext_lua_flag == nil then
			ext_lua_flag = {}
		end
	end
	return ext_lua_flag[file_name] == true
end

function external_lua_updater:GetConfigData()
	local config_data_url = GLOBAL_CONFIG:Urls().act_query_url
	if config_data_url == nil or config_data_url == "" then
		return nil
	end
	if self.local_config_data == nil then
		ext_lua_flag = {}
		-- local local_config_data = nil
		local file_path = localConfigPath()
		if FileTool.Exists(file_path) then
			self.local_config_data = cjson.decode(FileTool.ReadAllText(file_path))
		end
		if self.local_config_data and self.local_config_data.act_file_list then
			for _,v in  pairs(self.local_config_data.act_file_list) do
				ext_lua_flag[v.filename] = true
			end
		end
	end
	return self.local_config_data
end

function external_lua_updater:PerformUpdate(com_callback)
	self.perform_index = self.perform_index + 1
	if self.update_com_callback ~= nil then
		self.error = ExternalLuaErrorCode.CancelByNewPerform
		self:completeCallback()
	end

	self.update_com_callback = com_callback
	--重置状态机,如果前一次状态机未执行完将会被直接停止
	self.cur_state_index = 0
	self.next_state_index = 0
	self:nextState()
end

function external_lua_updater:Update()
	-- print("Update!!!!!cur index===" .. tostring(self.cur_state_index))
	if self.next_state_index > 0 then --begin new state
		self.cur_state_index = self.next_state_index
		self.next_state_index = 0
		local cur_state_funcs = self.state_funcs[self.cur_state_index]
		cur_state_funcs.start(self)
	elseif self.cur_state_index > 0 then
		local cur_state_funcs = self.state_funcs[self.cur_state_index]
		if cur_state_funcs ~= nil and cur_state_funcs.update ~= nil then
			cur_state_funcs.update(self)
		end
	end
end

function external_lua_updater:GetProgressData()
	return self.progress_data
end

function external_lua_updater:GetError()
	return self.error
end

function external_lua_updater:GetUpdateFiles()
	local file_names = {}
	if self.update_file_data ~= nil then
		for k,v in pairs(self.update_file_data) do
			table.insert(file_names,v.filename)
		end
	end
	-- table.insert(file_names,ExternalLua.C_AutoTreasureActivity)
	return file_names
end

function external_lua_updater:prepare()
	self.error = nil
	self.retry_count = 0
	self.progress_data.total = 0
	-- self:GetConfigData()
	self:nextState()
end

function external_lua_updater:loadRemoteConfigData()
	if self.retry_count > PULL_CONFIG_MAX_RETRY then
		self.error = ExternalLuaErrorCode.PullConfigFailed
		print("pull config failed!")
		self:completeCallback()
		return
	end
	local config_data_url = GLOBAL_CONFIG:Urls().act_query_url
	if config_data_url == nil or config_data_url == "" then
		print("external_lua_updater:Complete with remote colse")
		self:completeCallback()
		return
	else
		local pkg_config = GLOBAL_CONFIG:PkgInfo()
		config_data_url = string.format("%s?spid=%s&version=%s&pkg=%s&asset=%s",config_data_url,
			pkg_config.agent_id,
			pkg_config.version,
			pkg_config.pkg, 
			pkg_config.assets_version)
		print("request remote external lua config:" .. config_data_url)
	end

	local cur_index = self.perform_index
	WebRequester:Request(config_data_url, function (data, req_err)
		if cur_index ~= self.perform_index then
			return
		end
		if SIMULAR_FAILED and probability(remoteConfigFailedPer) then
			data = "remote config Simluar failed"
		end
		--data_list request call back ,parse data
		local suc = false
		if data ~= nil and data ~= "" then
			local status, config_data = xpcall(function() 
					return cjson.decode(data)
				end, 
				function (err)
					local trace = tostring(debug.traceback())
					UnityEngine.Debug.LogError("url is = "..config_data_url.."\nerror="..err.."\n"..trace)
				end)
			if status then
				suc = true
				self.remote_config_data = config_data
				--caclulate file need udpate
				self.update_file_data = {}
				local remote_file_data = self.remote_config_data ~= nil and 
					self.remote_config_data.act_file_list ~= nil and 
					self.remote_config_data.act_file_list or {}
				local local_config_data = self:GetConfigData()
				local local_file_data = local_config_data ~= nil and 
					local_config_data.act_file_list ~= nil and local_config_data.act_file_list or {}

				ext_lua_flag = {}
				for k,v in pairs(self.remote_config_data.act_file_list) do
					-- v.filename = "config/auto/" .. v.filename
					ext_lua_flag[v.filename] = true
					local match = false
					for lk,lv in pairs(local_file_data) do
						if lv.filename == v.filename and lv.v == v.v then
							match = true
							break
						end
					end
					if not match then
						table.insert(self.update_file_data, v)
					end
				end
			end		
		end
		--change state
		if not suc then
			self.retry_count = self.retry_count + 1
			self:retryCurState()	
		else
			print("end config get state,update_file_count===" .. tostring(#self.update_file_data))
			self.retry_count = 0
			self:nextState()
		end
	end,WebRequestDataType.Text)	
end

-- local update_count = 0
function external_lua_updater:updateFiles()
	self.retry_count = 0
	self.cur_download_index = 0

	-- update_count = 0
	self:downloadFile(1)
end

function external_lua_updater:downloadFile(index)
	self.cur_download_index = index

	if self.cur_download_index > #self.update_file_data then	--download state end
		self:nextState()	
		return
	end
	local file_data = self.update_file_data[self.cur_download_index]
	if self.retry_count > DOWNLOAD_MAX_RETRY then		--perform error!
		print("download file error!file=" .. file_data.filename)
		self.error = ExternalLuaErrorCode.FileDownloadFailed
		self:completeCallback()
		return
	end
	local url = (self.retry_count == 0 or self.retry_count%3~=0) and 
		self.remote_config_data.act_update_url or self.remote_config_data.act_update_url2
	url = string.format("%s%s.lua?v=%s",url,file_data.filename,file_data.v)
	print("external_lua_updater download url===" .. url)
	WebRequester:Request(url,function(text,err)
		self:updateFiles_OnOneComplete(text,err)
	end)
end

function external_lua_updater:updateFiles_OnOneComplete(text,err)

	local next_index = self.cur_download_index
	local www_error = err
	if SIMULAR_FAILED and probability(downloadFileFailedPer) then
		www_error = "Simular failed"
	end
	if www_error ~= nil and www_error ~= "" then	--retry
		print("download:" .. self.update_file_data[self.cur_download_index].filename .. ",error:" .. tostring(www_error))
		self.retry_count = self.retry_count + 1
	else					--download suc md5 check and write file
		local cur_dl_data = self.update_file_data[self.cur_download_index]
		local write_path = external_lua_updater.FileDir() .. cur_dl_data.filename
		local dir_path = FileTool.GetDirName(write_path)
		if not FileTool.DirExists(dir_path) then
			FileTool.CreateDir(dir_path)
		end
		local tmp_path = write_path-- .. ".tmp"
		local ddd = text

		FileTool.WriteAllText(tmp_path,ddd)
		--check md5
		local saved_md5 = string.upper(GameUtil.FileMd5(tmp_path))

		if saved_md5 ~= cur_dl_data.v then --md5 error
			self.retry_count = self.retry_count + 1
			print("md5 check failed:" .. cur_dl_data.filename .. ",retmote md5=" .. cur_dl_data.v .. ",local md5=" .. saved_md5)
		else
			-- AXX.FileToolToLua.MoveFile(tmp_path,write_path)
			self.progress_data.total = self.cur_download_index / #self.update_file_data
			next_index = next_index + 1
			self.retry_count = 0
			-- print("write suc:" .. cur_dl_data.filename)
		end
	end
	self:downloadFile(next_index)
end

function external_lua_updater:onUpdateComplete()
	--write remote 
	if self.error == nil or self.error == "" then
		local text_data = cjson.encode(self.remote_config_data)
		local file_path = localConfigPath()
		local dir_path = FileTool.GetDirName(file_path)
		if not FileTool.DirExists(dir_path) then
			FileTool.CreateDir(dir_path)
		end		
		FileTool.WriteAllText(file_path,text_data)
		self.local_config_data = self.remote_config_data
		self.remote_config_data = nil
	end
	self:completeCallback()
end

function external_lua_updater:completeCallback()
	if self.update_com_callback ~= nil then
		local tmp_callback = self.update_com_callback
		self.update_com_callback = nil
		tmp_callback()
	end
end

function external_lua_updater:nextState()
	self.next_state_index = self.cur_state_index + 1
end

function external_lua_updater:retryCurState()
	self.next_state_index = self.cur_state_index
end


local state_funcs = {}
table.insert(state_funcs,{start=external_lua_updater.prepare})
table.insert(state_funcs,{start=external_lua_updater.loadRemoteConfigData})
table.insert(state_funcs,{start=external_lua_updater.updateFiles--[[,update=external_lua_updater.updateFiles_Update]]})
table.insert(state_funcs,{start=external_lua_updater.onUpdateComplete})

external_lua_updater.state_funcs = state_funcs

return external_lua_updater