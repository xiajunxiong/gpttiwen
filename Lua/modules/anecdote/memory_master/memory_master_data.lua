
MemoryMasterData = MemoryMasterData or BaseClass()
function MemoryMasterData:__init()
    if MemoryMasterData.Instance ~= nil then
        Debuger.LogError("[MemoryMasterData] attempt to create singleton twice!")
        return
    end
    MemoryMasterData.Instance = self
    self:MemoryMasterData()
end

function MemoryMasterData:MemoryMasterData()
    self.config = Config.memory_master_auto
    self.config_other = self.config.other[1]

    self.cards_list = {}
    for _, v in ipairs(self.config.cards_group) do
        local t = {}
        for i = 1, 12 do
            t[i] = {value = v["card_"..i]}
        end
        table.insert(self.cards_list, t)
    end
    self.show_list = {}
    self.card_history = 0
    self.card_succ = SmartData.New({index = 0, num = 0, succ = false})
    self.count_down = SmartData.New({value = 0})
    self.card_effect = SmartData.New({index = 0})
end

function MemoryMasterData:RemTime()
    return self.config_other.rem_time
end

function MemoryMasterData:PlayTime()
    return self.config_other.play_time
end

function MemoryMasterData:GetShowList()
    self.show_list = self.cards_list[GameMath.Ramdon(1, #self.cards_list)]
    return self.show_list
end

function MemoryMasterData:StartReset()
    self.card_history = 0
    self.card_succ:Set({
        index = 0,
        num = 0,
        succ = false
    })
    self.count_down.value = 0
end

function MemoryMasterData:CountDownStart(index)
    if 0 >= self.count_down.value then
        self.count_down.value = index
    end
end

function MemoryMasterData:CardShowed(index, value)
    local to_behind, hide = false, false
    if 0 == self.card_history then
        self.card_history = index
    elseif self.card_history == index then
    else
        local value_pre = self.show_list[self.card_history] and self.show_list[self.card_history].value or 0
        hide = value_pre == value
        to_behind = not hide
        self.card_succ:Set({
            index = self.card_history,
            num = hide and self.card_succ.num + 2 or self.card_succ.num,
            succ = hide,
        })
        self.card_history = 0
    end
    return to_behind, hide
end

function MemoryMasterData:CardShow(index, value)
    local value_pre = self.show_list[self.card_history] and self.show_list[self.card_history].value or 0
    self.card_effect.index = value_pre == value and self.card_history or 0
    return value_pre == value
end