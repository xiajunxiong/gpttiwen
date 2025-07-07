-- 神域主界面·活动分块
LandsBetweenActTotalPanel = LandsBetweenActTotalPanel or DeclareMono("LandsBetweenActTotalPanel", UIWFlushPanel)
function LandsBetweenActTotalPanel:LandsBetweenActTotalPanel()
    self.data = LandsBetweenData.Instance

end

function LandsBetweenActTotalPanel:Awake()
    UIWFlushPanel.Awake(self)

end


function LandsBetweenActTotalPanel:FlushList()
    local act_list = self.data:GetLandsBetweenActList()
    self.ActList:SetData(act_list)
end
-- 神域活动图标子项
LandsBetweenActCells = LandsBetweenActCells or DeclareMono("LandsBetweenActCells", UIWidgetBaseItem)
function LandsBetweenActCells:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self:uncareData()

    local param = LandsBetweenData.Instance:GetExtralActShowParam(data.act_type)

    if param.listen_data ~= nil then 
        self:careData(param.listen_data, BindTool.Bind(self.flushIcon, self))
    end 
    if type(param.remin_data) == "function" then
        self.remin_data = param.remin_data()
    else
        self.remin_data = param.remin_data
    end
    if type(param.remind_func()) == "number" then
        self.remind_func = param.remind_func
    else
        self.remind_func = param.remind_func()
    end
    self.click_func = param.click_func
    self.time_show_func = param.time_show_func
    self.act_show_func = param.act_show_func
    self.act_status_func = param.act_status_func

     --注册红点
     if self.remin_data then 
        if not self.is_care then
            self.is_care = true
            self:careData(self.remin_data, BindTool.Bind(self.ActRedPoint, self))
        else 
            self:ActRedPoint()
        end
    end 
    -- dragon_vein_sub

    self.ActShow:SetActive(self.act_show_func())
    self.gameObject:SetActive(self.act_show_func())

    UH.SetText(self.Name,data.name_text)
    UH.SpriteName(self.Icon,data.icon_id)
    self.ActSpShow.gameObject:SetActive(param.show_sp_sub ~= nil)
    if param.show_sp_sub then 
        self.ActSpShow:ActiveSpSub({str = param.show_sp_sub})
    end 

    self:flushIcon()
end 

-- 刷新显示
function LandsBetweenActCells:flushIcon()
    if self.act_show_func == nil then 
        return 
    end 

    if ViewMgr:IsClosed(LandsBetweenMainView) then 
        return 
    end 

    self.ActShow:SetActive(self.act_show_func())

    local with_timer = self.time_show_func ~= nil and self.time_show_func() > 0
    self.Timer.gameObject:SetActive(with_timer)
    if with_timer then 
        self.Timer:StampTime(self.time_show_func(),TimeType.Type_Special_3) 
    end 

    if self.act_status_func ~= nil then 
        
        self.StatusWait:SetActive(self.act_status_func() == 1)
        self.StatusActing:SetActive(self.act_status_func() == 2)
    else 
        self.StatusWait:SetActive(false)
        self.StatusActing:SetActive(false)
    end 

end

-- 红点显隐
function LandsBetweenActCells:ActRedPoint()
    self.red_point = self.red_point or UiRedPoint.New(self.RedPos, Vector3.New(0,0,0))
	self.red_point:SetNum(self.remind_func())
end

-- 重置操作
function LandsBetweenActCells:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
	if not self.is_care then return end
    self:uncareData()
end

-- 点击操作
function LandsBetweenActCells:OnClickAct()
    if self.click_func ~= nil then 
        self.click_func()
    end 
    -- ViewMgr:OpenViewByKey(self.data.open)
end

-- 活动图标额外显示
LandsBetweenActIocnSp = LandsBetweenActIocnSp or DeclareMono("LandsBetweenActIocnSp", UIWFlushPanel)
function LandsBetweenActIocnSp:LandsBetweenActIocnSp()
    -- self.data = LandsBetweenData.Instance
end 

function LandsBetweenActIocnSp:ActiveSpSub(param)
    if self[param.str] ~= nil then
        self[param.str].gameObject:SetActive(true)
    end 
end


