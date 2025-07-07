TitleData = TitleData or BaseClass()

function TitleData:__init()
	if TitleData.Instance ~= nil then
		Debuger.LogError("[TitleData] attempt to create singleton twice!")
		return
	end
	TitleData.Instance = self
	self.popup_list = {}
end

function TitleData:AddPopupList(param_t)
	if self:IsNotSame(param_t.title_id) then
        table.insert(self.popup_list,param_t)
    end
end

--是否插入同一个
function TitleData:IsNotSame(title_id)
    if #self.popup_list == 0 then
        return true
    end
    for k,v in pairs(self.popup_list or {}) do
        if v.title_id == title_id then
            return false
        end
    end
    return true
end

--外部接口
function TitleData.GetTitleConfig(title_id)
	return Config.titleconfig_auto.title_list[title_id]
end

function TitleData.GetTitleName(title_id)
    local co = Config.titleconfig_auto.title_list[title_id]
	return co and co.name or (Language.Role.NotTitle .. title_id)
end

function TitleData.GetQuaName(title_id,shallow)
	local co = Config.titleconfig_auto.title_list[title_id]
	local color_str = shallow and QualityColorStrShallow or QualityColorStr
	return co and ColorStr(co.name,color_str[co.quality]) or ""
end

function TitleData.FormatPopup(notice,title_id)
	return {
		notice = notice or 0,
		title_id = title_id or -1,
        desc = Format(Language.Role.TitleGetTip[notice],TitleData.GetQuaName(title_id,true)),
		title = Language.Role.TitlePopup[notice],
		btn_name = Language.Role.TitleBtnName[notice]
    }
end

function TitleData.IsGuildTitle(title_id)
	return title_id == Config.titleconfig_auto.title_other[1].title_id
end