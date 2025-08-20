#include <stdafx.h>
#include "panel_config_binary.h"

#include <utils/guid_helpers.h>

#include <qwr/string_helpers.h>
#include <qwr/type_traits.h>
#include <qwr/winapi_error_helpers.h>

namespace smp::config::binary
{
	enum class JsValueType : uint32_t
	{ // Take care changing this: used in config
		pt_boolean = 0,
		pt_int32 = 1,
		pt_double = 2,
		pt_string = 3,
	};

	PanelSettings LoadSettings(stream_reader& reader, abort_callback& abort)
	{
		try
		{
			PanelSettings panelSettings;
			PanelSettings_InMemory payload;

			reader.skip_object(sizeof(false), abort); // skip "delay load"
			panelSettings.id = [&] {
				GUID guid;
				reader.read_object_t(guid, abort);

				const auto guidStr = utils::GuidToStr(guid);
				return qwr::ToU8(guidStr);
			}();
			reader.read_object(&panelSettings.edgeStyle, sizeof(panelSettings.edgeStyle), abort);
			panelSettings.properties = LoadProperties(reader, abort);
			reader.skip_object(sizeof(false), abort); // skip "disable before"
			reader.read_object_t(payload.shouldGrabFocus, abort);
			reader.skip_object(sizeof(WINDOWPLACEMENT), abort); // skip WINDOWPLACEMENT
			payload.script = reader.read_string(abort).get_ptr();
			reader.read_object_t(panelSettings.isPseudoTransparent, abort);

			panelSettings.payload = payload;

			return panelSettings;
		}
		catch (const pfc::exception& e)
		{
			throw qwr::QwrException(e.what());
		}
	}

	PanelProperties LoadProperties(stream_reader& reader, abort_callback& abort)
	{
		try
		{
			PanelProperties properties;

			uint32_t count;
			reader.read_lendian_t(count, abort);

			for (auto i: ranges::views::indices(count))
			{
				(void)i;

				mozjs::SerializedJsValue serializedValue;

				const std::string u8PropName = reader.read_string(abort).get_ptr();

				uint32_t valueType;
				reader.read_lendian_t(valueType, abort);

				switch (static_cast<JsValueType>(valueType))
				{
				case JsValueType::pt_boolean:
				{
					bool value;
					reader.read_lendian_t(value, abort);
					serializedValue = value;
					break;
				}
				case JsValueType::pt_int32:
				{
					int32_t value;
					reader.read_lendian_t(value, abort);
					serializedValue = value;
					break;
				}
				case JsValueType::pt_double:
				{
					double value;
					reader.read_lendian_t(value, abort);
					serializedValue = value;
					break;
				}
				case JsValueType::pt_string:
				{
					serializedValue = reader.read_string(abort).get_ptr();
					break;
				}
				default:
				{
					assert(0);
					continue;
				}
				}

				properties.values.emplace(qwr::ToWide(u8PropName), std::make_shared<mozjs::SerializedJsValue>(serializedValue));
			}

			return properties;
		}
		catch (const pfc::exception& e)
		{
			throw qwr::QwrException(e.what());
		}
	}
}
