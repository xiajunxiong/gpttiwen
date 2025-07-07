LBDragonPluseInspireView = LBDragonPluseInspireView or DeclareView("LBDragonPluseInspireView", "lb_dragon_pluse/lb_dragon_pulse_inspire")
function LBDragonPluseInspireView:LBDragonPluseInspireView()
    self.Board:SetData(self:GetType(), Language.LBDragonPluse.InspireTitle, Vector2.New(580, 525))
end

LBDragonPluseInspirePanel = LBDragonPluseInspirePanel or DeclareMono("LBDragonPluseInspirePanel",UIWFlushPanel)
function LBDragonPluseInspirePanel:LBDragonPluseInspirePanel()
    self.data = LBDragonPluseData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel, keys = {"inspire_lv"}},
    }
    self.cur_inspire = nil
end 

function LBDragonPluseInspirePanel:FlushPanel()
    local param = self.data:GetInspireShow()

    if self.cur_inspire ~= nil and self.cur_inspire ~= self.data.base_info.inspire_lv then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.InspireSuccess)
    end 

    self.cur_inspire = self.data.base_info.inspire_lv

    UH.SetText(self.CurEffect,param.eff_cur)
    UH.SetText(self.MaxEffect,param.eff_cur)
    UH.SetText(self.NextEffect,param.eff_next)

    self.CurTips:SetActive(param.eff_cur ~= "")
    self.NextTips:SetActive(param.eff_next ~= "")

    UH.SetText(self.Cost,param.cost)
    UH.SetText(self.CostCoin,param.costcoin)

    self.cost = param.cost
    self.costcoin = param.costcoin
    self.is_max = param.is_max

    self.PartLeft:SetActive(self.cur_inspire < self.data.dragon_cfg[1].inspire_level_max)
    self.PartRight:SetActive(self.cur_inspire < self.data.dragon_cfg[1].inspire_level_max)
    self.PartMax:SetActive(self.cur_inspire >= self.data.dragon_cfg[1].inspire_level_max)
end

function LBDragonPluseInspirePanel:OnClickCancel()
    ViewMgr:CloseView(LBDragonPluseInspireView)
end

function LBDragonPluseInspirePanel:OnClickReq()
    if self.is_max then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.MaxInspire)
        return 
    end 

    local info = {
        content = string.format(Language.LBDragonPluse.AskInspire,self.cost),
        confirm = {
            name = nil,
            func = function()
                LBDragonPluseCtrl.Instance:ReqDragonInspire({type = 0})
                -- ViewMgr:CloseView(LBDragonPluseInspireView)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end


function LBDragonPluseInspirePanel:OnClickReqCoin()
    if self.is_max then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.MaxInspire)
        return 
    end 

    local info = {
        content = string.format(Language.LBDragonPluse.AskInspireCoin,self.costcoin),
        confirm = {
            name = nil,
            func = function()
                LBDragonPluseCtrl.Instance:ReqDragonInspire({type = 1})
                -- ViewMgr:CloseView(LBDragonPluseInspireView)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end