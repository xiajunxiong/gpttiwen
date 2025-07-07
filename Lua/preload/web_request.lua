local unityWebRequest = UnityEngine.Networking.UnityWebRequest

local web_request = {
	requests = {},

}
WebRequestDataType = {
    Bytes = 1,
    Text = 2,
}
-- data_type == 默认为 WebRequestDataType.Text
function web_request:Request(url,callback,data_type)
	local request = unityWebRequest.Get(url)
	local wait = request:SendWebRequest()
	if callback ~= nil then
		table.insert(self.requests,{wait = wait,callback = callback,data_type = data_type})
	end
end

function web_request:Post(url,callback,form)
		local wwwform = UnityEngine.WWWForm.New()
		for k,v in pairs(form) do
			wwwform:AddField(tostring(k),tostring(v))
		end
		local request = unityWebRequest.Post(url, wwwform)
		local wait = request:SendWebRequest()
		if callback ~= nil then
			table.insert(self.requests,{wait = wait,callback = callback})
		end
	end

function web_request:Upload(url, data, callback)
	local upload = unityWebRequest.Put(url, data)
	local wait = upload:SendWebRequest()
	if callback ~= nil then
		table.insert(self.requests,{wait = wait,callback = callback})
	end
end

function web_request:Update()
	for i = #self.requests,1,-1 do
		local req_data = self.requests[i]
		if req_data.wait.isDone then
			local unity_req = req_data.wait.webRequest
			if req_data.data_type == WebRequestDataType.Bytes then
				req_data.callback(unity_req.downloadHandler.data,unity_req.error)
			elseif req_data.data_type == WebRequestDataType.Text then
				req_data.callback(unity_req.downloadHandler.text,unity_req.error)
			else
			    req_data.callback(unity_req.downloadHandler.text,unity_req.error)
			end
			table.remove(self.requests,i)
		end
	end
	--table.remove()
end

function web_request:UrlEncode(str)
  if (str) then
    str = string.gsub (str, "\n", "\r\n")
    str = string.gsub (str, "([^%w %-%_%.%~])",
        function (c) return string.format ("%%%02X", string.byte(c)) end)
    str = string.gsub (str, " ", "+")
  end
  return str
end


return web_request