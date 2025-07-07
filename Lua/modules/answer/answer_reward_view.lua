AnswerRewardView = AnswerRewardView or DeclareView("AnswerRewardView", "answer/answer_reward")

VIEW_DECLARE_MASK(AnswerRewardView,ViewMask.BgBlockClose)

function AnswerRewardView:LoadCallback(param)
	if param == nil then return end
	self.DescTxt.text = param.hint
	self.ItemList:SetData(param.items)
	if param.btn1 ~= nil then
		if param.time_param and param.time_param.time > 0 then
			if param.time_param.func then
				self.Btn1Time:SetCallBack(param.time_param.func)
			else
				self.Btn1Time:SetCallBack(function ()
					ViewMgr:CloseView(AnswerRewardView)
				end)
			end
			self.Btn1Time:SetEndText(param.btn1.text)
			self.Btn1Time:TotalTime(param.time_param.time, TimeType.Type_Special_5, param.time_param.desc)
		else
			self.Btn1Txt.text = param.btn1.text
		end
	else
		self.Btn1:SetActive(false)
	end
	if param.btn2 ~= nil then
		self.Btn2Txt.text = param.btn2.text
	else
		self.Btn2:SetActive(false)
	end
	self.data = param
end

function AnswerRewardView:OnBtn1Click()
	if self.data.btn1 ~= nil and self.data.btn1.func ~= nil then
		self.data.btn1.func()
	end
	if self.data.btn1.click_close ~= false then
		ViewMgr:CloseView(AnswerRewardView)
	end
end

function AnswerRewardView:OnBtn2Click()
	if self.data.btn2 ~= nil and self.data.btn2.func ~= nil then
		self.data.btn2.func()
	end
	if self.data.btn1.click_close ~= false then
		ViewMgr:CloseView(AnswerRewardView)
	end
end

function AnswerRewardView:OnCloseClick()
	ViewMgr:CloseView(AnswerRewardView)
end

AnswerRewardItem = AnswerRewardItem or DeclareMono("AnswerRewardItem",UIWidgetBaseItem)
function AnswerRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
	self.Item:SetData(self.data)
	UH.SetText(self.Name,string.format("%s",self.data:Name()))
end