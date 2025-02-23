require "io"
require "os"

function string.split(self, delim, limit)
	local arr = {}
	local iter = 1
	while iter <= #self do
		if limit then
			if limit == 0 then break
			else limit = limit - 1 end
		end
		local st, en = self:find(delim, iter)
		if st and en ~= 0 then
			local part = self:sub(iter, st - 1);
			if #part ~= 0 then table.insert(arr, part) end
			iter = en + 1
		else break end
	end
	local part = self:sub(iter, #self);
	if #part ~= 0 then table.insert(arr, part) end
	return arr
end

function verbose(msg)
	print(msg)
end

function toshell(cmd)
	return "sh -c \"".. cmd:gsub("\\", "\\\\"):gsub("\"", "\\\"") .."\""
end

function shexec(cmd)
	return os.execute(toshell(cmd))
end

function shexecf(cmd, ...)
	return os.execute(toshell(string.format(cmd, ...)))
end

-- check if directory is present and not empty
function checkdir(path)
	local ls_p = io.popen(toshell("/bin/ls -A \"".. path .."\" 2>/dev/null", "r"));
	if not ls_p then
		error("use human's unix shell u fumb duck")
	end
	local outp = ls_p:read("*all");
	local _, __, ec = ls_p:close();
	if ec ~= nil and ec ~= 0 then
		return false
	end
	outp = outp:gsub("[ \n\t\r\n\v\f]+", "");
	return #outp > 0
end

function checkfile(path)
	local fp = io.open(path, "r");
	if not fp then return false end
	fp:close()
	return path
end
