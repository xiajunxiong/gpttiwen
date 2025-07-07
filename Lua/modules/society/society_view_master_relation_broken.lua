SocietyMasterRelationBroken = SocietyMasterRelationBroken or
                                  DeclareView("SocietyMasterRelationBroken", "society/shitu/relation_broken")
function SocietyMasterRelationBroken:SocietyMasterRelationBroken()
end
function SocietyMasterRelationBroken:LoadCallback()

    self.Board:SetData(self:GetType(), Language.Society.Master.Title, Vector2.New(424.342, 421.1528))
end

SocietyMasterRelationBrokenPanel = SocietyMasterRelationBrokenPanel or
                                       DeclareMono("SocietyMasterRelationBrokenPanel", UIWFlushPanel)
function SocietyMasterRelationBrokenPanel:SocietyMasterRelationBrokenPanel()
    self.config = Config.mentor_cfg_auto
    self.other = self.config.other[1]
    self.society_data = SocietyData.Instance
end
function SocietyMasterRelationBrokenPanel:onFlush()
    UH.SetText(self.BtnTex1, Language.Society.Master.Btn1)
    UH.SetText(self.BtnTex2, Language.Society.Master.Btn2)
    UH.SetText(self.Desc1, self.other.apply_show)
    UH.SetText(self.Desc2, self.other.force_show)
    self.data = RoleInfoCtrl.Instance:Data()
    self.role_info = self.data:RoleInfo()
    self.info = self.data:RoleInfo().info
    if self.info then
        self.func_info = self.info.func_info or {}
    else
        self.func_info = {}
    end
end
function SocietyMasterRelationBrokenPanel:OnClickBtn1()
    --判断7天是否上线
    local l_time = self.func_info.login_time + Config.mentor_cfg_auto.other[1].not_online_time
    local o_time = TimeCtrl.Instance:GetServerTime() - l_time
    --大于0说明已经超过了
    local value, type = self.society_data:IsMasterOrApprentice(self.func_info.uid)
    if o_time <= 0 then
        PublicPopupCtrl.Instance:Center(Language.Society.Master.BlockTip[type])
        return
    end
    if value then
        SocietyCtrl.Instance:SendShiTuBreakRelationReq(1, self.func_info.uid, 0, type)
    end
    ViewMgr:CloseView(SocietyMasterRelationBroken)
end
function SocietyMasterRelationBrokenPanel:OnClickBtn2()
    local info = {
        content = string.format(Language.Society.Master.CloBlock, Config.mentor_cfg_auto.other[1].consume_amount),
        confirm = {
            func = function()
                local value, type = self.society_data:IsMasterOrApprentice(self.func_info.uid)
                if value then
                    SocietyCtrl.Instance:SendShiTuBreakRelationReq(1, self.func_info.uid, 1, type)
                end
                ViewMgr:CloseView(SocietyMasterRelationBroken)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)

    -- local value, type = self.society_data:IsMasterOrApprentice(self.func_info.uid)
    -- if value then
    --     SocietyCtrl.Instance:SendShiTuBreakRelationReq(1, self.func_info.uid, 1, type)
    -- end
    -- ViewMgr:CloseView(SocietyMasterRelationBroken)
end


SocietyMasterCooling = SocietyMasterCooling or DeclareView("SocietyMasterCooling", "society/shitu/master_cooling")
function SocietyMasterCooling:SocietyMasterCooling()

end
function SocietyMasterCooling:LoadCallback()
    self.data = RoleInfoCtrl.Instance:Data()
    self.role_info = self.data:RoleInfo()
    self.info = self.data:RoleInfo().info
    self.func_info = self.info.func_info or {}
    self.Time:SetCallBack(function ()
        ViewMgr:CloseView(SocietyMasterCooling)
    end)
    local e_time = self.func_info.relation_break_time-- + Config.mentor_cfg_auto.other[1].chill_time
    self.Time:StampTime(e_time,TimeType.Type_Time_1, Language.Society.Master.Time)
end
function SocietyMasterCooling:OnClickClose()
    ViewMgr:CloseView(SocietyMasterCooling)
end
function SocietyMasterCooling:OnClickCancel()
    local info = {
        content = Language.Society.Master.CancelBlock,
        confirm = {
            func = function()
                local value, type = SocietyData.Instance:IsMasterOrApprentice(self.func_info.uid)
                if value then
                    SocietyCtrl.Instance:SendShiTuBreakRelationReq(2, self.func_info.uid, 0, type)
                end
                ViewMgr:CloseView(SocietyMasterCooling)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
    -- local value, type = SocietyData.Instance:IsMasterOrApprentice(self.func_info.uid)
    -- if value then
    --     SocietyCtrl.Instance:SendShiTuBreakRelationReq(2, self.func_info.uid, 0, type)
    -- end
    -- ViewMgr:CloseView(SocietyMasterCooling)
end
