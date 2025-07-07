EditorSettings = {}

local UnityPrefs = UnityEngine.PlayerPrefs

function EditorSettings:OfficialLogin()
	return IS_EDITOR == false or UnityPrefs.GetInt("EditorEnableSimularAgentLogin",0) ~= 0
end

function EditorSettings:AgentId()
	if IS_EDITOR == false or UnityPrefs.GetInt("EditorEnableSimularAgentLogin",0) == 0 then
		return nil
	end
	return UnityPrefs.GetString("EditorSimularLoginAgent","dev")
end

function EditorSettings:IsEditorYYNet()
	if IS_EDITOR then
		return UnityPrefs.GetInt("EditorEnableYYNet",0)
	else
		return 0
	end
	
end

function EditorSettings:UpdateExternalLua()
	return IS_EDITOR == false or UnityPrefs.GetInt("ExternalLuaCheckRemote",0) ~= 0
end

function EditorSettings:NotUpdateAssetInEditor()
	return IS_EDITOR == false or UnityPrefs.GetInt("EditorUpdateAssetCacheOrNot",0) == 0
end

function EditorSettings:SimularIphoneX()
	return IS_EDITOR and UnityPrefs.GetInt("SimularIphoneX",0) == 1 or UnityPrefs.GetInt("SimularIphoneX",0) == 2 or UnityPrefs.GetInt("SimularIphoneX",0) == 3
end