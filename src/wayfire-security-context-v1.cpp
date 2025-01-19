#include <wayfire/core.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/nonstd/wlroots-full.hpp>
#include <wlr/types/wlr_security_context_v1.h>

class wayfire_security_context_v1_protocol_impl : public wf::plugin_interface_t
{
  public:
    void init() override
    {
        security_context_manager_v1 = wlr_security_context_manager_v1_create(wf::get_core().display);
        wl_display_set_global_filter(wf::get_core().display, security_context_global_filter, this);
    }

    void fini() override
    {}

    bool is_unloadable() override
    {
        return false;
    }

  private:
    static bool is_privileged(const struct wl_global *global)
    {
        if (global == nullptr)
        {
            return false;
        }

        auto const interface = wl_global_get_interface(global);

        if (interface != nullptr)
        {
            std::string name = interface->name;
            return
                name == "zwlr_output_manager_v1" ||
                name == "zwlr_output_power_manager_v1" ||
                name == "zwp_input_method_v1" ||
                name == "ext_foreign_toplevel_list_manager_v1" ||
                name == "zwlr_foreign_toplevel_manager_v1" ||
                name == "zxdg_importer_v1" ||
                name == "zxdg_importer_v2" ||
                name == "zxdg_exporter_v1" ||
                name == "zxdg_exporter_v2" ||
                name == "zwlr_data_control_manager_v1" ||
                name == "zwlr_screencopy_manager_v1" ||
                name == "zwlr_export_dmabuf_manager_v1" ||
                name == "wp_security_context_manager_v1" ||
                name == "zwlr_gamma_control_manager_v1" ||
                name == "zwlr_layer_shell_v1" ||
                name == "ext_session_lock_manager_v1" ||
                name == "zwp_keyboard_shortcuts_inhibit_manager_v1" ||
                name == "zwp_virtual_keyboard_manager_v1" ||
                name == "zwlr_virtual_pointer_manager_v1" ||
                name == "ext_transient_seat_manager_v1" ||
                name == "zxdg_output_manager_v1" ||
                name == "zwlr_input_inhibit_manager_v1" ||
                name == "zwp_input_method_manager_v2" ||
                name == "xwayland_shell_v1" ||
                name == "gtk_shell1" ||
                name == "zwf_shell_manager_v2" ||
                name == "wp_drm_lease_device_v1";
        }

        return false;
    }

    static bool security_context_global_filter(const struct wl_client *client, const struct wl_global *global,
        void *data)
    {
        auto wayfire_security_context = static_cast<wayfire_security_context_v1_protocol_impl*>(data);

        // Restrict usage of privileged protocols to unsandboxed clients
        // TODO: add a way for users to configure an allow-list
        const struct wlr_security_context_v1_state *security_context =
            wlr_security_context_manager_v1_lookup_client(
                wayfire_security_context->security_context_manager_v1, const_cast<struct wl_client*>(client));

        if (is_privileged(global))
        {
            return security_context == nullptr;
        }

        return true;
    }

    struct wlr_security_context_manager_v1 *security_context_manager_v1;
};

DECLARE_WAYFIRE_PLUGIN(wayfire_security_context_v1_protocol_impl);
