MainViewData = MainViewData or BaseClass()

function MainViewData:__init()
    if MainViewData.Instance ~= nil then
        Debuger.LogError("[MainViewData] attempt to create singleton twice!")
        return
    end

    MainViewData.Instance = self

    self.base = SmartData.New({is_battle = false})

    self.scene_obj_vos = SmartData.New() --key:vo,value:true or nil
    self.cur_game_state = SmartData.New({state = 0})
    -- 这不是战斗buff
    self.buff_list = SmartData.New()

    self.upgrade_list = SmartData.New()

    self.tt_task = SmartData.New {val = false}
    self.tt_team = SmartData.New {val = false}

    self.view_data = SmartData.New({
        flush_rb = false,
        hide_rb = false,
        hide_li = false,
        show_all = false,
        hide_lt = false,
        show_li = false,
        hide_task = false,
        show_task = false,
        show_lt = false,
        hide_la = false,
        show_la = false,
        force_hide_task = false,
        force_show_task = false,
    })
    --左上活动按钮 #62480又改成false咧
    self.left_top_icons_show = false
    self.rb_toggle = SmartData.New{isOn = false}
    self.rb_remind_nums = {}
    self.tl_remind_nums = {}
    self.remind_data = SmartData.New({flush_rb = false, flush_lt = false})
    self.show_mark = {}
    self.quietly_download_state_notify_flag = SmartData.New({flag = false})

    self.scene_ui_vos = {}
    --战斗buff仅供展示
    self.battle_buff_list = SmartData.New()
    self.custom_flush_smart = SmartData.New({taptap = false})
    self.battle_debuff_list = SmartData.New()

    self.loaded_rich_text_atlases = {rich_text_loaded = false, emoj_loaded = false}
end

function MainViewData:__delete()
    if MainViewData.Instance == self then
        MainViewData.Instance = nil
    end
end
function MainViewData:Clear()
    self.rb_remind_nums = {}
    self.tl_remind_nums = {}
    self.show_mark = {}
end

function MainViewData:RemoveBuffs()
    self.buff_list:Clear()
end
function MainViewData:NotifyBuffs()
    self.buff_list:Notify()
end

function MainViewData:SetBattle(battle)
    self.base.is_battle = battle
end

function MainViewData:BaseData()
    return self.base
end

function MainViewData:AddObjVo(vo)
    self.scene_obj_vos[vo] = true
end

function MainViewData:RemoveObjVo(vo)
    self.scene_obj_vos[vo] = nil
end

function MainViewData:ForeachObjVo(func)
    for vo, _ in pairs(self.scene_obj_vos) do
        local re = func(vo)
        if re == true then
            return
        end
    end
end

function MainViewData:SceneObjVos()
    return self.scene_obj_vos
end

function MainViewData:SetBuffInfo(protocol)
    self.buff_list:Clear()
    for i,v in ipairs(protocol.buff_list or {}) do
        self:AddBuff(v)
    end
end

local buff_id = 1
function MainViewData:AddBuff(data)
    local buff = SmartData.New(TableCopy(data))
    if buff_id > 10000 then
        buff_id = 1
    end
    buff.id = buff_id
    buff_id = buff_id + 1
    self.buff_list:Insert(buff, 1)
    if not buff.end_time or buff.end_time == 0 then
        buff.time_left = nil
    else
        if not buff then
            return
        end
        buff.time_left = buff.end_time
        if buff.time_left <= 0 then
            for k, v in pairs(self.buff_list) do
                if v == buff then
                    self.buff_list:Remove(k)
                    return
                end
            end
        end
    end
    return buff
end

function MainViewData:RemoveBuff(buff)
    for k, v in pairs(self.buff_list) do
        if v == buff then
            self.buff_list:Remove(k)
            return
        end
    end
end

function MainViewData:IsBattle(tips)
    if tips and self.base.is_battle then
        PublicPopupCtrl.Instance:Center(ErrorText[63])
    end
    return self.base.is_battle
end

function MainViewData:TopOfBuff()
    return self.buff_list[1]
end

function MainViewData:GetBuffCount()
    local num = 0
    for k, v in pairs(self.buff_list) do
        num = num + 1
    end
    return num
end

function MainViewData:BuffWhichHasLongestRemainingTime()
    local buff = self.buff_list[1]
    for k, v in pairs(self.buff_list) do
        if v.scene_buff > 0 then
            buff = v
            break
        end
        if buff.time_left and v.time_left then
            if buff.time_left < v.time_left then
                buff = v
            end
        end
    end
    return buff
end

