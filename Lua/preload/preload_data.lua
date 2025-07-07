PreloadData = PreloadData or BaseClass()
PreloadData.ProgSpeed = {
	Slow = 0,
	Fast = 1,
}

PreloadData.State = {
	None = -1,
	Connect = 0,
	UpdateCheck = 1,
	Update = 2,
	ExtluaUpdate = 3,
	UpdateLuaBundle = 4,
	Require = 5,
}

function PreloadData:__init()
	if PreloadData.Instance ~= nil then
		UnityEngine.Debug.LogError("[PreloadData] attempt to create singleton twice!")
		return
    end
	PreloadData.Instance = self


	--init query config connect data
	self.connect_retry_count = 1
	self.connect_max_retry_count = 1

	--asset update data
	self.update_error = false
	self.updated_bytes = 0
	self.update_total_bytes = 0
	self.update_speed = 0
	self.cache_update_speed = 0
	self.cache_update_speed_count = 0

	--lua require progress data
	self.script_require_per = 0	--require lua脚本的进度[0,1]

	self.update_luabundle_per = 0 --webGL下载lua的进度[0,1]
	self.progress_speed = PreloadData.ProgSpeed.Slow
	self.state = 0
end

function PreloadData:__delete()
	if PreloadData.Instance == self then
		PreloadData.Instance = nil
	end
end


function PreloadData:SetProgSpeed(speed)
	self.progress_speed = speed
end

function PreloadData:GetProgSpeed()
	return self.progress_speed
end

function PreloadData:GetScriptRequirePer()
	return self.script_require_per
end

function PreloadData:SetScriptRequirePer(per)
	self.script_require_per = per
end

function PreloadData:GetUpdateLuabundlePer()
	return self.update_luabundle_per
end

function PreloadData:SetUpdateLuabundlePer(per)
	self.update_luabundle_per = per
end

function PreloadData:GetExtluaUpdatePer()
	if ExternalLuaUpdater and ExternalLuaUpdater:GetProgressData() then
		return ExternalLuaUpdater:GetProgressData().total or 0
	end
	return 0
end

function PreloadData:GetState()
	return self.state
end

function PreloadData:SetState(state)
	self.state = state
end

function PreloadData:SetUpdateSpeed(speed)
	self.cache_update_speed = self.cache_update_speed + speed
	self.cache_update_speed_count = self.cache_update_speed_count + 1
	if self.cache_update_speed_count > 20 then	--间隔时间刷新下载速度
		self.update_speed = self.cache_update_speed/self.cache_update_speed_count
		self.cache_update_speed_count = 0
		self.cache_update_speed = 0
	elseif self.update_speed == 0 then
		self.update_speed = speed
	end
end

function PreloadData:GetUpdateSpeed()
	return self.update_speed
end

function PreloadData:SetUpdatedBytes(updated_bytes)
	self.updated_bytes = updated_bytes
end

function PreloadData:GetUpdatedBytes()
	return self.updated_bytes
end

function PreloadData:SetUpdateTotalBytes(total_bytes)
	self.update_total_bytes = total_bytes
end

function PreloadData:GetUpdateTotalBytes()
	return self.update_total_bytes
end

function PreloadData:SetConnectRetryCount(count)
	self.connect_retry_count = count
end

function PreloadData:GetConnectRetryCount()
	return self.connect_retry_count
end

function PreloadData:SetConnectMaxRetryCount(count)
	self.connect_max_retry_count = count
end

function PreloadData:GetConnectMaxRetryCount()
	return self.connect_max_retry_count
end

function PreloadData:SetUpdateError(error)
	self.update_error = error
end

function PreloadData:GetUpdateError()
	return self.update_error
end