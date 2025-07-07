InstallPackageInfo = InstallPackageInfo or {}

local pkg_data = {
	agent_id = nil,
	version = nil,
	pkg = nil,
}

local ApplicationProductName = nil
local streamingAssetsPath = UnityEngine.Application.streamingAssetsPath
if Nirvana.StreamingAssets.Existed("PackageInfo.txt") == true then
	local txt_data = Nirvana.StreamingAssets.ReadAllText("PackageInfo.txt")
	if IS_EDITOR then
		print("PLATFORM = "..PLATFORM.." txt_data = "..tostring(txt_data))
	end
	pkg_data = cjson.decode(txt_data)
elseif PLATFORM == PlatformType.Unknow then
	pkg_data = {agent_id = "naq", version = UnityEngine.Application.version}
elseif PLATFORM == PlatformType.IOS then
	pkg_data = {agent_id = "naq", version = UnityEngine.Application.version}
elseif PLATFORM == PlatformType.Android then
	pkg_data = {agent_id = "naq", version = UnityEngine.Application.version}
end

function InstallPackageInfo.AgentId()
	local editor_agent_id = EditorSettings:AgentId()
	if editor_agent_id ~= nil then
		return editor_agent_id
	end
	return pkg_data.agent_id
end

function InstallPackageInfo.Version()
	return pkg_data.version
end

function InstallPackageInfo.Pkg()
	return UnityEngine.Application.version
end

function InstallPackageInfo.Date()
	if pkg_data.data == nil or pkg_data.data == "" then
		return ""
	else
		return pkg_data.data
	end
end

function InstallPackageInfo.Time()
	return pkg_data.time or 0
end

function InstallPackageInfo.IsYouYan()
	if IS_EDITOR and EditorSettings:OfficialLogin() then
		return EditorSettings:IsEditorYYNet()
	else
		return pkg_data.yy_net and tonumber(pkg_data.yy_net) or 0
	end
end

function InstallPackageInfo.IsSmallPkg()
	return pkg_data.s_pkg and tonumber(pkg_data.s_pkg) or 0
end

function InstallPackageInfo.MiniGameType()
	return pkg_data.mini_game and tonumber(pkg_data.mini_game) or 0
end

function InstallPackageInfo.RootPkg()
	return pkg_data.root_pkg and tonumber(pkg_data.root_pkg) or 0
end

function InstallPackageInfo.IsSingle()
	return pkg_data.single ~= nil and pkg_data.single
end

function InstallPackageInfo.PCSDKTYPE()
	return pkg_data.pc_sdk_type ~= nil and pkg_data.pc_sdk_type or ""
end