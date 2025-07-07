TipsPopupData = TipsPopupData or BaseClass()

function TipsPopupData:__init()
    self.data = {}
end

function TipsPopupData:__delete()
end

-- tips_str
function TipsPopupData:SetData(data)
    self.data = data
end

function TipsPopupData:GetTipsValue()
    return self.data.tips_str or 0
end