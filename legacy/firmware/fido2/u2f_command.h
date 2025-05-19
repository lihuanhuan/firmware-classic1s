
typedef struct {
  uint8_t reserved;
  uint8_t appId[U2F_APPID_SIZE];
  uint8_t chal[U2F_CHAL_SIZE];
  uint8_t keyHandle[KEY_HANDLE_LEN];
  uint8_t pubKey[U2F_PUBKEY_LEN];
} U2F_REGISTER_SIG_STR;

typedef struct {
  uint8_t appId[U2F_APPID_SIZE];
  uint8_t flags;
  uint8_t ctr[4];
  uint8_t chal[U2F_CHAL_SIZE];
} U2F_AUTHENTICATE_SIG_STR;

void u2f_version(const APDU *a) {
  if (APDU_LEN(*a) != 0) {
    debugLog(0, "", "u2f version - badlen");
    send_u2f_error(U2F_SW_WRONG_LENGTH);
    return;
  }

  // INCLUDES SW_NO_ERROR
  static const uint8_t version_response[] = {'U', '2', 'F',  '_',
                                             'V', '2', 0x90, 0x00};
  debugLog(0, "", "u2f version");
  send_u2f_msg(version_response, sizeof(version_response));
}

void u2fhid_ping(const uint8_t *buf, uint32_t len) {
  debugLog(0, "", "u2fhid_ping");
  send_u2fhid_msg(U2FHID_PING, buf, len);
}

void u2fhid_wink(const uint8_t *buf, uint32_t len) {
  debugLog(0, "", "u2fhid_wink");
  (void)buf;

  if (len > 0) return send_u2fhid_error(cid, ERR_INVALID_LEN);

  U2FHID_FRAME f = {0};
  memzero(&f, sizeof(f));
  f.cid = cid;
  f.init.cmd = U2FHID_WINK;
  f.init.bcntl = 0;
  queue_u2f_pkt(&f);
  usb_u2f_data_send();
}

void getReadableAppId(const uint8_t appid[U2F_APPID_SIZE],
                      const char **appname) {
  static char buf[8 + 2 + 8 + 1];

  for (unsigned int i = 0; i < sizeof(u2f_well_known) / sizeof(U2FWellKnown);
       i++) {
    if (memcmp(appid, u2f_well_known[i].appid, U2F_APPID_SIZE) == 0) {
      *appname = u2f_well_known[i].appname;
      return;
    }
  }

  data2hex(appid, 4, &buf[0]);
  buf[8] = buf[9] = '.';
  data2hex(appid + (U2F_APPID_SIZE - 4), 4, &buf[10]);
  *appname = buf;
}