function MainViewData:GetBuffDescr(data)
    if data.type == BuffType.CHANGE_ATTR then
        local des = ""
        local attrs = data.attrs
        local new_value = 0
        for i, v in ipairs(attrs) do
            local value = v.val or 0
            new_value = value
            if DataHelper.IsPercent(tonumber(v.type)) then
                new_value = Percent(value)
            end
            if value ~= 0 then
                if i % 2 == 1 then
                    des = des .. string.format("%s <color=#%s>%s</color>        ", Language.Common.AttrList[v.type], COLORSTR.Green7, value > 0 and "+"..new_value or new_value)
                else
                    des = des .. string.format("%s <color=#%s>%s</color>", Language.Common.AttrList[v.type], COLORSTR.Green7, value > 0 and "+"..new_value or new_value)
                    if i ~= #attrs then
                        des = des .. "\n"
                    end
                end
            end
        end
        return des, math.floor((#attrs + 1) / 2)
    elseif data.type == BuffType.EXP_UP then
        return string.format("%s<color=#%s>%d%%</color>", Language.Main.GameBuff.Exp, COLORSTR.Green7, data.up_per), 1
    elseif data.type == BuffType.CHANGE_MOVE_SPEED then
        return string.format("%s<color=#%s>%d%%</color>", Language.Main.GameBuff.MoveSpeed, COLORSTR.Red10, data.change_per), 1
    elseif data.type == BuffType.RUSH_DUNGEON then
        local cfg = RushDungeonData.Instance:BuffCfg(data.real_buff_id)
        return cfg.desc
    end
end

function MainViewData:GetBuffIconName(data)
    local val = 0
    if data.type == BuffType.CHANGE_ATTR then
        if #data.attrs == 1 and data.attrs[1].type <= 4 then
            return "BuffAttr" .. data.attrs[1].type .. (data.attrs[1].val > 0 and "+" or "-")
        end
        val = data.attrs[1].val
    elseif data.type == BuffType.EXP_UP then
        val = data.up_per
    elseif data.type == BuffType.CHANGE_MOVE_SPEED then
        val = -data.change_per
    end
    return "BuffAttrCommon" .. (val >= 0 and "+" or "-")
end

function MainViewData:ToggleTTTask()
    self.tt_task.val = not self.tt_task.val
end

function MainViewData:ToggleTTTeam()
    self.tt_team.val = not self.tt_team.val
end

MainViewUpgradeRemindType = {
    WithRedPoint = "WithRedPoint",
    WithoutRedPoint = "WithoutRedPoint",
    Remove = "Remove"
}

local upgrade_remind_seq = 1
-- type==BianQiangData.ListType
-- origin==smdata(刷新源)
-- judge_func==返回MainViewUpgradeRemindType的方法 为空时默认MainViewUpgradeRemindType.WithoutRedPoint
-- on_click==点击事件
-- init_call 注册调用一次
function MainViewData:RegisterUpgradeRemind(type, origin, judge_func, on_click, init_call, ...)
    local info = BianQiangData.Instance:ListInfo()[type]
    if info == nil then
        BuglyReport("主界面变强列表配置为空请检查")
        return
    end
    local handle = {seq = info.type, origin = origin}
    local function task()
        local remindType = judge_func()
        if remindType == MainViewUpgradeRemindType.WithRedPoint then
            for i, v in ipairs(self.upgrade_list) do
                if v.seq == handle.seq then
                    v.red = true
                    self.upgrade_list:Notify()
                    return
                end
            end
            self.upgrade_list:Insert({seq = handle.seq, text = info.name, on_click = on_click, red = true, order = info.order, info = info})
        elseif remindType == MainViewUpgradeRemindType.WithoutRedPoint then
            for i, v in ipairs(self.upgrade_list) do
                if v.seq == handle.seq then
                    v.red = false
                    self.upgrade_list:Notify()
                    return
                end
            end
            self.upgrade_list:Insert({seq = handle.seq, text = info.name, on_click = on_click, red = false, order = info.order,info = info})
        elseif remindType == MainViewUpgradeRemindType.Remove then
            for i, v in ipairs(self.upgrade_list) do
                if v.seq == handle.seq then
                    self.upgrade_list:Remove(i)
                    return
                end
            end
        end
    end
    if origin then
        handle.data_care = origin:Care(task, ...)
    else
        handle.runner = TimeHelper:AddRunTimer(task, 0.5)
    end
    if init_call then
        task()
    end
    -- upgrade_remind_seq = upgrade_remind_seq + 1
    return handle
end

function MainViewData:UnegisterUpgradeRemind(handle)
    for i, v in ipairs(self.upgrade_list) do
        if v.seq == handle.seq then
            self.upgrade_list:Remove(i)
            break
        end
    end
    if handle.origin then
        handle.origin:Uncare(handle.data_care)
    else
        TimeHelper:CancelTimer(handle.runner)
    end
end
function MainViewData:FlushTopLRedPoint(mod, num)
    self.tl_remind_nums[mod] = num
    self:ChangeLtRedPoint()
end
function MainViewData:TlRemindNums()
    return self.tl_remind_nums
end

function MainViewData:IsHasTlRemindNums()
    for key, value in pairs(MainViewData.Instance:TlRemindNums()) do
        if value > 0 then
            return true
        end
    end
    return false
end
function MainViewData:FlushRbRedPoint(mod, num)
    self.rb_remind_nums[mod] = num
    self:ChangeRbRedPoint()
end
function MainViewData:RbRmindNums()
    return self.rb_remind_nums
end

function MainViewData:QuietlyDownloadStateChange()
    self.quietly_download_state_notify_flag.flag = not self.quietly_download_state_notify_flag.flag
end

function MainViewData:QuietlyDownloadStateNotifyFlag()
    return self.quietly_download_state_notify_flag
end

function MainViewData:SetHideButton()
    self.view_data.hide_rb = not self.view_data.hide_rb
	self.view_data.hide_li = not self.view_data.hide_li
    self.view_data.hide_lt = not self.view_data.hide_lt
end
--实则show all
function MainViewData:ShowRbButton()
    LogDG("通知右下角功能区展开")
    self.view_data.show_all = not self.view_data.show_all
end
function MainViewData:FlushRbButton()
    self.view_data.flush_rb = not self.view_data.flush_rb
end
function MainViewData:ChangeRbRedPoint()
    self.remind_data.flush_rb = not self.remind_data.flush_rb
end
function MainViewData:ChangeLtRedPoint()
    self.remind_data.flush_lt = not self.remind_data.flush_lt
end
function MainViewData:HideLtButton()
    self.view_data.hide_lt = not self.view_data.hide_lt
end
function MainViewData:ShowLtButton()
    self.view_data.show_lt = not self.view_data.show_lt
end
function MainViewData:HideRbButton()
    self.view_data.hide_rb = not self.view_data.hide_rb
end
function MainViewData:ShowLeftButton()
    self.view_data.show_li = not self.view_data.show_li
end
function MainViewData:HideLeftButton()
    self.view_data.hide_li = not self.view_data.hide_li
end

function MainViewData:ShowTask()
    self.view_data.show_task = not self.view_data.show_task
end
function MainViewData:HideTask()
    self.view_data.hide_task = not self.view_data.hide_task
end

function MainViewData:ForceHideTask(force)
    self.view_data.force_hide_task = force
    if force then
        self:HideTask()
    end
end

function MainViewData:ForceShowTask(force)
    self.view_data.force_show_task = force
    if force then
        self:ShowTask()
    end
end

--=============================================场景UI实例对象=============================================
function MainViewData:InitSceneUiVos()
    self.scene_ui_vos = {}
end

--这里保存实例化的对象池对象
function MainViewData:SetSceneUiVos(vo,ui)
    self.scene_ui_vos[vo] = ui
end

--获取场景UI列表
function MainViewData:GetSceneUiVos()
    return self.scene_ui_vos
end

--获取场景Ui对象
function MainViewData:GetSceneUiVo(func)
    for vo, ui in pairs(self.scene_ui_vos) do
        if func and func(vo) then
            return ui
        end
    end
end

--根据seq获取场景UI实例化对象池对象
function MainViewData:GetNpcBoard(seq)
    return self:GetSceneUiVo(function(vo)
        if vo.seq and vo.seq == seq then
            return true
        end
    end)
end

--根据key获取获取场景UI实例化对象池对象
function MainViewData:GetBoard(key,value)
    return self:GetSceneUiVo(function(vo)
        if vo[key] and vo[key] == value then
            return true
        end
    end) 
end
-- buff = {icon_id, buff_name, desc}
function MainViewData:AddBattleBuff(buff)
    self.battle_buff_list:Insert(buff, 1)
end
-- 出战斗就全清掉
function MainViewData:ClearBattleBuff()
    self.battle_buff_list:Clear()
end
function MainViewData:BattleBuffList()
    return self.battle_buff_list
end

function MainViewData:AddBattleDebuff(buff)
    self.battle_debuff_list:Insert(buff, 1)
end
-- 出战斗就全清掉
function MainViewData:ClearBattleDebuff()
    self.battle_debuff_list:Clear()
end
function MainViewData:BattleDebuffList()
    return self.battle_debuff_list
end

function MainViewData:GetGameState()
    return self.cur_game_state.state
end

-- 获取战斗功能按钮列表
function MainViewData:GetBattleFuncBtnList()
    local cfg = Config.sundries_auto.fight_screen_button
    local list = {}
    for i,v in pairs(cfg) do
        local open = GetModOpen(v.open)
        if FunOpen.Instance:CheckOpen(open) then
            table.insert(list,v)
        end
    end
    return list
end

function MainViewData:NeedHideTaskViewOpened()
    
end