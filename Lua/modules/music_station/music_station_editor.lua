
MusicStationEditView = MusicStationEditView or DeclareView("MusicStationEditView", MusicStationConfig.ResPath .. "music_station_edit")
-- VIEW_DECLARE_LEVEL(MusicStationEditView, ViewLevel.L0)
VIEW_DECLARE_MASK(MusicStationEditView, ViewMask.BgBlock)

function MusicStationEditView:LoadCallback()
end

function MusicStationEditView:OnClickLeave()
    ViewMgr:CloseView(MusicStationEditView)
end

MusicStationEditPanel= MusicStationEditPanel or DeclareMono("MusicStationEditPanel", UIWFlushPanel)
function MusicStationEditPanel:MusicStationEditPanel()
    self.data = MusicStationCtrl.Instance:Data()
    self.max_timeline =  self.data.editor.total_time

    self.data_cares = {
        {data = self.data.editor, func = self.FlushShow, init_call = false},
    }
end 

function MusicStationEditPanel:FlushShow()
    self.max_timeline =  self.data.editor.total_time
    self:FlushProgress(true)
end

function MusicStationEditPanel:Awake()
    UIWFlushPanel.Awake(self)

    self:InitShow()
end

function MusicStationEditPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_play)
end 

function MusicStationEditPanel:InitShow()
    self.Slider:SetChangedEvent(BindTool.Bind(self.SliderEvent,self))
    self.Slider:SetOffEvent(BindTool.Bind(self.SliderOffEvent,self))

    self.time_group_list = {}
    self.time_obj_list = {}
    self.btn_obj_list = {}
    self:FlushProgress()
end

function MusicStationEditPanel:SliderEvent()
    self:FlushProgress(true)
end

function MusicStationEditPanel:SliderOffEvent()
    self:FlushProgress()
end

-- 刷新进度槽
function MusicStationEditPanel:FlushProgress(need_fix)
    if need_fix then 
        local slider_value = self.Slider:GetValue()
        local fix_time = math.floor(math.floor(self.max_timeline * slider_value)/10) *10
        self.Slider:SetValue(fix_time/self.max_timeline)
    end 
    local slider_value = self.Slider:GetValue()
    local cur_time = math.floor(self.max_timeline * slider_value)
    UH.SetText(self.CurTime,cur_time)
    UH.SetText(self.CurRate,math.floor(slider_value*100).."%")

    for k,v in pairs(self.time_obj_list) do 
        local mono = self.time_group_list[k]

        v:SetActive(cur_time == k or (cur_time > k and (mono:GetData().time + mono.pass_time) >= cur_time))

        if cur_time ~= k then 
            mono.CanGroup.alpha = 0.5
        else 
            mono.CanGroup.alpha = 1
        end 
    end
end

