#include "notification/notification_manager.h"

#include <iostream>

namespace {

  bool check(bool condition, const char* message) {
    if (!condition) {
      std::cerr << "FAIL: " << message << '\n';
    }
    return condition;
  }

} // namespace

int main() {
  bool ok = true;

  // Free-function normalization with an explicit server default.
  ok &= check(normalizeNotifyExpireTimeout(-1, 2500) == 2500, "-1 resolves to the configured server default");
  ok &= check(normalizeNotifyExpireTimeout(-1) == kDefaultNotificationTimeout, "-1 defaults to the built-in default");
  ok &= check(normalizeNotifyExpireTimeout(0, 2500) == 0, "0 stays persistent regardless of the server default");
  ok &= check(normalizeNotifyExpireTimeout(900, 2500) == 900, "explicit positive timeouts are preserved");

  // Manager exposes the configured default and rejects negative values.
  NotificationManager manager;
  ok &= check(manager.defaultTimeoutMs() == kDefaultNotificationTimeout, "manager starts on the built-in default");
  manager.setDefaultTimeoutMs(2500);
  ok &= check(manager.defaultTimeoutMs() == 2500, "configured timeout is applied");
  manager.setDefaultTimeoutMs(-5);
  ok &= check(manager.defaultTimeoutMs() == kDefaultNotificationTimeout, "negative values fall back to the default");

  // allow_permanent = false expires otherwise-permanent notifications at the configured duration.
  manager.setDefaultTimeoutMs(2500);
  manager.setFilters({NotificationFilterConfig{
      .name = "transient-only",
      .match = "sticky",
      .allowPermanent = false,
  }});
  const uint32_t id = manager.addOrReplace(
      NotificationRequest{
          .appName = "Sticky App",
          .summary = "forever is a long time",
          .timeout = 0,
          .transient = true,
      }
  );
  bool found = false;
  for (const auto& notification : manager.all()) {
    if (notification.id == id) {
      found = true;
      ok &= check(notification.timeout == 2500, "disallowed permanent notification uses the configured duration");
    }
  }
  ok &= check(found, "filtered notification was stored");

  // Unmatched permanent notifications stay persistent.
  const uint32_t otherId = manager.addOrReplace(
      NotificationRequest{
          .appName = "Other App",
          .summary = "still here",
          .timeout = 0,
          .transient = true,
      }
  );
  for (const auto& notification : manager.all()) {
    if (notification.id == otherId) {
      ok &= check(notification.timeout == 0, "unmatched permanent notification remains persistent");
    }
  }

  return ok ? 0 : 1;
}
