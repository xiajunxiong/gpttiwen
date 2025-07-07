PublicPopupData = PublicPopupData or BaseClass()

function PublicPopupData:__init()
	if PublicPopupData.Instance ~= nil then
		Debuger.LogError("[PublicPopupData] attempt to create singleton twice!")
		return
	end
	PublicPopupData.Instance = self

	self.base_data = SmartData.New({
		wait_str = "",
		})
	self.click_data = SmartData.New({
		position = Vector3.zero
	})

	self.reward2_data = {
		items = nil,	---items
		hint_text = nil, ---string
		close_func = nil,--close func
	}

	self.battle_cinematics_data = {
		view_type =	BattleCinematics.Type.Normal, ---BattleCinematics.Type
		opened_func = nil,				--打开完成后的回调
		close_func = nil,				--关闭后的回调
		show_time = 0,	--界面展示时间 [0 or nil]=由外部手动关闭， （0，n]=固定时长关闭
		close_func_time = nil,	--close_func执行的时间，从动态完全展示完成开始计时，nil表示界面关闭时回调
	}

	self.top_layer_black = SmartData.New{
		is_show = false,
	}
end

function PublicPopupData:__delete()

end

function PublicPopupData:Base()
	return self.base_data
end
function PublicPopupData:ClickData()
	return self.click_data
end

function PublicPopupData:Reward2Data()
	return self.reward2_data
end

function PublicPopupData:SetReward2Data(items,hint_text,close_func,title_name)
	self.reward2_data.items = items
	self.reward2_data.hint_text = hint_text
	self.reward2_data.close_func = close_func
	self.reward2_data.title_name = title_name
end
function PublicPopupData:BattleCinematicsData()
	return self.battle_cinematics_data
end
function PublicPopupData:SetWarningData(data)
	self.public_warning_data = data
end
function PublicPopupData:GetWarningData()
	return self.public_warning_data
end