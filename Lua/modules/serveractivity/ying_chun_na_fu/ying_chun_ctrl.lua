FILE_REQ("modules/serveractivity/ying_chun_na_fu/ying_chun_data")
VIEW_REQ("modules/serveractivity/ying_chun_na_fu/ying_chun_probability")
VIEW_REQ("modules/serveractivity/ying_chun_na_fu/ying_chun_record")
VIEW_REQ("modules/serveractivity/ying_chun_na_fu/ying_chun_view")
VIEW_REQ("modules/serveractivity/ying_chun_na_fu/ying_chun_reward")

YingChunCtrl = YingChunCtrl or BaseClass(BaseCtrl)

YingChunCtrl.ReqType = {
    Info = 0, -- 请求信息
    Lotery = 1, -- 单抽
    Lotery10 = 2, -- 10抽
}

function YingChunCtrl:__init()
    if YingChunCtrl.Instance ~= nil then
        Debuger.LogError("[YingChunCtrl] attempt to create singleton twice!")
        return
    end
    YingChunCtrl.Instance = self
    self.data = YingChunData.New()
    self:RegisterAllProtocols()
    self:RegisterRemind()
end

function YingChunCtrl:__delete()
    self:UnRegisterAllProtocols()
    YingChunCtrl.Instance = nil
end

-- 红点
function YingChunCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.Remind, self)
    RemindCtrl.Instance:Remind():Register(Mod.RabbitYear.YingChunNaFu, self.data.ying_chun_data, remind_func)
    BagData.Instance:ListenItems({[1] = self.data:GetConsunmeItem()}, BindTool.Bind(self.FlushCurr, self))
end

function YingChunCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYingChunNaFuInfo, "SetYingChunInfo")
    self:RegisterProtocol(SCRAYingChunNaFuNotice, "SetYingChunNotice")
end

function YingChunCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRAYingChunNaFuInfo)
    self:UnRegisterProtocol(SCRAYingChunNaFuNotice)
end

function YingChunCtrl:SetYingChunInfo(protocol)
    self.data:SetYingChunInfo(protocol)
end

function YingChunCtrl:SetYingChunNotice(protocol)
    self.data:SetYingChunNotice(protocol)
end

function YingChunCtrl:FlushCurr()
    self.data:FlushCurr()
end

function YingChunCtrl:Remind()
    return self.data:Remind()
end

