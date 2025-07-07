----------------------------ColorfulIllustrateData----------------------------
ColorfulIllustrateData = ColorfulIllustrateData or BaseClass()
function ColorfulIllustrateData:__init()
	if ColorfulIllustrateData.Instance ~= nil then
		Debuger.LogError("[ColorfulIllustrateData] attempt to create singleton twice!")
		return
	end
    ColorfulIllustrateData.Instance = self
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoColorfulIllustrations,function() 
        self.color_config = Config.colorful_illustrations_auto.color_list
        self.pigment_config = Config.colorful_illustrations_auto.seven_color_picture_pigment
    end,true)
    self:Initialization()
    self:InitFormatData()
end

function ColorfulIllustrateData:InitFormatData()
    self.color_list = {}
    for k,v in pairs(self.color_config) do
        local seq,quality = v.seq + 1,v.quality + 1
        if self.color_list[seq] == nil then
            self.color_list[seq] = {seq = v.seq,colors = {}}
        end
        self.color_list[seq].colors[quality] = v
    end
    self.info_data=SmartData.New{pigment_list={},save_string="",is_fetch_make_flag = 0}
    self.red_piont_data = SmartData.New({pigment_num = 0,val = false})
    self.save_string_list = {}
end

function ColorfulIllustrateData:Initialization()
    self.select_data = SmartData.New()
end

function ColorfulIllustrateData:SetColorfulIllustrateInfo(protocol)
    self.info_data.save_string = protocol.save_string
    if self.info_data.is_fetch_make_flag == 0 and protocol.is_fetch_make_flag == 1 then
        self:SaveOldStringData()
    else
        UnityPlayerPrefs.SetString(self:GetKey(),"")
    end
    self.info_data.pigment_list = protocol.pigment_list
    self.info_data.is_fetch_make_flag = protocol.is_fetch_make_flag
    self.role_level = protocol.role_level or 0
    self.red_piont_data.pigment_num = protocol.pigment_num
    self:SetSaveStringList(protocol.save_string)
    self:CheckRedPiontData()
end

function ColorfulIllustrateData:CheckRedPiontData()
    self.red_piont_data.val = not self.red_piont_data.val
end

function ColorfulIllustrateData:SaveOldStringData()
    if UnityPlayerPrefs.GetString(self:GetKey()) == "" then
        UnityPlayerPrefs.SetString(self:GetKey(),self.info_data.save_string)
    end
end

function ColorfulIllustrateData:GetColorNum()
    return self.red_piont_data.pigment_num
end

function ColorfulIllustrateData:GetPigmentNum(seq,quality)
	return self.info_data.pigment_list[seq] and self.info_data.pigment_list[seq][quality] or 0
end

function ColorfulIllustrateData:SetSelectData(data)
	self.select_data:Set(data)
end

function ColorfulIllustrateData:GetSelectData()
	return self.select_data:Val()
end

function ColorfulIllustrateData:GetColorList()
    return self.color_list
end

--读取本地信息
function ColorfulIllustrateData:GetSaveData()
    return self:Interpret(self.info_data.save_string or "")
end

function ColorfulIllustrateData:GetKey()
    return "ColorfulIllustrate_" .. RoleData.Instance:GetRoleId()
end

function ColorfulIllustrateData:GetSaveStringData(index)
    return self.save_string_list[index] or ""
end

function ColorfulIllustrateData:SetSaveStringData(index,str)
    self.save_string_list[index] = str or ""
end

function ColorfulIllustrateData:ClearData()
    self.save_string_list = {}
    self.red_piont_data.pigment_num = 0
end

function ColorfulIllustrateData:GetListData()
    local list = {}
	for i=0,6 do
        list[i] = {}
        for j=0,2 do
            list[i][j] = 0
        end
    end
    return list
end

--计算评分
function ColorfulIllustrateData:GetScoreNum(data)
    local num = 0
    for k,v in pairs(data) do
        if v.sort then
            num = num + self.pigment_config[v.quality + 1].is_special
        end
    end
    return num
end

function ColorfulIllustrateData:Interpret(str,config)
    local list,num_list = {},{}
    local _,key_list = DataHelper.GetValueLength(str)
    for i=1,COLORFUL_ILLUSTRATE_MAX_NUM do
        list[i] = {}
        num_list[i] = 0
        for j=1,COLORFUL_ILLUSTRATE_IMAGE_NUM do
            local key = key_list[j + (i - 1) * COLORFUL_ILLUSTRATE_IMAGE_NUM] or " "
            if key ~= " " then
                local value = tonumber(string.byte(key))
                list[i][j] = self.color_config[value - 96]
                num_list[i] = num_list[i] + 1
            else
                list[i][j] = {}
            end
        end
    end
    return list,num_list
end

--保存下字符串列表
function ColorfulIllustrateData:SetSaveStringList(str)
    local not_color = ""
    for i=1,COLORFUL_ILLUSTRATE_IMAGE_NUM do
        not_color = not_color .. " "
    end
    for i=1,7 do
        local save_string = string.sub(str,COLORFUL_ILLUSTRATE_IMAGE_NUM * (i - 1) + 1,COLORFUL_ILLUSTRATE_IMAGE_NUM * i)
        self.save_string_list[i] = save_string ~= "" and save_string or not_color
    end
end

function ColorfulIllustrateData:GetRewardData()
    local list = {}
	local level = self.role_level or RoleData.Instance:GetRoleLevel()
    local config = Config.colorful_illustrations_auto.seven_color_picture_award
	for k,v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,config)) do
		if level >= v.grade_start and level <= v.grade_end then
			table.insert(list,v)
		elseif level >= v.grade_start and v.grade_end == 0 then
			table.insert(list, v)
		end
	end
	table.sort(list,function(a,b)return a.seq < b.seq end)
	return list
end

--红点逻辑
function ColorfulIllustrateData:GetRemindNum()
    if self.info_data.is_fetch_make_flag == 1 then
        return 0
    end
    if self:GetColorNum() > ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE) then
        return 1
    end
    return 0
end

--获取补齐奖励花费
function ColorfulIllustrateData:GetCompletionNum()
    local num = 0
    local _,num_list = self:GetSaveData()
    for i,v in ipairs(num_list) do
        num = num + (COLORFUL_ILLUSTRATE_IMAGE_NUM - v)
    end
    return num
end

--获取补齐的预设填色数据
function ColorfulIllustrateData:GetPreinstall()
    local preinstall = "ggggjjjjjaabjajjjjegjjjeejjccjjjjjjjpjrjpjjqrjcciijjjjgacabajjg"
    return self:Interpret(preinstall)
end

--是否到补齐开启时间
function ColorfulIllustrateData:IsPreinstall()
    if self:GetOtherConfig()[1].is_complement == 0 then
        return false
    end
	local activity_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE)
    local t = TimeHelper.FormatDHMS((activity_info.next_status_switch_time - TimeHelper.GetServerTime()))
    return t.day < self:GetOtherConfig()[1].time
end

function ColorfulIllustrateData:GetOtherConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,Config.colorful_illustrations_auto.other)
end