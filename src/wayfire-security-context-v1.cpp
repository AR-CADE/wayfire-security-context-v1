#include <wayfire/core.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/nonstd/wlroots-full.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wlr/types/wlr_security_context_v1.h>

const std::unordered_set<std::string> initial_blacklist = {
    "zwlr_output_manager_v1",
    "zwlr_output_power_manager_v1",
    "zwp_input_method_v1",
    "ext_foreign_toplevel_list_manager_v1",
    "zwlr_foreign_toplevel_manager_v1",
    "zxdg_importer_v1",
    "zxdg_importer_v2",
    "zxdg_exporter_v1",
    "zxdg_exporter_v2",
    "zwlr_data_control_manager_v1",
    "zwlr_screencopy_manager_v1",
    "zwlr_export_dmabuf_manager_v1",
    "wp_security_context_manager_v1",
    "zwlr_gamma_control_manager_v1",
    "zwlr_layer_shell_v1",
    "ext_session_lock_manager_v1",
    "zwp_keyboard_shortcuts_inhibit_manager_v1",
    "zwp_virtual_keyboard_manager_v1",
    "zwlr_virtual_pointer_manager_v1",
    "ext_transient_seat_manager_v1",
    "zxdg_output_manager_v1",
    "zwlr_input_inhibit_manager_v1",
    "zwp_input_method_manager_v2",
    "xwayland_shell_v1",
    "gtk_shell1",
    "zwf_shell_manager_v2",
    "wp_drm_lease_device_v1"};

typedef std::tuple<std::string, std::string, std::string, std::string> Application;
typedef std::pair<std::string, std::string> DenyRule;
typedef std::pair<std::string, std::string> AllowRule;

class wayfire_security_context_v1_protocol_impl : public wf::plugin_interface_t
{
  public:
    void init() override
    {
        reload();

        auto on_reload = [=] ()
        {
            reload();
        };

        blacklist_mode_option.set_callback(on_reload);
        applications_option.set_callback(on_reload);
        allowlist_option.set_callback(on_reload);
        denylist_option.set_callback(on_reload);
        blacklist_option.set_callback(on_reload);

        security_context_manager_v1 = wlr_security_context_manager_v1_create(wf::get_core().display);
        wl_display_set_global_filter(wf::get_core().display, security_context_can_activate, this);
    }

    void fini() override
    {
        wl_display_set_global_filter(wf::get_core().display, nullptr, nullptr);
        blacklist_mode_option.set_callback(nullptr);
        applications_option.set_callback(nullptr);
        allowlist_option.set_callback(nullptr);
        denylist_option.set_callback(nullptr);
        blacklist_option.set_callback(nullptr);
    }

    bool is_unloadable() override
    {
        return false;
    }

  private:
    wf::option_wrapper_t<std::string> blacklist_mode_option{
        "security-context-v1/blacklist_mode"};
    wf::option_wrapper_t<wf::config::compound_list_t<std::string, std::string,
        std::string>> applications_option{"security-context-v1/applications"};
    wf::option_wrapper_t<wf::config::compound_list_t<std::string>> allowlist_option{
        "security-context-v1/allowlist"};
    wf::option_wrapper_t<wf::config::compound_list_t<std::string>> denylist_option{
        "security-context-v1/denylist"};
    wf::option_wrapper_t<wf::config::compound_list_t<std::string>> blacklist_option{
        "security-context-v1/blacklist"};

    std::unordered_set<std::string> blacklist;
    std::unordered_map<std::string, Application> applications;
    std::vector<DenyRule> denylist;
    std::vector<AllowRule> allowlist;

    static bool application_is_blacklisted(const std::string & protocol, const Application application,
        const wayfire_security_context_v1_protocol_impl *wayfire_security_context)
    {
        const auto [id, app_id, sandbox_engine, blacklist_mode] = application;

        std::unordered_set<std::string> application_blacklist;

        if (blacklist_mode == "inherit")
        {
            std::copy(wayfire_security_context->blacklist.begin(),
                wayfire_security_context->blacklist.end(),
                std::inserter(application_blacklist, application_blacklist.end()));
        } else if (blacklist_mode == "initial")
        {
            std::copy(initial_blacklist.begin(), initial_blacklist.end(),
                std::inserter(application_blacklist, application_blacklist.end()));
        } else if (blacklist_mode == "reset")
        {
            //
            // do nothing
            //
        }

        for (const auto & denied : wayfire_security_context->denylist)
        {
            if (denied.first == id)
            {
                application_blacklist.insert(denied.second);
            }
        }

        for (const auto & allowed : wayfire_security_context->allowlist)
        {
            if (allowed.first == id)
            {
                application_blacklist.erase(allowed.second);
            }
        }

        return application_blacklist.find(protocol) != application_blacklist.end();
    }

