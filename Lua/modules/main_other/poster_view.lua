PosterHelper = {}

PosterHelper.OpenList = {
    [1] = {sub_name = "sword_sub",func = function()
        return LunJianData.Instance:GetPosterOpen()
    end},
    [2] = {sub_name = "period_activity_sub",func = function()
        return PeriodActivityCtrl.Instance:GetPosterOpen()
    end},
}
function PosterHelper.OpenPoster()
    local data_list = {}
    for i,v in ipairs(PosterHelper.OpenList) do
        if v.func() == true then
            table.insert(data_list,v.sub_name)
        end
    end
    if #data_list > 0 then
        ViewMgr:OpenView(PosterView,{data_list = data_list})
    end
end

PosterView = PosterView or DeclareView("PosterView", "main_other/poster_view")

VIEW_DECLARE_LEVEL(PosterView,ViewLevel.L1)

function PosterView:PosterView()
    self.make_num = 1
    self.data_list = {}
end

function PosterView:LoadCallback(param_t)
    for i,view_name in ipairs(param_t.data_list) do
        if self[view_name] then
            table.insert(self.data_list,self[view_name])
        end
    end
    self.max_num = #self.data_list
    self:OnClickItem(0)
end

function PosterView:OnClickItem(num)
    self.make_num = self.make_num + num
    self:FlushButtonView()
    self:FlushPanel()
end

function PosterView:FlushButtonView()
    self.BtnLeft:SetActive(self.make_num > 1)
    self.BtnRight:SetActive(self.make_num < self.max_num)
end

function PosterView:FlushPanel()
    for i,v in ipairs(self.data_list) do
        v:SetActive(self.make_num == i)
    end
end