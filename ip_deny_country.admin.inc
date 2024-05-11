<?php
/**
 * @file
 * Admin forms to manage module
 *
 */

/**
 * Define the form
 *
 */
function ip_deny_country_form($form, &$form_state) {
  $ip_deny_country_last_download = state_get('ip_deny_country_last_download');
  $last_download_time = is_null($ip_deny_country_last_download) ? t('not available')
    : date('Y-m-d H:i:s', $ip_deny_country_last_download);

  $form['#attached']['css'] = array(
    backdrop_get_path('module', 'ip_deny_country') . '/css/ip_deny_country.admin.css',
  );
  $form['#attached']['js'] = array(
    backdrop_get_path('module', 'ip_deny_country') . '/js/ip_deny_country.admin.js',
  );

  $config             = config('ip_deny_country.settings');
  $selected_countries = $config->get('selected_countries');
  $countries          = include_once backdrop_get_path('module', 'ip_deny_country') . '/includes/countries.php';
  $all_countries      = array();
  $return_code        = $config->get('return_code');
  $access_restriction = $config->get('access_restriction');
  $selected_list      = '';

  foreach ($countries as $country) {
    $all_countries[$country['alpha2']] = $country['name'];
  }

  $form['top'] = array(
    '#markup' => t('This page allows you to select the type of access restriction for countries from which visitors are not welcome.'),
  );

  $form['access_restriction'] = array(
    '#title' => t('Access restriction type:'),
    '#type' => 'radios',
    '#options' => array(
      'none' => t('None (allow from any country)'),
      'allow' => t('Allow from selected countries'),
      'deny' => t('Deny from selected countries'),
    ),
    '#default_value' => $access_restriction,
  );

  if (!empty($selected_countries)) {
    foreach ($selected_countries as $selected_country) {
      $selected_list .= $all_countries[$selected_country] . ', ';
    }
    $selected_list = rtrim($selected_list, ', ');
  }

  $list = $selected_list == '' ? t('none') : '<span class="selected_countries">' . $selected_list . '</span>';

  $form['cfs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Selected countries: ') . $list,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#states' => array(
      'invisible' => array(
        ':input[name="access_restriction"]' => array('value' => 'none'),
      ),
    ),
  );

  $form['cfs']['countries'] = array(
    '#title' => t('World countries') . ':',
    '#type' => 'checkboxes',
    '#options' => $all_countries,
    '#default_value' => !empty($selected_countries) ? $selected_countries : array(),
    '#attributes' => array('class' => array('ip-deny-country')),
  );

  $form['update_every'] = array(
    '#title' => t('How often the data for the countries should be updated:'),
    '#type' => 'select',
    '#options' => array(
      86400 => t('Every 24 hours'),
      604800 => t('Every 7 days'),
      2592000 => t('Every 30 days'),
      0 => t('Manually (Save Settings)'),
    ),
    '#default_value' => $config->get('update_every'),
    '#description' => t('Latest time data for countries has been updated: ') . $last_download_time,
    '#states' => array(
      'disabled' => array(
        ':input[name="access_restriction"]' => array('value' => 'none'),
      ),
    ),
  );

  $form['return_code'] = array(
    '#type' => 'radios',
    '#title' => t('Select return code:'),
    '#options' => array(
      400 => t('400 Bad request'),
      403 => t('403 Forbidden'),
      404 => t('404 Not Found'),
    ),
    '#default_value' => $return_code,
    '#description' => t('The return code to display to the unwanted site visitor.'),
    '#states' => array(
      'disabled' => array(
        ':input[name="access_restriction"]' => array('value' => 'none'),
      ),
    ),
  );

  $form['log_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => $config->get('log_enabled'),
    '#description' => t('Enable logging of denied access attempts.'),
    '#states' => array(
      'disabled' => array(
        ':input[name="access_restriction"]' => array('value' => 'none'),
      ),
    ),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['spinner'] = array(
    '#markup' => '<div id="spinner"></div>',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Implements hook_form_alter().
 *
 */
function ip_deny_country_form_alter(&$form, &$form_state, $form_id) {
  // Disable the checkbox that denies the current user country.
  if ($form_id == 'ip_deny_country_form') {
    $ip = check_plain(ip_address());
    $ip_data = json_decode(file_get_contents("http://www.geoplugin.net/json.gp?ip=" . $ip));
    if ($ip_data && is_string($ip_data->geoplugin_countryCode)) {
      $country_code = strtolower($ip_data->geoplugin_countryCode);
      $form['cfs']['countries'][$country_code]['#states'] = array(
        'disabled' => array(
          ':input[name="access_restriction"]' => array('value' => 'deny'),
        ),
      );
    }
  }
}

/**
 * Implements hook_validate().
 *
 */
function ip_deny_country_form_validate(&$form, &$form_state) {
  if ($form_state['values']['access_restriction'] != 'none'
  && count(array_filter($form_state['values']['countries'])) == 0) {
    form_set_error('countries', t('Please select at least one country.'));
  }
}

/**
* Implements hook_submit().
 *
 */
function ip_deny_country_form_submit($form, &$form_state) {
  $config                = config('ip_deny_country.settings');
  $update_every          = (int) $form_state['values']['update_every'];
  $return_code           = (int) $form_state['values']['return_code'];
  $log_enabled           = (int) $form_state['values']['log_enabled'];
  $access_restriction    = $form_state['values']['access_restriction'];
  $selected_codes        = [];

  if ($access_restriction != 'none') {
    $selected_countries = $form_state['values']['countries'];

    foreach (array_keys($selected_countries, 0, true) as $key) {
      unset($selected_countries[$key]);
    }

    $selected_codes = array_keys($selected_countries);
  }

  $config->set('access_restriction', $access_restriction);
  $config->set('selected_countries', $selected_codes);
  $config->set('update_every', $update_every);
  $config->set('return_code', $return_code);
  $config->set('log_enabled', $log_enabled);

  $config->set('public_files_path', BACKDROP_ROOT . '/' . config_get('system.core','file_public_path' ));

  $config->save();

  if (ip_deny_country_update($selected_codes)) {
    backdrop_set_message(t('The configuration options have been saved.'));
  }
}
