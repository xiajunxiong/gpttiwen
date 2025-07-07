local_config = {
	data = {
		config_url = "",
		report_url = "",
	},
	loaded = false,
}

local file_path = UnityEngine.Application.persistentDataPath .. "/Config.txt"

function local_config:Get()
	if self.loaded == false then
		if FileTool.Exists(file_path) then
			local config_str = FileTool.ReadAllText(file_path)
			self.data = cjson.decode(config_str)
		else
			self.data = {}
		end
		self.loaded = true
	end
	return self.data
end

function local_config:Save()
	if not IS_AUDIT_VERSION then
		FileTool.WriteAllText(file_path,cjson.encode(self.data))
	end
end

return local_config