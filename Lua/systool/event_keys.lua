--key == string and key >= 100000
EventProtocol = {
	Start = "EventProtocol_100",
}

function EventProtocol.IdToEventKey(msg_type)
	return string.format("EventProtocol_%d",msg_type)
end

function EventProtocol.ToEventKey(protocol)
	local proc = ProtocolPool.Instance:GetProtocol(protocol)
	if proc ~= nil then
		return EventProtocol.IdToEventKey(proc.msg_type)
	else
		return nil
	end
end

---example key = string or int
GlobalEventKeys = {
	RoleNameChange = "RoleNameChange",
	TOUCH_BEGAN = "touch_began",										-- 触摸事件 按下
	TOUCH_MOVED = "touch_moved",										-- 触摸事件 移动
	TOUCH_ENDED = "touch_ended",										-- 触摸事件 抬起
	RoleLevelChange = "role_level_change",								-- 人物等级变化
	BattleSummon = "battle_summon",										-- 战斗召唤事件
	BattleEnter = "battle_enter",									 	-- 进入战斗
	BattleEnd = "battle_end",									 		-- 战斗结束
	RolePosChange = "role_pos_change",									-- 主角位置变化
	LoginSuccess = "LoginSuccess",										-- 登录成功
	OpenDialogView = "OpenDialogView",									-- 打开对话界面
	CloseDialogView = "CloseDialogView",								-- 关闭对话界面
	CloseMapMini = "CloseMapMini",										-- 关闭小地图界面
	GuideViewOpen = "GuideViewOpen",									-- 指引界面第一次打开
	GuideViewClose = "GuideViewClose",									-- 指引界面第一次关闭
	RoleNoticeComplete = "RoleNoticeComplete",                          -- 人物数据通知完毕
	RoleNoticeChange = "RoleNoticeChange",                              -- 人物数据通知改变
	RoleProfessionChange = "RoleProfessionChange",                      -- 人物职业改变
	RoleSettingChange = "RoleSettingChange",                            -- 人物设置改变
	SceneDataInited = "SceneDataInited",                            	-- 设置场景数据
	SceneDataDisposed = "SceneDataDisposed",                            -- 卸载场景数据
	RoleCreateEnterScene = "RoleCreateEnterScene",						-- 新建人物第一次进场景
	ServerOpenDayChange = "ServerOpenDayChange",                        -- 游戏天数改变
	TimeMinuteChange = "TimeMinuteChange",                        		-- 游戏时间分钟改变
	TaskCompleteDataNotice = "TaskCompleteDataNotice",					-- 任务完成列表下发通知
}