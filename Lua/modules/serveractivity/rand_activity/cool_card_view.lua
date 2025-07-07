CoolCardView = CoolCardView or DeclareView("CoolCardView","serveractivity/cool_card", Mod.SummerFirst.CoolCard)
VIEW_DECLARE_LEVEL(CoolCardView, ViewLevel.L1)
function CoolCardView:CoolCardView()
    -- ServerActivityCtrl.Instance:SendRAYuanQiTurningInfo()

    self.BgAnim:ListenEvent("end", function ()
		for i = 1, self.HuDies:Length() do
			self.HuDies[i]:SetActive(true)
		end
	end)
end

function CoolCardView:OnClickClose()
    ViewMgr:CloseView(CoolCardView)
end

CoolCardViewF = CoolCardViewF or DeclareMono("CoolCardViewF", UIWFlushPanel)

function CoolCardViewF:CoolCardViewF()
    self.data = ServerActivityCtrl.Instance:Data()
    self.language = Language.CoolCard

    self.data_cares = {
        {data = self.data.cool_card_data, func = self.FlushShow},
    }
    
    self.init_show = true
    self.is_animing = false

    self.CardsAnim:ListenEvent("end", function ()
        self.ResultAnim:SetTrigger(APH("show"))
    end)
    self.ResultAnim:ListenEvent("end", function ()
        self.is_animing = false
    end)
    self.ResultAnim:ListenEvent("start", function ()
        self:ResultShow(true)
    end)
end

function CoolCardViewF:FlushShow()
    if self.init_show then
        self:InitShow()
    else
        self.is_animing = true
        local value = math.min(math.max(1, self.data.cool_card_data.open_index), 3)
        self.CardsAnim:SetTrigger(APH("move" .. value))
    end
    self.init_show = false

    local ccd = self.data.cool_card_data
    if ccd.result then
        local result = ccd.result
        for i = 1, self.ResultLucks:Length() do
            self.ResultLucks[i].text = result.luck
        end
        for i = 1, self.ResultIds:Length() do
            self.ResultIds[i].text = string.format(self.language.ResultId, result.sign_id)
        end
        for i = 1, self.ResultSigns:Length() do
            self.ResultSigns[i].text = result.sign
        end
        self.ResultSolution.text = result.sign_solution
    end
    if ccd.rewards then
        self.ResultRewards:SetData(ccd.rewards)
    end
end

function CoolCardViewF:InitShow()
    local show_result = self.data.cool_card_data.open_index > 0
    self:ResultShow(show_result)
    self.Card2:SetActive(not show_result)
end

function CoolCardViewF:ResultShow(value)
    self.ResultBlock:SetActive(value)
    self.ResultCard:SetActive(value)
    self.BtnCloseObj:SetActive(not value)
end

function CoolCardViewF:OnClickCard(index)
    if self.is_animing then return end
    ServerActivityCtrl.Instance:SendRAYuanQiTurningOpen(index)
end

function CoolCardViewF:OnClickSolute()
    if self.is_animing then return end
    self.is_animing = true
    self.ResultAnim:SetTrigger(APH("solute"))
end

function CoolCardViewF:OnClickReturn()
    if self.is_animing then return end
    self.ResultAnim:SetTrigger(APH("return"))
end