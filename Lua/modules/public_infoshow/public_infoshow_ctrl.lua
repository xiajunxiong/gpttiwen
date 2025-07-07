VIEW_REQ("modules/public_infoshow/public_pageshow_view")

PublicInfoShowCtrl = PublicInfoShowCtrl or BaseClass(BaseCtrl)

function PublicInfoShowCtrl:__init()
	if PublicInfoShowCtrl.Instance ~= nil then
		Debuger.LogError("[PublicInfoShowCtrl] attempt to create singleton twice!")
		return
	end
	PublicInfoShowCtrl.Instance = self
	self.page_show_data = {}
end

-- param_t.title_name 界面的标题，没有则不显示
-- param_t.tab_list 切页按钮列表 元素需要有name——名字，tab_index——列表标记值
-- param_t.show_list 主list列表 
-- 元素需要有qua——图标颜色(12345)，icon——图标，
-- desc——描述 ,tab_index——列表标记 , icon_type——图标类型 name——名字
-- param_t.cell_func 点击单元的函数 
-- param_t.empty_lab
function PublicInfoShowCtrl:SetPublicPageShow(param_t)
	self:HadlePageData(param_t)
	ViewMgr:OpenView(PublicPageShowView)
end

function PublicInfoShowCtrl:HadlePageData(param_t)
	self.page_show_data = param_t
end


-- function PublicInfoShowCtrl:TestButton()
-- 	local param_t = {}
-- 	param_t.title_name = "特效奖池"	
-- 	param_t.tab_list ={
-- 		{name = "普通",tab_index = 3},
-- 		{name = "稀有",tab_index = 4},
-- 		{name = "传奇",tab_index = 5},
-- 	}
-- 	param_t.show_list = {}

-- 	for k,v in pairs(Config.special_effect_auto.spe_list) do 
-- 		local vo = {}
-- 		vo.qua = v.sp_priority
-- 		vo.tab_index = v.sp_priority
-- 		vo.desc = v.description
-- 		vo.icon = v.icon_id
-- 		vo.icon_type = ICON_TYPE.BUFF
-- 		vo.name = v.name

-- 		table.insert( param_t.show_list, vo )
-- 	end 

-- 	param_t.cell_func = function (data)
-- 		LogError("click!",data.name)
-- 	end
-- 	self:SetPublicPageShow(param_t)
-- end