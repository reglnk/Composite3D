require "os"
require "io"
require "gmake/util"

local actions, targetOS, buildProfile, outDir = ...

local outFile = outDir .."/assimp.cache"

-- == what can be done to obtain libs ==
-- 'clone' implies 'build'
-- 'build' will run cmake and build the library if needed, according to build profile
-- 'systemlib' will try to find systemwide installed library and use it
-- btw, need some way to detect whether system assimp is built with double precision or not (TODO)
-- 'provided' will try to use prebuilt library by paths provided here
-- if nothing of above, only existing library will be used
local lact = actions:split(",")

-- where to locate or clone the library
local ldir = "external/assimp"

local linc = ldir .."/include"
local lbdir = ldir .."/build/".. targetOS .."/".. buildProfile

local message_verbose = function(...)
	print("[assimp.lua] ".. table.concat({...}))
end

-- ================

shexecf('mkdir -p "%s"', outDir);

local function tryProvided()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
		return
	end
	local bobj = checkdir(lbdir) and checkdir(lbdir .."/include");
	if not bobj then
		message_verbose("not all or no build objects found at [".. lbdir .."]")
		return
	end
	-- TODO find static library if assimp's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = lbdir .. "/bin/libassimp.so";
	else
		ilib = lbdir .. "/lib/libassimp.dll.a";
	end
	if not checkfile(ilib) then
		message_verbose("library not found at [".. ilib .."]");
		return
	end
	return {
		assimp_DIR = ldir,
		assimp_INC = linc ..":".. lbdir .."/include",
		assimp_BDIR = lbdir,
		assimp_LIB = ilib
	}
end

local function trySystemUnix()
	local res = {assimp_INC = "/usr/include/assimp"};
	if not checkdir(res.assimp_INC) then
		message_verbose("includes not found at [".. res.assimp_INC .."]")
		return
	end
	for ld in pairs {"/usr/lib", "/usr/local/lib", "/usr/lib64", "/usr/local/lib64"} do
		if checkfile(ld .."/libassimp.so") then
			res.assimp_LIB = ld .."/libassimp.so";
		end
	end
	if not res.assimp_LIB then
		return
	end
	return res
end

local function trySystemWindows()
	local res = {assimp_DIR = "C:/Program Files (x86)/assimp"};
	if not checkdir(res.assimp_DIR) then
		message_verbose("not found at [".. res.assimp_DIR .."]")
		return
	end
	-- todo
	return res
end

local function tryBuild()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
		return
	end
	shexecf('mkdir -p "%s"', lbdir);
	local fmt = 'cmake -S "%s" -B "%s" -DCMAKE_BUILD_TYPE=Release -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_BUILD_TESTS=OFF';
	if targetOS == "Windows" then fmt = fmt .. ' -G "MinGW Makefiles"' end
	shexecf(fmt, ldir, lbdir);
	shexecf('gmake -C "%s" -j4', lbdir);
	-- TODO find static library if assimp's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = lbdir .. "/bin/libassimp.so";
	else
		ilib = lbdir .. "/lib/libassimp.dll.a";
	end
	if not checkfile(ilib) then
		message_verbose("library not found at [".. ilib .."] - after a build attempt");
		return
	end
	return {
		assimp_DIR = ldir,
		assimp_INC = linc ..":".. lbdir .."/include",
		assimp_BDIR = lbdir,
		assimp_LIB = ilib
	}
end

local function tryClone()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]. cloning...")
		shexecf('git clone https://github.com/assimp/assimp "%s"', ldir)
	end
	shexecf('git -C "%s" checkout c35200e38ea8f058812b83de2ef32c6093b0ece2', ldir) -- 5.4.3
	return tryBuild()
end

local acttab = {
	["provided"] = tryProvided,
	["systemlib"] = trySystemUnix,
	["build"] = tryBuild,
	["clone"] = tryClone,
	["0"] = function() end
};

local success = main_action(lact, acttab, outFile, message_verbose);
if success then
	return
end

message_error("no one of methods worked");
