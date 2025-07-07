GuildBuild = GuildBuild or DeclareMono("GuildBuild", UIWFlushPanel)

function GuildBuild:GuildBuild()
    self.data = GuildData.Instance
    self.lang = Language.Guild
    self.data_cares = {
        {data = self.data.build_goods_flush, func = self.FlushResultInfo},
        {data = self.data.build_info_flush, func = self.FlushBuildList},
    }
end

function GuildBuild:onFlush()
	-- LogError("onFlush")
    -- self.BuildList:SetData(self.data:GetBuildList())
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BUILD_INFO,param = self.data:GetGuildID()})
end
function GuildBuild:FlushBuildList()
    self.BuildList:SetData(self.data:GetBuildList())
    if self.build_info then
        self.BuildList:ClickItemData(self.build_info)
    else
        self.BuildList:ClickItem(1)
    end
end
function GuildBuild:OnClickBuildItem(data)
    self.build_info = data
    local param_t = {
        guild_id = self.data:GetGuildID(),
        build_seq = data.seq
    }
    self.data:ClearGoodsInfo()
    self:FlushBuildInfo()
end
function GuildBuild:FlushBuildInfo()
    local info = self.data:GetGuildInfo()
    UH.SetText(self.BuildName, self.build_info.build_name)
    UH.SetText(self.BuildDesc, self.build_info.desc)
    local limit = info.level >= self.build_info.level_limit
    
    UH.SetText(
        self.BuildLevel,
        string.format(
            self.lang.GuildLevel,
            limit and QualityColorStr[2] or QualityColorStr[6],
            self.build_info.level_limit
        )
    )
    local cur_value = self.data:GetBuildValue(self.build_info.seq)
    if self.build_info.build_num > 0 then
        self.BarObj:SetActive(true)
        self.BuildNone:SetActive(false)
        UH.SetText(self.BuildValue, string.format("%d/%d", cur_value, self.build_info.build_num))
        self.BuildBar:SetProgress(cur_value / self.build_info.build_num)
    else
        self.BarObj:SetActive(false)
        self.BuildNone:SetActive(true)
    end
    if not limit then
        UH.SetText(self.BuildValue,"???/???")
        self.BuildBar:SetProgress(1)
        self.GoodsList:SetData({})
        self.BuildReady:SetActive(true)
    else
        self.BuildReady:SetActive(false)
        local list = self.data:GetDonateList(self.build_info.seq)
        self.GoodsList:SetData(list)
    end
    local finish = self.data:GetBuildFinish(self.build_info.seq)
    if self.build_info.build_num > 0 and cur_value >= self.build_info.build_num then
        self.mVo = self.data:GetMemberVo()
        if (self.mVo.post == 0 or self.mVo.post == 1) and  not self.data:GetBuildFinish(self.build_info.seq) then
            self.BuildBuild:SetActive(true)
            self.BuildGive:SetActive(false)
        end
    else
        self.BuildBuild:SetActive(false)
        self.BuildGive:SetActive(true)
    end
end
function GuildBuild:FlushResultInfo()
    local info = self.data:GetGoodsInfo()
    if next(info) == nil then
        UH.SetText(self.BuildGX, string.format(self.lang.GXGetDesc, 0))
        UH.SetText(self.BuildJZ, string.format(self.lang.JZGetDesc, 0))
        return
    end
    local list = self.data:GetDonateList(self.build_info.seq)
    local gx = 0
    local jz = 0
    for k, v in pairs(info) do
        gx = gx + list[k].family_contribute * v
        jz = jz + list[k].build_num * v
    end
    UH.SetText(self.BuildGX, string.format(self.lang.GXGetDesc, gx))
    UH.SetText(self.BuildJZ, string.format(self.lang.JZGetDesc, self.build_info.build_num == 0 and 0 or jz))
end
function GuildBuild:OnClickJuanXian()
    local info = self.data:GetGoodsInfo()
    local list = self.data:GetDonateList(self.build_info.seq)
    local param_t = {
        guild_id = self.data:GetGuildID(),
        item_id = 0,
        num = 0,
        build_seq = self.build_info.seq
    }
    for k, v in pairs(info) do
        param_t.item_id = list[k].stuff_id
        param_t.num = v
        GuildCtrl.SendGuildBuildDonateReq(param_t)
    end
end

function GuildBuild:OnClickBuild()
    local param_t = {
        guild_id = self.data:GetGuildID(),
        build_seq = self.build_info.seq
    }
    -- GuildCtrl.SendGuildBuildReq(param_t)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BUILD_INFO,param = self.data:GetGuildID()})
end

GuildBuildItem = GuildBuildItem or DeclareMono("GuildBuildItem", UIWidgetBaseItem)
function GuildBuildItem:GuildBuildItem()
    -- body
end

function GuildBuildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.build_name)
end

function GuildBuildItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

GuildBuildGoodsItem = GuildBuildGoodsItem or DeclareMono("GuildBuildGoodsItem", UIWidgetBaseItem)

function GuildBuildGoodsItem:GuildBuildGoodsItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end

function GuildBuildGoodsItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.New({item_id = data.stuff_id})
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:Name())
    UH.SetText(self.Num, 0)
    UH.SetText(self.HaveNum, string.format(self.lang.HaveNum, item:Num()))
    UH.SetText(self.GongXian, string.format(self.lang.GXDesc, data.family_contribute))
    UH.SetText(self.JianZaoZhi, string.format(self.lang.JZDesc, data.build_num))
    self.cur_num = item:Num()
    self.create_num = 0
end

function GuildBuildGoodsItem:OnClickAdd(num)
    if self.cur_num == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[42])
        return
    end
    self.create_num = self.create_num + num
    if self.create_num < 0 then
        self.create_num = 0
    end
    if self.create_num > self.cur_num then
        self.create_num = self.cur_num
    end
    local today = self.guild_data:GetDonateCount(self.data.index)
    local limit = self.data.donate_limit - today
    if self.create_num > limit then
        self.create_num = limit
        if limit == 0 then
            PublicPopupCtrl.Instance:Center(ErrorText[43])
        end
    end
    self:FlushResult()
end
function GuildBuildGoodsItem:FlushResult()
    UH.SetText(self.Num, self.create_num)
    self.guild_data:CalcuDonateResult(self.data.index, self.create_num)
end
