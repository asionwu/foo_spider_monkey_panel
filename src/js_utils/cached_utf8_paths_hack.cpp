#include <stdafx.h>
#include "cached_utf8_paths_hack.h"

namespace mozjs::hack
{

Map g_cachedPaths;

std::string CacheUtf8Path(std::string_view path)
{
	const std::string hash = hasher_md5::get()->process_single_string(path.data()).asString().get_ptr();
	g_cachedPaths.try_emplace(hash, path);
	return hash;
}

std::optional<std::filesystem::path> GetCachedUtf8Path(std::string_view pathId)
{
	const auto it = g_cachedPaths.find({ pathId.data(), pathId.size() });
	if (it == g_cachedPaths.end())
	{
		return std::nullopt;
	}

	return qwr::ToWide(it->second);
}

const Map& GetAllCachedUtf8Paths()
{
	return g_cachedPaths;
}

} // namespace mozjs::hack
