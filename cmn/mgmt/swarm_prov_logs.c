/*** Activate logs ***/
DEF_PROV_LOG_ENTRY(prov_activate_start, PROV_STATE_ACTIVATE, PROV_LOG_PENDING,
    "Attempting provisioning via Activate server: %s");

DEF_PROV_LOG_ENTRY(prov_activate_timed_out, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Provisioning failed: did not receive a response from Activate server after %s seconds");

DEF_PROV_LOG_ENTRY(prov_activate_server_payload_empty, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Provisioning failed: received an empty response from Activate server, aborting provisioning");

DEF_PROV_LOG_ENTRY(prov_activate_other_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Provisioning failed: Unexpected error when communicating with Activate Server: %s");

DEF_PROV_LOG_ENTRY(prov_activate_challenge_response_sent, PROV_STATE_ACTIVATE, PROV_LOG_DEBUG,
    "Sent challenge response to Activate Server: %s");

DEF_PROV_LOG_ENTRY(prov_activate_got_rap_convert, PROV_STATE_ACTIVATE, PROV_LOG_SUCCESS,
    "Received instruction from Activate Server to convert to a Remote AP of mobility controller at %s");

DEF_PROV_LOG_ENTRY(prov_activate_got_cap_convert, PROV_STATE_ACTIVATE, PROV_LOG_SUCCESS,
    "Received instruction from Activate Server to convert to a Campus AP of mobility controller at %s");

DEF_PROV_LOG_ENTRY(prov_activate_got_central, PROV_STATE_ACTIVATE, PROV_LOG_SUCCESS,
    "Received instruction from Activate Server to connect to " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_activate_got_amp, PROV_STATE_ACTIVATE, PROV_LOG_SUCCESS,
    "Received instruction from Activate Server to connect to AMP server at %s using organization '%s'");
  
DEF_PROV_LOG_ENTRY(prov_activate_no_rule, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Received message from Activate Server: there is no provisioning rule specified for this device");
 
DEF_PROV_LOG_ENTRY(prov_activate_got_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Received an error message from Activate server: '%s'");
 
DEF_PROV_LOG_ENTRY(prov_activate_dns_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "DNS look up failed for Activate server at %s");

DEF_PROV_LOG_ENTRY(prov_activate_socket_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Socket error while communicating with Activate server at %s");

DEF_PROV_LOG_ENTRY(prov_activate_connect_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Could not establish TCP connection to Activate server at %s");

DEF_PROV_LOG_ENTRY(prov_activate_proxy_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "HTTPS proxy error while connecting to Activate server at %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_connect_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Error establishing SSL connection to Activate server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "SSL error while communicating with Activate server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_cert_verification_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Could not validate certificate of Activate server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_cert_cn_name_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Certificate of Activate server at %s does not have a matching Common Name");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_tpm_clientauth_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Local tpm clientauth error while communicating with Activate server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_cacert_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Local CA cert error while communicating with Activate server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_activate_ssl_cert_before_date_error, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Before date error while loading local CA cert or establishing ssl connection");

DEF_PROV_LOG_ENTRY(prov_activate_disabled, PROV_STATE_ACTIVATE, PROV_LOG_ERROR,
    "Activate disabled: %s");

/*** Central Logs ***/
DEF_PROV_LOG_ENTRY(prov_central_connect, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Connecting to " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_central_challenge_response_sent, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Sent challenege response to " CENTRAL_NAME " server: %s");
 
DEF_PROV_LOG_ENTRY(prov_central_empty_message_during_login, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "" CENTRAL_NAME " connection error: Received an empty message from " CENTRAL_NAME " during login");

DEF_PROV_LOG_ENTRY(prov_central_empty_message_after_login, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "" CENTRAL_NAME " connection error: Received an empty message from " CENTRAL_NAME " after login");

DEF_PROV_LOG_ENTRY(prov_central_timed_out, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "" CENTRAL_NAME " connection error: did not receive a response from " CENTRAL_NAME " after %s seconds");

DEF_PROV_LOG_ENTRY(prov_central_logged_in, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Logged into " CENTRAL_NAME " server at %s, awaiting command...");

DEF_PROV_LOG_ENTRY(prov_central_config_audit, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received configuration audit command from " CENTRAL_NAME "");

DEF_PROV_LOG_ENTRY(prov_central_config_sent, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Sent current config to " CENTRAL_NAME " for audit");

DEF_PROV_LOG_ENTRY(prov_central_config_received, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received new network configuration settings from " CENTRAL_NAME "");

DEF_PROV_LOG_ENTRY(prov_central_config_applied, PROV_STATE_ATHENA, PROV_LOG_SUCCESS,
    "Applied new configuration settings from " CENTRAL_NAME ", provisioning completed");

DEF_PROV_LOG_ENTRY(prov_central_new_singleclass_image_received, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received upgrade command from " CENTRAL_NAME ", URL: %s");

DEF_PROV_LOG_ENTRY(prov_central_new_multiclass_image_received, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received upgrade command from " CENTRAL_NAME ", version: %s, URL: %s");

DEF_PROV_LOG_ENTRY(prov_central_other_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "" CENTRAL_NAME " connection error: %s");

DEF_PROV_LOG_ENTRY(prov_central_server_changed, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received new " CENTRAL_NAME " server address: %s");

DEF_PROV_LOG_ENTRY(prov_central_dns_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "DNS look up failed for " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_central_socket_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Socket error while communicating with " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_central_connect_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Could not establish TCP connection to " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_central_proxy_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "HTTPS proxy error while connecting to " CENTRAL_NAME " server at %s");

DEF_PROV_LOG_ENTRY(prov_central_ssl_connect_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Error establishing SSL connection to " CENTRAL_NAME " server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_central_ssl_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "SSL error while communicating with " CENTRAL_NAME " server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_central_ssl_cert_verification_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Could not validate certificate of " CENTRAL_NAME " server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_central_ssl_cert_cn_name_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Certificate of " CENTRAL_NAME " server at %s does not have a matching Common Name");

DEF_PROV_LOG_ENTRY(prov_central_ssl_tpm_clientauth_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Local tpm clientauth error while communicating with " CENTRAL_NAME " server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_central_ssl_cacert_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Local CA cert error while communicating with " CENTRAL_NAME " server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_central_config_received_has_no_network, PROV_STATE_ATHENA, PROV_LOG_WARNING,
    "Received configuration from Aruba central server but it contained no network information");

DEF_PROV_LOG_ENTRY(prov_central_config_apply_failed, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Error when applying configuration from Aruba central server");

DEF_PROV_LOG_ENTRY(prov_central_ssl_cert_before_date_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Before date error while loading local CA cert or establishing ssl connection");

DEF_PROV_LOG_ENTRY(prov_central_command, PROV_STATE_ATHENA, PROV_LOG_WARNING,
    "UAP received unsupport command %s from " CENTRAL_NAME);

DEF_PROV_LOG_ENTRY(prov_domain_list_received, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Received Domain list from Aruba central server");

DEF_PROV_LOG_ENTRY(prov_domain_list_received_empty, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Received Domain list from Aruba central server but it is empty");

DEF_PROV_LOG_ENTRY(prov_domain_central_program, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Program Domain for Aruba central server %s");

DEF_PROV_LOG_ENTRY(prov_domain_refresh, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Refresh Domain list");

DEF_PROV_LOG_ENTRY(prov_additional_domain_program, PROV_STATE_ATHENA, PROV_LOG_DEBUG,
    "Program Additional Domain list");

/*** append websocket logs ***/

DEF_PROV_LOG_ENTRY(prov_central_connect_fail, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Connection error with " CENTRAL_NAME " server %s reason %s");

DEF_PROV_LOG_ENTRY(prov_central_connect_success, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Estabished connection with " CENTRAL_NAME " server %s, authenticating...");

DEF_PROV_LOG_ENTRY(prov_central_disconnect_after_login, PROV_STATE_ATHENA, PROV_LOG_WARNING,
    "Disconnect with " CENTRAL_NAME " server %s reason %s after login");

DEF_PROV_LOG_ENTRY(prov_central_disconnect_before_login, PROV_STATE_ATHENA, PROV_LOG_WARNING,
    "Disconnect with " CENTRAL_NAME " server %s reason %s before login");

DEF_PROV_LOG_ENTRY(prov_central_connect_trigger, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Connecting to " CENTRAL_NAME " server %s, triggered by %s");

DEF_PROV_LOG_ENTRY(prov_central_login_done, PROV_STATE_ATHENA, PROV_LOG_SUCCESS,
    "Login done to " CENTRAL_NAME " server %s by websocket");

DEF_PROV_LOG_ENTRY(prov_central_debug, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Received debug command from " CENTRAL_NAME "");

DEF_PROV_LOG_ENTRY(prov_central_debug_sent, PROV_STATE_ATHENA, PROV_LOG_PENDING,
    "Sent debug response to " CENTRAL_NAME "");

/*** AMP Logs ***/
DEF_PROV_LOG_ENTRY(prov_amp_primary_connect, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Connecting to primary AMP server at %s...");

DEF_PROV_LOG_ENTRY(prov_amp_backup_connect, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Connecting to backup AMP server at %s...");

DEF_PROV_LOG_ENTRY(prov_amp_primary_connected, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Connected with primary AMP server %s, logging in...");

DEF_PROV_LOG_ENTRY(prov_amp_backup_connected, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Connected with backup AMP server %s, logging in...");

DEF_PROV_LOG_ENTRY(prov_amp_primary_logged_in, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Login successful with primary AMP server %s, awaiting command...");

DEF_PROV_LOG_ENTRY(prov_amp_backup_logged_in, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Login successful with backup AMP server %s, awaiting command...");

DEF_PROV_LOG_ENTRY(prov_amp_login_aborted, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Login aborted due to incomplete response from AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_logout, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Logging out of AMP server %s");

DEF_PROV_LOG_ENTRY(prov_amp_config_audit, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Received configuration audit command from AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_config_sent, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Sent current config to AMP server for audit");

DEF_PROV_LOG_ENTRY(prov_amp_config_received, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Received new network configruation settings from AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_config_applied, PROV_STATE_AMP, PROV_LOG_SUCCESS,
    "Applied new configuration settings from AMP server, provisioning completed");

DEF_PROV_LOG_ENTRY(prov_amp_primary_identified, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Received identity string from primary AMP server: %s");

DEF_PROV_LOG_ENTRY(prov_amp_backup_identified, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Received identity string from backup AMP server: %s");

DEF_PROV_LOG_ENTRY(prov_amp_primary_auth_pending, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Received auth pending status from primary AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_backup_auth_pending, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Received auth pending status from backup AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_primary_got_bad_connect_state, PROV_STATE_AMP, PROV_LOG_DEBUG,
    "Received auth pending status from primary AMP server %s, cmd is '%s'");

DEF_PROV_LOG_ENTRY(prov_amp_primary_login_aborted, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Login aborted due to incomplete response from primary AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_backup_login_aborted, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Login aborted due to incomplete response from backup AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_new_singleclass_image_received, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Received upgrade command from AMP server, URL: %s");

DEF_PROV_LOG_ENTRY(prov_amp_new_multiclass_image_received, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Received upgrade command from AMP server, version: %s, URL: %s");

DEF_PROV_LOG_ENTRY(prov_amp_dns_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "DNS look up failed for AMP server at %s");

DEF_PROV_LOG_ENTRY(prov_amp_socket_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Socket error while communicating with AMP server at %s");

DEF_PROV_LOG_ENTRY(prov_amp_connect_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Could not establish TCP connection to AMP server at %s");

DEF_PROV_LOG_ENTRY(prov_amp_proxy_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "HTTPS proxy error while connecting to AMP server at %s");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_connect_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Error establishing SSL connection to AMP server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "SSL error while communicating with AMP server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_cert_verification_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Could not validate certificate of AMP server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_cert_cn_name_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Certificate of AMP server at %s does not have a matching Common Name");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_tpm_clientauth_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Local tpm clientauth error while communicating with AMP server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_cacert_error, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Local CA cert error while communicating with AMP server at %s: %s");

DEF_PROV_LOG_ENTRY(prov_amp_config_received_has_no_network, PROV_STATE_AMP, PROV_LOG_WARNING,
    "Received configuration from AMP serverr but it contained no network information");

DEF_PROV_LOG_ENTRY(prov_amp_config_apply_failed, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Error when applying configuration from AMP server");

DEF_PROV_LOG_ENTRY(prov_amp_ssl_cert_before_date_error, PROV_STATE_ATHENA, PROV_LOG_ERROR,
    "Before date error while loading local CA cert or establishing ssl connection");

DEF_PROV_LOG_ENTRY(prov_amp_command, PROV_STATE_AMP, PROV_LOG_WARNING,
    "UAP received unsupport command %s from AMP server");

/*** append websocket logs ***/
DEF_PROV_LOG_ENTRY(prov_amp_server_changed, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Received AMP redirect server address: %s");

DEF_PROV_LOG_ENTRY(prov_amp_connect_fail, PROV_STATE_AMP, PROV_LOG_ERROR,
    "Connection error with AMP server %s reason %s");

DEF_PROV_LOG_ENTRY(prov_amp_connect_success, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Estabished connection with AMP server %s, authenticating...");

DEF_PROV_LOG_ENTRY(prov_amp_disconnect_after_login, PROV_STATE_AMP, PROV_LOG_WARNING,
    "Disconnect with AMP server %s reason %s after login");

DEF_PROV_LOG_ENTRY(prov_amp_disconnect_before_login, PROV_STATE_AMP, PROV_LOG_WARNING,
    "Disconnect with AMP server %s reason %s before login");

DEF_PROV_LOG_ENTRY(prov_amp_connect_trigger, PROV_STATE_AMP, PROV_LOG_PENDING,
    "Connecting to AMP server %s, triggered by %s");

DEF_PROV_LOG_ENTRY(prov_amp_login_done, PROV_STATE_AMP, PROV_LOG_SUCCESS,
    "Login done to AMP server %s by websocket");

/*** DHCP Option Logs ***/
DEF_PROV_LOG_ENTRY(prov_dhcp_discover_sent, PROV_STATE_DHCP_OPTION, PROV_LOG_PENDING,
    "Performing DHCP discovery");

DEF_PROV_LOG_ENTRY(prov_dhcp_lease_obtained, PROV_STATE_DHCP_OPTION, PROV_LOG_PENDING,
    "DHCP lease of %s obtained, lease time %s seconds");

DEF_PROV_LOG_ENTRY(prov_dhcp_option_obtained, PROV_STATE_DHCP_OPTION, PROV_LOG_DEBUG,
    "DHCP Option 43, len: %s, buf: '%s'");

DEF_PROV_LOG_ENTRY(prov_dhcp_option_error, PROV_STATE_DHCP_OPTION, PROV_LOG_ERROR,
    "DHCP Option error, len: %s, buf: '%s'");

DEF_PROV_LOG_ENTRY(prov_dhcp_option_parsed_simple, PROV_STATE_DHCP_OPTION, PROV_LOG_DEBUG,
    "DHCP Option parsed, AMP Server: %s, Organization: %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_proxy_server_parsed, PROV_STATE_DHCP_OPTION, PROV_LOG_DEBUG,
    "Proxy Server Option parsed, Proxy Server: %s, Port: %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_option_parsed_advanced, PROV_STATE_DHCP_OPTION, PROV_LOG_DEBUG,
    "DHCP Option parsed, AMP Server: %s, IP1: %s, IP2: %s, Organization: %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_set_amp_simple, PROV_STATE_DHCP_OPTION, PROV_LOG_SUCCESS,
    "DHCP Option applied, AMP Server: %s, Organization: %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_set_proxy_simple, PROV_STATE_DHCP_OPTION, PROV_LOG_SUCCESS,
    "Proxy Server Option applied, Proxy Server: %s, Port: %d");

DEF_PROV_LOG_ENTRY(prov_dhcp_set_amp_advanced, PROV_STATE_DHCP_OPTION, PROV_LOG_SUCCESS,
    "DHCP Option applied, AMP Server: %s, IP1: %s, IP2: %s, Organization: %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_yield_to_static_config, PROV_STATE_DHCP_OPTION, PROV_LOG_WARNING,
    "Provsioning information in DHCP option ignored because of existing configuration for AMP: server %s, organization %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_yield_to_managed_mode, PROV_STATE_DHCP_OPTION, PROV_LOG_WARNING,
    "Provisioning information in DHCP option ignored because of existing configuration for management server at %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_yield_to_central, PROV_STATE_DHCP_OPTION, PROV_LOG_WARNING,
    "Information in DHCP option ignored because IAP is being managed by central server at %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_cannot_read, PROV_STATE_DHCP_OPTION, PROV_LOG_ERROR,
    "Provisioning information in DHCP option is not valid and cannot be used");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_got_disable, PROV_STATE_DHCP_OPTION, PROV_LOG_WARNING,
    "Provisioning information in DHCP option has deactivated the use of the current AMP server %s, organization %s");

DEF_PROV_LOG_ENTRY(prov_dhcp_amp_got_nothing, PROV_STATE_DHCP_OPTION, PROV_LOG_FAIL_THROUGH,
    "No DHCP Option-based provsioning information are present, failing-through to other provisioning options");

DEF_PROV_LOG_ENTRY(prov_dhcp_activate_disable, PROV_STATE_DHCP_OPTION, PROV_LOG_WARNING,
    "DHCP Option activate-disable: %s");

/*** DNS based Airwave auto discovery Logs ***/
DEF_PROV_LOG_ENTRY(prov_autodiscovery_detecting_amp, PROV_STATE_AMP_AUTODISCOVERY, PROV_LOG_PENDING,
    "Performing DNS based Airwave auto discovery, AMP Domain: %s");

DEF_PROV_LOG_ENTRY(prov_autodiscovery_cannot_resolve_amp, PROV_STATE_AMP_AUTODISCOVERY, PROV_LOG_WARNING,
    "Failed to resolve AMP Domain %s during Airwave auto discovery");

DEF_PROV_LOG_ENTRY(prov_autodiscovery_resolve_amp, PROV_STATE_AMP_AUTODISCOVERY, PROV_LOG_SUCCESS,
    "Successfully resolve AMP Domain %s to %s during Airwave auto discovery");

/* UAP controller discovery */
DEF_PROV_LOG_ENTRY(prov_uap_adp_success, PROV_STATE_UAP_ADP, PROV_LOG_SUCCESS,
    "ADP success: %s");

DEF_PROV_LOG_ENTRY(prov_uap_adp_warning, PROV_STATE_UAP_ADP, PROV_LOG_WARNING,
    "ADP info: %s");

DEF_PROV_LOG_ENTRY(prov_uap_adp_error, PROV_STATE_UAP_ADP, PROV_LOG_ERROR,
    "ADP error: %s");

#define PROV_LOG_FILE_TEMPLATE_XML  "/tmp/prov_logs/%s.xml"
#define PROV_LOG_FILE_TEMPLATE_XML_OLD  "/tmp/prov_logs/%s.xml.%d"
#define PROV_LOG_FILE_TEMPLATE_TEXT "/tmp/prov_logs/%s.log"
#define PROV_LOG_FILE_TEMPLATE_TEXT_OLD "/tmp/prov_logs/%s.log.%d"
#define PROV_LOG_HISTORY 5

static double
getSysUptime_log(void)
{
    FILE *fp;
    double uptime_d = -1;

    if ((fp = fopen ("/proc/uptime", "r")) != NULL) {
        fscanf(fp, "%lf", &uptime_d);
        fclose (fp);
    }

    return uptime_d;
}

static void
append_provision_log_msg(const char *file_prefix, provision_state_t prov_state, const char *text_msg, provision_log_type_t log_type, const char *name, const char *args)
{
    struct timeval t_now = {0, 0};
    char fn_text[256];
    char fn_xml[256];
    FILE *fp_text = NULL;
    FILE *fp_xml = NULL;
    char msg[256];
    double uptime_d = getSysUptime_log();
    char *msg_type = "N/A", *msg_state = "N/A";
    struct stat file_stat,file_stat1;
    char file_oversize = 0;

    gettimeofday(&t_now, NULL);

    snprintf(fn_xml, sizeof(fn_xml), PROV_LOG_FILE_TEMPLATE_XML, file_prefix);
    fp_xml = fopen(fn_xml, "at");

    if (!fp_xml) {
        snprintf(msg, sizeof(msg), "Could not open log file %s", fn_xml);
        syslog(LOG_EMERG, msg);
        return;
    }

    snprintf(fn_text, sizeof(fn_text), PROV_LOG_FILE_TEMPLATE_TEXT, file_prefix);
    fp_text = fopen(fn_text, "at");
    if (!fp_text) {
        fclose(fp_xml);
        snprintf(msg, sizeof(msg), "Could not open log file %s", fn_xml);
        syslog(LOG_EMERG, msg);
        return;
    }

    fprintf(fp_xml, "%010.2lf,", uptime_d);
    switch(prov_state) {
    case PROV_STATE_INIT:
        msg_state = "Start";
        break;
    case PROV_STATE_DHCP_OPTION:
        msg_state = "DHCP Option";
        break;
    case PROV_STATE_AMP_AUTODISCOVERY:
        msg_state = "Airwave Auto Discovery";
        break;
    case PROV_STATE_ACTIVATE:
        msg_state = "Activate";
        break;
    case PROV_STATE_AMP:
        msg_state = "Airwave";
        break;
    case PROV_STATE_ATHENA:
        msg_state = "Central";
        break;
    case PROV_STATE_MGMT_SERVER:
        msg_state = "Management Server";
        break;
    case PROV_STATE_UAP_ADP:
        msg_state = "UAP ADP";
        break;
    }
    fprintf(fp_xml, "%s,", msg_state);
    switch(log_type) {
    case PROV_LOG_PENDING:
        msg_type = "In progress";
        break;
    case PROV_LOG_SUCCESS:
        msg_type = "Completed";
        break;
    case PROV_LOG_WARNING:
        msg_type = "Warning";
        break;
    case PROV_LOG_ERROR:
        msg_type = "Failed";
        break;
    case PROV_LOG_FAIL_THROUGH:
        msg_type = "Fail through";
        break;
    case PROV_LOG_DEBUG:
        msg_type = "Debug";
        break;
    }
    fprintf(fp_xml, "%s,", msg_type);
    fprintf(fp_xml, "%s", name);
    if (text_msg) {
        fprintf(fp_text, "%010.2lf,%s,%s,%s\n", uptime_d, msg_state, msg_type, text_msg);
    }
    if (args && args[0]) {
        fprintf(fp_xml, ",%s", args);
    }
    fprintf(fp_xml, "\n");
    fclose(fp_xml);
    fclose(fp_text);

    stat((char *)fn_xml,&file_stat);
    stat((char *)fn_text,&file_stat1);
    
    if (file_stat.st_size >= 32*1024 || file_stat1.st_size >= 32*1024) {
        file_oversize =1;
    } 

    if (log_type == PROV_LOG_ERROR || file_oversize) {
        char fn_text_old[256];
        char fn_xml_old[256];
        char fn_text_old1[256];
        char fn_xml_old1[256];
        int i;

        for (i = PROV_LOG_HISTORY; i > 1; i--) {
            snprintf(fn_xml_old, sizeof(fn_xml_old), PROV_LOG_FILE_TEMPLATE_XML_OLD, file_prefix, i-1);
            snprintf(fn_text_old, sizeof(fn_text_old), PROV_LOG_FILE_TEMPLATE_TEXT_OLD, file_prefix, i-1);
            snprintf(fn_xml_old1, sizeof(fn_xml_old), PROV_LOG_FILE_TEMPLATE_XML_OLD, file_prefix, i);
            snprintf(fn_text_old1, sizeof(fn_text_old), PROV_LOG_FILE_TEMPLATE_TEXT_OLD, file_prefix, i);
            rename(fn_xml_old, fn_xml_old1);
            rename(fn_text_old, fn_text_old1);
        }
        rename(fn_xml, fn_xml_old);
        rename(fn_text, fn_text_old);
    }
}

void
format_log_args (char * buf, int maxlen, char *format, ...)
{
    va_list ap;
    int s = 0;
    char *key = format;
    char *val;

    buf[0] = 0;

    va_start (ap, format);

    while ((key = strstr(key, "\%s")) && (s < maxlen)) {
            val = va_arg (ap, char *);

            s += snprintf(buf + s, maxlen - s, "%s,", val);
            key += 2;
    }
    
    va_end(ap);
}

extern int prov_log_disabled(provision_state_t prov_state);

void
append_provision_log_entry(provision_log_entry_t *entry, const char *msg, const char *args, int no_repeat)
{
#if 0
    static char last_msg[256] = "";
#endif

    provision_state_t prov_state = entry->state;
    
    if (prov_log_disabled(prov_state)) {
        return;
    }

#if 0
    if (no_repeat) {
        if (strcmp(last_msg, msg) == 0) {
            return;
        }
        strlcpy(last_msg, msg, sizeof(last_msg));
    }
#endif

    switch(prov_state) {
    case PROV_STATE_INIT:
    case PROV_STATE_DHCP_OPTION:
    case PROV_STATE_AMP_AUTODISCOVERY:
        append_provision_log_msg("init", prov_state, msg, entry->type, entry->name, args);
        break;

    case PROV_STATE_ACTIVATE:
        append_provision_log_msg("provision", prov_state, msg, entry->type, entry->name, args);
        break;

    case PROV_STATE_AMP:
    case PROV_STATE_ATHENA:
    case PROV_STATE_MGMT_SERVER:
        append_provision_log_msg("nms", prov_state, msg, entry->type, entry->name, args);
        break;
    case PROV_STATE_UAP_ADP:
        append_provision_log_msg("adp", prov_state, msg, entry->type, entry->name, args);
        break;
    }
}