-- 地图格追加显示
LandsBetweenActSpShow = LandsBetweenActSpShow or DeclareMono("LandsBetweenActSpShow", UIWidgetBaseItem)
function LandsBetweenActSpShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    

    self.SpCalls.gameObject:SetActive(data.ui_show_sp_sub ~= nil)
    if data.ui_show_sp_sub then 
        self.SpCalls:ActiveSpSub({str = data.ui_show_sp_sub,link_mark = data.link_mark or "0|0" })
    end 
end 

LandsBetweenActMapShow = LandsBetweenActMapShow or DeclareMono("LandsBetweenActMapShow", UIWFlushPanel)
function LandsBetweenActMapShow:LandsBetweenActMapShow()
    self.obj_strs = {}
end 

function LandsBetweenActMapShow:ActiveSpSub(param)

    if self[param.str] ~= nil then
        self[param.str].gameObject:SetActive(true)

        if param.link_mark ~= nil and self[param.str].SetLinkMark ~= nil then 
            self[param.str]:SetLinkMark(param.link_mark)
        end 
        if self.obj_strs[param.str] == nil then 
            self.obj_strs[param.str] = self[param.str].gameObject
        end 
    end 
end

-- 隐藏所有的obj
function LandsBetweenActMapShow:HideSpSub()
    for k,v in pairs(self.obj_strs) do 
        v:SetActive(false)
    end
end

-- 测试面板
LandsBetweenTestPanel = LandsBetweenTestPanel or DeclareMono("LandsBetweenTestPanel", UIWFlushPanel)
function LandsBetweenTestPanel:LandsBetweenTestPanel()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.operating_info, func = self.FlushText1,keys = {"test_obj"}},
        {data = self.data.operating_info, func = self.FlushText2,keys = {"test_obj"}},
    }

    self.TestTxt1.gameObject:SetActive(DEBUG_MODE)
    self.TestTxt2.gameObject:SetActive(DEBUG_MODE)
end 

function LandsBetweenTestPanel:FlushText1()

    if self.data.operating_info.test_obj ~= nil then 
        local str = ""
        local map_pic = self.data:GetMapPicTestClick()
        str = str.."坐标:"..map_pic.static.link_mark.."| 视野:"..map_pic.static.map_show.visible_area .." |"
            ..(map_pic.static.map_show.type == 0 and "外域" or " 内域") .. " | " 


        local faction = map_pic.net_info == nil and -1 or (map_pic.net_info.occupied_faction ~= nil 
            and map_pic.net_info.occupied_faction or map_pic.net_info.faction)
        local net_str = map_pic.net_info == nil and (map_pic.linking_city and "连接地块" or "孤立地块") or 
            (faction == self.data:GetMyFaction() and "我方地块" or "敌方地块" )

        local type_str = " | "..(self.data:IsAltar(map_pic.static.map_show) and "神坛" or "空地")
        str = str .. net_str ..type_str
        UH.SetText(self.TestTxt1,str)
    end 
end

function LandsBetweenTestPanel:FlushText2()
    if self.data.operating_info.test_obj ~= nil then 
        local str = ""
        local map_pic = self.data:GetMapPicTestClick()
        local net_str = map_pic.net_info == nil and "无占领信息" or "|" 
        if map_pic.net_info ~= nil then
            local occupied_faction = map_pic.net_info.occupied_faction ~= nil and 
                Language.LandsBetween.FactionName[map_pic.net_info.occupied_faction] or 
                Language.LandsBetween.FactionName[map_pic.net_info.faction]
            
            local occupied_user = map_pic.net_info.occupy_plat_type.."|"..map_pic.net_info.occupy_uid
            local prayer_str = map_pic.net_info.prayer_uid ~= nil and map_pic.net_info.prayer_plat_type.."|"..map_pic.net_info.prayer_uid or "无祈祷"

            net_str = "阵营:".. occupied_faction .." | 占领者:".. occupied_user.." | 祈祷者:".. prayer_str .." | "
        end 
        local sp_str = ""
        local param = self.data:GetResOfSpPicByLinkMark(map_pic.static.link_mark)
        if param ~= nil then
            sp_str = param.name
        end
        
        str = net_str .. sp_str
        UH.SetText(self.TestTxt2,str)
    end 
end