    static bool is_blacklisted(const std::string & protocol, const std::string & app_id,
        const std::string & sandbox_engine,
        const wayfire_security_context_v1_protocol_impl *wayfire_security_context)
    {
        auto const found_application = wayfire_security_context->applications.find(app_id);

        if (found_application != wayfire_security_context->applications.end())
        {
            const auto [_, found_app_id, found_sandbox_engine, __] = found_application->second;

            if ((app_id == found_app_id) &&
                (((sandbox_engine.empty() == false) &&
                  (sandbox_engine == found_sandbox_engine)) || found_sandbox_engine.empty()))
            {
                return application_is_blacklisted(protocol, found_application->second,
                    wayfire_security_context);
            }
        }

        return wayfire_security_context->blacklist.find(protocol) !=
               wayfire_security_context->blacklist.end();
    }

    static bool can_activate(const std::string & protocol, const std::string & app_id,
        const std::string & sandbox_engine,
        const wayfire_security_context_v1_protocol_impl *wayfire_security_context)
    {
        return is_blacklisted(protocol, app_id, sandbox_engine,
            wayfire_security_context) ? false : true;
    }

    static bool security_context_can_activate(const struct wl_client *client,
        const struct wl_global *global,
        void *data)
    {
        if (data == nullptr)
        {
            return true;
        }

        if (global == nullptr)
        {
            return true;
        }

        auto const wayfire_security_context = static_cast<wayfire_security_context_v1_protocol_impl*>(data);

        // Restrict usage of privileged protocols to unsandboxed clients
        const struct wlr_security_context_v1_state *security_context =
            wlr_security_context_manager_v1_lookup_client(
                wayfire_security_context->security_context_manager_v1, const_cast<struct wl_client*>(client));

        if (security_context != nullptr)
        {
            auto interface = wl_global_get_interface(global);

            if ((interface == nullptr) || (interface->name == nullptr))
            {
                return true;
            }

            std::string const name = interface->name;

            if (name.empty())
            {
                return true;
            }

            return can_activate(name, security_context->app_id, security_context->sandbox_engine,
                wayfire_security_context);
        }

        return true;
    }

    void setup_applications_from_config()
    {
        applications.clear();

        auto const list = applications_option.value();

        for (const auto& [id, app_id, sandbox_engine, blacklist_mode] : list)
        {
            applications[app_id] = {id, app_id, sandbox_engine, blacklist_mode};
        }
    }

    void setup_allowlist_from_config()
    {
        allowlist.clear();

        auto const list = allowlist_option.value();

        for (const auto& [ids, rule] : list)
        {
            std::stringstream stream(ids);

            std::string id;
            if (!getline(stream, id, '_'))
            {
                continue;
            }

            std::string version;
            if (!getline(stream, version, '_'))
            {
                continue;
            }

            allowlist.push_back(std::pair(id, rule));
        }
    }

    void setup_denylist_from_config()
    {
        denylist.clear();

        auto const list = denylist_option.value();

        for (const auto& [ids, rule] : list)
        {
            std::stringstream stream(ids);

            std::string id;
            if (!getline(stream, id, '_'))
            {
                continue;
            }

            std::string version;
            if (!getline(stream, version, '_'))
            {
                continue;
            }

            denylist.push_back(std::pair(id, rule));
        }
    }

    void setup_blacklist_from_config()
    {
        blacklist.clear();

        if (blacklist_mode_option.value() == "initial")
        {
            std::copy(initial_blacklist.begin(), initial_blacklist.end(),
                std::inserter(blacklist, blacklist.end()));
        } else if (blacklist_mode_option.value() == "reset")
        {
            //
            // do nothing
            //
        }

        auto const list = blacklist_option.value();

        for (const auto& [_, rule] : list)
        {
            blacklist.insert(rule);
        }
    }

    void reload()
    {
        setup_blacklist_from_config();
        setup_applications_from_config();
        setup_allowlist_from_config();
        setup_denylist_from_config();
    }

    struct wlr_security_context_manager_v1 *security_context_manager_v1;
};

DECLARE_WAYFIRE_PLUGIN(wayfire_security_context_v1_protocol_impl);
