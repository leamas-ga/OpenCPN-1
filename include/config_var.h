#ifndef CONFIG_VAR_H
#define CONFIG_VAR_H

#include <map>
#include <string>

#include <wx/config.h>
#include <wx/event.h>

/**
 * Notify/listen support for coordination between the MVC Model and
 * Controller. Here is three use cases:
 *   - GlobalVar handles notify/listening for changes in a global variable
 *   - ConfigVar handles notify/listening on a configuration variable.
 *   - EventVar offers generic notify/listen based on a shared variable.
 */
namespace ocpn {

/** Return address as printable string. */
std::string ptr_key(const void* ptr);

/**
 *  Helper class, not for public consumption. Basically a singleton map of
 *  listeners where singletons are managed by key, one for each key value.
 */
class SingletonVar {
public:
  static SingletonVar* getInstance(const std::string& key);
  std::map<wxEvtHandler*, wxEventType> listeners;

private:
  SingletonVar() {}
  SingletonVar(const SingletonVar&);    // not implemented
  void operator=(const SingletonVar&);  // not implemented
};


/**  The observable notify/listen basic nuts and bolts.  */
class ObservedVar {
public:
  ObservedVar(const std::string& key)
      : singleton(SingletonVar::getInstance(key)) {}

  /** Set object to send ev_type to listener on variable changes. */
  void listen(wxEvtHandler* listener, wxEventType type);

  /** Notify all listeners about variable change. */
  const virtual void notify();

protected:
  /**
   * Notify all listeners: send them a 'type' wxCommandEvent message
   * as defined by listen() with optional data available using GetString()
   * and/or GetClientData().
   */
  const void notify(const std::string& s, void* client_data);


private:
  SingletonVar* const singleton;
};

/**
 * Generic event handling between MVC Model and Controller based on a
 * shared EventVar variable
 *
 * Model usage:
 *
 *     class Model: ...
 *     public:
 *       EventVar change;
 *
 *       void some_method() {
 *         ...
 *         change.notify("new value")
 *       }
 *
 * Controller/GUI usage:
 *     wxDEFINE_EVENT(EVT_FOO, wxCommandEvent);
 *     class  Gui: public wxWindow {
 *     public:
 *       Gui:Gui(Model& model) {
 *         model.change.listen(this, EVT_FOO);
 *         Bind(EVT_FOO, [&](wxCommandEvent ev) {
 *           auto s = ev.GetString();    s -> "new value"
 *           ... do something;
 *         });
 *       }
 *     }
 */
class EventVar : public ObservedVar {
public:
  EventVar() : ObservedVar(autokey()) {}

  /** Notify all listeners, no data supplied. */
  const void notify() { ObservedVar::notify("", 0); }

  /** Notify all listeners about variable change with ClientData. */
  const void notify(void* data) { ObservedVar::notify("", data); }

  /** Notify all listeners about variable change with a string. */
  const void notify(const std::string& s) { ObservedVar::notify(s, 0); }

private:
  std::string autokey();
};


/**
 *  Support for configuration variables which lives in a wxConfigBase
 *  object. Supports int, bool, double, std::string and wxString. Besides
 *  basic set()/get() also provides notification events when value changes.
 *
 *  Client usage, reading and setting a value:
 *
 *     ocpn::ConfigVar<bool> expert("/PlugIns", "CatalogExpert", pConfig);
 *     bool old_value = expert.get(false);
 *     expert.set(false);
 *
 *  Client usage, listening to value changes:
 *
 *     ocpn::ConfigVar<bool> expert("/PlugIns", "CatalogExpert", pConfig);
 *
 *     // expert sends a wxCommandEVent of type EVT_FOO to this on changes:
 *     wxDEFINE_EVENT(EVT_FOO, wxCommandEvent);
 *     expert.listen(this, EVT_FOO)
 *
 *     // Handle  EVT_FOO as any event when it arrives, for example:
 *     Bind(EVT_FOO, [](wxCommandEvent&) { cout << "value has changed"; });
 *
 */
template <typename T = std::string>
class ConfigVar : public ObservedVar {
public:
  ConfigVar(const std::string& section_, const std::string& key_,
            wxConfigBase* cb);

  void set(const T& arg);

  const T get(const T& default_val);

private:
  ConfigVar();  // not implemented

  const std::string section;
  const std::string key;
  wxConfigBase* const config;
};

/**
 *  Notify/listen for global variables, supports notification events when
 *  value changes.
 *
 *  Client usage, writing a value + notifying listeners:
 *
 *     ocpn::GlobalVar<wxString> compat_os(&g_compatOS);
 *     compat_os.set("ubuntu-gtk3-x86_64");
 *
 *  Client usage, modifying a value + notifying listeners:
 *
 *     ocpn::GlobalVar<wxString> plugin_array_var(&plugin_array);
 *     plugin_array.Add(new_pic);
 *     plugin_array_var.notify();
 *
 *  Client usage, listening to value changes:
 *
 *     ocpn::GlobalVar<wxString> compat_os(&g_compatOS);
 *
 *     // compat_os sends a wxCommandEVent of type EVT_FOO to this on changes:
 *     wxDEFINE_EVENT(EVT_FOO, wxCommandEvent);
 *     compat_os.listen(this, EVT_FOO)
 *
 *     // Handle  EVT_FOO as any event when it arrives, for example:
 *     Bind(EVT_FOO, [](wxCommandEvent&) { cout << "value has changed"; });
 *
 */
template <typename T>
class GlobalVar : public ObservedVar {
public:
  GlobalVar(T* ptr) : ObservedVar(ptr_key(ptr)), variable(ptr) {}

  void set(const T& arg) {
    *variable = arg;
    ObservedVar::notify();
  }

  const T get() { return *variable; }

private:
  GlobalVar();  // not implemented

  T* const variable;
};

}  // namespace ocpn

#endif
