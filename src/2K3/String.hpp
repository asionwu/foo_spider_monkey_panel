#pragma once

using Strings = std::vector<std::string>;


inline pfc::string_base& operator<<(pfc::string_base& fmt, const std::string& source)
{
	fmt.add_string_(source.c_str());
	return fmt;
}

static std::string json_to_string(JSON& j)
{
	if (j.is_string())
	{
		const auto str = j.get<std::string>();
		return pfc::string8(str.c_str()).trim(' ').get_ptr();
	}
	else if (j.is_number())
	{
		return j.dump();
	}
	else
	{
		return {};
	}
}

static Strings json_to_strings(JSON& j)
{
	if (!j.is_array())
	{
		j = JSON::array({ j });
	}

	auto transform = [](auto&& j2) { return json_to_string(j2); };
	auto filter = [](auto&& str) { return str.length() > 0; };
	return j | ranges::views::transform(transform) | ranges::views::filter(filter) | ranges::to<Strings>();
}
