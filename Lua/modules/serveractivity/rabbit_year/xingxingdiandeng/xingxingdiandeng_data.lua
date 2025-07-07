--===========================Data===========================
--ClearData() //清除数据 在OnUnloadGameLogic被调用
XingXingDianDengData = XingXingDianDengData or BaseClass()

function XingXingDianDengData:__init()
    if XingXingDianDengData.Instance ~= nil then
        Debuger.LogError("[XingXingDianDengData] attempt to create singleton twice!")
        return
    end
    XingXingDianDengData.Instance = self
    self.ser_info = SmartData.New({count = 0, flag = {}})
    self.OP_TYPE = {INFO = 0, LOTERY = 1, LOTERY5 = 2, RECEIVE = 3}
    --信息 抽1 抽5 领奖
end

function XingXingDianDengData:ClearData()
end
 
function XingXingDianDengData:SetSerInfo(protocol)
    self.ser_info.count = protocol.lottery_count
    self.ser_info.flag = protocol.receive_flag
end
function XingXingDianDengData:GetSerInfo()
    return self.ser_info
end
function XingXingDianDengData:GetConfig()
    return ServerActivityData.Instance:GetActivityOpenCfg(nil,nil,Config.xingxingdiandeng_auto.basis_configuration)
end
function XingXingDianDengData:IsGet(seq)
    return (self.ser_info.flag[seq] or 0) == 1
end
function XingXingDianDengData:IsLock(count)
    return self.ser_info.count < count
end
function XingXingDianDengData:GetXingXingDianDengRed()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_STAR_LIGHTING) then
        return 0
    end
    local item_id = Config.xingxingdiandeng_auto.other[1].consume_item
    if Item.GetNum(item_id) > 0 then
        return 1
    end
    local list = self:GetConfig()
    for i, v in ipairs(list) do
        if not self:IsGet(v.seq) and not self:IsLock(v.add_light_num) then
            return 1
        end 
    end
    return 0
end