function MusicStationEditPanel:SpawnItem(index)
    -- 生成时间框
    local cur_time = tonumber(self.CurTime.text)
    if self.time_group_list[cur_time] == nil or self.time_obj_list[cur_time] == nil then 
        local time_group_obj = self.TimeGroup.gameObject:Clone(self.ShowParent)
        time_group_obj.name = cur_time
        local group_mono = time_group_obj:GetLuaComponent(self.TimeGroup:GetTypeName())
        local init_data = {time = cur_time,item_list = {},}
        group_mono:SetData(init_data)
  
        UH.LocalPosG(time_group_obj, Vector3.New(0,0,0))
        self.time_obj_list[cur_time] = time_group_obj
        self.time_group_list[cur_time] = group_mono
        self.btn_obj_list[cur_time] = {}
    end 

    -- 生成按钮
    local btn_obj = self.ShowBtnObjs[index].gameObject:Clone(self.time_obj_list[cur_time])
    UH.LocalPosG(btn_obj, Vector3.New(0,0,0))
    btn_obj.name = "btn_"..index.."_time"
    table.insert(self.btn_obj_list[cur_time],btn_obj)
    local item_mono = btn_obj:GetLuaComponent(self.ShowBtnObjs[index]:GetTypeName())
    local init_data = {type = index,time = cur_time,pos_index = #self.btn_obj_list[cur_time]}
    item_mono:SetData(init_data)

    self.time_group_list[cur_time]:InsetBtn(item_mono)
    self.TimeList:SetData(self.time_group_list)
    self:FlushProgress()
end 

function MusicStationEditPanel:OnClickYellowBtn(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickBlueBtn1(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickBlueBtn2(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickBlueBtn3(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickBlueBtn4(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickRedBtn(index)
    self:SpawnItem(index)
end

function MusicStationEditPanel:OnClickTimeChange()
    local timer = self.TimeInput.text
    self.data.editor.total_time = tonumber(timer)
end

function MusicStationEditPanel:OnClickSave()
    local str = ""

    local check_list = {}
    for k,v in pairs(self.time_group_list) do 
        table.insert(check_list,v)
    end 
    
    table.sort(check_list, function (a, b)
        return a:GetData().time < b:GetData().time
    end)
    for k,v in pairs(check_list) do 
        for i,j in pairs(v:GetData().item_list) do 
            local pos = j.gameObject.transform.anchoredPosition
            str = str .."time="..j:GetData().time.."|type="..j:GetBtnType().."|pos_x="..pos.x.."|pos_y="..pos.y.."\n"
        end 
    end 

    UnityEngine.GUIUtility.systemCopyBuffer = str
end

function MusicStationEditPanel:OnClickBtnList(data)
    self.Slider:SetValue(data:GetData().time/ self.max_timeline)
end

function MusicStationEditPanel:CancelItem(data)
    self.time_group_list[data.time]:RemoveBtn(data.pos_index)
    UnityGameObj.Destroy(self.btn_obj_list[data.time][data.pos_index])
    table.remove(self.btn_obj_list[data.time],data.pos_index)
end

function MusicStationEditPanel:OnClickPlay()
    self.BtnPlay:SetActive(false)

    self.Slider:SetValue(0)
    self.play_num = 0
    self.timer_play = TimeHelper:AddCountDownTTA(function ()
        self.Slider:SetValue(((self.play_num+0.1)*10)/self.max_timeline)
        self.play_num = self.play_num + 1
    end,function ()
        self:OnClickPause()
    end,math.floor(self.max_timeline/100),0.1)
end

function MusicStationEditPanel:OnClickPause()
    self.BtnPlay:SetActive(true)
    TimeHelper:CancelTimer(self.timer_play)
end

MusicStationEditTimeCell = DeclareMono("MusicStationEditTimeCell", UIWidgetBaseItem)
function MusicStationEditTimeCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    local str_num = "黄"..data.yellow_num.."蓝"..data.blue_num.."红"..data.red_num.."\n"
    local str_time = data:GetData().time
    UH.SetText(self.Str,str_num..str_time)
end 

MusicStationYellowBtnCell = DeclareMono("MusicStationYellowBtnCell", UIWidgetBaseItem)
function MusicStationYellowBtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationYellowBtnCell:GetBtnType()
    return self.data.type 
end

MusicStationBlue1BtnCell = DeclareMono("MusicStationBlue1BtnCell", UIWidgetBaseItem)
function MusicStationBlue1BtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationBlue1BtnCell:GetBtnType()
    return self.data.type 
end

MusicStationBlue2BtnCell = DeclareMono("MusicStationBlue2BtnCell", UIWidgetBaseItem)
function MusicStationBlue2BtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationBlue2BtnCell:GetBtnType()
    return self.data.type 
end

MusicStationBlue3BtnCell = DeclareMono("MusicStationBlue3BtnCell", UIWidgetBaseItem)
function MusicStationBlue3BtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationBlue3BtnCell:GetBtnType()
    return self.data.type 
end

MusicStationBlue4BtnCell = DeclareMono("MusicStationBlue4BtnCell", UIWidgetBaseItem)
function MusicStationBlue4BtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationBlue4BtnCell:GetBtnType()
    return self.data.type 
end


MusicStationRedBtnCell = DeclareMono("MusicStationRedBtnCell", UIWidgetBaseItem)
function MusicStationRedBtnCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

end 

function MusicStationRedBtnCell:GetBtnType()
    return self.data.type 
end


MusicStationEditTimeGroup = DeclareMono("MusicStationEditTimeGroup", UIWidgetBaseItem)
function MusicStationEditTimeGroup:MusicStationEditTimeGroup()
    self.red_num = 0
    self.yellow_num = 0
    self.blue_num = 0

    self.pass_time = 0
end
function MusicStationEditTimeGroup:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
end 

-- 这是个mono
function MusicStationEditTimeGroup:InsetBtn(mono)
    self.data.item_list[mono:GetData().pos_index] = mono

    local new_type = mono:GetBtnType()
    self.pass_time = Config.audio_game_auto.button_type[1].disappear_time
    if new_type == 1 then 
        self.yellow_num = self.yellow_num + 1
    elseif new_type == 6 then 
        self.red_num = self.red_num + 1
    else
        self.blue_num = self.blue_num +1
        self.pass_time = Config.audio_game_auto.button_type[2].disappear_time
    end 
end
function MusicStationEditTimeGroup:RemoveBtn(index)
    local target = self.data.item_list[index]
    local type = target:GetBtnType()

    if type == 1 then 
        self.yellow_num = self.yellow_num - 1
    elseif type == 6 then 
        self.red_num = self.red_num - 1
    else
        self.blue_num = self.blue_num - 1
    end 

    self.data.item_list[index] = nil
end