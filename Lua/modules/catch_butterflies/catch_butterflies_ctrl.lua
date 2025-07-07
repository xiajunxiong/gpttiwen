FILE_REQ("modules/catch_butterflies/catch_butterflies_data")
VIEW_REQ("modules/catch_butterflies/catch_butterflies")
VIEW_REQ("modules/catch_butterflies/capturing")

CatchButterfliesCtrl = CatchButterfliesCtrl or BaseClass(BaseCtrl)
function CatchButterfliesCtrl:__init()
    if CatchButterfliesCtrl.Instance ~= nil then
        Debuger.LogError("[CatchButterfliesCtrl] attempt to create singleton twice!")
        return
    end
    CatchButterfliesCtrl.Instance = self
    self.data = CatchButterfliesData.New()
    
    self:RegisterProtocol(SCRAXunMengHuDieInfo,"SCRAXunMengHuDieInfo")
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.Butterfly, self.data:Info(), function ()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.CATCH_BUTTERFLY) then
			return 0
		end
        if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.Butterfly) then
            return 0
        end
        local flag = self.data:IsCantCatch()
        return flag and 0 or 1
    end)
end
function CatchButterfliesCtrl:SCRAXunMengHuDieInfo(protocol)
    self.data:SetCount(protocol.count)
    local flag = self.data:IsCantCatch()
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.CATCH_BUTTERFLY, flag and MayDayConfig.MainBtnState.NextDay or MayDayConfig.MainBtnState.Go)
end
function CatchButterfliesCtrl.ReqInfo()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.CATCH_BUTTERFLY
	param_t.opera_type = 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end
function CatchButterfliesCtrl.Capturing(obj_id, is_win)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.CATCH_BUTTERFLY
	param_t.opera_type = 1
    param_t.param_1 = obj_id
    param_t.param_2 = is_win
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end
function CatchButterfliesCtrl:OnSCRAResultNotice(protocol)
    if protocol.result_type == 2 then
        PublicPopupCtrl.Instance:Center(Language.Butterfly.CatchFail)
    else
        MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(protocol.item_list),nil,true,"_LocBuZhuoChengGong",true)
    end
end