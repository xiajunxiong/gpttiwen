JoySeeksFavorsData = JoySeeksFavorsData or BaseClass()

function JoySeeksFavorsData:__init()
    if JoySeeksFavorsData.Instance ~= nil then
        Debuger.LogError("[JoySeeksFavorsData] attempt to create singleton twice!")
        return
    end
    JoySeeksFavorsData.Instance = self
    self.view_data =
        SmartData.New(
        {
            is_match = 0,
            ready_tamp = 0,
            show_tamp = 0,
            next_move_tamp = 0,
            opera = false,
            map_index = 0,
            map_index2 = 1,
            has_find = 0,
            joy_seq = 0,
        }
    )
    self.myself_map = SmartData.New({})
    self.enemy_map = SmartData.New({})
    self.catch_tips= SmartData.New({val = ""})
	self.my_egg_info = {1,1,1}
    self.ey_egg_info = {1,1,1}
    
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoJoySeeksFavors,function() 
            self.all_info = Config.joy_seeks_favors_auto
            self.reward_info = self.all_info.seek_pets
        end,true)
end
function JoySeeksFavorsData:Reward()
    return self.reward_info[1].reward_item
end
function JoySeeksFavorsData:ViewData()
    return self.view_data
end
function JoySeeksFavorsData:Clear()
	self.view_data.is_match = 0
	self.view_data.ready_tamp = 0
	self.view_data.show_tamp = 0
	self.view_data.next_move_tamp = 0
	self.view_data.opera = false
	self.view_data.map_index1 = 0
	self.view_data.map_index2 = 1
	self.view_data.has_find = 0
	self.view_data.joy_seq = 0
    self.myself_map:Set({})
    self.enemy_map:Set({})
    self.my_egg_info = {1,1,1}
    self.ey_egg_info = {1,1,1}
    --[[ local base_data = BattleData.Instance:BaseData()
    base_data.server_fighting = false
    base_data.battle_mode = BATTLE_MODE.BATTLE_MODE_NULL ]]
end
function JoySeeksFavorsData:SetMySelfMap(data)
    self.myself_map:Set(data)
end
function JoySeeksFavorsData:SetEnemyMap(data)
    self.enemy_map:Set(data)
end
function JoySeeksFavorsData:MySelfMap()
    return self.myself_map
end
function JoySeeksFavorsData:EnemyMap()
    return self.enemy_map
end
function JoySeeksFavorsData:SetFindInfo(info)
    self.view_data.has_find = info.has_find
    self.view_data.joy_seq = info.joy_seq
end
function JoySeeksFavorsData:MyEggInfo(joy_seq)
	if joy_seq == nil then
		return self.my_egg_info	
	end
	self.my_egg_info[joy_seq] = 0
end
function JoySeeksFavorsData:EyEggInfo(joy_seq)
	if joy_seq == nil then
		return self.ey_egg_info	
	end
	self.ey_egg_info[joy_seq] = 0
end
function JoySeeksFavorsData:GetFailTip()
    return 
end

function JoySeeksFavorsData:SetCatchTips(tips)
    self.catch_tips.val = tips
end