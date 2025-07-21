#include <stdafx.h>

#include <com_objects/drop_target_impl.h>
#include <events/event_dispatcher.h>
#include <events/event_js_callback.h>
#include <utils/colour_helpers.h>

#include <panel/js_panel_window.h>

namespace smp::panel
{
	class js_panel_window_cui : public js_panel_window, public uie::container_uie_window_v3
	{
	protected:
		DWORD GetColour(unsigned type, const GUID& guid) final
		{
			COLORREF colour = 0; ///< black
			if (type <= cui::colours::colour_active_item_frame)
			{
				cui::colours::helper helper(guid);
				colour = helper.get_colour(static_cast<cui::colours::colour_identifier_t>(type));
			}

			return smp::colour::ColorrefToArgb(colour);
		}

		HFONT GetFont(unsigned type, const GUID& guid) final
		{
			auto api = fb2k::std_api_get<cui::fonts::manager>();

			if (guid != pfc::guid_null)
				return api->get_font(guid);
			else if (type <= cui::fonts::font_type_labels)
				return api->get_font(static_cast<cui::fonts::font_type_t>(type));
			else
				return nullptr;
		}

		void notify_size_limit_changed(LPARAM lp) final
		{
			get_host()->on_size_limit_change(GetHWND(), lp);
		}

	public:
		js_panel_window_cui() : js_panel_window(PanelType::CUI) {}

		LRESULT on_message(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) final
		{
			switch (msg)
			{
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				if (uie::window::g_process_keydown_keyboard_shortcuts(wp))
					return 0;
				break;
			}
			case static_cast<UINT>(smp::MiscMessage::size_limit_changed):
			{
				notify_size_limit_changed(wp);
				return 0;
			}
			default:
				break;
			}

			return OnMessage(hwnd, msg, wp, lp);
		}

		bool have_config_popup() const final
		{
			return true;
		}

		bool is_available(const uie::window_host_ptr&) const final
		{
			return true;
		}

		bool show_config_popup(HWND parent) final
		{
			ShowConfigure(parent);
			return true;
		}

		const GUID& get_extension_guid() const final
		{
			return smp::guid::window_cui;
		}

		unsigned get_type() const final
		{
			return uie::type_toolbar | uie::type_panel;
		}

		void get_category(pfc::string_base& out) const final
		{
			out = "Panels";
		}

		void get_config(stream_writer* writer, abort_callback& abort) const final
		{
			SaveSettings(*writer, abort);
		}

		void get_name(pfc::string_base& out) const final
		{
			out = SMP_NAME;
		}

		void set_config(stream_reader* reader, t_size size, abort_callback& abort)
		{
			LoadSettings(*reader, size, abort, false);
		}

		uie::container_window_v3_config get_window_config() final
		{
			return { TEXT(SMP_WINDOW_CLASS_NAME), false, CS_DBLCLKS };
		}
	};

	uie::window_factory<smp::panel::js_panel_window_cui> g_js_panel_window_cui;
}
