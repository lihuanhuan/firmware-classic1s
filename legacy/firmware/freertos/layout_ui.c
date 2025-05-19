#include "FreeRTOS.h"

#include "key_task.h"
#include "layout_ui.h"

#include "common.h"
#include "config.h"
#include "gettext.h"
#include "layout2.h"
#include "util.h"

#define TIMER1S_PERIOD pdMS_TO_TICKS(1000)

void layout_ui_home(void) { layoutHome(); }

void layout_ui_status_bar(void) { layoutStatusLogoEx(true); }

void layout_ui_pin_input(char *title, int pos, int index) {
  layoutInputPin(pos, title, index, false);
}

void layout_ui_pin_error(bool retry) {
  char desc[128] = "";
  char times_str[3] = {0};
  snprintf(desc, 128, "%s", _(C__INCORRECT_PIN_STR_ATTEMPT_LEFT_TRY_AGAIN));

  uint32_t fails = config_getPinFails();
  if (fails > 0 && fails < 10) {
    uint2str(10 - fails, times_str);
    bracket_replace(desc, times_str);
    layoutDialogCenterAdapterV2(
        NULL, &bmp_icon_warning, NULL,
        retry ? &bmp_bottom_right_retry : &bmp_bottom_right_confirm, NULL, NULL,
        NULL, NULL, NULL, NULL, desc);
  } else {
    layoutDialogCenterAdapterV2(
        NULL, &bmp_icon_warning, NULL,
        retry ? &bmp_bottom_right_retry : &bmp_bottom_right_confirm, NULL, NULL,
        NULL, NULL, NULL, NULL,
        _(C__INCORRECT_PIN_0_ATTEMPT_LEFT_DEVICE_WILL_BE_RESET_NOW));
    key_wait_for_exit(TIMER1S_PERIOD);

    uint8_t ui_language_bak = ui_language;
    config_wipe();
    if (ui_language_bak) {
      ui_language = ui_language_bak;
    }
    layoutDialogCenterAdapterV2(
        NULL, &bmp_icon_ok, NULL, &bmp_bottom_right_confirm, NULL, NULL, NULL,
        NULL, NULL, NULL, _(C__DEVICE_RESET_COMPLETE_RESTART_NOW_EXCLAM));
    key_wait_for_exit(0);
#if !EMULATOR
    svc_system_reset();
#endif
  }
  key_wait_for_exit(0);

  if (fails >= 5) {
    // fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
    memset(desc, 0, 128);
    memset(times_str, 0, 3);
    snprintf(desc, 128, "%s",
             _(C__CAUTION_DEVICE_WILL_BE_RESET_AFTER_STR_MORE_TIME_WRONG));
    uint2str(10 - fails, times_str);
    bracket_replace(desc, times_str);
    layoutDialogCenterAdapterV2(NULL, &bmp_icon_warning, NULL,
                                &bmp_bottom_right_arrow, NULL, NULL, NULL, NULL,
                                NULL, NULL, desc);
    key_wait_for_exit(0);
  }
}
