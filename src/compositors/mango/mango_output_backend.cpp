#include "compositors/mango/mango_output_backend.h"

#include "compositors/mango/mango_runtime.h"
#include "wayland/wayland_connection.h"

#include <algorithm>
#include <string>
#include <vector>

namespace compositors::mango {

  bool setOutputPower(MangoRuntime& runtime, WaylandConnection& wayland, bool on) {
    static std::vector<std::string> s_knownConnectors;
    for (const auto& output : wayland.outputs()) {
      if (!output.connectorName.empty()
          && std::ranges::find(s_knownConnectors, output.connectorName) == s_knownConnectors.end()) {
        s_knownConnectors.push_back(output.connectorName);
      }
    }

    bool launchedAny = false;
    for (const auto& connector : s_knownConnectors) {
      // sleep_monitor/wakeup_monitor toggle DPMS-style power only. The
      // similarly-named enable_monitor/disable_monitor instead remove and
      // re-add the output from mango's layout entirely, which drops any
      // configured monitorrule position (x/y, transform) and can leave
      // outputs re-placed in the wrong order on wake.
      if (runtime.dispatch((std::string(on ? "wakeup_monitor," : "sleep_monitor,") + connector))) {
        launchedAny = true;
      }
    }
    return launchedAny;
  }

} // namespace compositors::mango
