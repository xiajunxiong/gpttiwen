ActFlowerTouchData = ActFlowerTouchData or BaseClass()
function ActFlowerTouchData:__init()
    if ActFlowerTouchData.Instance ~= nil then
        Debuger.LogError("[ActFlowerTouchData] attempt to create singleton twice!")
        return
    end
    ActFlowerTouchData.Instance = self

    self.gameplay_data = SmartData.New({cur_point = 0,is_playing = false})
    self.flower_info = SmartData.New({times = 0,cur_intergral = 0,challenge_end_timestamp = 0})
    self.edit_save = nil
    
    self.red_info = SmartData.New({val = false})
end

function ActFlowerTouchData:__delete()
	ActFlowerTouchData.Instance = nil
end

function ActFlowerTouchData:ClearData()
    
end

function ActFlowerTouchData:initConfig()
    
end

function ActFlowerTouchData:GetFlowerList()
    local cur_index = 0
    local list = {}
    for i = 1,60 do
        cur_index = cur_index + 1 
        local vo = {}
        vo.index = cur_index 
        
        table.insert(list,vo)
    end 
    return list
end

function ActFlowerTouchData:GetPlayList()
    if self.edit_save ~= nil then 
        return self:FixStrFlowerList(self.edit_save)
    end 

    local mark_pos = GameMath.Ramdon(1,2) 
    if Config.yunzefanhualu_auto.other[1]["editor"..mark_pos] ~= "" then
        return self:FixStrFlowerList(Config.yunzefanhualu_auto.other[1]["editor"..mark_pos])
    end 

    local play = {
        [5] = {
            {index = 5,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 10,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 15,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
        },
        [15] = {
            {index = 5,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 10,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 15,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
        },
        [25] = {
            {index = 5,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 10,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 15,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
        },
        [35] = {
            {index = 5,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 10,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 15,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
        },
        [55] = {
            {index = 5,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 10,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
            {index = 15,ph1_speed = 1,ph2_speed = 1,ph3_speed = 1,ph4_speed = 1,},
        },
    }
    return play
end

function ActFlowerTouchData:FlushRed()
    local list = self:GetRewardList()
    local red = false
    for k,v in pairs(list) do 
        red = self.flower_info.cur_intergral >= v.integral and not v.is_got
        if red then break end 
    end 

    self.red_info.val = red
end

--
function ActFlowerTouchData:GetRewardList()
    local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD, Config.yunzefanhualu_auto.reward, "item_id")
    local base_list = Config.yunzefanhualu_auto.basics

    local neo_list = {}

    for k,v in pairs(base_list) do 
        local vo = {}
        vo.index = k
        vo.integral = v.integral

        for i,j in pairs(list) do 
            if k == j.item_id then 
                vo.reward_item = j.reward_item
                break
            end 
        end 

        vo.is_got = self.flower_info.reward_list ~= nil and  self.flower_info.reward_list[k] == 1 or false
        table.insert(neo_list,vo)
    end 

    return neo_list
end

function ActFlowerTouchData:OnFlowerInfo(protocol)
    -- LogDG("?OnFlowerInfo!",protocol)
    self.flower_info.times = protocol.times
    self.flower_info.cur_intergral = protocol.cur_intergral
    self.flower_info.challenge_end_timestamp = protocol.challenge_end_timestamp
    self.flower_info.reward_list = protocol.reward_list

    self:FlushRed()
end

function ActFlowerTouchData:GetTodayTime()
    local num = Config.yunzefanhualu_auto.other[1].frequency - self.flower_info.times
    return num
end

function ActFlowerTouchData:CheckIsGameStart()
    -- return self.gameplay_data.is_playing
    return self.flower_info.challenge_end_timestamp > 0
end


function ActFlowerTouchData:GamePlusClick(point)
    self.gameplay_data.cur_point = self.gameplay_data.cur_point + point
end

function ActFlowerTouchData:GameComplete()
    LogDG("花狩已结束！")
    self.gameplay_data.cur_point = 0
    self.gameplay_data.is_playing = false
end

function ActFlowerTouchData:GameStart()
    LogDG("花狩已开始！")
    self.gameplay_data.is_playing = true
end

function ActFlowerTouchData:FixStrFlowerList(str)
    local scond_str = Split(str,"|")

    local play_list = {}
    for k,v in pairs(scond_str) do 
        local flower_str = Split(v,":")
        local flower_list = {}
        flower_list.second = tonumber(flower_str[1])
        flower_list.data_list = {}
        for i = 2,#flower_str do 
            local cell_str = Split(flower_str[i],",")
            local vo = {}
            vo.index = tonumber(cell_str[1])
            vo.ph1_speed = tonumber(cell_str[2])
            vo.ph2_speed = tonumber(cell_str[3])
            vo.ph3_speed = tonumber(cell_str[4])
            vo.ph4_speed = tonumber(cell_str[5])
            
            table.insert(flower_list.data_list,vo)
        end 

        play_list[flower_list.second] = flower_list.data_list
    end 

    return play_list
end

function ActFlowerTouchData:CacheEditPlay(str)
    self.edit_save = str 
end