void handle_u2f_register(const APDU *a) {
  static U2F_REGISTER_REQ last_req;
  const U2F_REGISTER_REQ *req = (U2F_REGISTER_REQ *)a->data;
  bool new_request = false;

  if (!config_isInitialized()) {
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  if (!session_isUnlocked()) {
    create_pin_task(PIN_OPERATION_VERIFY);
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  if (!se_fido_get_seed_cached()) {
    create_gen_seed_task();
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  // If this request is different from last request, reset state machine
  if (memcmp(&last_req, req, sizeof(last_req)) != 0) {
    memcpy(&last_req, req, sizeof(last_req));
    new_request = true;
  }

  // Validate basic request parameters
  if (APDU_LEN(*a) != sizeof(U2F_REGISTER_REQ)) {
    send_u2f_error(U2F_SW_WRONG_LENGTH);
    return;
  }

  if (new_request) {
    if (0 == memcmp(req->appId, BOGUS_APPID_CHROME, U2F_APPID_SIZE) ||
        0 == memcmp(req->appId, BOGUS_APPID_FIREFOX, U2F_APPID_SIZE)) {
      if (cid == last_good_auth_check_cid) {
        layoutDialogCenterAdapterV2(
            _(T__U2F_ALREADY_REGISTER), NULL, NULL, &bmp_bottom_right_confirm,
            NULL, NULL, NULL, NULL, NULL, NULL,
            _(C__THIS_U2F_DEVICE_IS_ALREADY_REGISTERED_IN_THIS_APP));
      } else {
        layoutDialogCenterAdapterV2(
            _(T__U2F_NOT_REGISTER), NULL, NULL, &bmp_bottom_right_confirm, NULL,
            NULL, NULL, NULL, NULL, NULL,
            _(C__THIS_U2F_DEVICE_IS_NOT_REGISTERED_IN_THIS_APP));
      }
    } else {
      const char *appname = NULL;
      getReadableAppId(req->appId, &appname);
      layoutDialogAdapterEx(_(T__U2F_REGISTER), NULL, NULL,
                            &bmp_bottom_right_arrow, NULL, NULL,
                            _(I__APP_NAME_COLON), appname, NULL, NULL);
    }
    set_key_state(KEY_STATE_CMD);
    xQueueReset(cmd_key_msg_queue);
  }
  key_msg_t msg;
  if (xQueueReceive(cmd_key_msg_queue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
    if (msg.value != KEY_CONFIRM) {
      layout_set_home();
      send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
      return;
    }
  } else {
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  layout_set_home();
  memset(&last_req, 0, sizeof(last_req));

  // Buttons said yes
  uint8_t data[sizeof(U2F_REGISTER_RESP) + 2] = {0};
  U2F_REGISTER_RESP *resp = (U2F_REGISTER_RESP *)&data;
  memzero(data, sizeof(data));

  resp->registerId = U2F_REGISTER_ID;
  resp->keyHandleLen = KEY_HANDLE_LEN;

  memcpy(resp->keyHandleCertSig + resp->keyHandleLen, U2F_ATT_CERT,
         sizeof(U2F_ATT_CERT));

  uint8_t sig[64] = {0};

  if (!se_u2f_register(req->appId, req->chal, resp->keyHandleCertSig,
                       (uint8_t *)&resp->pubKey, sig)) {
    send_u2f_error(U2F_SW_WRONG_DATA);
    return;
  }

  // Where to write the signature in the response
  uint8_t *resp_sig =
      resp->keyHandleCertSig + resp->keyHandleLen + sizeof(U2F_ATT_CERT);
  // Convert to der for the response
  const uint8_t sig_len = ecdsa_sig_to_der(sig, resp_sig);

  // Append success bytes
  memcpy(resp->keyHandleCertSig + resp->keyHandleLen + sizeof(U2F_ATT_CERT) +
             sig_len,
         "\x90\x00", 2);

  int l = 1 /* registerId */ + U2F_PUBKEY_LEN + 1 /* keyhandleLen */ +
          resp->keyHandleLen + sizeof(U2F_ATT_CERT) + sig_len + 2;

  send_u2f_msg(data, l);

  return;
}

void handle_u2f_authenticate(const APDU *a) {
  const U2F_AUTHENTICATE_REQ *req = (U2F_AUTHENTICATE_REQ *)a->data;
  static U2F_AUTHENTICATE_REQ last_req;
  bool new_request = false;

  if (APDU_LEN(*a) < 64) {  /// FIXME: decent value
    send_u2f_error(U2F_SW_WRONG_LENGTH);
    return;
  }

  if (req->keyHandleLen != KEY_HANDLE_LEN) {
    send_u2f_error(U2F_SW_WRONG_DATA);  // error:bad key handle
    return;
  }

  if (!config_isInitialized()) {
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  if (!session_isUnlocked()) {
    create_pin_task(PIN_OPERATION_VERIFY);
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  if (!se_fido_get_seed_cached()) {
    create_gen_seed_task();
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  if (!se_u2f_validate_handle(req->appId, req->keyHandle)) {
    send_u2f_error(U2F_SW_WRONG_DATA);
    return;
  }

  if (a->p1 == U2F_AUTH_CHECK_ONLY) {
    // This is a success for a good keyhandle
    // A failed check would have happened earlier
    // error: testof-user-presence is required
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    last_good_auth_check_cid = cid;
    return;
  }

  if (a->p1 != U2F_AUTH_ENFORCE && a->p1 != U2F_NOT_AUTH_ENFORCE) {
    // error:bad key handle
    send_u2f_error(U2F_SW_WRONG_DATA);
    return;
  }

  // If this request is different from last request, reset state machine
  if (memcmp(&last_req, req, sizeof(last_req)) != 0) {
    memcpy(&last_req, req, sizeof(last_req));
    new_request = true;
  }

  if (new_request) {
    const char *appname = NULL;
    getReadableAppId(req->appId, &appname);
    layoutDialogAdapterEx(_(T__U2F_AUTHENTICATE), NULL, NULL,
                          &bmp_bottom_right_arrow, NULL, NULL,
                          _(I__APP_NAME_COLON), appname, NULL, NULL);
    set_key_state(KEY_STATE_CMD);
    xQueueReset(cmd_key_msg_queue);
  }
  key_msg_t msg;
  if (xQueueReceive(cmd_key_msg_queue, &msg, pdMS_TO_TICKS(200)) == pdPASS) {
    if (msg.value != KEY_CONFIRM) {
      layout_set_home();
      send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
      return;
    }
  } else {
    send_u2f_error(U2F_SW_CONDITIONS_NOT_SATISFIED);
    return;
  }

  layout_set_home();
  memset(&last_req, 0, sizeof(last_req));

  // Buttons said yes
  uint8_t buf[(sizeof(U2F_AUTHENTICATE_RESP)) + 2] = {0};
  U2F_AUTHENTICATE_RESP *resp = (U2F_AUTHENTICATE_RESP *)&buf;

  uint8_t sig[64] = {0};
  resp->flags = a->p1 == U2F_AUTH_ENFORCE ? U2F_AUTH_FLAG_TUP : 0;

  if (!se_u2f_authenticate(req->appId, req->keyHandle, req->chal, resp->ctr,
                           sig)) {
    send_u2f_error(U2F_SW_WRONG_DATA);
    return;
  }

  // Copy DER encoded signature into response
  const uint8_t sig_len = ecdsa_sig_to_der(sig, resp->sig);

  // Append OK
  memcpy(buf + sizeof(U2F_AUTHENTICATE_RESP) - U2F_MAX_EC_SIG_SIZE + sig_len,
         "\x90\x00", 2);
  send_u2f_msg(
      buf, sizeof(U2F_AUTHENTICATE_RESP) - U2F_MAX_EC_SIG_SIZE + sig_len + 2);

  return;
}
