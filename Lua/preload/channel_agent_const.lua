ChannelAgentConsts = {}

local function isResCached(path)
	if ResourceManager ~= nil then
		return ResourceManager.HasBundle(path)
	else
		return AssetManager.IsVersionCached(path)
	end
end

local agent_res_path_format = nil
local function getAgentResPathFormat()
	if agent_res_path_format == nil then 
		local agent_id = InstallPackageInfo:AgentId()
		agent_res_path_format = string.format("agent/%s/",agent_id) .. "%s";
	end
	return agent_res_path_format
end

local function getStreamingAgentResPathFormat()
	return "NirvanaDeploy/agent/%s"
end

local video_agent_res_path_format = nil
local function videoAgentResPathFormat()
	if video_agent_res_path_format == nil then
		local agent_id = "ma1"--InstallPackageInfo:AgentId()
		video_agent_res_path_format = string.format("video/%s/",agent_id).."%s"
	end
	return video_agent_res_path_format
end
-- function ChannelAgentConsts.ReadAuditInfo()	
-- 	return Nirvana.StreamingAssets.ReadAllText(string.format(getStreamingAgentResPathFormat(),"AssetAudit.txt"))
-- end

function ChannelAgentConsts.HasSpecialRoleCreateVideo()
	local video_res_path = string.format(videoAgentResPathFormat(),"story.mp4")
	--LogError("video_res_path",video_res_path,isResCached(video_res_path))
	if isResCached(video_res_path) then
		return video_res_path
	end
end

function ChannelAgentConsts.LogoPath()
	local streaming_logo = string.format(getStreamingAgentResPathFormat(),"login_logo.png")
	local res_logo = string.format(getAgentResPathFormat(),"login_logo.png")
	--优先使用包外（bundle）资源，然后在找包内资源（agent文件夹中的），最后使用游戏默认资源
	if GLOBAL_CONFIG:Flags().show_default_logo then
		res_logo = "ui/textures/login/login_logo.png"
		return res_logo
	end
	if isResCached(res_logo) then
		return res_logo, false
	elseif Nirvana.StreamingAssets.Existed(streaming_logo) == true then
		return streaming_logo, true
	else
		res_logo = "ui/textures/login/login_logo.png"
		return res_logo
	end
end

-- function ChannelAgentConsts.LogoSPath()
-- 	local streaming_small_logo = string.format(getStreamingAgentResPathFormat(),"small_logo.png")
-- 	if Nirvana.StreamingAssets.Existed(streaming_small_logo) == true then
-- 		return streaming_small_logo, true
-- 	end
-- end

-- function ChannelAgentConsts.PublishInfoPath()
-- 	local streaming_publish_info = string.format(getStreamingAgentResPathFormat(),"publish_info.png")
-- 	if Nirvana.StreamingAssets.Existed(streaming_publish_info) == true then
-- 		return streaming_publish_info, true
-- 	end
-- end

function ChannelAgentConsts.UpdateBgPath(index)
	local streaming_update = string.format(getStreamingAgentResPathFormat(),"update_bg0.png")
	local res_update = string.format(getAgentResPathFormat(),"update_bg0.png")

	if Nirvana.StreamingAssets.Existed(streaming_update) == true then
		if index then
			local path = string.format(getStreamingAgentResPathFormat(),"update_bg"..tostring(index % 4)..".png")
			if Nirvana.StreamingAssets.Existed(path) then
				return path, true
			end
			-- UnityEngine.Debug.LogError("index = " ..tostring(index).." path "..tostring(path))
		end
		-- UnityEngine.Debug.logError("index = "..tostring(index).." streaming_update "..tostring(streaming_update))
		return streaming_update,true
	else
		return res_update,false
	end
end

function ChannelAgentConsts.SplashBgPath()
	local streaming_splash = string.format(getStreamingAgentResPathFormat(),"splash.png")
	if Nirvana.StreamingAssets.Existed(streaming_splash) == true then
		return streaming_splash
	end
end

-- function ChannelAgentConsts.UpdateBgAssetName()
-- 	return "update_bg"
-- end

local agent_login_path_cache = nil 
local agent_login_is_streaming_asset = false
function ChannelAgentConsts.LoginBgPath()
	if agent_login_path_cache == nil then
		agent_login_path_cache = string.format(getStreamingAgentResPathFormat(),"login_bg.png") --streamingasset中有的优先用
		if Nirvana.StreamingAssets.Existed(agent_login_path_cache) == true then
			agent_login_is_streaming_asset = true
		else
			agent_login_path_cache = string.format(getAgentResPathFormat(),"login_bg.png")
			if isResCached(agent_login_path_cache) == false then
				agent_login_path_cache = "ui/textures/login/login_bg1.png"			--default loading png
				agent_login_is_streaming_asset = nil
			end
		end
	end
	-- logError("agent_login_path_cache===" .. agent_login_path_cache)
	return agent_login_path_cache, agent_login_is_streaming_asset
end


local agent_loading_path_streaming = nil
local loading_path_ramdon_min = -1
local loading_path_ramdon_max = -1
local loading_path_format = nil
function ChannelAgentConsts.LoadingBgPath()
	if agent_loading_path_streaming == nil then
		function caculate_pic_exist_and_count(loading_dir,check_exist_func)
			local normal_dir = string.format(loading_dir,"loading.png")
			if check_exist_func(normal_dir) == true then
				loading_path_ramdon_min = 0
				loading_path_ramdon_max = 0
				for i = 1,6 do
					local temp_dir = string.format(loading_dir,"loading" .. tostring(i) .. ".png")
					if check_exist_func(temp_dir) == false then
						break
					else
						loading_path_ramdon_max = loading_path_ramdon_max + 1
					end
				end
				if loading_path_ramdon_max == 0 then
					loading_path_format = normal_dir
				else
					loading_path_format = string.format(loading_dir,"loading%s.png")
				end
				return true
			else
				return false
			end
		end
		
		if caculate_pic_exist_and_count(getAgentResPathFormat(),
			function(path) return isResCached(path) end) == true then
			agent_loading_path_streaming = false
		elseif caculate_pic_exist_and_count(getStreamingAgentResPathFormat(),
			function (path) return Nirvana.StreamingAssets.Existed(path) end) then
			agent_loading_path_streaming = true
		else
			loading_path_format = "ui/textures/loading/loading%s.png"
			loading_path_ramdon_min = 2
			loading_path_ramdon_max = 3
			agent_loading_path_streaming = nil
		end

	end

	if loading_path_ramdon_max == 0 then
		return loading_path_format,agent_loading_path_streaming
	else
		local rnd = math.random(loading_path_ramdon_min,loading_path_ramdon_max)
		if rnd == 0 then rnd = "" end
		return string.format(loading_path_format,tostring(rnd)),agent_loading_path_streaming
	end
end

-----AuditUI---------------------------
function ChannelAgentConsts.GetUIStremingAsset(img_name)
	local img_streaming_path = string.format(getStreamingAgentResPathFormat(),img_name..".png")
	if Nirvana.StreamingAssets.Existed(img_streaming_path) then
		return img_streaming_path, true
	end
	return "", false
end

function ChannelAgentConsts.SetRawImg(raw_img,img_path)
	local tex = ResourceManager.LoadStreamingTexture(img_path)
	raw_img.texture = tex
end