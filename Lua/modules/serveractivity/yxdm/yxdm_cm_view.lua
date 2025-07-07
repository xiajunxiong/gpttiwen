-- 月夕灯谜 猜谜界面
YxdmCmView = YxdmCmView or DeclareView("YxdmCmView", "serveractivity/yxdm/yxdm_cm")
VIEW_DECLARE_LEVEL(YxdmCmView, ViewLevel.Lt)
VIEW_DECLARE_MASK(YxdmCmView, ViewMask.BgBlock)

function YxdmCmView:YxdmCmView()
end

function YxdmCmView:LoadCallback(param_t)
end

function YxdmCmView:OnCloseClick()
    ViewMgr:CloseView(YxdmCmView)
end

function YxdmCmView:OnFlush()
    self.Panel:OnFlush()
end

----------------------------YxdmCmPanel----------------------------
YxdmCmPanel = YxdmCmPanel or DeclareMono("YxdmCmPanel", UIWFlushPanel)
function YxdmCmPanel:YxdmCmPanel()
    self.Data = YxdmData.Instance
    self.data = self.Data.yxdm_data
    self.answerRes_data = YxdmAnSwerResData.Instance.answerRes_data
    self.language = Language.YxDm
    self.data_cares = {{
        data = self.answerRes_data,
        func = self.FlushResult,
        init_call = false
    }}
    self:OnFlush()
    UH.SetText(self.TxtMd, self.language.md)
UH.SetText(self.Placeholder, self.language.cm_tip)
UH.SetText(self.TxtBtn, Language.Common.Comfirm)
end

-- 显示答题结果
function YxdmCmPanel:FlushResult()
    self.ImgJg:SetObjActive(true);
    if self.answerRes_data.info.is_true == 1 then
        UH.SpriteName(self.ImgJg, "_LocGongXiDaDuiLe")
    else
        UH.SpriteName(self.ImgJg, "_LocHenYiHanDaCuoLe")
        TimeHelper:CancelTimer(self.timer)
        self.timer = Invoke(BindTool.Bind1(self.OnFlush, self), 1)
    end
end

-- 提交答案
function YxdmCmPanel:Submmit()
    local id = self.data.info.cur_question_id ~= 0 and self.data.info.cur_question_id or 1;
    local cfg = self.Data.cfg.lantern_riddle_configuration[id]
    local answer = self.InpAnswer.text

    if answer == nil or answer == "" then
        PublicPopupCtrl.Instance:Center(self.language.AnswerNoOk)
    else
        if self.data.info.today_answer_count ~= self.Data.today_max_answer then
            self.BtnClose.Interactable = false
            self.BtnSubmmit.Interactable = false
            YxdmCtrl.Instance:SendReq(YxdmReqType.Answer, answer)
        end
    end
end

function YxdmCmPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function YxdmCmPanel:OnFlush()
    if self.data.info.today_answer_count ~= self.Data.today_max_answer then
        local id = self.data.info.cur_question_id -- ~= 0 and self.data.info.cur_question_id or 1;
        local cfg = self.Data.cfg.lantern_riddle_configuration[id]
        UH.SetText(self.TxtDesc, cfg.riddle)
        UH.SetText(self.TxtTip, "(" .. cfg.mystery_orders .. ")")
        UH.SetText(self.Title, string.format(Language.YxDm.Title1,self.data.info.today_answer_count + 1,self.Data.today_max_answer))
        UH.SetText(self.InpAnswer, "")
        self.BtnClose.Interactable = true
        self.BtnSubmmit.Interactable = true
        self.ImgJg:SetObjActive(false);

    else
        ViewMgr:CloseView(YxdmCmView)
        ViewMgr:OpenView(YxdmView)
    end
end

function YxdmCmPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer)
end
