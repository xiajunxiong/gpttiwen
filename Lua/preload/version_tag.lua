VersionTag = {
	Test = "Test",		--测试字段
	ShadowSize = "ShadowSize", --shasowCamera中添加SetSize接口可由Lua层调用设置
	NotAndroidXJar = "NotAndroidXJar", --包内应用了非androidX jar包
	ClipAnima2D = "ClipAnima2D", --1、含有2d骨骼切割的cs代码 2、将Transform注册为RectTransform的基类到Lua
	SkillPlayerStop = "SkillPlayerStop", --skill player 添加Stop接口给Lua调用
	ShowCGSupport = "ShowCGSupport",--VideoPlayer注册到Lua中方便使用Pause等接口、
	LuaPegLib = "LuaPegLib",    --装载新的匹配lpeg机制
	LineRenderer = "LineRenderer", --线渲染器
	TextExtend = "TextExtend", --Text相关 TextGenerator、UILineInfo等
	AssmgrCleanWithoutFI = "AssmgrCleanWithoutFI", --AssetManager清理缓存，不清理FileInfo
	PlatformNative = "PlatformNative",		--PlatformNative 注册到Lua中
	FileInfoJson = "FileInfoJson",			--FileInfo信息存为json格式
	RemoveGCloud = "RemoveGCloud",			--移除GCloud语音库，减少包大小
	WebGLRTDFlags = "WebGLRTDFlags",		--RunTimeDetails里webgl的标识符
	ManifestFileBundleInfoHash = "ManifestFileBundleInfoHash",	--
	GameAgentPC = "GameAgentPC", 					--PC端sdk（360sdk）
	HMACSHA1 = "HMACSHA1",							--添加HMACSHA1加密，QQPCsdk中使用
	AssetCachePath = "AssetCachePath",				--返回AssetCachePath接口
	GameQuitDelg = "GameQuitDelg",					--PC端sdk添加关闭窗口返回值回调
	PCUserAuthentic = "PCUserAuthentic",				--PC端sdk添加实名制检查接口
	AxisState = "AxisState",						--AxisState结构体注册到Lua
}

local tag_val_cache = {}

local internal_version_tag = Game.VersionTag

function VersionTag.HasTag(tag)
	local cach_val = tag_val_cache[tag]
	if cach_val == nil then
		cach_val = internal_version_tag ~= nil and internal_version_tag.HasTag(tag) or false
		tag_val_cache[tag] = cach_val
	end
	return cach_val
end
--Example: LogError("VersionTag test", VersionTag.HasTag(VersionTag.Test))